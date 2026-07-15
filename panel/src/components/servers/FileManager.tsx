"use client";

import React, { useState, useEffect } from "react";
import type { FileEntry } from "@/types";

interface FileManagerProps {
  serverId: string;
}

export default function FileManager({ serverId }: FileManagerProps) {
  const [files, setFiles] = useState<FileEntry[]>([]);
  const [currentPath, setCurrentPath] = useState("/");
  const [loading, setLoading] = useState(false);
  const [selectedFiles, setSelectedFiles] = useState<string[]>([]);

  useEffect(() => {
    loadFiles(currentPath);
  }, [serverId, currentPath]);

  const loadFiles = async (path: string) => {
    setLoading(true);
    try {
      const response = await fetch(
        `/api/servers/${serverId}/files?path=${encodeURIComponent(path)}`
      );
      const data = await response.json();
      if (data.success) {
        setFiles(data.data);
      }
    } catch {
      console.error("Failed to load files");
    } finally {
      setLoading(false);
    }
  };

  const navigateTo = (path: string) => {
    setCurrentPath(path);
    setSelectedFiles([]);
  };

  const goUp = () => {
    const parts = currentPath.split("/").filter(Boolean);
    parts.pop();
    navigateTo("/" + parts.join("/"));
  };

  const toggleSelect = (name: string) => {
    setSelectedFiles((prev) =>
      prev.includes(name) ? prev.filter((n) => n !== name) : [...prev, name]
    );
  };

  const formatSize = (bytes: number) => {
    if (bytes === 0) return "0 B";
    const k = 1024;
    const sizes = ["B", "KB", "MB", "GB", "TB"];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + " " + sizes[i];
  };

  return (
    <div className="flex flex-col h-full bg-primary rounded-xl border border-primary-200 overflow-hidden">
      <div className="flex items-center justify-between px-4 py-3 border-b border-primary-200">
        <div className="flex items-center gap-2">
          <svg
            className="w-5 h-5 text-accent-blue"
            fill="none"
            stroke="currentColor"
            viewBox="0 0 24 24"
          >
            <path
              strokeLinecap="round"
              strokeLinejoin="round"
              strokeWidth={2}
              d="M3 7v10a2 2 0 002 2h14a2 2 0 002-2V9a2 2 0 00-2-2h-6l-2-2H5a2 2 0 00-2 2z"
            />
          </svg>
          <span className="text-sm font-medium text-text-primary">File Manager</span>
        </div>
        <div className="flex items-center gap-2">
          <button
            onClick={() => loadFiles(currentPath)}
            className="px-3 py-1.5 text-xs font-medium text-text-secondary hover:text-text-primary bg-tertiary rounded-lg transition-colors"
          >
            Refresh
          </button>
          <button className="px-3 py-1.5 text-xs font-medium text-primary bg-accent-blue rounded-lg transition-colors">
            Upload
          </button>
        </div>
      </div>

      <div className="flex items-center gap-1 px-4 py-2 border-b border-primary-200 text-sm">
        <button
          onClick={() => navigateTo("/")}
          className="text-accent-blue hover:underline"
        >
          /
        </button>
        {currentPath
          .split("/")
          .filter(Boolean)
          .map((part, i, arr) => (
            <React.Fragment key={i}>
              <span className="text-text-muted">/</span>
              <button
                onClick={() =>
                  navigateTo("/" + arr.slice(0, i + 1).join("/"))
                }
                className="text-accent-blue hover:underline"
              >
                {part}
              </button>
            </React.Fragment>
          ))}
      </div>

      <div className="flex-1 overflow-y-auto">
        {loading ? (
          <div className="flex items-center justify-center h-32">
            <div className="animate-spin w-6 h-6 border-2 border-accent-blue border-t-transparent rounded-full" />
          </div>
        ) : files.length === 0 ? (
          <div className="text-center py-8 text-text-muted">No files in this directory</div>
        ) : (
          <div className="divide-y divide-primary-200">
            {currentPath !== "/" && (
              <button
                onClick={goUp}
                className="w-full flex items-center gap-3 px-4 py-3 hover:bg-tertiary transition-colors text-left"
              >
                <svg
                  className="w-5 h-5 text-text-muted"
                  fill="none"
                  stroke="currentColor"
                  viewBox="0 0 24 24"
                >
                  <path
                    strokeLinecap="round"
                    strokeLinejoin="round"
                    strokeWidth={2}
                    d="M15 19l-7-7 7-7"
                  />
                </svg>
                <span className="text-sm text-text-secondary">..</span>
              </button>
            )}
            {files.map((file) => (
              <button
                key={file.name}
                onClick={() =>
                  file.is_directory
                    ? navigateTo(
                        currentPath === "/"
                          ? `/${file.name}`
                          : `${currentPath}/${file.name}`
                      )
                    : toggleSelect(file.name)
                }
                className={`w-full flex items-center gap-3 px-4 py-3 hover:bg-tertiary transition-colors text-left ${
                  selectedFiles.includes(file.name) ? "bg-accent-blue/10" : ""
                }`}
              >
                <input
                  type="checkbox"
                  checked={selectedFiles.includes(file.name)}
                  onChange={() => toggleSelect(file.name)}
                  className="w-4 h-4 rounded border-primary-300 text-accent-blue focus:ring-accent-blue"
                  onClick={(e) => e.stopPropagation()}
                />
                {file.is_directory ? (
                  <svg
                    className="w-5 h-5 text-accent-yellow"
                    fill="currentColor"
                    viewBox="0 0 24 24"
                  >
                    <path d="M10 4H4c-1.1 0-1.99.9-1.99 2L2 18c0 1.1.9 2 2 2h16c1.1 0 2-.9 2-2V8c0-1.1-.9-2-2-2h-8l-2-2z" />
                  </svg>
                ) : (
                  <svg
                    className="w-5 h-5 text-text-muted"
                    fill="none"
                    stroke="currentColor"
                    viewBox="0 0 24 24"
                  >
                    <path
                      strokeLinecap="round"
                      strokeLinejoin="round"
                      strokeWidth={2}
                      d="M9 12h6m-6 4h6m2 5H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z"
                    />
                  </svg>
                )}
                <div className="flex-1 min-w-0">
                  <p className="text-sm text-text-primary truncate">{file.name}</p>
                </div>
                <span className="text-xs text-text-muted">
                  {file.is_directory ? "-" : formatSize(file.size)}
                </span>
              </button>
            ))}
          </div>
        )}
      </div>
    </div>
  );
}
