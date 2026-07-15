"use client";

import React from "react";
import type { ActivityLog } from "@/types";

interface ActivityFeedProps {
  activities: ActivityLog[];
}

export default function ActivityFeed({ activities }: ActivityFeedProps) {
  const getEventIcon = (event: string) => {
    if (event.includes("server")) {
      return (
        <svg className="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">
          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M5 12h14M5 12a2 2 0 01-2-2V6a2 2 0 012-2h14a2 2 0 012 2v4a2 2 0 01-2 2M5 12a2 2 0 00-2 2v4a2 2 0 002 2h14a2 2 0 002-2v-4a2 2 0 00-2-2m-2-4h.01M17 16h.01" />
        </svg>
      );
    }
    if (event.includes("user") || event.includes("login")) {
      return (
        <svg className="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">
          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M16 7a4 4 0 11-8 0 4 4 0 018 0zM12 14a7 7 0 00-7 7h14a7 7 0 00-7-7z" />
        </svg>
      );
    }
    return (
      <svg className="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">
        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M13 16h-1v-4h-1m1-4h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
      </svg>
    );
  };

  return (
    <div className="bg-secondary rounded-xl border border-primary-200 p-6">
      <h3 className="text-lg font-semibold text-text-primary mb-4">Recent Activity</h3>
      {activities.length === 0 ? (
        <p className="text-text-muted text-sm text-center py-4">No recent activity</p>
      ) : (
        <div className="space-y-3">
          {activities.slice(0, 10).map((activity) => (
            <div
              key={activity.id}
              className="flex items-start gap-3 p-3 rounded-lg bg-primary hover:bg-primary-200 transition-colors"
            >
              <div className="w-8 h-8 rounded-full bg-accent-blue/10 flex items-center justify-center text-accent-blue flex-shrink-0">
                {getEventIcon(activity.event)}
              </div>
              <div className="flex-1 min-w-0">
                <p className="text-sm text-text-primary">{activity.event}</p>
                <p className="text-xs text-text-muted">
                  {new Date(activity.created_at).toLocaleString()}
                </p>
              </div>
            </div>
          ))}
        </div>
      )}
    </div>
  );
}
