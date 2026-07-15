import { WebSocketServer, WebSocket } from "ws";
import { Server } from "http";
import { verifyToken } from "./auth";
import type { JWTPayload } from "@/types";

interface SocketConnection {
  ws: WebSocket;
  user: JWTPayload;
  serverId: string;
}

const connections: Map<string, SocketConnection[]> = new Map();

export function initializeWebSocket(server: Server): WebSocketServer {
  const wss = new WebSocketServer({ server, path: "/ws" });

  wss.on("connection", (ws, req) => {
    const url = new URL(req.url || "", `http://${req.headers.host}`);
    const token = url.searchParams.get("token");
    const serverId = url.searchParams.get("serverId");

    if (!token || !serverId) {
      ws.close(1008, "Missing token or serverId");
      return;
    }

    const user = verifyToken(token);
    if (!user) {
      ws.close(1008, "Invalid token");
      return;
    }

    const connection: SocketConnection = { ws, user, serverId };

    const serverConnections = connections.get(serverId) || [];
    serverConnections.push(connection);
    connections.set(serverId, serverConnections);

    ws.on("message", (data) => {
      try {
        const message = JSON.parse(data.toString());
        handleWebSocketMessage(connection, message);
      } catch {
        ws.send(JSON.stringify({ type: "error", message: "Invalid message format" }));
      }
    });

    ws.on("close", () => {
      const conns = connections.get(serverId) || [];
      const index = conns.indexOf(connection);
      if (index > -1) {
        conns.splice(index, 1);
      }
      if (conns.length === 0) {
        connections.delete(serverId);
      }
    });

    ws.send(
      JSON.stringify({
        type: "connected",
        message: "Connected to Codix Panel WebSocket",
      })
    );
  });

  return wss;
}

function handleWebSocketMessage(
  connection: SocketConnection,
  message: { type: string; data?: unknown }
): void {
  switch (message.type) {
    case "console:subscribe":
      connection.ws.send(
        JSON.stringify({ type: "console:subscribed", serverId: connection.serverId })
      );
      break;

    case "stats:subscribe":
      connection.ws.send(
        JSON.stringify({ type: "stats:subscribed", serverId: connection.serverId })
      );
      break;

    case "ping":
      connection.ws.send(JSON.stringify({ type: "pong" }));
      break;

    default:
      connection.ws.send(
        JSON.stringify({ type: "error", message: "Unknown message type" })
      );
  }
}

export function broadcastToServer(
  serverId: string,
  message: Record<string, unknown>
): void {
  const serverConnections = connections.get(serverId) || [];
  const messageStr = JSON.stringify(message);

  serverConnections.forEach((conn) => {
    if (conn.ws.readyState === WebSocket.OPEN) {
      conn.ws.send(messageStr);
    }
  });
}

export function getConnectionCount(): number {
  let count = 0;
  connections.forEach((conns) => {
    count += conns.length;
  });
  return count;
}
