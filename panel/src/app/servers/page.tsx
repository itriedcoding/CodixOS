"use client";

import React, { useEffect, useState } from "react";
import Sidebar from "@/components/layout/Sidebar";
import Header from "@/components/layout/Header";
import ServerCard from "@/components/servers/ServerCard";
import Button from "@/components/ui/Button";
import Modal from "@/components/ui/Modal";
import Input from "@/components/ui/Input";
import type { Server } from "@/types";

export default function ServersPage() {
  const [servers, setServers] = useState<Server[]>([]);
  const [loading, setLoading] = useState(true);
  const [showCreateModal, setShowCreateModal] = useState(false);
  const [creating, setCreating] = useState(false);
  const [newServer, setNewServer] = useState({
    name: "",
    egg_id: "",
    node_id: "",
  });

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

  const createServer = async (e: React.FormEvent) => {
    e.preventDefault();
    setCreating(true);

    try {
      const response = await fetch("/api/servers", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(newServer),
      });

      const data = await response.json();
      if (data.success) {
        setServers((prev) => [data.data, ...prev]);
        setShowCreateModal(false);
        setNewServer({ name: "", egg_id: "", node_id: "" });
      }
    } catch (error) {
      console.error("Failed to create server:", error);
    } finally {
      setCreating(false);
    }
  };

  return (
    <div className="flex h-screen bg-primary">
      <Sidebar />
      <div className="flex-1 flex flex-col overflow-hidden">
        <Header
          title="Servers"
          actions={
            <Button onClick={() => setShowCreateModal(true)}>
              <svg className="w-4 h-4 mr-2" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 6v6m0 0v6m0-6h6m-6 0H6" />
              </svg>
              New Server
            </Button>
          }
        />
        <main className="flex-1 overflow-y-auto p-6">
          {loading ? (
            <div className="flex items-center justify-center h-64">
              <div className="animate-spin w-8 h-8 border-2 border-accent-blue border-t-transparent rounded-full" />
            </div>
          ) : servers.length === 0 ? (
            <div className="flex flex-col items-center justify-center h-64">
              <svg className="w-16 h-16 text-text-muted mb-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={1.5} d="M5 12h14M5 12a2 2 0 01-2-2V6a2 2 0 012-2h14a2 2 0 012 2v4a2 2 0 01-2 2M5 12a2 2 0 00-2 2v4a2 2 0 002 2h14a2 2 0 002-2v-4a2 2 0 00-2-2m-2-4h.01M17 16h.01" />
              </svg>
              <h3 className="text-lg font-semibold text-text-primary mb-2">No servers</h3>
              <p className="text-text-muted mb-4">Create your first server to get started</p>
              <Button onClick={() => setShowCreateModal(true)}>Create Server</Button>
            </div>
          ) : (
            <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
              {servers.map((server) => (
                <ServerCard key={server.id} server={server} />
              ))}
            </div>
          )}
        </main>
      </div>

      <Modal
        isOpen={showCreateModal}
        onClose={() => setShowCreateModal(false)}
        title="Create Server"
      >
        <form onSubmit={createServer} className="space-y-4">
          <Input
            label="Server Name"
            placeholder="My Server"
            value={newServer.name}
            onChange={(e) => setNewServer((prev) => ({ ...prev, name: e.target.value }))}
            required
          />
          <Input
            label="Egg ID"
            placeholder="Enter egg ID"
            value={newServer.egg_id}
            onChange={(e) => setNewServer((prev) => ({ ...prev, egg_id: e.target.value }))}
            required
          />
          <Input
            label="Node ID"
            placeholder="Enter node ID"
            value={newServer.node_id}
            onChange={(e) => setNewServer((prev) => ({ ...prev, node_id: e.target.value }))}
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
              Create Server
            </Button>
          </div>
        </form>
      </Modal>
    </div>
  );
}
