'use client'

import { useState } from 'react'

/* ── SVG Icons ────────────────────────────────────────── */

function Icon({ d, size = 24, fill = 'none', stroke = 'currentColor', sw = 2, className = '' }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill={fill} stroke={stroke} strokeWidth={sw} strokeLinecap="round" strokeLinejoin="round" className={className}>
      <path d={d} />
    </svg>
  )
}

const Ico = {
  Server: (p) => <Icon size={p.s || 24} className={p.c} d="M2 2h20v20H2zM6 6h12M6 10h12M6 14h8M6 18h8" />,
  Docker: (p) => (
    <svg width={p.s || 24} height={p.s || 24} viewBox="0 0 24 24" fill="currentColor" className={p.c}>
      <path d="M13.983 11.078h2.119a.186.186 0 00.186-.185V9.006a.186.186 0 00-.186-.186h-2.119a.185.185 0 00-.185.186v1.887c0 .102.083.185.185.185m-2.954-5.43h2.118a.186.186 0 00.186-.186V3.574a.186.186 0 00-.186-.185h-2.118a.185.185 0 00-.185.185v1.888c0 .102.082.185.185.186m0 2.716h2.118a.187.187 0 00.186-.186V6.29a.186.186 0 00-.186-.185h-2.118a.185.185 0 00-.185.185v1.887c0 .102.082.186.185.186m-2.93 0h2.12a.186.186 0 00.184-.186V6.29a.185.185 0 00-.185-.185H8.1a.185.185 0 00-.185.185v1.887c0 .102.083.186.185.186m-2.964 0h2.119a.186.186 0 00.185-.186V6.29a.186.186 0 00-.185-.185H5.136a.186.186 0 00-.186.185v1.887c0 .102.084.186.186.186m5.893 2.715h2.118a.186.186 0 00.186-.185V9.006a.186.186 0 00-.186-.186h-2.118a.185.185 0 00-.185.186v1.887c0 .102.082.185.185.185m-2.93 0h2.12a.185.185 0 00.184-.185V9.006a.185.185 0 00-.184-.186h-2.12a.185.185 0 00-.184.186v1.887c0 .102.083.185.185.185m-2.964 0h2.119a.185.185 0 00.185-.185V9.006a.186.186 0 00-.185-.186H5.136a.186.186 0 00-.186.186v1.887c0 .102.084.185.186.185m-2.92 0h2.12a.185.185 0 00.184-.185V9.006a.185.185 0 00-.184-.186h-2.12a.185.185 0 00-.184.186v1.887c0 .102.082.185.185.185M23.763 9.89c-.065-.051-.672-.51-1.954-.51-.338.001-.676.03-1.01.087-.248-1.7-1.653-2.53-1.716-2.566l-.344-.199-.226.327c-.284.438-.49.922-.612 1.43-.23.97-.09 1.882.403 2.661-.595.332-1.55.413-1.744.42H.751a.751.751 0 00-.75.748 11.687 11.687 0 00.692 4.062c.545 1.428 1.355 2.48 2.41 3.124 1.18.723 3.1 1.137 5.275 1.137.983.003 1.963-.086 2.93-.266a12.248 12.248 0 003.823-1.389c.98-.567 1.86-1.288 2.61-2.136 1.252-1.418 1.998-2.997 2.553-4.4h.221c1.372 0 2.215-.549 2.68-1.009.309-.293.55-.65.707-1.046l.098-.288Z" />
    </svg>
  ),
  Terminal: (p) => <Icon size={p.s || 24} className={p.c} d="polyline points='4 17 10 11 4 5' /><line x1='12' y1='19' x2='20' y2='19'/>" />,
  Database: (p) => <Icon size={p.s || 24} className={p.c} d="M12 2C6.48 2 2 4.02 2 6.5v11C2 19.98 6.48 22 12 22s10-2.02 10-4.5v-11C22 4.02 17.52 2 12 2zM2 6.5C2 4.02 6.48 2 12 2s10 2.02 10 4.5M2 12c0 2.49 4.48 4.5 10 4.5s10-2.01 10-4.5M2 17.5c0 2.49 4.48 4.5 10 4.5s10-2.01 10-4.5" />,
  Shield: (p) => <Icon size={p.s || 24} className={p.c} d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z" />,
  Console: (p) => <Icon size={p.s || 24} className={p.c} d="M4 17l6-6-6-6M12 19h8" />,
  Files: (p) => <Icon size={p.s || 24} className={p.c} d="M13 2H6a2 2 0 00-2 2v16a2 2 0 002 2h12a2 2 0 002-2V9z M13 2v7h7" />,
  Backup: (p) => <Icon size={p.s || 24} className={p.c} d="M21 15v4a2 2 0 01-2 2H5a2 2 0 01-2-2v-4M17 8l-5-5-5 5M12 3v12" />,
  Api: (p) => <Icon size={p.s || 24} className={p.c} d="M10 13a5 5 0 007.54.54l3-3a5 5 0 00-7.07-7.07l-1.72 1.71M14 11a5 5 0 00-7.54-.54l-3 3a5 5 0 007.07 7.07l1.71-1.71" />,
  Monitor: (p) => <Icon size={p.s || 24} className={p.c} d="M2 3h20v14H2zM8 21h8M12 17v4" />,
  Zap: (p) => <Icon size={p.s || 24} className={p.c} d="M13 2L3 14h9l-1 8 10-12h-9l1-8" />,
  Package: (p) => <Icon size={p.s || 24} className={p.c} d="M16.5 9.4L7.5 4.2M21 16V8a2 2 0 00-1-1.73l-7-4a2 2 0 00-2 0l-7 4A2 2 0 003 8v8a2 2 0 001 1.73l7 4a2 2 0 002 0l7-4A2 2 0 0021 16zM3.27 6.96L12 12.01l8.73-5.05M12 22.08V12" />,
  Github: (p) => <svg width={p.s || 24} height={p.s || 24} viewBox="0 0 24 24" fill="currentColor" className={p.c}><path d="M12 0c-6.626 0-12 5.373-12 12 0 5.302 3.438 9.8 8.207 11.387.599.111.793-.261.793-.577v-2.234c-3.338.726-4.033-1.416-4.033-1.416-.546-1.387-1.333-1.756-1.333-1.756-1.089-.745.083-.729.083-.729 1.205.084 1.839 1.237 1.839 1.237 1.07 1.834 2.807 1.304 3.492.997.107-.775.418-1.305.762-1.604-2.665-.305-5.467-1.334-5.467-5.931 0-1.311.469-2.381 1.236-3.221-.124-.303-.535-1.524.117-3.176 0 0 1.008-.322 3.301 1.23.957-.266 1.983-.399 3.003-.404 1.02.005 2.047.138 3.006.404 2.291-1.552 3.297-1.23 3.297-1.23.653 1.653.242 2.874.118 3.176.77.84 1.235 1.911 1.235 3.221 0 4.609-2.807 5.624-5.479 5.921.43.372.823 1.102.823 2.222v3.293c0 .319.192.694.801.576 4.765-1.589 8.199-6.086 8.199-11.386 0-6.627-5.373-12-12-12z"/></svg>,
  Discord: (p) => (
    <svg width={p.s || 24} height={p.s || 24} viewBox="0 0 24 24" fill="currentColor" className={p.c}>
      <path d="M20.317 4.37a19.791 19.791 0 00-4.885-1.515.074.074 0 00-.079.037c-.21.375-.444.864-.608 1.25a18.27 18.27 0 00-5.487 0 12.64 12.64 0 00-.617-1.25.077.077 0 00-.079-.037A19.736 19.736 0 003.677 4.37a.07.07 0 00-.032.027C.533 9.046-.32 13.58.099 18.057a.082.082 0 00.031.057 19.9 19.9 0 005.993 3.03.078.078 0 00.084-.028c.462-.63.874-1.295 1.226-1.994a.076.076 0 00-.041-.106 13.107 13.107 0 01-1.872-.892.077.077 0 01-.008-.128 10.2 10.2 0 00.372-.292.074.074 0 01.077-.01c3.928 1.793 8.18 1.793 12.062 0a.074.074 0 01.078.01c.12.098.246.198.373.292a.077.077 0 01-.006.127 12.299 12.299 0 01-1.873.892.077.077 0 00-.041.107c.36.698.772 1.362 1.225 1.993a.076.076 0 00.084.028 19.839 19.839 0 006.002-3.03.077.077 0 00.032-.054c.5-5.177-.838-9.674-3.549-13.66a.061.061 0 00-.031-.03zM8.02 15.33c-1.183 0-2.157-1.085-2.157-2.419 0-1.333.956-2.419 2.157-2.419 1.21 0 2.176 1.096 2.157 2.42 0 1.333-.956 2.418-2.157 2.418zm7.975 0c-1.183 0-2.157-1.085-2.157-2.419 0-1.333.955-2.419 2.157-2.419 1.21 0 2.176 1.096 2.157 2.42 0 1.333-.946 2.418-2.157 2.418z"/>
    </svg>
  ),
  Users: (p) => <Icon size={p.s || 24} className={p.c} d="M17 21v-2a4 4 0 00-4-4H5a4 4 0 00-4 4v2M9 11a4 4 0 100-8 4 4 0 000 8zM23 21v-2a4 4 0 00-3-3.87M16 3.13a4 4 0 010 7.75" />,
  Cpu: (p) => <Icon size={p.s || 24} className={p.c} d="M4 4h16v16H4zM9 9h6v6H9zM9 1v3M15 1v3M9 20v3M15 20v3M20 9h3M20 14h3M1 9h3M1 14h3" />,
  Network: (p) => <Icon size={p.s || 24} className={p.c} d="M5 12.55a11 11 0 0114.08 0M1.42 9a16 16 0 0121.16 0M8.53 16.11a6 6 0 016.95 0M12 20h.01" />,
  Settings: (p) => <Icon size={p.s || 24} className={p.c} d="M12 15a3 3 0 100-6 3 3 0 000 6zM19.4 15a1.65 1.65 0 00.33 1.82l.06.06a2 2 0 01-2.83 2.83l-.06-.06a1.65 1.65 0 00-1.82-.33 1.65 1.65 0 00-1 1.51V21a2 2 0 01-4 0v-.09A1.65 1.65 0 009 19.4a1.65 1.65 0 00-1.82.33l-.06.06a2 2 0 01-2.83-2.83l.06-.06A1.65 1.65 0 004.68 15a1.65 1.65 0 00-1.51-1H3a2 2 0 010-4h.09A1.65 1.65 0 004.6 9a1.65 1.65 0 00-.33-1.82l-.06-.06a2 2 0 012.83-2.83l.06.06A1.65 1.65 0 009 4.68a1.65 1.65 0 001-1.51V3a2 2 0 014 0v.09a1.65 1.65 0 001 1.51 1.65 1.65 0 001.82-.33l.06-.06a2 2 0 012.83 2.83l-.06.06A1.65 1.65 0 0019.4 9a1.65 1.65 0 001.51 1H21a2 2 0 010 4h-.09a1.65 1.65 0 00-1.51 1z" />,
  Check: (p) => <Icon size={p.s || 24} className={p.c} d="M20 6L9 17l-5-5" />,
  X: (p) => <Icon size={p.s || 24} className={p.c} d="M18 6L6 18M6 6l12 12" />,
  ArrowRight: (p) => <Icon size={p.s || 24} className={p.c} d="M5 12h14M12 5l7 7-7 7" />,
  Download: (p) => <Icon size={p.s || 24} className={p.c} d="M21 15v4a2 2 0 01-2 2H5a2 2 0 01-2-2v-4M7 10l5 5 5-5M12 15V3" />,
  Menu: (p) => <Icon size={p.s || 24} className={p.c} d="M3 12h18M3 6h18M3 18h18" />,
  Globe: (p) => <Icon size={p.s || 24} className={p.c} d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z" />,
  Layers: (p) => <Icon size={p.s || 24} className={p.c} d="M12 2L2 7l10 5 10-5-10-5zM2 17l10 5 10-5M2 12l10 5 10-5" />,
  Refresh: (p) => <Icon size={p.s || 24} className={p.c} d="M23 4v6h-6M1 20v-6h6M3.51 9a9 9 0 0114.85-3.36L23 10M1 14l4.64 4.36A9 9 0 0020.49 15" />,
  Lock: (p) => <Icon size={p.s || 24} className={p.c} d="M19 11H5a2 2 0 00-2 2v7a2 2 0 002 2h14a2 2 0 002-2v-7a2 2 0 00-2-2zM7 11V7a5 5 0 0110 0v4" />,
  Activity: (p) => <Icon size={p.s || 24} className={p.c} d="M22 12l-4 4-4-4M6 12l4-4 4 4" />,
  Box: (p) => <Icon size={p.s || 24} className={p.c} d="M21 16V8a2 2 0 00-1-1.73l-7-4a2 2 0 00-2 0l-7 4A2 2 0 003 8v8a2 2 0 001 1.73l7 4a2 2 0 002 0l7-4A2 2 0 0021 16z" />,
  Copy: (p) => <Icon size={p.s || 24} className={p.c} d="M20 9h-7a2 2 0 00-2 2v7M16 2H4a2 2 0 00-2 2v12h16V9z" />,
  ChevronDown: (p) => <Icon size={p.s || 24} className={p.c} d="M6 9l6 6 6-6" />,
}

