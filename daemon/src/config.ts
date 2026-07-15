import dotenv from "dotenv";
import path from "path";

dotenv.config({ path: path.resolve(__dirname, "../.env") });

export interface Config {
  panelUrl: string;
  panelSecret: string;
  daemonPort: number;
  dockerSocket: string;
  dataDir: string;
}

function env(key: string, fallback?: string): string {
  const value = process.env[key] ?? fallback;
  if (value === undefined) {
    throw new Error(`Missing required environment variable: ${key}`);
  }
  return value;
}

export const config: Config = {
  panelUrl: env("PANEL_URL", "http://localhost:3000"),
  panelSecret: env("PANEL_SECRET", "change-me-to-a-random-secret"),
  daemonPort: parseInt(env("DAEMON_PORT", "8080"), 10),
  dockerSocket: env("DOCKER_SOCKET", "/var/run/docker.sock"),
  dataDir: env("DATA_DIR", "/var/lib/codix-servers"),
};
