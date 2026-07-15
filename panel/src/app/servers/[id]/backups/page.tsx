"use client";

import React, { useEffect, useState } from "react";
import { useParams, useRouter } from "next/navigation";
import Link from "next/link";
import Sidebar from "@/components/layout/Sidebar";
import Header from "@/components/layout/Header";
import Button from "@/components/ui/Button";
import Modal from "@/components/ui/Modal";
import Input from "@/components/ui/Input";
import type { Server, Backup } from "@/types";

export default function ServerBackupsPage() {
  const params = useParams();
  const router = useRouter();
  const serverId = params.id as string;
  const [server, setServer] = useState<Server | null>(null);
  const [backups, setBackups] = useState<Backup[]>([]);
  const [loading, setLoading] = useState(true);
  const [showCreateModal, setShowCreateModal] = useState(false);
  const [creating, setCreating] = useState(false);
  const [backupName, setBackupName] = useState("");

  useEffect(() => {
    fetchServer();
    fetchBackups();
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
    }
  };

  const fetchBackups = async () => {
    try {
      const response = await fetch(`/api/servers/${serverId}/backups`);
      const data = await response.json();
      if (data.success) {
        setBackups(data.data);
      }
    } catch (error) {
      console.error("Failed to fetch backups:", error);
    } finally {
      setLoading(false);
    }
  };

  const createBackup = async (e: React.FormEvent) => {
    e.preventDefault();
    setCreating(true);

    try {
      const response = await fetch(`/api/servers/${serverId}/backups`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ name: backupName }),
      });

      const data = await response.json();
      if (data.success) {
        setBackups((prev) => [data.data, ...prev]);
        setShowCreateModal(false);
        setBackupName("");
      }
    } catch (error) {
      console.error("Failed to create backup:", error);
    } finally {
      setCreating(false);
    }
  };

  const formatSize = (bytes: number) => {
    if (bytes === 0) return "0 B";
    const k = 1024;
    const sizes = ["B", "KB", "MB", "GB"];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + " " + sizes[i];
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
        <Header
          title={`${server.name} - Backups`}
          actions={
            <Button onClick={() => setShowCreateModal(true)}>
              <svg className="w-4 h-4 mr-2" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 6v6m0 0v6m0-6h6m-6 0H6" />
              </svg>
              Create Backup
            </Button>
          }
        />
        <main className="flex-1 overflow-hidden flex flex-col">
          <div className="border-b border-primary-200 px-6">
            <nav className="flex gap-1 -mb-px">
              {tabs.map((tab) => (
                <Link
                  key={tab.href}
                  href={tab.href}
                  className={`px-4 py-3 text-sm font-medium border-b-2 transition-colors ${
                    tab.href === `/servers/${serverId}/backups`
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
            {backups.length === 0 ? (
              <div className="flex flex-col items-center justify-center h-64">
                <svg className="w-16 h-16 text-text-muted mb-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={1.5} d="M19 11H5m14 0a2 2 0 012 2v6a2 2 0 01-2 2H5a2 2 0 01-2-2v-6a2 2 0 012-2m14 0V9a2 2 0 00-2-2M5 11V9a2 2 0 012-2m0 0V5a2 2 0 012-2h6a2 2 0 012 2v2M7 7h10" />
                </svg>
                <h3 className="text-lg font-semibold text-text-primary mb-2">No backups</h3>
                <p className="text-text-muted mb-4">Create a backup for this server</p>
                <Button onClick={() => setShowCreateModal(true)}>Create Backup</Button>
              </div>
            ) : (
              <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
                {backups.map((backup) => (
                  <div
                    key={backup.id}
                    className="bg-secondary rounded-xl border border-primary-200 p-4"
                  >
                    <div className="flex items-start justify-between mb-3">
                      <div className="flex items-center gap-2">
                        <svg className="w-5 h-5 text-accent-blue" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M19 11H5m14 0a2 2 0 012 2v6a2 2 0 01-2 2H5a2 2 0 01-2-2v-6a2 2 0 012-2m14 0V9a2 2 0 00-2-2M5 11V9a2 2 0 012-2m0 0V5a2 2 0 012-2h6a2 2 0 012 2v2M7 7h10" />
                        </svg>
                        <span className="text-sm font-medium text-text-primary">{backup.name}</span>
                      </div>
                    </div>
                    <div className="space-y-2">
                      <div className="flex items-center justify-between text-xs">
                        <span className="text-text-muted">Size</span>
                        <span className="text-text-primary">{formatSize(Number(backup.size))}</span>
                      </div>
                      <div className="flex items-center justify-between text-xs">
                        <span className="text-text-muted">Created</span>
                        <span className="text-text-primary">
                          {new Date(backup.created_at).toLocaleDateString()}
                        </span>
                      </div>
                    </div>
                  </div>
                ))}
              </div>
            )}
          </div>
        </main>
      </div>

      <Modal
        isOpen={showCreateModal}
        onClose={() => setShowCreateModal(false)}
        title="Create Backup"
      >
        <form onSubmit={createBackup} className="space-y-4">
          <Input
            label="Backup Name"
            placeholder="backup_2024_01_01"
            value={backupName}
            onChange={(e) => setBackupName(e.target.value)}
            required
          />
          <div className="flex justify-end gap-3 pt-4">
            <Button
              type="button"
              variant="secondary"
              onClick={() => setShowCreateModal(false)}
            >
              Cancel
            </Button>
            <Button type="submit" loading={creating}>
              Create Backup
            </Button>
          </div>
        </form>
      </Modal>
    </div>
  );
}
