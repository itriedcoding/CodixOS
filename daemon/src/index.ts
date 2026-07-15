import http from "http";
import express from "express";
import { WebSocketServer } from "ws";
import { config } from "./config";
import router from "./router";
import { setupWebSocket } from "./websocket";
import logger from "./logger";
import * as docker from "./docker";

async function main(): Promise<void> {
  logger.info("Starting Codix Wings daemon", { port: config.daemonPort });

  try {
    const info = await docker.getDocker().info();
    logger.info("Docker connected", { version: info.ServerVersion });
  } catch (err) {
    logger.error("Failed to connect to Docker", { error: err });
    process.exit(1);
  }

  const app = express();
  app.use(express.json({ limit: "50mb" }));
  app.use(router);

  const server = http.createServer(app);

  const wss = new WebSocketServer({ server, path: "/ws" });
  setupWebSocket(wss);

  server.listen(config.daemonPort, () => {
    logger.info("Daemon API listening", { port: config.daemonPort });
  });

  setInterval(async () => {
    try {
      const res = await fetch(`${config.panelUrl}/api/nodes/heartbeat`, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Authorization: `Bearer ${config.panelSecret}`,
        },
        body: JSON.stringify({
          port: config.daemonPort,
          servers: (await docker.listContainers()).length,
        }),
      });
      if (!res.ok) {
        logger.warn("Heartbeat failed", { status: res.status });
      }
    } catch {
      logger.warn("Heartbeat unreachable (panel may be offline)");
    }
  }, 30_000);

  process.on("SIGTERM", async () => {
    logger.info("Received SIGTERM, shutting down");
    wss.close();
    server.close();
    process.exit(0);
  });

  process.on("SIGINT", async () => {
    logger.info("Received SIGINT, shutting down");
    wss.close();
    server.close();
    process.exit(0);
  });
}

main().catch((err) => {
  logger.error("Fatal error", { error: err });
  process.exit(1);
});
