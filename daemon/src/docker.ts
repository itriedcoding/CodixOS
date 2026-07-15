import Dockerode from "dockerode";
import { Writable } from "stream";
import { config } from "./config";
import logger from "./logger";

const docker = new Dockerode({ socketPath: config.dockerSocket });

export interface ServerConfig {
  id: string;
  name: string;
  image: string;
  cpuLimit?: number;
  memoryLimit?: number;
  environment?: Record<string, string>;
  ports?: Record<string, string>;
  volumes?: Array<{ host: string; container: string }>;
}

const PREFIX = "codix-";

function containerName(serverId: string): string {
  return `${PREFIX}${serverId}`;
}

export async function createServer(cfg: ServerConfig): Promise<void> {
  const name = containerName(cfg.id);
  const envVars = Object.entries(cfg.environment ?? {}).map(([k, v]) => `${k}=${v}`);
  const portBindings: Record<string, { HostPort: string }[]> = {};
  const exposedPorts: Record<string, Record<string, never>> = {};

  for (const [hostPort, containerPort] of Object.entries(cfg.ports ?? {})) {
    exposedPorts[`${containerPort}/tcp`] = {};
    portBindings[`${containerPort}/tcp`] = [{ HostPort: hostPort }];
  }

  const hostConfig: Dockerode.HostConfig = {
    AutoRemove: false,
    Binds: (cfg.volumes ?? []).map((v) => `${v.host}:${v.container}`),
    PortBindings: portBindings,
    Memory: cfg.memoryLimit,
    NanoCpus: cfg.cpuLimit ? cfg.cpuLimit * 1e9 : undefined,
  };

  await docker.createContainer({
    Image: cfg.image,
    name,
    Env: envVars,
    ExposedPorts: exposedPorts,
    HostConfig: hostConfig,
    Labels: {
      "codix.managed": "true",
      "codix.server.id": cfg.id,
      "codix.server.name": cfg.name,
    },
  });

  logger.info("Container created", { serverId: cfg.id, name });
}

export async function startServer(serverId: string): Promise<void> {
  const container = docker.getContainer(containerName(serverId));
  await container.start();
  logger.info("Server started", { serverId });
}

export async function stopServer(serverId: string): Promise<void> {
  const container = docker.getContainer(containerName(serverId));
  try {
    await container.stop({ t: 30 });
    logger.info("Server stopped (SIGTERM)", { serverId });
  } catch (err: unknown) {
    if (err instanceof Error && "statusCode" in err && (err as { statusCode: number }).statusCode === 304) {
      logger.warn("Server already stopped", { serverId });
      return;
    }
    logger.warn("Graceful stop failed, force killing", { serverId });
    await container.kill();
    logger.info("Server killed", { serverId });
  }
}

export async function killServer(serverId: string): Promise<void> {
  const container = docker.getContainer(containerName(serverId));
  try {
    await container.kill();
    logger.info("Server force killed", { serverId });
  } catch (err: unknown) {
    if (err instanceof Error && "statusCode" in err && (err as { statusCode: number }).statusCode === 404) {
      return;
    }
    throw err;
  }
}

export async function removeServer(serverId: string): Promise<void> {
  const container = docker.getContainer(containerName(serverId));
  try {
    await container.remove({ force: true });
    logger.info("Container removed", { serverId });
  } catch (err: unknown) {
    if (err instanceof Error && "statusCode" in err && (err as { statusCode: number }).statusCode === 404) {
      return;
    }
    throw err;
  }
}

export async function getContainerStats(serverId: string): Promise<{
  cpuPercent: number;
  memoryUsage: number;
  memoryLimit: number;
  memoryPercent: number;
  networkRx: number;
  networkTx: number;
}> {
  const container = docker.getContainer(containerName(serverId));
  const stats = await container.stats({ stream: false });

  const cpuDelta = stats.cpu_stats.cpu_usage.total_usage - stats.precpu_stats.cpu_usage.total_usage;
  const systemDelta = stats.cpu_stats.system_cpu_usage - stats.precpu_stats.system_cpu_usage;
  const cpuCount = stats.cpu_stats.online_cpus || 1;
  const cpuPercent = systemDelta > 0 ? (cpuDelta / systemDelta) * cpuCount * 100 : 0;

  const memoryUsage = stats.memory_stats.usage ?? 0;
  const memoryLimit = stats.memory_stats.limit ?? 1;
  const memoryPercent = (memoryUsage / memoryLimit) * 100;

  const networks = stats.networks ?? {};
  let networkRx = 0;
  let networkTx = 0;
  for (const net of Object.values(networks)) {
    networkRx += net.rx_bytes ?? 0;
    networkTx += net.tx_bytes ?? 0;
  }

  return { cpuPercent, memoryUsage, memoryLimit, memoryPercent, networkRx, networkTx };
}

export async function streamConsole(
  serverId: string,
  writable: Writable
): Promise<NodeJS.ReadWriteStream> {
  const container = docker.getContainer(containerName(serverId));
  const stream = await container.attach({
    stream: true,
    stdout: true,
    stderr: true,
    logs: true,
  });

  stream.pipe(writable);
  return stream;
}

export async function sendCommand(serverId: string, command: string): Promise<void> {
  const container = docker.getContainer(containerName(serverId));
  const exec = await container.exec({
    Cmd: ["sh", "-c", command],
    AttachStdout: true,
    AttachStderr: true,
  });
  await exec.start({ Detach: true });
}

export async function listContainers(): Promise<Dockerode.ContainerInfo[]> {
  const containers = await docker.listContainers({
    all: true,
    filters: { label: ["codix.managed=true"] },
  });
  return containers;
}

export function getDocker(): Dockerode {
  return docker;
}
