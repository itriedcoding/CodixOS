"use client";

import React, { useEffect, useState } from "react";
import Link from "next/link";
import Sidebar from "@/components/layout/Sidebar";
import Header from "@/components/layout/Header";
import StatsCard from "@/components/dashboard/StatsCard";
import ActivityFeed from "@/components/dashboard/ActivityFeed";
import Button from "@/components/ui/Button";
import type { Server, ActivityLog } from "@/types";

export default function DashboardPage() {
  const [servers, setServers] = useState<Server[]>([]);
  const [activities, setActivities] = useState<ActivityLog[]>([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    fetchData();
  }, []);

  const fetchData = async () => {
    try {
      const [serversRes, activitiesRes] = await Promise.all([
        fetch("/api/servers"),
        fetch("/api/admin/users").catch(() => ({ json: () => ({ success: false, data: { data: [] } }) })),
      ]);

      const serversData = await serversRes.json();
      if (serversData.success) {
        setServers(serversData.data);
      }
    } catch (error) {
      console.error("Failed to fetch dashboard data:", error);
    } finally {
      setLoading(false);
    }
  };

  const runningServers = servers.filter((s) => s.status === "running").length;
  const stoppedServers = servers.filter((s) => s.status === "stopped").length;

  return (
    <div className="flex h-screen bg-primary">
      <Sidebar />
      <div className="flex-1 flex flex-col overflow-hidden">
        <Header title="Dashboard" />
        <main className="flex-1 overflow-y-auto p-6">
          {loading ? (
            <div className="flex items-center justify-center h-64">
              <div className="animate-spin w-8 h-8 border-2 border-accent-blue border-t-transparent rounded-full" />
            </div>
          ) : (
            <div className="space-y-6">
              <div className="flex items-center justify-between">
                <div>
                  <h2 className="text-2xl font-bold text-text-primary">Dashboard</h2>
                  <p className="text-text-secondary mt-1">Welcome to Codix Panel</p>
                </div>
                <Link href="/servers">
                  <Button>Create Server</Button>
                </Link>
              </div>

              <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
                <StatsCard
                  title="Total Servers"
                  value={servers.length}
                  icon={
                    <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M5 12h14M5 12a2 2 0 01-2-2V6a2 2 0 012-2h14a2 2 0 012 2v4a2 2 0 01-2 2M5 12a2 2 0 00-2 2v4a2 2 0 002 2h14a2 2 0 002-2v-4a2 2 0 00-2-2m-2-4h.01M17 16h.01" />
                    </svg>
                  }
                />
                <StatsCard
                  title="Running"
                  value={runningServers}
                  change={`${runningServers} active`}
                  changeType="increase"
                  icon={
                    <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M5 13l4 4L19 7" />
                    </svg>
                  }
                />
                <StatsCard
                  title="Stopped"
                  value={stoppedServers}
                  change={`${stoppedServers} inactive`}
                  changeType="decrease"
                  icon={
                    <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
                    </svg>
                  }
                />
                <StatsCard
                  title="Account"
                  value="Active"
                  change="Member"
                  changeType="neutral"
                  icon={
                    <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M16 7a4 4 0 11-8 0 4 4 0 018 0zM12 14a7 7 0 00-7 7h14a7 7 0 00-7-7z" />
                    </svg>
                  }
                />
              </div>

              <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
                <div className="lg:col-span-2">
                  <ActivityFeed activities={activities} />
                </div>
                <div className="space-y-4">
                  <div className="bg-secondary rounded-xl border border-primary-200 p-6">
                    <h3 className="text-lg font-semibold text-text-primary mb-4">Quick Actions</h3>
                    <div className="space-y-2">
                      <Link href="/servers" className="block">
                        <Button variant="secondary" className="w-full justify-start">
                          <svg className="w-4 h-4 mr-2" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                            <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 6v6m0 0v6m0-6h6m-6 0H6" />
                          </svg>
                          Create Server
                        </Button>
                      </Link>
                      <Link href="/servers" className="block">
                        <Button variant="secondary" className="w-full justify-start">
                          <svg className="w-4 h-4 mr-2" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                            <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M4 6h16M4 10h16M4 14h16M4 18h16" />
                          </svg>
                          View Servers
                        </Button>
                      </Link>
                    </div>
                  </div>

                  <div className="bg-secondary rounded-xl border border-primary-200 p-6">
                    <h3 className="text-lg font-semibold text-text-primary mb-4">Server List</h3>
                    {servers.length === 0 ? (
                      <p className="text-text-muted text-sm text-center py-4">
                        No servers yet
                      </p>
                    ) : (
                      <div className="space-y-2">
                        {servers.slice(0, 5).map((server) => (
                          <Link
                            key={server.id}
                            href={`/servers/${server.id}`}
                            className="flex items-center gap-3 p-2 rounded-lg hover:bg-tertiary transition-colors"
                          >
                            <div
                              className={`w-2 h-2 rounded-full ${
                                server.status === "running" ? "bg-accent-green" : "bg-accent-red"
                              }`}
                            />
                            <span className="text-sm text-text-primary truncate">
                              {server.name}
                            </span>
                          </Link>
                        ))}
                      </div>
                    )}
                  </div>
                </div>
              </div>
            </div>
          )}
        </main>
      </div>
    </div>
  );
}
