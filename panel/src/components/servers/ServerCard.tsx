"use client";

import React from "react";
import Link from "next/link";
import Badge from "@/components/ui/Badge";
import type { Server } from "@/types";

interface ServerCardProps {
  server: Server;
}

export default function ServerCard({ server }: ServerCardProps) {
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

  const status = server.status || "stopped";

  return (
    <Link href={`/servers/${server.id}`}>
      <div className="bg-secondary rounded-xl border border-primary-200 p-6 hover:border-accent-blue/50 transition-all cursor-pointer group">
        <div className="flex items-start justify-between mb-4">
          <div className="flex items-center gap-3">
            <div
              className={`w-10 h-10 rounded-lg flex items-center justify-center ${
                status === "running"
                  ? "bg-accent-green/10"
                  : status === "error"
                  ? "bg-accent-red/10"
                  : "bg-tertiary"
              }`}
            >
              <svg
                className={`w-5 h-5 ${
                  status === "running"
                    ? "text-accent-green"
                    : status === "error"
                    ? "text-accent-red"
                    : "text-text-muted"
                }`}
                fill="none"
                stroke="currentColor"
                viewBox="0 0 24 24"
              >
                <path
                  strokeLinecap="round"
                  strokeLinejoin="round"
                  strokeWidth={2}
                  d="M5 12h14M5 12a2 2 0 01-2-2V6a2 2 0 012-2h14a2 2 0 012 2v4a2 2 0 01-2 2M5 12a2 2 0 00-2 2v4a2 2 0 002 2h14a2 2 0 002-2v-4a2 2 0 00-2-2m-2-4h.01M17 16h.01"
                />
              </svg>
            </div>
            <div>
              <h3 className="text-sm font-semibold text-text-primary group-hover:text-accent-blue transition-colors">
                {server.name}
              </h3>
              <p className="text-xs text-text-muted">{server.egg?.name || "Unknown Egg"}</p>
            </div>
          </div>
          <Badge variant={statusVariant[status]}>{statusText[status]}</Badge>
        </div>

        <div className="grid grid-cols-2 gap-4">
          <div>
            <p className="text-xs text-text-muted mb-1">CPU</p>
            <div className="w-full bg-primary-200 rounded-full h-1.5">
              <div
                className="bg-accent-blue h-1.5 rounded-full transition-all"
                style={{ width: `${Math.min(0, 100)}%` }}
              />
            </div>
          </div>
          <div>
            <p className="text-xs text-text-muted mb-1">Memory</p>
            <div className="w-full bg-primary-200 rounded-full h-1.5">
              <div
                className="bg-accent-green h-1.5 rounded-full transition-all"
                style={{ width: `${Math.min(0, 100)}%` }}
              />
            </div>
          </div>
        </div>

        <div className="mt-4 flex items-center justify-between text-xs text-text-muted">
          <span>{server.node?.name || "Unknown Node"}</span>
          <span>Created {new Date(server.created_at).toLocaleDateString()}</span>
        </div>
      </div>
    </Link>
  );
}
