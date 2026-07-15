"use client";

import React, { useEffect, useState } from "react";
import { useParams, useRouter } from "next/navigation";
import Link from "next/link";
import Sidebar from "@/components/layout/Sidebar";
import Header from "@/components/layout/Header";
import Button from "@/components/ui/Button";
import Input from "@/components/ui/Input";
import Modal from "@/components/ui/Modal";
import type { Server } from "@/types";

export default function ServerSettingsPage() {
  const params = useParams();
  const router = useRouter();
  const serverId = params.id as string;
  const [server, setServer] = useState<Server | null>(null);
  const [loading, setLoading] = useState(true);
  const [saving, setSaving] = useState(false);
  const [showDeleteModal, setShowDeleteModal] = useState(false);
  const [deleting, setDeleting] = useState(false);
  const [formData, setFormData] = useState({
    name: "",
    startup: "",
  });

  useEffect(() => {
    fetchServer();
  }, [serverId]);

  const fetchServer = async () => {
    try {
      const response = await fetch(`/api/servers/${serverId}`);
      const data = await response.json();
      if (data.success) {
        setServer(data.data);
        setFormData({
          name: data.data.name,
          startup: data.data.startup,
        });
      } else {
        router.push("/servers");
      }
    } catch (error) {
      console.error("Failed to fetch server:", error);
    } finally {
      setLoading(false);
    }
  };

  const handleSave = async (e: React.FormEvent) => {
    e.preventDefault();
    setSaving(true);

    try {
      const response = await fetch(`/api/servers/${serverId}`, {
        method: "PATCH",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(formData),
      });

      if (response.ok) {
        const data = await response.json();
        if (data.success) {
          setServer(data.data);
        }
      }
    } catch (error) {
      console.error("Failed to save server:", error);
    } finally {
      setSaving(false);
    }
  };

  const handleDelete = async () => {
    setDeleting(true);

    try {
      const response = await fetch(`/api/servers/${serverId}`, {
        method: "DELETE",
      });

      if (response.ok) {
        router.push("/servers");
      }
    } catch (error) {
      console.error("Failed to delete server:", error);
    } finally {
      setDeleting(false);
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
        <Header title={`${server.name} - Settings`} />
        <main className="flex-1 overflow-hidden flex flex-col">
          <div className="border-b border-primary-200 px-6">
            <nav className="flex gap-1 -mb-px">
              {tabs.map((tab) => (
                <Link
                  key={tab.href}
                  href={tab.href}
                  className={`px-4 py-3 text-sm font-medium border-b-2 transition-colors ${
                    tab.href === `/servers/${serverId}/settings`
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
            <div className="max-w-2xl space-y-6">
              <div className="bg-secondary rounded-xl border border-primary-200 p-6">
                <h3 className="text-lg font-semibold text-text-primary mb-4">General Settings</h3>
                <form onSubmit={handleSave} className="space-y-4">
                  <Input
                    label="Server Name"
                    value={formData.name}
                    onChange={(e) =>
                      setFormData((prev) => ({ ...prev, name: e.target.value }))
                    }
                    required
                  />
                  <Input
                    label="Startup Command"
                    value={formData.startup}
                    onChange={(e) =>
                      setFormData((prev) => ({ ...prev, startup: e.target.value }))
                    }
                  />
                  <div className="flex justify-end">
                    <Button type="submit" loading={saving}>
                      Save Changes
                    </Button>
                  </div>
                </form>
              </div>

              <div className="bg-secondary rounded-xl border border-accent-red/20 p-6">
                <h3 className="text-lg font-semibold text-accent-red mb-2">Danger Zone</h3>
                <p className="text-sm text-text-secondary mb-4">
                  Deleting this server will permanently remove all data. This action cannot be
                  undone.
                </p>
                <Button
                  variant="danger"
                  onClick={() => setShowDeleteModal(true)}
                >
                  Delete Server
                </Button>
              </div>
            </div>
          </div>
        </main>
      </div>

      <Modal
        isOpen={showDeleteModal}
        onClose={() => setShowDeleteModal(false)}
        title="Delete Server"
      >
        <div className="space-y-4">
          <p className="text-text-secondary">
            Are you sure you want to delete <strong>{server.name}</strong>? This will permanently
            remove all server data including files, databases, and backups.
          </p>
          <div className="flex justify-end gap-3">
            <Button
              variant="secondary"
              onClick={() => setShowDeleteModal(false)}
            >
              Cancel
            </Button>
            <Button
              variant="danger"
              onClick={handleDelete}
              loading={deleting}
            >
              Delete Server
            </Button>
          </div>
        </div>
      </Modal>
    </div>
  );
}
