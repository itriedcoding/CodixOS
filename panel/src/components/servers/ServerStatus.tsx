"use client";

import React from "react";
import Badge from "@/components/ui/Badge";

interface ServerStatusProps {
  status: "running" | "stopped" | "starting" | "stopping" | "error";
  cpu?: number;
  memory?: number;
  memoryLimit?: number;
  disk?: number;
  diskLimit?: number;
  network?: { rx: number; tx: number };
  uptime?: number;
}

export default function ServerStatus({
  status,
  cpu = 0,
  memory = 0,
  memoryLimit = 0,
  disk = 0,
  diskLimit = 0,
  network = { rx: 0, tx: 0 },
  uptime = 0,
}: ServerStatusProps) {
  const formatBytes = (bytes: number) => {
    if (bytes === 0) return "0 B";
    const k = 1024;
    const sizes = ["B", "KB", "MB", "GB", "TB"];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + " " + sizes[i];
  };

  const formatUptime = (seconds: number) => {
    const days = Math.floor(seconds / 86400);
    const hours = Math.floor((seconds % 86400) / 3600);
    const mins = Math.floor((seconds % 3600) / 60);
    return `${days}d ${hours}h ${mins}m`;
  };

  const statusVariant = {
    running: "success" as const,
    stopped: "danger" as const,
    starting: "warning" as const,
    stopping: "warning" as const,
    error: "danger" as const,
  };

  const statusText = {
    running: "Running",
    stopped: "Stopped",
    starting: "Starting",
    stopping: "Stopping",
    error: "Error",
  };

  return (
    <div className="space-y-4">
      <div className="flex items-center justify-between">
        <span className="text-sm text-text-secondary">Status</span>
        <Badge variant={statusVariant[status]}>{statusText[status]}</Badge>
      </div>

      <div>
        <div className="flex items-center justify-between mb-1">
          <span className="text-sm text-text-secondary">CPU</span>
          <span className="text-sm text-text-primary">{cpu.toFixed(1)}%</span>
        </div>
        <div className="w-full bg-primary-200 rounded-full h-2">
          <div
            className={`h-2 rounded-full transition-all ${
              cpu > 80 ? "bg-accent-red" : cpu > 50 ? "bg-accent-yellow" : "bg-accent-blue"
            }`}
            style={{ width: `${Math.min(cpu, 100)}%` }}
          />
        </div>
      </div>

      <div>
        <div className="flex items-center justify-between mb-1">
          <span className="text-sm text-text-secondary">Memory</span>
          <span className="text-sm text-text-primary">
            {formatBytes(memory)} / {formatBytes(memoryLimit)}
          </span>
        </div>
        <div className="w-full bg-primary-200 rounded-full h-2">
          <div
            className="bg-accent-green h-2 rounded-full transition-all"
            style={{
              width: `${memoryLimit > 0 ? (memory / memoryLimit) * 100 : 0}%`,
            }}
          />
        </div>
      </div>

      <div>
        <div className="flex items-center justify-between mb-1">
          <span className="text-sm text-text-secondary">Disk</span>
          <span className="text-sm text-text-primary">
            {formatBytes(disk)} / {formatBytes(diskLimit)}
          </span>
        </div>
        <div className="w-full bg-primary-200 rounded-full h-2">
          <div
            className="bg-accent-mauve h-2 rounded-full transition-all"
            style={{
              width: `${diskLimit > 0 ? (disk / diskLimit) * 100 : 0}%`,
            }}
          />
        </div>
      </div>

      <div className="grid grid-cols-2 gap-4 pt-2">
        <div className="bg-primary rounded-lg p-3">
          <p className="text-xs text-text-muted mb-1">Network RX</p>
          <p className="text-sm font-medium text-text-primary">
            {formatBytes(network.rx)}
          </p>
        </div>
        <div className="bg-primary rounded-lg p-3">
          <p className="text-xs text-text-muted mb-1">Network TX</p>
          <p className="text-sm font-medium text-text-primary">
            {formatBytes(network.tx)}
          </p>
        </div>
      </div>

      {status === "running" && (
        <div className="pt-2">
          <p className="text-xs text-text-muted">Uptime</p>
          <p className="text-sm font-medium text-text-primary">
            {formatUptime(uptime)}
          </p>
        </div>
      )}
    </div>
  );
}
