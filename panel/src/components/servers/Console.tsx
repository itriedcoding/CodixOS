"use client";

import React, { useState, useRef, useEffect } from "react";
import type { ConsoleMessage } from "@/types";

interface ConsoleProps {
  serverId: string;
  isRunning: boolean;
}

export default function Console({ serverId, isRunning }: ConsoleProps) {
  const [messages, setMessages] = useState<ConsoleMessage[]>([]);
  const [command, setCommand] = useState("");
  const [connected, setConnected] = useState(false);
  const terminalRef = useRef<HTMLDivElement>(null);
  const inputRef = useRef<HTMLInputElement>(null);

  useEffect(() => {
    if (terminalRef.current) {
      terminalRef.current.scrollTop = terminalRef.current.scrollHeight;
    }
  }, [messages]);

  useEffect(() => {
    if (isRunning) {
      connectWebSocket();
    }
    return () => {
      setConnected(false);
    };
  }, [serverId, isRunning]);

  const connectWebSocket = () => {
    try {
      const protocol = window.location.protocol === "https:" ? "wss:" : "ws:";
      const token = document.cookie
        .split("; ")
        .find((row) => row.startsWith("token="))
        ?.split("=")[1];

      const ws = new WebSocket(
        `${protocol}//${window.location.host}/ws?token=${token}&serverId=${serverId}`
      );

      ws.onopen = () => {
        setConnected(true);
        addMessage("system", "Connected to console");
        ws.send(JSON.stringify({ type: "console:subscribe" }));
      };

      ws.onmessage = (event) => {
        const data = JSON.parse(event.data);
        if (data.type === "console:output") {
          addMessage(data.data.type || "stdout", data.data.message);
        }
      };

      ws.onclose = () => {
        setConnected(false);
        addMessage("system", "Disconnected from console");
      };
    } catch {
      addMessage("system", "Failed to connect to WebSocket");
    }
  };

  const addMessage = (type: ConsoleMessage["type"], message: string) => {
    setMessages((prev) => [
      ...prev,
      { type, message, timestamp: new Date() },
    ]);
  };

  const sendCommand = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!command.trim() || !isRunning) return;

    addMessage("stdout", `$ ${command}`);

    try {
      const response = await fetch(`/api/servers/${serverId}/command`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ command }),
      });

      const data = await response.json();
      if (data.success && data.data?.output) {
        addMessage("stdout", data.data.output);
      }
    } catch {
      addMessage("stderr", "Failed to send command");
    }

    setCommand("");
  };

  return (
    <div className="flex flex-col h-full bg-primary rounded-xl border border-primary-200 overflow-hidden">
      <div className="flex items-center justify-between px-4 py-3 border-b border-primary-200">
        <div className="flex items-center gap-2">
          <div
            className={`w-2 h-2 rounded-full ${
              connected ? "bg-accent-green" : "bg-accent-red"
            }`}
          />
          <span className="text-sm font-medium text-text-primary">Console</span>
        </div>
        <span className="text-xs text-text-muted">
          {connected ? "Connected" : "Disconnected"}
        </span>
      </div>

      <div
        ref={terminalRef}
        className="flex-1 overflow-y-auto p-4 font-mono text-sm"
      >
        {messages.map((msg, i) => (
          <div
            key={i}
            className={`py-0.5 ${
              msg.type === "system"
                ? "text-accent-yellow"
                : msg.type === "stderr"
                ? "text-accent-red"
                : "text-text-primary"
            }`}
          >
            <span className="text-text-muted text-xs mr-2">
              {msg.timestamp.toLocaleTimeString()}
            </span>
            <span className="whitespace-pre-wrap">{msg.message}</span>
          </div>
        ))}
        {messages.length === 0 && (
          <div className="text-text-muted text-center py-8">
            No console output yet
          </div>
        )}
      </div>

      <form
        onSubmit={sendCommand}
        className="flex items-center gap-2 px-4 py-3 border-t border-primary-200"
      >
        <span className="text-accent-green font-mono">$</span>
        <input
          ref={inputRef}
          type="text"
          value={command}
          onChange={(e) => setCommand(e.target.value)}
          placeholder={isRunning ? "Type a command..." : "Server is not running"}
          disabled={!isRunning}
          className="flex-1 bg-transparent text-text-primary font-mono text-sm focus:outline-none placeholder-text-muted disabled:opacity-50"
        />
      </form>
    </div>
  );
}
