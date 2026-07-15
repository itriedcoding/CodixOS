"use client";

import React from "react";

export default function Footer() {
  return (
    <footer className="bg-secondary border-t border-primary-200 py-4 px-6">
      <div className="flex items-center justify-between">
        <p className="text-sm text-text-muted">
          Codix Panel v1.0.0
        </p>
        <p className="text-sm text-text-muted">
          Open-source game server management
        </p>
      </div>
    </footer>
  );
}