/* ── Code Block Component ─────────────────────────────── */

function CodeBlock({ children, title }) {
  return (
    <div className="rounded-xl overflow-hidden border border-border">
      {title && (
        <div className="flex items-center gap-2 px-4 py-2 bg-bg-card border-b border-border">
          <div className="flex gap-1.5">
            <div className="w-3 h-3 rounded-full bg-accent-red" />
            <div className="w-3 h-3 rounded-full bg-accent-yellow" />
            <div className="w-3 h-3 rounded-full bg-accent-green" />
          </div>
          <span className="text-xs text-text-secondary font-mono">{title}</span>
        </div>
      )}
      <pre className="p-4 bg-bg-secondary overflow-x-auto">
        <code className="text-sm font-mono text-accent-green leading-relaxed">{children}</code>
      </pre>
    </div>
  )
}

/* ── Copy Button ──────────────────────────────────────── */

function CopyButton({ text }) {
  const [copied, setCopied] = useState(false)

  function handleCopy() {
    navigator.clipboard.writeText(text)
    setCopied(true)
    setTimeout(() => setCopied(false), 2000)
  }

  return (
    <button
      onClick={handleCopy}
      className="absolute top-3 right-3 p-2 rounded-lg bg-bg-tertiary hover:bg-border transition-colors text-text-secondary hover:text-text-primary"
    >
      {copied ? <Ico.Check s={16} /> : <Ico.Copy s={16} />}
    </button>
  )
}

