"use client";

import React, { useEffect, useState } from "react";
import Sidebar from "@/components/layout/Sidebar";
import Header from "@/components/layout/Header";
import Button from "@/components/ui/Button";
import Modal from "@/components/ui/Modal";
import Input from "@/components/ui/Input";
import Badge from "@/components/ui/Badge";
import type { Node } from "@/types";

export default function AdminNodesPage() {
  const [nodes, setNodes] = useState<Node[]>([]);
  const [loading, setLoading] = useState(true);
  const [showCreateModal, setShowCreateModal] = useState(false);
  const [creating, setCreating] = useState(false);
  const [newNode, setNewNode] = useState({
    name: "",
    location_id: "",
    fqdn: "",
    memory: 8192,
    disk: 102400,
    cpu: 100,
  });

  useEffect(() => {
    fetchNodes();
  }, []);

  const fetchNodes = async () => {
    try {
      const response = await fetch("/api/admin/nodes");
      const data = await response.json();
      if (data.success) {
        setNodes(data.data);
      }
    } catch (error) {
      console.error("Failed to fetch nodes:", error);
    } finally {
      setLoading(false);
    }
  };

  const createNode = async (e: React.FormEvent) => {
    e.preventDefault();
    setCreating(true);

    try {
      const response = await fetch("/api/admin/nodes", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(newNode),
      });

      const data = await response.json();
      if (data.success) {
        setNodes((prev) => [data.data, ...prev]);
        setShowCreateModal(false);
        setNewNode({
          name: "",
          location_id: "",
          fqdn: "",
          memory: 8192,
          disk: 102400,
          cpu: 100,
        });
      }
    } catch (error) {
      console.error("Failed to create node:", error);
    } finally {
      setCreating(false);
    }
  };

  const deleteNode = async (nodeId: string) => {
    if (!confirm("Are you sure you want to delete this node?")) return;

    try {
      const response = await fetch(`/api/admin/nodes?id=${nodeId}`, {
        method: "DELETE",
      });

      if (response.ok) {
        setNodes((prev) => prev.filter((n) => n.id !== nodeId));
      }
    } catch (error) {
      console.error("Failed to delete node:", error);
    }
  };

  const formatBytes = (mb: number) => {
    if (mb >= 1024) return `${(mb / 1024).toFixed(1)} GB`;
    return `${mb} MB`;
  };

  return (
    <div className="flex h-screen bg-primary">
      <Sidebar isAdmin />
      <div className="flex-1 flex flex-col overflow-hidden">
        <Header
          title="Manage Nodes"
          actions={
            <Button onClick={() => setShowCreateModal(true)}>
              <svg className="w-4 h-4 mr-2" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 6v6m0 0v6m0-6h6m-6 0H6" />
              </svg>
              Add Node
            </Button>
          }
        />
        <main className="flex-1 overflow-y-auto p-6">
          <div className="space-y-6">
            <div>
              <h2 className="text-2xl font-bold text-text-primary">Nodes</h2>
              <p className="text-text-secondary mt-1">Manage server nodes</p>
            </div>

            {loading ? (
              <div className="flex items-center justify-center h-64">
                <div className="animate-spin w-8 h-8 border-2 border-accent-blue border-t-transparent rounded-full" />
              </div>
            ) : nodes.length === 0 ? (
              <div className="text-center py-8 text-text-muted">No nodes found</div>
            ) : (
              <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
                {nodes.map((node) => (
                  <div
                    key={node.id}
                    className="bg-secondary rounded-xl border border-primary-200 p-6"
                  >
                    <div className="flex items-start justify-between mb-4">
                      <div>
                        <h3 className="text-lg font-semibold text-text-primary">{node.name}</h3>
                        <p className="text-sm text-text-muted">{node.fqdn}</p>
                      </div>
                      <Badge variant="success">Online</Badge>
                    </div>

                    <div className="space-y-3">
                      <div>
                        <div className="flex items-center justify-between text-xs mb-1">
                          <span className="text-text-muted">Memory</span>
                          <span className="text-text-primary">
                            {formatBytes(node.memory)}
                          </span>
                        </div>
                        <div className="w-full bg-primary-200 rounded-full h-1.5">
                          <div
                            className="bg-accent-blue h-1.5 rounded-full"
                            style={{
                              width: `${((node.allocated_memory || 0) / node.memory) * 100}%`,
                            }}
                          />
                        </div>
                      </div>

                      <div>
                        <div className="flex items-center justify-between text-xs mb-1">
                          <span className="text-text-muted">Disk</span>
                          <span className="text-text-primary">
                            {formatBytes(node.disk)}
                          </span>
                        </div>
                        <div className="w-full bg-primary-200 rounded-full h-1.5">
                          <div
                            className="bg-accent-green h-1.5 rounded-full"
                            style={{
                              width: `${((node.allocated_disk || 0) / node.disk) * 100}%`,
                            }}
                          />
                        </div>
                      </div>

                      <div className="grid grid-cols-2 gap-2 pt-2">
                        <div className="bg-primary rounded-lg p-2">
                          <p className="text-xs text-text-muted">CPU</p>
                          <p className="text-sm font-medium text-text-primary">{node.cpu}%</p>
                        </div>
                        <div className="bg-primary rounded-lg p-2">
                          <p className="text-xs text-text-muted">Servers</p>
                          <p className="text-sm font-medium text-text-primary">
                            {node.servers_count || 0}
                          </p>
                        </div>
                      </div>
                    </div>

                    <div className="mt-4 pt-4 border-t border-primary-200">
                      <Button
                        variant="danger"
                        size="sm"
                        className="w-full"
                        onClick={() => deleteNode(node.id)}
                      >
                        Delete Node
                      </Button>
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
        title="Add Node"
      >
        <form onSubmit={createNode} className="space-y-4">
          <Input
            label="Node Name"
            placeholder="Node 1"
            value={newNode.name}
            onChange={(e) => setNewNode((prev) => ({ ...prev, name: e.target.value }))}
            required
          />
          <Input
            label="Location ID"
            placeholder="Enter location ID"
            value={newNode.location_id}
            onChange={(e) =>
              setNewNode((prev) => ({ ...prev, location_id: e.target.value }))
            }
            required
          />
          <Input
            label="FQDN"
            placeholder="node1.example.com"
            value={newNode.fqdn}
            onChange={(e) => setNewNode((prev) => ({ ...prev, fqdn: e.target.value }))}
            required
          />
          <div className="grid grid-cols-3 gap-4">
            <Input
              label="Memory (MB)"
              type="number"
              value={newNode.memory}
              onChange={(e) =>
                setNewNode((prev) => ({ ...prev, memory: parseInt(e.target.value) || 0 }))
              }
            />
            <Input
              label="Disk (MB)"
              type="number"
              value={newNode.disk}
              onChange={(e) =>
                setNewNode((prev) => ({ ...prev, disk: parseInt(e.target.value) || 0 }))
              }
            />
            <Input
              label="CPU (%)"
              type="number"
              value={newNode.cpu}
              onChange={(e) =>
                setNewNode((prev) => ({ ...prev, cpu: parseInt(e.target.value) || 0 }))
              }
            />
          </div>
          <div className="flex justify-end gap-3 pt-4">
            <Button
              type="button"
              variant="secondary"
              onClick={() => setShowCreateModal(false)}
            >
              Cancel
            </Button>
            <Button type="submit" loading={creating}>
              Add Node
            </Button>
          </div>
        </form>
      </Modal>
    </div>
  );
}
