"use client";

import React, { useEffect, useState } from "react";
import { useParams, useRouter } from "next/navigation";
import Link from "next/link";
import Sidebar from "@/components/layout/Sidebar";
import Header from "@/components/layout/Header";
import Badge from "@/components/ui/Badge";
import type { Server, Schedule } from "@/types";

export default function ServerSchedulesPage() {
  const params = useParams();
  const router = useRouter();
  const serverId = params.id as string;
  const [server, setServer] = useState<Server | null>(null);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    fetchServer();
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

  const tabs = [
    { label: "Overview", href: `/servers/${serverId}` },
    { label: "Console", href: `/servers/${serverId}/console` },
    { label: "Files", href: `/servers/${serverId}/files` },
    { label: "Databases", href: `/servers/${serverId}/databases` },
    { label: "Schedules", href: `/servers/${serverId}/schedules` },
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
        <Header title={`${server.name} - Schedules`} />
        <main className="flex-1 overflow-hidden flex flex-col">
          <div className="border-b border-primary-200 px-6">
            <nav className="flex gap-1 -mb-px">
              {tabs.map((tab) => (
                <Link
                  key={tab.href}
                  href={tab.href}
                  className={`px-4 py-3 text-sm font-medium border-b-2 transition-colors ${
                    tab.href === `/servers/${serverId}/schedules`
                      ? "border-accent-blue text-accent-blue"
                      : "border-transparent text-text-secondary hover:text-text-primary hover:border-primary-300"
                  }`}
                >
                  {tab.label}
                </Link>
              ))}
            </nav>
          </div>

          <div className="flex-1 p-6 overflow-y-auto">
            <div className="flex flex-col items-center justify-center h-64">
              <svg className="w-16 h-16 text-text-muted mb-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={1.5} d="M12 8v4l3 3m6-3a9 9 0 11-18 0 9 9 0 0118 0z" />
              </svg>
              <h3 className="text-lg font-semibold text-text-primary mb-2">No schedules</h3>
              <p className="text-text-muted">Schedules coming soon</p>
            </div>
          </div>
        </main>
      </div>
    </div>
  );
}