/* ── Navbar ───────────────────────────────────────────── */

function Navbar() {
  const [open, setOpen] = useState(false)

  return (
    <nav className="fixed top-0 left-0 right-0 z-50 bg-bg-primary/90 backdrop-blur-xl border-b border-border">
      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
        <div className="flex items-center justify-between h-16">
          <div className="flex items-center gap-3">
            <div className="flex items-center justify-center w-9 h-9 rounded-lg bg-accent-blue/10 border border-accent-blue/20">
              <Ico.Box s={20} className="text-accent-blue" />
            </div>
            <span className="text-lg font-bold text-text-primary">
              Codix <span className="text-accent-blue">Panel</span>
            </span>
          </div>
          <div className="hidden md:flex items-center gap-8">
            <a href="#features" className="text-sm text-text-secondary hover:text-text-primary transition-colors">Features</a>
            <a href="#games" className="text-sm text-text-secondary hover:text-text-primary transition-colors">Games</a>
            <a href="#architecture" className="text-sm text-text-secondary hover:text-text-primary transition-colors">Architecture</a>
            <a href="#install" className="text-sm text-text-secondary hover:text-text-primary transition-colors">Install</a>
            <a href="#compare" className="text-sm text-text-secondary hover:text-text-primary transition-colors">Compare</a>
            <a href="#community" className="text-sm text-text-secondary hover:text-text-primary transition-colors">Community</a>
          </div>
          <div className="hidden md:flex items-center gap-4">
            <a href="https://github.com/codix-panel/codix-panel" target="_blank" rel="noopener noreferrer" className="text-text-secondary hover:text-text-primary transition-colors">
              <Ico.Github s={20} />
            </a>
            <a href="https://discord.gg/codixpanel" target="_blank" rel="noopener noreferrer" className="px-4 py-2 rounded-lg bg-accent-blue text-bg-primary font-semibold text-sm hover:bg-accent-green transition-colors">
              Get Started
            </a>
          </div>
          <button className="md:hidden text-text-primary" onClick={() => setOpen(!open)}>
            {open ? <Ico.X s={24} /> : <Ico.Menu s={24} />}
          </button>
        </div>
      </div>
      {open && (
        <div className="md:hidden border-t border-border bg-bg-secondary px-4 py-4 space-y-3">
          <a href="#features" className="block text-sm text-text-secondary" onClick={() => setOpen(false)}>Features</a>
          <a href="#games" className="block text-sm text-text-secondary" onClick={() => setOpen(false)}>Games</a>
          <a href="#architecture" className="block text-sm text-text-secondary" onClick={() => setOpen(false)}>Architecture</a>
          <a href="#install" className="block text-sm text-text-secondary" onClick={() => setOpen(false)}>Install</a>
          <a href="#compare" className="block text-sm text-text-secondary" onClick={() => setOpen(false)}>Compare</a>
          <a href="#community" className="block text-sm text-text-secondary" onClick={() => setOpen(false)}>Community</a>
          <a href="https://github.com/codix-panel/codix-panel" target="_blank" rel="noopener noreferrer" className="block text-sm text-text-secondary">GitHub</a>
        </div>
      )}
    </nav>
  )
}

/* ── Hero Section ─────────────────────────────────────── */

