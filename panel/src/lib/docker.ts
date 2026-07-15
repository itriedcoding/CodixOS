import Dockerode from "dockerode";

const docker = new Dockerode({
  socketPath: process.env.DOCKER_HOST || "/var/run/docker.sock",
});

export interface ContainerCreateOptions {
  name: string;
  Image: string;
  Cmd?: string[];
  Env?: string[];
  HostConfig?: {
    Memory?: number;
    CpuPeriod?: number;
    CpuQuota?: number;
    Binds?: string[];
    PortBindings?: Record<string, { HostPort: string }[]>;
    RestartPolicy?: { Name: string; MaximumRetryCount?: number };
  };
  Labels?: Record<string, string>;
}

export async function createContainer(
  options: ContainerCreateOptions
): Promise<Dockerode.Container> {
  const container = await docker.createContainer({
    name: options.name,
    Image: options.Image,
    Cmd: options.Cmd,
    Env: options.Env,
    HostConfig: options.HostConfig,
    Labels: {
      ...options.Labels,
      "managed-by": "codix-panel",
    },
  });
  return container;
}

export async function startContainer(
  containerId: string
): Promise<void> {
  const container = docker.getContainer(containerId);
  await container.start();
}

export async function stopContainer(
  containerId: string,
  timeout?: number
): Promise<void> {
  const container = docker.getContainer(containerId);
  await container.stop({ t: timeout || 10 });
}

export async function removeContainer(
  containerId: string,
  force?: boolean
): Promise<void> {
  const container = docker.getContainer(containerId);
  await container.remove({ force: force || false });
}

export async function execCommand(
  containerId: string,
  command: string[]
): Promise<string> {
  const container = docker.getContainer(containerId);
  const exec = await container.exec({
    Cmd: command,
    AttachStdout: true,
    AttachStderr: true,
  });

  const stream = await exec.start({ Detach: false });

  return new Promise((resolve, reject) => {
    let output = "";
    stream.on("data", (chunk: Buffer) => {
      output += chunk.toString();
    });
    stream.on("end", () => resolve(output));
    stream.on("error", reject);
  });
}

export async function getContainerStats(
  containerId: string
): Promise<{
  cpu_percent: number;
  memory_usage: number;
  memory_limit: number;
  network_rx: number;
  network_tx: number;
}> {
  const container = docker.getContainer(containerId);
  const stats = await container.stats({ stream: false });

  const cpuDelta =
    stats.cpu_stats.cpu_usage.total_usage -
    (stats.precpu_stats.cpu_usage.total_usage || 0);
  const systemDelta =
    stats.cpu_stats.system_cpu_usage -
    (stats.precpu_stats.system_cpu_usage || 0);
  const cpuPercent =
    systemDelta > 0 ? (cpuDelta / systemDelta) * stats.cpu_stats.online_cpus * 100 : 0;

  return {
    cpu_percent: Math.round(cpuPercent * 100) / 100,
    memory_usage: stats.memory_stats.usage || 0,
    memory_limit: stats.memory_stats.limit || 0,
    network_rx: stats.networks?.eth0?.rx_bytes || 0,
    network_tx: stats.networks?.eth0?.tx_bytes || 0,
  };
}

export async function listContainers(
  all?: boolean
): Promise<Dockerode.ContainerInfo[]> {
  return docker.listContainers({ all: all || false });
}

export async function getContainer(
  containerId: string
): Promise<Dockerode.Container> {
  return docker.getContainer(containerId);
}

export async function getContainerInfo(
  containerId: string
): Promise<Dockerode.ContainerInspectInfo> {
  const container = docker.getContainer(containerId);
  return container.inspect();
}

export default docker;
