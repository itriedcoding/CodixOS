"use client";

import React from "react";
import Link from "next/link";

interface HeaderProps {
  title?: string;
  actions?: React.ReactNode;
}

export default function Header({ title, actions }: HeaderProps) {
  return (
    <header className="h-16 bg-secondary border-b border-primary-200 flex items-center justify-between px-6">
      <div className="flex items-center gap-4">
        {title && (
          <h1 className="text-xl font-semibold text-text-primary">{title}</h1>
        )}
      </div>

      <div className="flex items-center gap-4">
        {actions}
        <Link
          href="/api/auth/logout"
          className="text-text-muted hover:text-text-primary transition-colors"
        >
          <svg
            className="w-5 h-5"
            fill="none"
            stroke="currentColor"
            viewBox="0 0 24 24"
          >
            <path
              strokeLinecap="round"
              strokeLinejoin="round"
              strokeWidth={2}
              d="M17 16l4-4m0 0l-4-4m4 4H7m6 4v1a3 3 0 01-3 3H6a3 3 0 01-3-3V7a3 3 0 013-3h4a3 3 0 013 3v1"
            />
          </svg>
        </Link>
      </div>
    </header>
  );
}