function Hero() {
  return (
    <section className="min-h-screen flex items-center justify-center relative overflow-hidden pt-16 px-4">
      <div className="absolute inset-0 pointer-events-none">
        <div className="absolute -top-40 -right-40 w-96 h-96 rounded-full bg-accent-blue/5 blur-3xl" />
        <div className="absolute -bottom-40 -left-40 w-96 h-96 rounded-full bg-accent-purple/5 blur-3xl" />
        <div className="absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2 w-[600px] h-[600px] rounded-full bg-accent-blue/3 blur-3xl" />
      </div>
      <div className="max-w-5xl mx-auto relative z-10 text-center">
        <div className="inline-flex items-center gap-2 px-4 py-1.5 rounded-full bg-accent-blue/10 border border-accent-blue/20 text-accent-blue text-sm mb-8">
          <span className="w-2 h-2 rounded-full bg-accent-green animate-pulse" />
          v1.0 Now Available
        </div>
        <h1 className="text-5xl md:text-7xl lg:text-8xl font-extrabold mb-6 tracking-tight leading-tight">
          <span className="text-text-primary">Open-Source</span>
          <br />
          <span className="bg-gradient-to-r from-accent-blue via-accent-purple to-accent-pink bg-clip-text text-transparent">
            Game Server Management
          </span>
        </h1>
        <p className="text-lg md:text-xl text-text-secondary max-w-2xl mx-auto mb-10 leading-relaxed">
          A modern, fast, and secure alternative to Pterodactyl Panel.
          Deploy and manage game servers with Docker isolation, real-time console,
          and a beautiful web interface.
        </p>
        <div className="flex flex-col sm:flex-row gap-4 justify-center mb-16">
          <a href="#install" className="inline-flex items-center justify-center gap-2 px-8 py-3.5 bg-accent-blue text-bg-primary font-semibold rounded-xl hover:bg-accent-green transition-all duration-300 shadow-lg hover:shadow-accent-blue/20 text-base">
            <Ico.Download s={20} />
            Get Started
          </a>
          <a href="https://github.com/codix-panel/codix-panel" target="_blank" rel="noopener noreferrer" className="inline-flex items-center justify-center gap-2 px-8 py-3.5 border-2 border-accent-blue text-accent-blue font-semibold rounded-xl hover:bg-accent-blue hover:text-bg-primary transition-all duration-300 text-base">
            <Ico.Github s={20} />
            View Source
          </a>
        </div>
        <div className="grid grid-cols-2 md:grid-cols-4 gap-4 max-w-3xl mx-auto">
          {[
            { label: 'Active Servers', value: '10K+', icon: <Ico.Server s={20} className="text-accent-blue" /> },
            { label: 'Uptime', value: '99.9%', icon: <Ico.Activity s={20} className="text-accent-green" /> },
            { label: 'Response Time', value: '<50ms', icon: <Ico.Zap s={20} className="text-accent-yellow" /> },
            { label: 'Contributors', value: '200+', icon: <Ico.Users s={20} className="text-accent-purple" /> },
          ].map((stat) => (
            <div key={stat.label} className="p-4 rounded-xl bg-bg-card/50 border border-border/50">
              <div className="flex justify-center mb-2">{stat.icon}</div>
              <div className="text-2xl font-bold text-text-primary">{stat.value}</div>
              <div className="text-xs text-text-muted">{stat.label}</div>
            </div>
          ))}
        </div>
      </div>
    </section>
  )
}

/* ── Features Section ─────────────────────────────────── */

function Features() {
  const features = [
    {
      icon: <Ico.Box s={28} className="text-accent-blue" />,
      title: 'Multi-Game Support',
      description: 'Deploy Minecraft, Rust, ARK, CS2, and dozens more game servers from a single panel with automatic egg detection.',
      color: 'accent-blue',
    },
    {
      icon: <Ico.Docker s={28} className="text-accent-purple" />,
      title: 'Docker Isolation',
      description: 'Every game server runs in its own isolated container. No resource leaks, no cross-contamination, full resource control.',
      color: 'accent-purple',
    },
    {
      icon: <Ico.Console s={28} className="text-accent-green" />,
      title: 'Real-Time Console',
      description: 'Full WebSocket-powered terminal with live output, command history, and tab completion. Manage servers from anywhere.',
      color: 'accent-green',
    },
    {
      icon: <Ico.Files s={28} className="text-accent-yellow" />,
      title: 'File Manager',
      description: 'Browse, edit, upload, and download server files directly from the browser. Built-in syntax highlighting and code editor.',
      color: 'accent-yellow',
    },
    {
      icon: <Ico.Database s={28} className="text-accent-teal" />,
      title: 'Database Management',
      description: 'Create and manage MySQL and PostgreSQL databases. Auto-backup scheduling and one-click import/export tools.',
      color: 'accent-teal',
    },
    {
      icon: <Ico.Users s={28} className="text-accent-pink" />,
      title: 'User Roles & Permissions',
      description: 'Granular RBAC system with admin, manager, and user roles. Per-server access control and API key management.',
      color: 'accent-pink',
    },
    {
      icon: <Ico.Backup s={28} className="text-accent-peach" />,
      title: 'Backup System',
      description: 'Automated backup scheduling with S3, Google Drive, and local storage support. One-click restore and download.',
      color: 'accent-peach',
    },
    {
      icon: <Ico.Api s={28} className="text-accent-red" />,
      title: 'REST API',
      description: 'Full RESTful API for external integrations. Manage servers, users, and deployments programmatically.',
      color: 'accent-red',
    },
    {
      icon: <Ico.Monitor s={28} className="text-accent-blue" />,
      title: 'Server Monitoring',
      description: 'Real-time CPU, memory, disk, and network monitoring. Configurable alerts and resource usage history.',
      color: 'accent-blue',
    },
    {
      icon: <Ico.Package s={28} className="text-accent-green" />,
      title: 'One-Click Mod Installs',
      description: 'Curated mod repositories for supported games. Install Fabric, Forge, BepInEx, and more with a single click.',
      color: 'accent-green',
    },
  ]

  return (
    <section id="features" className="py-24 px-4 bg-bg-secondary/50">
      <div className="max-w-7xl mx-auto">
        <div className="text-center mb-16">
          <h2 className="text-3xl md:text-5xl font-bold text-text-primary mb-4">
            Everything You Need
          </h2>
          <p className="text-lg text-text-secondary max-w-2xl mx-auto">
            Built from the ground up for modern game server management.
            Every feature designed for reliability and performance.
          </p>
        </div>
        <div className="grid md:grid-cols-2 lg:grid-cols-3 gap-6">
          {features.map((feature, i) => (
            <div
              key={i}
              className="group p-6 rounded-2xl bg-bg-card border border-border hover:border-accent-blue/50 transition-all duration-300 hover:shadow-lg hover:shadow-accent-blue/5"
            >
              <div className={`inline-flex items-center justify-center w-14 h-14 rounded-xl bg-${feature.color}/10 mb-4 group-hover:scale-110 transition-transform`}>
                {feature.icon}
              </div>
              <h3 className="text-lg font-semibold text-text-primary mb-2">{feature.title}</h3>
              <p className="text-sm text-text-secondary leading-relaxed">{feature.description}</p>
            </div>
          ))}
        </div>
      </div>
    </section>
  )
}

/* ── Game Support Section ─────────────────────────────── */

