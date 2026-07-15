import { WebSocketServer, WebSocket } from "ws";
import { IncomingMessage } from "http";
import jwt from "jsonwebtoken";
import { config } from "./config";
import * as docker from "./docker";
import logger from "./logger";

export function setupWebSocket(wss: WebSocketServer): void {
  wss.on("connection", (ws: WebSocket, req: IncomingMessage) => {
    const url = new URL(req.url ?? "/", `http://${req.headers.host}`);
    const token = url.searchParams.get("token");
    const serverId = url.searchParams.get("serverId");

    if (!token || !serverId) {
      ws.close(4001, "Missing token or serverId");
      return;
    }

    try {
      jwt.verify(token, config.panelSecret);
    } catch {
      ws.close(4001, "Invalid token");
      return;
    }

    logger.info("WebSocket client connected", { serverId });

    let dockerStream: NodeJS.ReadWriteStream | null = null;
    let statsInterval: ReturnType<typeof setInterval> | null = null;

    ws.on("message", async (data: Buffer) => {
      try {
        const msg = JSON.parse(data.toString()) as { action: string; payload?: unknown };

        switch (msg.action) {
          case "attach": {
            const writable = new (await import("stream")).PassThrough();
            dockerStream = await docker.streamConsole(serverId, writable);

            writable.on("data", (chunk: Buffer) => {
              if (ws.readyState === WebSocket.OPEN) {
                ws.send(JSON.stringify({ type: "output", data: chunk.toString() }));
              }
            });

            ws.send(JSON.stringify({ type: "attached" }));
            break;
          }

          case "command": {
            const cmd = msg.payload as string;
            await docker.sendCommand(serverId, cmd);
            break;
          }

          case "subscribeStats": {
            statsInterval = setInterval(async () => {
              try {
                const stats = await docker.getContainerStats(serverId);
                if (ws.readyState === WebSocket.OPEN) {
                  ws.send(JSON.stringify({ type: "stats", data: stats }));
                }
              } catch {
                // Container might be stopped
              }
            }, 2000);
            break;
          }

          case "unsubscribeStats": {
            if (statsInterval) {
              clearInterval(statsInterval);
              statsInterval = null;
            }
            break;
          }
        }
      } catch (err) {
        logger.error("WebSocket message error", { error: err });
      }
    });

    ws.on("close", () => {
      if (dockerStream) {
        (dockerStream as unknown as NodeJS.ReadableStream).unpipe?.();
      }
      if (statsInterval) {
        clearInterval(statsInterval);
      }
      logger.info("WebSocket client disconnected", { serverId });
    });
  });
}
