export interface User {
  id: string;
  email: string;
  username: string;
  password: string;
  root_admin: boolean;
  created_at: Date;
  updated_at: Date;
  _count?: {
    servers: number;
  };
}

export interface Server {
  id: string;
  user_id: string;
  name: string;
  node_id: string;
  egg_id: string;
  uuid: string;
  startup: string;
  environment: Record<string, string>;
  image: string;
  skip_egg_install: boolean;
  created_at: Date;
  status?: "running" | "stopped" | "starting" | "stopping" | "error";
  node?: Node;
  egg?: Egg;
  allocations?: Allocation[];
}

export interface Node {
  id: string;
  name: string;
  location_id: string;
  fqdn: string;
  scheme: string;
  behind_proxy: boolean;
  memory: number;
  disk: number;
  cpu: number;
  io: number;
  created_at: Date;
  allocated_memory?: number;
  allocated_disk?: number;
  servers_count?: number;
}

export interface Egg {
  id: string;
  name: string;
  category: string;
  description: string;
  docker_image: string;
  startup: string;
  config_files: string[];
  created_at: Date;
}

export interface Database {
  id: string;
  server_id: string;
  name: string;
  host: string;
  port: number;
  username: string;
  password: string;
  created_at: Date;
}

export interface Backup {
  id: string;
  server_id: string;
  name: string;
  size: number;
  created_at: Date;
}

export interface ActivityLog {
  id: string;
  user_id: string;
  event: string;
  ip: string;
  data: Record<string, unknown>;
  created_at: Date;
}

export interface ApiKey {
  id: string;
  user_id: string;
  memo: string;
  token: string;
  created_at: Date;
}

export interface Schedule {
  id: string;
  server_id: string;
  name: string;
  is_active: boolean;
  created_at: Date;
}

export interface Allocation {
  id: string;
  node_id: string;
  ip: string;
  port: number;
  server_id: string | null;
}

export interface JWTPayload {
  userId: string;
  email: string;
  username: string;
  root_admin: boolean;
}

export interface ApiResponse<T = unknown> {
  success: boolean;
  data?: T;
  error?: string;
  message?: string;
}

export interface PaginatedResponse<T> {
  data: T[];
  total: number;
  page: number;
  per_page: number;
  total_pages: number;
}

export interface ServerStats {
  cpu_percent: number;
  memory_usage: number;
  memory_limit: number;
  disk_usage: number;
  disk_limit: number;
  network_rx: number;
  network_tx: number;
  uptime: number;
}

export interface FileEntry {
  name: string;
  path: string;
  is_directory: boolean;
  size: number;
  modified: Date;
  created: Date;
  mime_type?: string;
}

export interface ConsoleMessage {
  type: "stdout" | "stderr" | "system";
  message: string;
  timestamp: Date;
}