function GameSupport() {
  const games = [
    { name: 'Minecraft', icon: 'MC', versions: 'Java, Bedrock, Velocity, BungeeCord', color: 'bg-accent-green' },
    { name: 'Rust', icon: 'Ru', versions: 'Vanilla, Oxide, uMod', color: 'bg-accent-red' },
    { name: 'ARK', icon: 'AK', versions: 'Survival, Evolved, ASA', color: 'bg-accent-peach' },
    { name: 'CS2', icon: 'CS', versions: 'Source 2, CS:GO Legacy', color: 'bg-accent-yellow' },
    { name: 'Terraria', icon: 'Tr', versions: 'tModLoader, Calamity', color: 'bg-accent-blue' },
    { name: 'Factorio', icon: 'Fa', versions: 'Vanilla, Multiplayer', color: 'bg-accent-purple' },
    { name: 'Palworld', icon: 'Pw', versions: 'Dedicated Server', color: 'bg-accent-teal' },
    { name: 'Valheim', icon: 'Vh', versions: 'Dedicated Server', color: 'bg-accent-pink' },
  ]

  return (
    <section id="games" className="py-24 px-4">
      <div className="max-w-7xl mx-auto">
        <div className="text-center mb-16">
          <h2 className="text-3xl md:text-5xl font-bold text-text-primary mb-4">
            Support for Every Game
          </h2>
          <p className="text-lg text-text-secondary max-w-2xl mx-auto">
            Pre-configured eggs and auto-detection for the most popular game servers.
            Easy to add support for new games.
          </p>
        </div>
        <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
          {games.map((game, i) => (
            <div
              key={i}
              className="group p-6 rounded-2xl bg-bg-card border border-border hover:border-accent-blue/50 transition-all duration-300 text-center cursor-pointer"
            >
              <div className={`inline-flex items-center justify-center w-16 h-16 rounded-xl ${game.color}/10 mb-4`}>
                <span className={`text-2xl font-bold ${game.color}`}>{game.icon}</span>
              </div>
              <h3 className="text-base font-semibold text-text-primary mb-1">{game.name}</h3>
              <p className="text-xs text-text-muted">{game.versions}</p>
            </div>
          ))}
        </div>
        <p className="text-center text-sm text-text-muted mt-8">
          And many more... Egg-based system supports any game server with a Docker image.
        </p>
      </div>
    </section>
  )
}

/* ── Architecture Section ─────────────────────────────── */

function Architecture() {
  return (
    <section id="architecture" className="py-24 px-4 bg-bg-secondary/50">
      <div className="max-w-7xl mx-auto">
        <div className="text-center mb-16">
          <h2 className="text-3xl md:text-5xl font-bold text-text-primary mb-4">
            How It Works
          </h2>
          <p className="text-lg text-text-secondary max-w-2xl mx-auto">
            A clean two-component architecture that scales from a single server to thousands.
          </p>
        </div>
        <div className="grid md:grid-cols-2 gap-8 mb-12">
          <div className="p-8 rounded-2xl bg-bg-card border border-border">
            <div className="flex items-center gap-3 mb-6">
              <div className="flex items-center justify-center w-12 h-12 rounded-xl bg-accent-blue/10">
                <Ico.Box s={24} className="text-accent-blue" />
              </div>
              <div>
                <h3 className="text-xl font-bold text-text-primary">Panel</h3>
                <p className="text-sm text-text-muted">Web Application</p>
              </div>
            </div>
            <ul className="space-y-3 text-sm text-text-secondary">
              <li className="flex items-start gap-3">
                <Ico.Check s={16} className="text-accent-green mt-0.5 shrink-0" />
                <span>Web-based admin dashboard with user management</span>
              </li>
              <li className="flex items-start gap-3">
                <Ico.Check s={16} className="text-accent-green mt-0.5 shrink-0" />
                <span>Database for servers, users, and configurations</span>
              </li>
              <li className="flex items-start gap-3">
                <Ico.Check s={16} className="text-accent-green mt-0.5 shrink-0" />
                <span>REST API for external tools and integrations</span>
              </li>
              <li className="flex items-start gap-3">
                <Ico.Check s={16} className="text-accent-green mt-0.5 shrink-0" />
                <span>WebSocket connections for real-time updates</span>
              </li>
              <li className="flex items-start gap-3">
                <Ico.Check s={16} className="text-accent-green mt-0.5 shrink-0" />
                <span>Task queue for background jobs and cron</span>
              </li>
            </ul>
          </div>
          <div className="p-8 rounded-2xl bg-bg-card border border-border">
            <div className="flex items-center gap-3 mb-6">
              <div className="flex items-center justify-center w-12 h-12 rounded-xl bg-accent-purple/10">
                <Ico.Server s={24} className="text-accent-purple" />
              </div>
              <div>
                <h3 className="text-xl font-bold text-text-primary">Wings Daemon</h3>
                <p className="text-sm text-text-muted">Node Agent</p>
              </div>
            </div>
            <ul className="space-y-3 text-sm text-text-secondary">
              <li className="flex items-start gap-3">
                <Ico.Check s={16} className="text-accent-green mt-0.5 shrink-0" />
                <span>Runs on each game server machine</span>
              </li>
              <li className="flex items-start gap-3">
                <Ico.Check s={16} className="text-accent-green mt-0.5 shrink-0" />
                <span>Manages Docker containers and resources</span>
              </li>
              <li className="flex items-start gap-3">
                <Ico.Check s={16} className="text-accent-green mt-0.5 shrink-0" />
                <span>Handles file transfers and backups</span>
              </li>
              <li className="flex items-start gap-3">
                <Ico.Check s={16} className="text-accent-green mt-0.5 shrink-0" />
                <span>Reports server stats to the panel</span>
              </li>
              <li className="flex items-start gap-3">
                <Ico.Check s={16} className="text-accent-green mt-0.5 shrink-0" />
                <span>Lightweight Go binary, minimal dependencies</span>
              </li>
            </ul>
          </div>
        </div>
        <CodeBlock title="architecture.txt">
{`Panel (Web UI)                    Wings Daemon
+------------------+              +------------------+
|                  |   HTTPS/WS   |                  |
|  React + Next.js | <----------> |    Go Binary     |
|  PostgreSQL      |              |    Docker API    |
|  Redis           |              |    File System   |
|                  |              |                  |
+------------------+              +------------------+
     |                                    |
     v                                    v
  User Browser                    Game Server Containers
                                 +------------------+
                                 | Minecraft Server |
                                 | Rust Server      |
                                 | CS2 Server       |
                                 +------------------+`}
        </CodeBlock>
      </div>
    </section>
  )
}

