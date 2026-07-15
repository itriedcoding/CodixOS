"use client";

import React from "react";

interface StatsCardProps {
  title: string;
  value: string | number;
  change?: string;
  changeType?: "increase" | "decrease" | "neutral";
  icon: React.ReactNode;
}

export default function StatsCard({
  title,
  value,
  change,
  changeType = "neutral",
  icon,
}: StatsCardProps) {
  const changeColors = {
    increase: "text-accent-green",
    decrease: "text-accent-red",
    neutral: "text-text-muted",
  };

  return (
    <div className="bg-secondary rounded-xl border border-primary-200 p-6">
      <div className="flex items-start justify-between">
        <div>
          <p className="text-sm text-text-secondary">{title}</p>
          <p className="text-2xl font-bold text-text-primary mt-1">{value}</p>
          {change && (
            <p className={`text-xs mt-1 ${changeColors[changeType]}`}>{change}</p>
          )}
        </div>
        <div className="w-10 h-10 bg-accent-blue/10 rounded-lg flex items-center justify-center text-accent-blue">
          {icon}
        </div>
      </div>
    </div>
  );
}
