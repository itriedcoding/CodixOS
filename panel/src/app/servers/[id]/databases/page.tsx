"use client";

import React, { useEffect, useState } from "react";
import { useParams, useRouter } from "next/navigation";
import Link from "next/link";
import Sidebar from "@/components/layout/Sidebar";
import Header from "@/components/layout/Header";
import Button from "@/components/ui/Button";
import Modal from "@/components/ui/Modal";
import Input from "@/components/ui/Input";
import Badge from "@/components/ui/Badge";
import type { Server, Database } from "@/types";

export default function ServerDatabasesPage() {
  const params = useParams();
  const router = useRouter();
  const serverId = params.id as string;
  const [server, setServer] = useState<Server | null>(null);
  const [databases, setDatabases] = useState<Database[]>([]);
  const [loading, setLoading] = useState(true);
  const [showCreateModal, setShowCreateModal] = useState(false);
  const [creating, setCreating] = useState(false);
  const [dbName, setDbName] = useState("");

  useEffect(() => {
    fetchServer();
    fetchDatabases();
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

  const fetchDatabases = async () => {
    try {
      const response = await fetch(`/api/servers/${serverId}/databases`);
      const data = await response.json();
      if (data.success) {
        setDatabases(data.data);
      }
    } catch (error) {
      console.error("Failed to fetch databases:", error);
    } finally {
      setLoading(false);
    }
  };

  const createDatabase = async (e: React.FormEvent) => {
    e.preventDefault();
    setCreating(true);

    try {
      const response = await fetch(`/api/servers/${serverId}/databases`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ name: dbName }),
      });

      const data = await response.json();
      if (data.success) {
        setDatabases((prev) => [data.data, ...prev]);
        setShowCreateModal(false);
        setDbName("");
      }
    } catch (error) {
      console.error("Failed to create database:", error);
    } finally {
      setCreating(false);
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
        <Header
          title={`${server.name} - Databases`}
          actions={
            <Button onClick={() => setShowCreateModal(true)}>
              <svg className="w-4 h-4 mr-2" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 6v6m0 0v6m0-6h6m-6 0H6" />
              </svg>
              New Database
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
                    tab.href === `/servers/${serverId}/databases`
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
            {databases.length === 0 ? (
              <div className="flex flex-col items-center justify-center h-64">
                <svg className="w-16 h-16 text-text-muted mb-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={1.5} d="M4 7v10c0 2.21 3.582 4 8 4s8-1.79 8-4V7M4 7c0 2.21 3.582 4 8 4s8-1.79 8-4M4 7c0-2.21 3.582-4 8-4s8 1.79 8 4m0 5c0 2.21-3.582 4-8 4s-8-1.79-8-4" />
                </svg>
                <h3 className="text-lg font-semibold text-text-primary mb-2">No databases</h3>
                <p className="text-text-muted mb-4">Create a database for this server</p>
                <Button onClick={() => setShowCreateModal(true)}>Create Database</Button>
              </div>
            ) : (
              <div className="bg-secondary rounded-xl border border-primary-200 overflow-hidden">
                <table className="w-full">
                  <thead>
                    <tr className="border-b border-primary-200">
                      <th className="text-left px-6 py-3 text-sm font-medium text-text-secondary">Name</th>
                      <th className="text-left px-6 py-3 text-sm font-medium text-text-secondary">Host</th>
                      <th className="text-left px-6 py-3 text-sm font-medium text-text-secondary">Port</th>
                      <th className="text-left px-6 py-3 text-sm font-medium text-text-secondary">Username</th>
                      <th className="text-left px-6 py-3 text-sm font-medium text-text-secondary">Created</th>
                    </tr>
                  </thead>
                  <tbody className="divide-y divide-primary-200">
                    {databases.map((db) => (
                      <tr key={db.id} className="hover:bg-primary transition-colors">
                        <td className="px-6 py-4 text-sm text-text-primary">{db.name}</td>
                        <td className="px-6 py-4 text-sm text-text-primary font-mono">{db.host}</td>
                        <td className="px-6 py-4 text-sm text-text-primary">{db.port}</td>
                        <td className="px-6 py-4 text-sm text-text-primary font-mono">{db.username}</td>
                        <td className="px-6 py-4 text-sm text-text-muted">
                          {new Date(db.created_at).toLocaleDateString()}
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

      <Modal
        isOpen={showCreateModal}
        onClose={() => setShowCreateModal(false)}
        title="Create Database"
      >
        <form onSubmit={createDatabase} className="space-y-4">
          <Input
            label="Database Name"
            placeholder="my_database"
            value={dbName}
            onChange={(e) => setDbName(e.target.value)}
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
              Create Database
            </Button>
          </div>
        </form>
      </Modal>
    </div>
  );
}