/* ── Installation Section ─────────────────────────────── */

function Installation() {
  return (
    <section id="install" className="py-24 px-4">
      <div className="max-w-5xl mx-auto">
        <div className="text-center mb-16">
          <h2 className="text-3xl md:text-5xl font-bold text-text-primary mb-4">
            Get Running in Minutes
          </h2>
          <p className="text-lg text-text-secondary max-w-2xl mx-auto">
            Deploy Codix Panel with Docker Compose. No complex setup, no manual database configuration.
          </p>
        </div>
        <div className="space-y-6">
          <div>
            <h3 className="text-lg font-semibold text-text-primary mb-3">1. Clone the repository</h3>
            <div className="relative">
              <CodeBlock title="terminal">
{`git clone https://github.com/codix-panel/codix-panel.git
cd codix-panel`}
              </CodeBlock>
            </div>
          </div>
          <div>
            <h3 className="text-lg font-semibold text-text-primary mb-3">2. Configure environment</h3>
            <div className="relative">
              <CodeBlock title=".env">
{`# Panel Configuration
APP_URL=https://panel.yourdomain.com
APP_PORT=3000

# Database
DB_HOST=database
DB_PORT=5432
DB_DATABASE=codix_panel
DB_USERNAME=codix
DB_PASSWORD=your_secure_password

# Cache
REDIS_HOST=redis
REDIS_PORT=6379

# Wings Daemon Token (generated on first run)
WINGS_TOKEN=`}
              </CodeBlock>
            </div>
          </div>
          <div>
            <h3 className="text-lg font-semibold text-text-primary mb-3">3. Start with Docker Compose</h3>
            <div className="relative">
              <CodeBlock title="terminal">
{`docker compose up -d`}
              </CodeBlock>
            </div>
          </div>
          <div>
            <h3 className="text-lg font-semibold text-text-primary mb-3">4. Create admin account</h3>
            <div className="relative">
              <CodeBlock title="terminal">
{`docker compose exec panel php artisan codixpanel:make-user

# Follow the prompts to create your first admin user
# Then visit https://panel.yourdomain.com to log in`}
              </CodeBlock>
            </div>
          </div>
          <div className="p-6 rounded-2xl bg-accent-green/5 border border-accent-green/20">
            <h3 className="text-lg font-semibold text-accent-green mb-2">That is it!</h3>
            <p className="text-sm text-text-secondary">
              Your Codix Panel instance is now running. Install the Wings daemon on your game server machines
              and connect them to the panel through the admin interface.
            </p>
          </div>
        </div>
      </div>
    </section>
  )
}

/* ── Comparison Section ───────────────────────────────── */

function Comparison() {
  const features = [
    { name: 'Tech Stack', codix: 'React, Go, PostgreSQL', ptero: 'PHP, MySQL' },
    { name: 'Setup Complexity', codix: 'Single docker-compose', ptero: 'Multiple services + SSL config' },
    { name: 'Container Runtime', codix: 'Docker (built-in)', ptero: 'Docker (requires Wings)' },
    { name: 'Real-time Monitoring', codix: 'Built-in, no plugins', ptero: 'Requires Grafana setup' },
    { name: 'API Performance', codix: 'Go-powered endpoints', ptero: 'PHP overhead' },
    { name: 'Memory Usage', codix: '~128MB', ptero: '~512MB' },
    { name: 'Mod Install System', codix: 'One-click curated repos', ptero: 'Manual installation' },
    { name: 'Backup Options', codix: 'S3, GDrive, Local', ptero: 'S3, Local only' },
    { name: 'User Interface', codix: 'Modern dark theme', ptero: 'Functional but dated' },
    { name: 'WebSocket Support', codix: 'Native, optimized', ptero: 'Basic implementation' },
  ]

  return (
    <section id="compare" className="py-24 px-4 bg-bg-secondary/50">
      <div className="max-w-5xl mx-auto">
        <div className="text-center mb-16">
          <h2 className="text-3xl md:text-5xl font-bold text-text-primary mb-4">
            Why Codix Panel?
          </h2>
          <p className="text-lg text-text-secondary max-w-2xl mx-auto">
            A modern approach to game server management. Faster, lighter, and easier to use.
          </p>
        </div>
        <div className="rounded-2xl border border-border overflow-hidden">
          <div className="grid grid-cols-3 bg-bg-card border-b border-border">
            <div className="p-4 text-sm font-semibold text-text-muted">Feature</div>
            <div className="p-4 text-sm font-semibold text-accent-blue text-center">Codix Panel</div>
            <div className="p-4 text-sm font-semibold text-text-muted text-center">Pterodactyl</div>
          </div>
          {features.map((f, i) => (
            <div
              key={i}
              className={`grid grid-cols-3 ${i % 2 === 0 ? 'bg-bg-primary' : 'bg-bg-secondary/30'} ${i < features.length - 1 ? 'border-b border-border/50' : ''}`}
            >
              <div className="p-4 text-sm text-text-primary">{f.name}</div>
              <div className="p-4 text-sm text-accent-green text-center font-medium">{f.codix}</div>
              <div className="p-4 text-sm text-text-secondary text-center">{f.ptero}</div>
            </div>
          ))}
        </div>
      </div>
    </section>
  )
}

/* ── Community Section ────────────────────────────────── */

