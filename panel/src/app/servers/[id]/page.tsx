"use client";

import React, { useEffect, useState } from "react";
import { useParams, useRouter } from "next/navigation";
import Link from "next/link";
import Sidebar from "@/components/layout/Sidebar";
import Header from "@/components/layout/Header";
import ServerStatus from "@/components/servers/ServerStatus";
import Button from "@/components/ui/Button";
import Badge from "@/components/ui/Badge";
import type { Server, ServerStats } from "@/types";

export default function ServerOverviewPage() {
  const params = useParams();
  const router = useRouter();
  const serverId = params.id as string;
  const [server, setServer] = useState<Server | null>(null);
  const [stats, setStats] = useState<ServerStats | null>(null);
  const [loading, setLoading] = useState(true);
  const [powerAction, setPowerAction] = useState<string | null>(null);

  useEffect(() => {
    fetchServer();
    fetchStats();
  }, [serverId]);

  const fetchServer = async () => {
    try {
      const response = await fetch(`/api/servers/${serverId}`);
      const data = await response.json();
      if (data.success) {
        setServer(data.data);
      } else {
        router.push("/servers");
      }
    } catch (error) {
      console.error("Failed to fetch server:", error);
    } finally {
      setLoading(false);
    }
  };

  const fetchStats = async () => {
    try {
      const response = await fetch(`/api/servers/${serverId}/console`);
      const data = await response.json();
      if (data.success) {
        setStats(data.data);
      }
    } catch (error) {
      console.error("Failed to fetch stats:", error);
    }
  };

  const handlePowerAction = async (action: string) => {
    setPowerAction(action);
    try {
      const response = await fetch(`/api/servers/${serverId}/power`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ action }),
      });

      if (response.ok) {
        setTimeout(fetchServer, 1000);
      }
    } catch (error) {
      console.error("Power action failed:", error);
    } finally {
      setPowerAction(null);
    }
  };

  const tabs = [
    { label: "Overview", href: `/servers/${serverId}` },
    { label: "Console", href: `/servers/${serverId}/console` },
    { label: "Files", href: `/servers/${serverId}/files` },
    { label: "Databases", href: `/servers/${serverId}/databases` },
    { label: "Backups", href: `/servers/${serverId}/backups` },
    { label: "Settings", href: `/servers/${serverId}/settings` },
  ];

  if (loading) {
    return (
      <div className="flex h-screen bg-primary">
        <Sidebar />
        <div className="flex-1 flex items-center justify-center">
          <div className="animate-spin w-8 h-8 border-2 border-accent-blue border-t-transparent rounded-full" />
        </div>
      </div>
    );
  }

  if (!server) {
    return null;
  }

  return (
    <div className="flex h-screen bg-primary">
      <Sidebar />
      <div className="flex-1 flex flex-col overflow-hidden">
        <Header title={server.name} />
        <main className="flex-1 overflow-y-auto">
          <div className="border-b border-primary-200 px-6">
            <nav className="flex gap-1 -mb-px">
              {tabs.map((tab) => (
                <Link
                  key={tab.href}
                  href={tab.href}
                  className={`px-4 py-3 text-sm font-medium border-b-2 transition-colors ${
                    tab.href === `/servers/${serverId}`
                      ? "border-accent-blue text-accent-blue"
                      : "border-transparent text-text-secondary hover:text-text-primary hover:border-primary-300"
                  }`}
                >
                  {tab.label}
                </Link>
              ))}
            </nav>
          </div>

          <div className="p-6">
            <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
              <div className="lg:col-span-2">
                <div className="bg-secondary rounded-xl border border-primary-200 p-6">
                  <div className="flex items-center justify-between mb-6">
                    <h3 className="text-lg font-semibold text-text-primary">Server Details</h3>
                    <div className="flex items-center gap-2">
                      <Button
                        variant="secondary"
                        size="sm"
                        onClick={() => handlePowerAction("start")}
                        loading={powerAction === "start"}
                        disabled={server.status === "running"}
                      >
                        Start
                      </Button>
                      <Button
                        variant="secondary"
                        size="sm"
                        onClick={() => handlePowerAction("stop")}
                        loading={powerAction === "stop"}
                        disabled={server.status === "stopped"}
                      >
                        Stop
                      </Button>
                      <Button
                        variant="secondary"
                        size="sm"
                        onClick={() => handlePowerAction("restart")}
                        loading={powerAction === "restart"}
                        disabled={server.status === "stopped"}
                      >
                        Restart
                      </Button>
                    </div>
                  </div>

                  <div className="grid grid-cols-2 gap-4">
                    <div>
                      <p className="text-sm text-text-muted mb-1">Server ID</p>
                      <p className="text-sm text-text-primary font-mono">{server.id}</p>
                    </div>
                    <div>
                      <p className="text-sm text-text-muted mb-1">UUID</p>
                      <p className="text-sm text-text-primary font-mono">{server.uuid}</p>
                    </div>
                    <div>
                      <p className="text-sm text-text-muted mb-1">Node</p>
                      <p className="text-sm text-text-primary">{server.node?.name || "Unknown"}</p>
                    </div>
                    <div>
                      <p className="text-sm text-text-muted mb-1">Egg</p>
                      <p className="text-sm text-text-primary">{server.egg?.name || "Unknown"}</p>
                    </div>
                    <div>
                      <p className="text-sm text-text-muted mb-1">Docker Image</p>
                      <p className="text-sm text-text-primary font-mono">{server.image}</p>
                    </div>
                    <div>
                      <p className="text-sm text-text-muted mb-1">Created</p>
                      <p className="text-sm text-text-primary">
                        {new Date(server.created_at).toLocaleDateString()}
                      </p>
                    </div>
                  </div>
                </div>
              </div>

              <div>
                <div className="bg-secondary rounded-xl border border-primary-200 p-6">
                  <h3 className="text-lg font-semibold text-text-primary mb-4">Statistics</h3>
                  <ServerStatus
                    status={server.status || "stopped"}
                    cpu={stats?.cpu_percent}
                    memory={stats?.memory_usage}
                    memoryLimit={stats?.memory_limit}
                    uptime={stats?.uptime}
                  />
                </div>
              </div>
            </div>
          </div>
        </main>
      </div>
    </div>
  );
}
