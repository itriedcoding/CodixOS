"use client";

import React, { useEffect, useState } from "react";
import Sidebar from "@/components/layout/Sidebar";
import Header from "@/components/layout/Header";
import Button from "@/components/ui/Button";
import Badge from "@/components/ui/Badge";
import Input from "@/components/ui/Input";
import type { User, PaginatedResponse } from "@/types";

export default function AdminUsersPage() {
  const [users, setUsers] = useState<PaginatedResponse<User> | null>(null);
  const [loading, setLoading] = useState(true);
  const [search, setSearch] = useState("");

  useEffect(() => {
    fetchUsers();
  }, [search]);

  const fetchUsers = async () => {
    setLoading(true);
    try {
      const params = new URLSearchParams();
      if (search) params.set("search", search);

      const response = await fetch(`/api/admin/users?${params}`);
      const data = await response.json();
      if (data.success) {
        setUsers(data.data);
      }
    } catch (error) {
      console.error("Failed to fetch users:", error);
    } finally {
      setLoading(false);
    }
  };

  const toggleAdmin = async (userId: string, currentAdmin: boolean) => {
    try {
      const response = await fetch("/api/admin/users", {
        method: "PATCH",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ userId, root_admin: !currentAdmin }),
      });

      if (response.ok) {
        fetchUsers();
      }
    } catch (error) {
      console.error("Failed to update user:", error);
    }
  };

  const deleteUser = async (userId: string) => {
    if (!confirm("Are you sure you want to delete this user?")) return;

    try {
      const response = await fetch(`/api/admin/users?id=${userId}`, {
        method: "DELETE",
      });

      if (response.ok) {
        fetchUsers();
      }
    } catch (error) {
      console.error("Failed to delete user:", error);
    }
  };

  return (
    <div className="flex h-screen bg-primary">
      <Sidebar isAdmin />
      <div className="flex-1 flex flex-col overflow-hidden">
        <Header title="Manage Users" />
        <main className="flex-1 overflow-y-auto p-6">
          <div className="space-y-6">
            <div className="flex items-center justify-between">
              <div>
                <h2 className="text-2xl font-bold text-text-primary">Users</h2>
                <p className="text-text-secondary mt-1">Manage user accounts</p>
              </div>
              <div className="w-64">
                <Input
                  placeholder="Search users..."
                  value={search}
                  onChange={(e) => setSearch(e.target.value)}
                />
              </div>
            </div>

            {loading ? (
              <div className="flex items-center justify-center h-64">
                <div className="animate-spin w-8 h-8 border-2 border-accent-blue border-t-transparent rounded-full" />
              </div>
            ) : !users || users.data.length === 0 ? (
              <div className="text-center py-8 text-text-muted">No users found</div>
            ) : (
              <div className="bg-secondary rounded-xl border border-primary-200 overflow-hidden">
                <table className="w-full">
                  <thead>
                    <tr className="border-b border-primary-200">
                      <th className="text-left px-6 py-3 text-sm font-medium text-text-secondary">User</th>
                      <th className="text-left px-6 py-3 text-sm font-medium text-text-secondary">Email</th>
                      <th className="text-left px-6 py-3 text-sm font-medium text-text-secondary">Role</th>
                      <th className="text-left px-6 py-3 text-sm font-medium text-text-secondary">Servers</th>
                      <th className="text-left px-6 py-3 text-sm font-medium text-text-secondary">Created</th>
                      <th className="text-right px-6 py-3 text-sm font-medium text-text-secondary">Actions</th>
                    </tr>
                  </thead>
                  <tbody className="divide-y divide-primary-200">
                    {users.data.map((user) => (
                      <tr key={user.id} className="hover:bg-primary transition-colors">
                        <td className="px-6 py-4">
                          <div className="flex items-center gap-3">
                            <div className="w-8 h-8 bg-accent-mauve rounded-full flex items-center justify-center">
                              <span className="text-primary font-medium text-sm">
                                {user.username.charAt(0).toUpperCase()}
                              </span>
                            </div>
                            <span className="text-sm text-text-primary">{user.username}</span>
                          </div>
                        </td>
                        <td className="px-6 py-4 text-sm text-text-secondary">{user.email}</td>
                        <td className="px-6 py-4">
                          <Badge variant={user.root_admin ? "warning" : "default"}>
                            {user.root_admin ? "Admin" : "User"}
                          </Badge>
                        </td>
                        <td className="px-6 py-4 text-sm text-text-primary">
                          {user._count?.servers || 0}
                        </td>
                        <td className="px-6 py-4 text-sm text-text-muted">
                          {new Date(user.created_at).toLocaleDateString()}
                        </td>
                        <td className="px-6 py-4 text-right">
                          <div className="flex items-center justify-end gap-2">
                            <Button
                              variant="ghost"
                              size="sm"
                              onClick={() => toggleAdmin(user.id, user.root_admin)}
                            >
                              {user.root_admin ? "Demote" : "Promote"}
                            </Button>
                            <Button
                              variant="danger"
                              size="sm"
                              onClick={() => deleteUser(user.id)}
                            >
                              Delete
                            </Button>
                          </div>
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