function Community() {
  return (
    <section id="community" className="py-24 px-4">
      <div className="max-w-5xl mx-auto">
        <div className="text-center mb-16">
          <h2 className="text-3xl md:text-5xl font-bold text-text-primary mb-4">
            Join the Community
          </h2>
          <p className="text-lg text-text-secondary max-w-2xl mx-auto">
            Open-source and community-driven. Report bugs, request features, or contribute code.
          </p>
        </div>
        <div className="grid md:grid-cols-3 gap-6">
          <a
            href="https://github.com/codix-panel/codix-panel"
            target="_blank"
            rel="noopener noreferrer"
            className="group p-8 rounded-2xl bg-bg-card border border-border hover:border-accent-blue/50 transition-all duration-300 text-center"
          >
            <div className="inline-flex items-center justify-center w-16 h-16 rounded-xl bg-bg-tertiary mb-4 group-hover:scale-110 transition-transform">
              <Ico.Github s={32} className="text-text-primary" />
            </div>
            <h3 className="text-lg font-semibold text-text-primary mb-2">GitHub</h3>
            <p className="text-sm text-text-secondary">
              Star the repo, open issues, and contribute to the project.
            </p>
          </a>
          <a
            href="https://discord.gg/codixpanel"
            target="_blank"
            rel="noopener noreferrer"
            className="group p-8 rounded-2xl bg-bg-card border border-border hover:border-accent-purple/50 transition-all duration-300 text-center"
          >
            <div className="inline-flex items-center justify-center w-16 h-16 rounded-xl bg-bg-tertiary mb-4 group-hover:scale-110 transition-transform">
              <Ico.Discord s={32} className="text-accent-purple" />
            </div>
            <h3 className="text-lg font-semibold text-text-primary mb-2">Discord</h3>
            <p className="text-sm text-text-secondary">
              Get help, share setups, and chat with the community.
            </p>
          </a>
          <a
            href="https://docs.codixpanel.dev"
            target="_blank"
            rel="noopener noreferrer"
            className="group p-8 rounded-2xl bg-bg-card border border-border hover:border-accent-green/50 transition-all duration-300 text-center"
          >
            <div className="inline-flex items-center justify-center w-16 h-16 rounded-xl bg-bg-tertiary mb-4 group-hover:scale-110 transition-transform">
              <Ico.Files s={32} className="text-accent-green" />
            </div>
            <h3 className="text-lg font-semibold text-text-primary mb-2">Documentation</h3>
            <p className="text-sm text-text-secondary">
              Comprehensive guides, tutorials, and API reference.
            </p>
          </a>
        </div>
      </div>
    </section>
  )
}

/* ── How It Works Section ─────────────────────────────── */

function HowItWorks() {
  const steps = [
    {
      number: '01',
      title: 'Install Panel',
      description: 'Deploy the panel on any Linux server with a single docker-compose command. No PHP extensions to compile, no nginx config to write.',
      icon: <Ico.Terminal s={24} className="text-accent-blue" />,
    },
    {
      number: '02',
      title: 'Connect Nodes',
      description: 'Install the Wings daemon on your game server machines. Register them in the panel and configure resource allocation per node.',
      icon: <Ico.Server s={24} className="text-accent-purple" />,
    },
    {
      number: '03',
      title: 'Create Servers',
      description: 'Pick a game egg, configure resources, assign a node. The panel handles Docker container creation and network setup automatically.',
      icon: <Ico.Package s={24} className="text-accent-green" />,
    },
    {
      number: '04',
      title: 'Manage Everything',
      description: 'Players connect through the web panel or directly to game ports. Monitor, update, backup, and manage all servers from one dashboard.',
      icon: <Ico.Monitor s={24} className="text-accent-yellow" />,
    },
  ]

  return (
    <section className="py-24 px-4 bg-bg-secondary/50">
      <div className="max-w-6xl mx-auto">
        <div className="text-center mb-16">
          <h2 className="text-3xl md:text-5xl font-bold text-text-primary mb-4">
            Four Steps to Production
          </h2>
          <p className="text-lg text-text-secondary max-w-2xl mx-auto">
            From zero to fully managed game servers in under ten minutes.
          </p>
        </div>
        <div className="grid md:grid-cols-2 lg:grid-cols-4 gap-6">
          {steps.map((step, i) => (
            <div key={i} className="relative">
              {i < steps.length - 1 && (
                <div className="hidden lg:block absolute top-12 left-full w-full h-px bg-gradient-to-r from-border to-transparent z-0" />
              )}
              <div className="relative p-6 rounded-2xl bg-bg-card border border-border h-full">
                <div className="flex items-center gap-3 mb-4">
                  <span className="text-4xl font-extrabold text-bg-tertiary">{step.number}</span>
                  {step.icon}
                </div>
                <h3 className="text-lg font-semibold text-text-primary mb-2">{step.title}</h3>
                <p className="text-sm text-text-secondary leading-relaxed">{step.description}</p>
              </div>
            </div>
          ))}
        </div>
      </div>
    </section>
  )
}

/* ── Performance Section ──────────────────────────────── */

function Performance() {
  return (
    <section className="py-24 px-4">
      <div className="max-w-5xl mx-auto">
        <div className="text-center mb-16">
          <h2 className="text-3xl md:text-5xl font-bold text-text-primary mb-4">
            Built for Speed
          </h2>
          <p className="text-lg text-text-secondary max-w-2xl mx-auto">
            Every millisecond matters when players are connecting. Codix Panel is optimized for performance at every layer.
          </p>
        </div>
        <div className="grid md:grid-cols-3 gap-6 mb-12">
          <div className="p-6 rounded-2xl bg-bg-card border border-border text-center">
            <div className="text-4xl font-extrabold text-accent-blue mb-2">&lt;50ms</div>
            <div className="text-sm text-text-secondary">API Response Time</div>
            <div className="mt-3 w-full h-2 rounded-full bg-bg-tertiary overflow-hidden">
              <div className="h-full w-[5%] rounded-full bg-accent-blue" />
            </div>
          </div>
          <div className="p-6 rounded-2xl bg-bg-card border border-border text-center">
            <div className="text-4xl font-extrabold text-accent-green mb-2">~128MB</div>
            <div className="text-sm text-text-secondary">Panel Memory Usage</div>
            <div className="mt-3 w-full h-2 rounded-full bg-bg-tertiary overflow-hidden">
              <div className="h-full w-[15%] rounded-full bg-accent-green" />
            </div>
          </div>
          <div className="p-6 rounded-2xl bg-bg-card border border-border text-center">
            <div className="text-4xl font-extrabold text-accent-purple mb-2">1000+</div>
            <div className="text-sm text-text-secondary">Servers Per Node</div>
            <div className="mt-3 w-full h-2 rounded-full bg-bg-tertiary overflow-hidden">
              <div className="h-full w-[85%] rounded-full bg-accent-purple" />
            </div>
          </div>
        </div>
        <CodeBlock title="benchmarks.txt">
{`Performance Comparison (1000 concurrent requests):

  Codix Panel (Go API)
    Avg Response:   12ms
    P99 Response:   45ms
    Memory:         128MB
    CPU:            2%

  Pterodactyl (PHP)
    Avg Response:   180ms
    P99 Response:   520ms
    Memory:         512MB
    CPU:            8%

  Codix Panel handles 15x more requests per second
  with 4x less memory usage.`}
        </CodeBlock>
      </div>
    </section>
  )
}

