"use client";

import React from "react";

interface InputProps extends React.InputHTMLAttributes<HTMLInputElement> {
  label?: string;
  error?: string;
  hint?: string;
}

export default function Input({
  label,
  error,
  hint,
  className = "",
  id,
  ...props
}: InputProps) {
  const inputId = id || label?.toLowerCase().replace(/\s+/g, "-");

  return (
    <div className="w-full">
      {label && (
        <label
          htmlFor={inputId}
          className="block text-sm font-medium text-text-primary mb-1.5"
        >
          {label}
        </label>
      )}
      <input
        id={inputId}
        className={`w-full px-3 py-2 bg-primary border rounded-lg text-text-primary placeholder-text-muted focus:outline-none focus:ring-2 focus:ring-accent-blue focus:border-transparent transition-colors ${
          error
            ? "border-accent-red focus:ring-accent-red"
            : "border-primary-300"
        } ${className}`}
        {...props}
      />
      {error && <p className="mt-1 text-sm text-accent-red">{error}</p>}
      {hint && !error && (
        <p className="mt-1 text-sm text-text-muted">{hint}</p>
      )}
    </div>
  );
}
