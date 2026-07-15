"use client";

import React, { useEffect, useState } from "react";
import Link from "next/link";
import Sidebar from "@/components/layout/Sidebar";
import Header from "@/components/layout/Header";
import StatsCard from "@/components/dashboard/StatsCard";

export default function AdminPage() {
  const [stats, setStats] = useState({
    users: 0,
    servers: 0,
    nodes: 0,
  });
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    fetchStats();
  }, []);

  const fetchStats = async () => {
    try {
      const [usersRes, serversRes, nodesRes] = await Promise.all([
        fetch("/api/admin/users"),
        fetch("/api/servers"),
        fetch("/api/admin/nodes"),
      ]);

      const usersData = await usersRes.json();
      const serversData = await serversRes.json();
      const nodesData = await nodesRes.json();

      setStats({
        users: usersData.success ? usersData.data.total : 0,
        servers: serversData.success ? serversData.data.length : 0,
        nodes: nodesData.success ? nodesData.data.length : 0,
      });
    } catch (error) {
      console.error("Failed to fetch admin stats:", error);
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="flex h-screen bg-primary">
      <Sidebar isAdmin />
      <div className="flex-1 flex flex-col overflow-hidden">
        <Header title="Admin Dashboard" />
        <main className="flex-1 overflow-y-auto p-6">
          {loading ? (
            <div className="flex items-center justify-center h-64">
              <div className="animate-spin w-8 h-8 border-2 border-accent-blue border-t-transparent rounded-full" />
            </div>
          ) : (
            <div className="space-y-6">
              <div>
                <h2 className="text-2xl font-bold text-text-primary">Administration</h2>
                <p className="text-text-secondary mt-1">Manage your Codix Panel instance</p>
              </div>

              <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
                <StatsCard
                  title="Users"
                  value={stats.users}
                  icon={
                    <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 4.354a4 4 0 110 5.292M15 21H3v-1a6 6 0 0112 0v1zm0 0h6v-1a6 6 0 00-9-5.197m13.5-9a2.5 2.5 0 11-5 0 2.5 2.5 0 015 0z" />
                    </svg>
                  }
                />
                <StatsCard
                  title="Servers"
                  value={stats.servers}
                  icon={
                    <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M5 12h14M5 12a2 2 0 01-2-2V6a2 2 0 012-2h14a2 2 0 012 2v4a2 2 0 01-2 2M5 12a2 2 0 00-2 2v4a2 2 0 002 2h14a2 2 0 002-2v-4a2 2 0 00-2-2m-2-4h.01M17 16h.01" />
                    </svg>
                  }
                />
                <StatsCard
                  title="Nodes"
                  value={stats.nodes}
                  icon={
                    <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M19 11H5m14 0a2 2 0 012 2v6a2 2 0 01-2 2H5a2 2 0 01-2-2v-6a2 2 0 012-2m14 0V9a2 2 0 00-2-2M5 11V9a2 2 0 012-2m0 0V5a2 2 0 012-2h6a2 2 0 012 2v2M7 7h10" />
                    </svg>
                  }
                />
              </div>

              <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
                <Link
                  href="/admin/users"
                  className="bg-secondary rounded-xl border border-primary-200 p-6 hover:border-accent-blue/50 transition-colors"
                >
                  <div className="flex items-center gap-3">
                    <div className="w-10 h-10 bg-accent-blue/10 rounded-lg flex items-center justify-center">
                      <svg className="w-5 h-5 text-accent-blue" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 4.354a4 4 0 110 5.292M15 21H3v-1a6 6 0 0112 0v1zm0 0h6v-1a6 6 0 00-9-5.197m13.5-9a2.5 2.5 0 11-5 0 2.5 2.5 0 015 0z" />
                      </svg>
                    </div>
                    <div>
                      <h3 className="text-sm font-semibold text-text-primary">Manage Users</h3>
                      <p className="text-xs text-text-muted">View and manage user accounts</p>
                    </div>
                  </div>
                </Link>

                <Link
                  href="/admin/servers"
                  className="bg-secondary rounded-xl border border-primary-200 p-6 hover:border-accent-blue/50 transition-colors"
                >
                  <div className="flex items-center gap-3">
                    <div className="w-10 h-10 bg-accent-green/10 rounded-lg flex items-center justify-center">
                      <svg className="w-5 h-5 text-accent-green" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M5 12h14M5 12a2 2 0 01-2-2V6a2 2 0 012-2h14a2 2 0 012 2v4a2 2 0 01-2 2M5 12a2 2 0 00-2 2v4a2 2 0 002 2h14a2 2 0 002-2v-4a2 2 0 00-2-2m-2-4h.01M17 16h.01" />
                      </svg>
                    </div>
                    <div>
                      <h3 className="text-sm font-semibold text-text-primary">Manage Servers</h3>
                      <p className="text-xs text-text-muted">View and manage all servers</p>
                    </div>
                  </div>
                </Link>

                <Link
                  href="/admin/nodes"
                  className="bg-secondary rounded-xl border border-primary-200 p-6 hover:border-accent-blue/50 transition-colors"
                >
                  <div className="flex items-center gap-3">
                    <div className="w-10 h-10 bg-accent-mauve/10 rounded-lg flex items-center justify-center">
                      <svg className="w-5 h-5 text-accent-mauve" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M19 11H5m14 0a2 2 0 012 2v6a2 2 0 01-2 2H5a2 2 0 01-2-2v-6a2 2 0 012-2m14 0V9a2 2 0 00-2-2M5 11V9a2 2 0 012-2m0 0V5a2 2 0 012-2h6a2 2 0 012 2v2M7 7h10" />
                      </svg>
                    </div>
                    <div>
                      <h3 className="text-sm font-semibold text-text-primary">Manage Nodes</h3>
                      <p className="text-xs text-text-muted">Configure server nodes</p>
                    </div>
                  </div>
                </Link>
              </div>
            </div>
          )}
        </main>
      </div>
    </div>
  );
}