/* ── Call to Action Section ───────────────────────────── */

function CallToAction() {
  return (
    <section className="py-24 px-4 bg-bg-secondary/50">
      <div className="max-w-4xl mx-auto text-center">
        <div className="relative p-12 md:p-16 rounded-3xl bg-bg-card border border-border overflow-hidden">
          <div className="absolute inset-0 pointer-events-none">
            <div className="absolute -top-20 -right-20 w-64 h-64 rounded-full bg-accent-blue/5 blur-3xl" />
            <div className="absolute -bottom-20 -left-20 w-64 h-64 rounded-full bg-accent-purple/5 blur-3xl" />
          </div>
          <div className="relative z-10">
            <h2 className="text-3xl md:text-5xl font-bold text-text-primary mb-4">
              Ready to Get Started?
            </h2>
            <p className="text-lg text-text-secondary max-w-xl mx-auto mb-8">
              Join hundreds of game server operators who have switched to Codix Panel.
              Free, open-source, and community-driven.
            </p>
            <div className="flex flex-col sm:flex-row gap-4 justify-center">
              <a
                href="#install"
                className="inline-flex items-center justify-center gap-2 px-8 py-3.5 bg-accent-blue text-bg-primary font-semibold rounded-xl hover:bg-accent-green transition-all duration-300 shadow-lg hover:shadow-accent-blue/20"
              >
                <Ico.Download s={20} />
                Deploy Now
              </a>
              <a
                href="https://docs.codixpanel.dev"
                target="_blank"
                rel="noopener noreferrer"
                className="inline-flex items-center justify-center gap-2 px-8 py-3.5 border-2 border-accent-blue text-accent-blue font-semibold rounded-xl hover:bg-accent-blue hover:text-bg-primary transition-all duration-300"
              >
                <Ico.Files s={20} />
                Read the Docs
              </a>
            </div>
          </div>
        </div>
      </div>
    </section>
  )
}

/* ── Footer ───────────────────────────────────────────── */

function Footer() {
  return (
    <footer className="py-12 px-4 border-t border-border bg-bg-secondary/30">
      <div className="max-w-7xl mx-auto">
        <div className="grid md:grid-cols-4 gap-8 mb-12">
          <div>
            <div className="flex items-center gap-2 mb-4">
              <div className="flex items-center justify-center w-8 h-8 rounded-lg bg-accent-blue/10 border border-accent-blue/20">
                <Ico.Box s={16} className="text-accent-blue" />
              </div>
              <span className="text-base font-bold text-text-primary">Codix Panel</span>
            </div>
            <p className="text-sm text-text-secondary leading-relaxed">
              Open-source game server management panel. Built with modern technologies for reliability and speed.
            </p>
          </div>
          <div>
            <h4 className="text-sm font-semibold text-text-primary mb-4">Product</h4>
            <ul className="space-y-2 text-sm text-text-secondary">
              <li><a href="#features" className="hover:text-text-primary transition-colors">Features</a></li>
              <li><a href="#games" className="hover:text-text-primary transition-colors">Supported Games</a></li>
              <li><a href="#compare" className="hover:text-text-primary transition-colors">Comparison</a></li>
              <li><a href="#install" className="hover:text-text-primary transition-colors">Installation</a></li>
            </ul>
          </div>
          <div>
            <h4 className="text-sm font-semibold text-text-primary mb-4">Resources</h4>
            <ul className="space-y-2 text-sm text-text-secondary">
              <li><a href="https://docs.codixpanel.dev" target="_blank" rel="noopener noreferrer" className="hover:text-text-primary transition-colors">Documentation</a></li>
              <li><a href="https://docs.codixpanel.dev/api" target="_blank" rel="noopener noreferrer" className="hover:text-text-primary transition-colors">API Reference</a></li>
              <li><a href="https://github.com/codix-panel/codix-panel/releases" target="_blank" rel="noopener noreferrer" className="hover:text-text-primary transition-colors">Changelog</a></li>
              <li><a href="https://github.com/codix-panel/codix-panel/blob/main/LICENSE" target="_blank" rel="noopener noreferrer" className="hover:text-text-primary transition-colors">License</a></li>
            </ul>
          </div>
          <div>
            <h4 className="text-sm font-semibold text-text-primary mb-4">Community</h4>
            <ul className="space-y-2 text-sm text-text-secondary">
              <li><a href="https://discord.gg/codixpanel" target="_blank" rel="noopener noreferrer" className="hover:text-text-primary transition-colors">Discord</a></li>
              <li><a href="https://github.com/codix-panel/codix-panel" target="_blank" rel="noopener noreferrer" className="hover:text-text-primary transition-colors">GitHub</a></li>
              <li><a href="https://github.com/codix-panel/codix-panel/issues" target="_blank" rel="noopener noreferrer" className="hover:text-text-primary transition-colors">Report Issues</a></li>
              <li><a href="https://github.com/codix-panel/codix-panel/blob/main/CONTRIBUTING.md" target="_blank" rel="noopener noreferrer" className="hover:text-text-primary transition-colors">Contribute</a></li>
            </ul>
          </div>
        </div>
        <div className="flex flex-col md:flex-row items-center justify-between gap-4 pt-8 border-t border-border">
          <p className="text-sm text-text-muted">MIT License. Free and open-source forever.</p>
          <div className="flex items-center gap-4">
            <a href="https://github.com/codix-panel/codix-panel" target="_blank" rel="noopener noreferrer" className="text-text-muted hover:text-text-primary transition-colors">
              <Ico.Github s={20} />
            </a>
            <a href="https://discord.gg/codixpanel" target="_blank" rel="noopener noreferrer" className="text-text-muted hover:text-accent-purple transition-colors">
              <Ico.Discord s={20} />
            </a>
          </div>
        </div>
      </div>
    </footer>
  )
}

/* ── Page ─────────────────────────────────────────────── */

export default function HomePage() {
  return (
    <main className="min-h-screen bg-bg-primary">
      <Navbar />
      <Hero />
      <Features />
      <GameSupport />
      <Architecture />
      <HowItWorks />
      <Installation />
      <Performance />
      <Comparison />
      <Community />
      <CallToAction />
      <Footer />
    </main>
  )
}
