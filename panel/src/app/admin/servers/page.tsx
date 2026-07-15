"use client";

import React, { useEffect, useState } from "react";
import Sidebar from "@/components/layout/Sidebar";
import Header from "@/components/layout/Header";
import Badge from "@/components/ui/Badge";
import type { Server } from "@/types";

export default function AdminServersPage() {
  const [servers, setServers] = useState<Server[]>([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    fetchServers();
  }, []);

  const fetchServers = async () => {
    try {
      const response = await fetch("/api/servers");
      const data = await response.json();
      if (data.success) {
        setServers(data.data);
      }
    } catch (error) {
      console.error("Failed to fetch servers:", error);
    } finally {
      setLoading(false);
    }
  };

  const statusVariant = {
    running: "success" as const,
    stopped: "danger" as const,
    starting: "warning" as const,
    stopping: "warning" as const,
    error: "danger" as const,
  };

  return (
    <div className="flex h-screen bg-primary">
      <Sidebar isAdmin />
      <div className="flex-1 flex flex-col overflow-hidden">
        <Header title="Manage Servers" />
        <main className="flex-1 overflow-y-auto p-6">
          <div className="space-y-6">
            <div>
              <h2 className="text-2xl font-bold text-text-primary">Servers</h2>
              <p className="text-text-secondary mt-1">View all servers across the panel</p>
            </div>

            {loading ? (
              <div className="flex items-center justify-center h-64">
                <div className="animate-spin w-8 h-8 border-2 border-accent-blue border-t-transparent rounded-full" />
              </div>
            ) : servers.length === 0 ? (
              <div className="text-center py-8 text-text-muted">No servers found</div>
            ) : (
              <div className="bg-secondary rounded-xl border border-primary-200 overflow-hidden">
                <table className="w-full">
                  <thead>
                    <tr className="border-b border-primary-200">
                      <th className="text-left px-6 py-3 text-sm font-medium text-text-secondary">Name</th>
                      <th className="text-left px-6 py-3 text-sm font-medium text-text-secondary">Node</th>
                      <th className="text-left px-6 py-3 text-sm font-medium text-text-secondary">Egg</th>
                      <th className="text-left px-6 py-3 text-sm font-medium text-text-secondary">Status</th>
                      <th className="text-left px-6 py-3 text-sm font-medium text-text-secondary">Created</th>
                    </tr>
                  </thead>
                  <tbody className="divide-y divide-primary-200">
                    {servers.map((server) => (
                      <tr key={server.id} className="hover:bg-primary transition-colors">
                        <td className="px-6 py-4">
                          <div>
                            <p className="text-sm font-medium text-text-primary">{server.name}</p>
                            <p className="text-xs text-text-muted">{server.id}</p>
                          </div>
                        </td>
                        <td className="px-6 py-4 text-sm text-text-primary">
                          {server.node?.name || "Unknown"}
                        </td>
                        <td className="px-6 py-4 text-sm text-text-primary">
                          {server.egg?.name || "Unknown"}
                        </td>
                        <td className="px-6 py-4">
                          <Badge variant={statusVariant[server.status || "stopped"]}>
                            {server.status || "stopped"}
                          </Badge>
                        </td>
                        <td className="px-6 py-4 text-sm text-text-muted">
                          {new Date(server.created_at).toLocaleDateString()}
                        </td>
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
            )}
          </div>
        </main>
      </div>
    </div>
  );
}
