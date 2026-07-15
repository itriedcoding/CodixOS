import { Router, Request, Response } from "express";
import jwt from "jsonwebtoken";
import fs from "fs-extra";
import path from "path";
import archiver from "archiver";
import extract from "extract-zip";
import { config } from "./config";
import * as docker from "./docker";
import logger from "./logger";

const router = Router();

interface AuthPayload {
  serverId: string;
  panelUrl: string;
}

function authMiddleware(req: Request, res: Response, next: Function): void {
  const header = req.headers.authorization;
  if (!header?.startsWith("Bearer ")) {
    res.status(401).json({ error: "Missing or invalid Authorization header" });
    return;
  }

  try {
    const payload = jwt.verify(header.slice(7), config.panelSecret) as AuthPayload;
    (req as Request & { auth: AuthPayload }).auth = payload;
    next();
  } catch {
    res.status(401).json({ error: "Invalid or expired JWT" });
  }
}

router.use(authMiddleware);

router.get("/health", async (_req: Request, res: Response) => {
  const containers = await docker.listContainers();
  res.json({ status: "ok", managedServers: containers.length });
});

router.get("/servers", async (_req: Request, res: Response) => {
  const containers = await docker.listContainers();
  const servers = containers.map((c) => ({
    id: c.Labels?.["codix.server.id"] ?? c.Id.slice(0, 12),
    name: c.Labels?.["codix.server.name"] ?? "unknown",
    status: c.State,
    ports: c.Ports,
  }));
  res.json(servers);
});

router.post("/servers/:id/start", async (req: Request, res: Response) => {
  try {
    await docker.startServer(req.params.id);
    res.json({ status: "started" });
  } catch (err) {
    logger.error("Failed to start server", { error: err });
    res.status(500).json({ error: "Failed to start server" });
  }
});

router.post("/servers/:id/stop", async (req: Request, res: Response) => {
  try {
    await docker.stopServer(req.params.id);
    res.json({ status: "stopped" });
  } catch (err) {
    logger.error("Failed to stop server", { error: err });
    res.status(500).json({ error: "Failed to stop server" });
  }
});

router.post("/servers/:id/restart", async (req: Request, res: Response) => {
  try {
    await docker.stopServer(req.params.id);
    await docker.startServer(req.params.id);
    res.json({ status: "restarted" });
  } catch (err) {
    logger.error("Failed to restart server", { error: err });
    res.status(500).json({ error: "Failed to restart server" });
  }
});

router.post("/servers/:id/kill", async (req: Request, res: Response) => {
  try {
    await docker.killServer(req.params.id);
    res.json({ status: "killed" });
  } catch (err) {
    logger.error("Failed to kill server", { error: err });
    res.status(500).json({ error: "Failed to kill server" });
  }
});

router.get("/servers/:id/stats", async (req: Request, res: Response) => {
  try {
    const stats = await docker.getContainerStats(req.params.id);
    res.json(stats);
  } catch (err) {
    logger.error("Failed to get stats", { error: err });
    res.status(500).json({ error: "Failed to get stats" });
  }
});

router.post("/servers/:id/command", async (req: Request, res: Response) => {
  const { command } = req.body;
  if (!command) {
    res.status(400).json({ error: "Missing command" });
    return;
  }

  try {
    await docker.sendCommand(req.params.id, command);
    res.json({ status: "sent" });
  } catch (err) {
    logger.error("Failed to send command", { error: err });
    res.status(500).json({ error: "Failed to send command" });
  }
});

router.get("/servers/:id/files", async (req: Request, res: Response) => {
  const dirPath = path.join(config.dataDir, req.params.id, req.query.path as string || "");
  try {
    const entries = await fs.readdir(dirPath, { withFileTypes: true });
    const files = entries.map((e) => ({
      name: e.name,
      isDirectory: e.isDirectory(),
      size: e.isFile() ? fs.statSync(path.join(dirPath, e.name)).size : null,
      modified: fs.statSync(path.join(dirPath, e.name)).mtime,
    }));
    res.json(files);
  } catch (err) {
    res.status(404).json({ error: "Directory not found" });
  }
});

router.get("/servers/:id/files/*", async (req: Request, res: Response) => {
  const filePath = path.join(config.dataDir, req.params.id, (req.params as Record<string, string>)["0"]);
  try {
    await fs.access(filePath);
    res.sendFile(filePath);
  } catch {
    res.status(404).json({ error: "File not found" });
  }
});

router.put("/servers/:id/files/*", async (req: Request, res: Response) => {
  const filePath = path.join(config.dataDir, req.params.id, (req.params as Record<string, string>)["0"]);
  try {
    await fs.ensureDir(path.dirname(filePath));
    await fs.writeFile(filePath, req.body.content);
    res.json({ status: "written" });
  } catch (err) {
    res.status(500).json({ error: "Failed to write file" });
  }
});

router.delete("/servers/:id/files/*", async (req: Request, res: Response) => {
  const filePath = path.join(config.dataDir, req.params.id, (req.params as Record<string, string>)["0"]);
  try {
    await fs.remove(filePath);
    res.json({ status: "deleted" });
  } catch (err) {
    res.status(500).json({ error: "Failed to delete file" });
  }
});

router.post("/servers/:id/download", async (req: Request, res: Response) => {
  const { file } = req.body;
  const filePath = path.join(config.dataDir, req.params.id, file);
  try {
    await fs.access(filePath);
    res.download(filePath);
  } catch {
    res.status(404).json({ error: "File not found" });
  }
});

router.post("/servers/:id/upload", async (req: Request, res: Response) => {
  const { filePath: relPath, content } = req.body;
  const filePath = path.join(config.dataDir, req.params.id, relPath);
  try {
    await fs.ensureDir(path.dirname(filePath));
    await fs.writeFile(filePath, content);
    res.json({ status: "uploaded" });
  } catch (err) {
    res.status(500).json({ error: "Failed to upload file" });
  }
});

export default router;
