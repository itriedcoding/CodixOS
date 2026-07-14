'use client'

import { useState } from 'react'

/* ── Inline SVG Icons ─────────────────────────────────── */

function IconTerminal({ size = 24, className = '' }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={className}>
      <polyline points="4 17 10 11 4 5" />
      <line x1="12" y1="19" x2="20" y2="19" />
    </svg>
  )
}

function IconDownload({ size = 24, className = '' }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={className}>
      <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4" />
      <polyline points="7 10 12 15 17 10" />
      <line x1="12" y1="15" x2="12" y2="3" />
    </svg>
  )
}

function IconGithub({ size = 24, className = '' }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="currentColor" className={className}>
      <path d="M12 0c-6.626 0-12 5.373-12 12 0 5.302 3.438 9.8 8.207 11.387.599.111.793-.261.793-.577v-2.234c-3.338.726-4.033-1.416-4.033-1.416-.546-1.387-1.333-1.756-1.333-1.756-1.089-.745.083-.729.083-.729 1.205.084 1.839 1.237 1.839 1.237 1.07 1.834 2.807 1.304 3.492.997.107-.775.418-1.305.762-1.604-2.665-.305-5.467-1.334-5.467-5.931 0-1.311.469-2.381 1.236-3.221-.124-.303-.535-1.524.117-3.176 0 0 1.008-.322 3.301 1.23.957-.266 1.983-.399 3.003-.404 1.02.005 2.047.138 3.006.404 2.291-1.552 3.297-1.23 3.297-1.23.653 1.653.242 2.874.118 3.176.77.84 1.235 1.911 1.235 3.221 0 4.609-2.807 5.624-5.479 5.921.43.372.823 1.102.823 2.222v3.293c0 .319.192.694.801.576 4.765-1.589 8.199-6.086 8.199-11.386 0-6.627-5.373-12-12-12z" />
    </svg>
  )
}

function IconCpu({ size = 24, className = '' }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={className}>
      <rect x="4" y="4" width="16" height="16" rx="2" ry="2" />
      <rect x="9" y="9" width="6" height="6" />
      <line x1="9" y1="1" x2="9" y2="4" />
      <line x1="15" y1="1" x2="15" y2="4" />
      <line x1="9" y1="20" x2="9" y2="23" />
      <line x1="15" y1="20" x2="15" y2="23" />
      <line x1="20" y1="9" x2="23" y2="9" />
      <line x1="20" y1="14" x2="23" y2="14" />
      <line x1="1" y1="9" x2="4" y2="9" />
      <line x1="1" y1="14" x2="4" y2="14" />
    </svg>
  )
}

function IconHardDrive({ size = 24, className = '' }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={className}>
      <line x1="22" y1="12" x2="2" y2="12" />
      <path d="M5.45 5.11L2 12v6a2 2 0 0 0 2 2h16a2 2 0 0 0 2-2v-6l-3.45-6.89A2 2 0 0 0 16.76 4H7.24a2 2 0 0 0-1.79 1.11z" />
      <line x1="6" y1="16" x2="6.01" y2="16" />
      <line x1="10" y1="16" x2="10.01" y2="16" />
    </svg>
  )
}

function IconActivity({ size = 24, className = '' }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={className}>
      <polyline points="22 12 18 12 15 21 9 3 6 12 2 12" />
    </svg>
  )
}

function IconMonitor({ size = 24, className = '' }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={className}>
      <rect x="2" y="3" width="20" height="14" rx="2" ry="2" />
      <line x1="8" y1="21" x2="16" y2="21" />
      <line x1="12" y1="17" x2="12" y2="21" />
    </svg>
  )
}

function IconZap({ size = 24, className = '' }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={className}>
      <polygon points="13 2 3 14 12 14 11 22 21 10 12 10 13 2" />
    </svg>
  )
}

function IconShield({ size = 24, className = '' }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={className}>
      <path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z" />
    </svg>
  )
}

function IconPackage({ size = 24, className = '' }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={className}>
      <line x1="16.5" y1="9.4" x2="7.5" y2="4.21" />
      <path d="M21 16V8a2 2 0 0 0-1-1.73l-7-4a2 2 0 0 0-2 0l-7 4A2 2 0 0 0 3 8v8a2 2 0 0 0 1 1.73l7 4a2 2 0 0 0 2 0l7-4A2 2 0 0 0 21 16z" />
      <polyline points="3.27 6.96 12 12.01 20.73 6.96" />
      <line x1="12" y1="22.08" x2="12" y2="12" />
    </svg>
  )
}

function IconChevronDown({ size = 24, className = '' }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={className}>
      <polyline points="6 9 12 15 18 9" />
    </svg>
  )
}

function IconMenu({ size = 24, className = '' }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={className}>
      <line x1="3" y1="12" x2="21" y2="12" />
      <line x1="3" y1="6" x2="21" y2="6" />
      <line x1="3" y1="18" x2="21" y2="18" />
    </svg>
  )
}

function IconX({ size = 24, className = '' }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={className}>
      <line x1="18" y1="6" x2="6" y2="18" />
      <line x1="6" y1="6" x2="18" y2="18" />
    </svg>
  )
}

/* ── Components ───────────────────────────────────────── */

function TerminalDemo() {
  const [started, setStarted] = useState(false)

  const lines = [
    '$ neofetch',
    'OS: CodixOS 1.0.0 x86_64',
    'Kernel: codix-kernel 1.0.0',
    'Shell: codix-sh 1.0.0',
    'CPU: Virtual CPU @ 2.4GHz',
    'Memory: 32MB / 256MB',
    '',
    '$ ls -la',
    'total 24',
    'drwxr-xr-x 2 codix codix 4096 .',
    '-rw-r--r-- 1 codix codix  128 .bashrc',
    'drwxr-xr-x 2 codix codix 4096 Documents',
    'drwxr-xr-x 2 codix codix 4096 Downloads',
    '',
    '$ pkg install vim',
    'Resolving dependencies... done',
    'Installing vim 9.0... done',
    '',
    '$ free -h',
    '              total    used    free',
    'Mem:          256M     32M    224M',
    'Swap:         128M      0M    128M',
  ]

  return (
    <div
      onClick={() => setStarted(true)}
      className="max-w-3xl mx-auto rounded-xl overflow-hidden border border-codix-overlay shadow-2xl cursor-pointer"
      style={{ backgroundColor: 'var(--codix-bg)' }}
    >
      <div className="flex items-center gap-2 px-4 py-2" style={{ backgroundColor: 'var(--codix-surface)' }}>
        <div className="w-3 h-3 rounded-full" style={{ backgroundColor: 'var(--codix-red)' }} />
        <div className="w-3 h-3 rounded-full" style={{ backgroundColor: 'var(--codix-yellow)' }} />
        <div className="w-3 h-3 rounded-full" style={{ backgroundColor: 'var(--codix-green)' }} />
        <span className="ml-2 text-sm" style={{ color: 'var(--codix-subtext)' }}>codix@codixos:~</span>
      </div>
      <div className="p-4 font-mono text-sm h-72 overflow-y-auto" style={{ fontFamily: "'Courier New', monospace" }}>
        {!started ? (
          <div className="flex flex-col items-center justify-center h-full" style={{ color: 'var(--codix-subtext)' }}>
            <IconTerminal size={48} className="mb-4" style={{ color: 'var(--codix-blue)' }} />
            <p>Click to start terminal demo</p>
          </div>
        ) : (
          <>
            {lines.map((line, i) => (
              <div key={i} className="leading-relaxed" style={{ color: line.startsWith('$') ? 'var(--codix-blue)' : line === '' ? 'transparent' : 'var(--codix-subtext)' }}>
                {line.startsWith('$') ? (
                  <>
                    <span style={{ color: 'var(--codix-green)' }}>$</span>
                    <span className="ml-2" style={{ color: 'var(--codix-text)' }}>{line.slice(2)}</span>
                  </>
                ) : (
                  line
                )}
              </div>
            ))}
            <div className="flex items-center mt-1">
              <span style={{ color: 'var(--codix-green)' }}>$</span>
              <span className="ml-2 animate-pulse" style={{ color: 'var(--codix-text)' }}>|</span>
            </div>
          </>
        )}
      </div>
    </div>
  )
}

function Navbar() {
  const [open, setOpen] = useState(false)

  return (
    <nav className="fixed top-0 left-0 right-0 z-50 border-b" style={{ backgroundColor: 'rgba(30,30,46,0.85)', backdropFilter: 'blur(12px)', borderColor: 'var(--codix-overlay)' }}>
      <div className="max-w-6xl mx-auto px-4 sm:px-6 lg:px-8">
        <div className="flex items-center justify-between h-16">
          <a href="#" className="flex items-center gap-2">
            <IconTerminal size={28} className="text-codix-blue" />
            <span className="text-xl font-bold" style={{ color: 'var(--codix-text)' }}>CodixOS</span>
          </a>

          <div className="hidden md:flex items-center gap-8">
            <a href="#features" className="text-sm hover:text-codix-blue transition-colors" style={{ color: 'var(--codix-subtext)' }}>Features</a>
            <a href="#demo" className="text-sm hover:text-codix-blue transition-colors" style={{ color: 'var(--codix-subtext)' }}>Demo</a>
            <a href="#download" className="text-sm hover:text-codix-blue transition-colors" style={{ color: 'var(--codix-subtext)' }}>Download</a>
            <a href="https://github.com/itriedcoding/CodixOS" target="_blank" rel="noopener noreferrer" className="hover:text-codix-blue transition-colors" style={{ color: 'var(--codix-subtext)' }}>
              <IconGithub size={20} />
            </a>
            <a href="#download" className="btn-primary text-sm">Download</a>
          </div>

          <button className="md:hidden" onClick={() => setOpen(!open)} style={{ color: 'var(--codix-text)' }}>
            {open ? <IconX size={24} /> : <IconMenu size={24} />}
          </button>
        </div>
      </div>

      {open && (
        <div className="md:hidden border-t" style={{ backgroundColor: 'var(--codix-surface)', borderColor: 'var(--codix-overlay)' }}>
          <div className="px-4 py-4 space-y-3">
            <a href="#features" className="block text-sm" style={{ color: 'var(--codix-subtext)' }} onClick={() => setOpen(false)}>Features</a>
            <a href="#demo" className="block text-sm" style={{ color: 'var(--codix-subtext)' }} onClick={() => setOpen(false)}>Demo</a>
            <a href="#download" className="block text-sm" style={{ color: 'var(--codix-subtext)' }} onClick={() => setOpen(false)}>Download</a>
          </div>
        </div>
      )}
    </nav>
  )
}

function FeatureCard({ icon: Icon, title, description }) {
  return (
    <div className="card group">
      <div className="mb-4 text-codix-blue group-hover:text-codix-green transition-colors">
        <Icon size={28} />
      </div>
      <h3 className="text-lg font-semibold mb-2" style={{ color: 'var(--codix-text)' }}>{title}</h3>
      <p className="text-sm leading-relaxed" style={{ color: 'var(--codix-subtext)' }}>{description}</p>
    </div>
  )
}

/* ── Page ─────────────────────────────────────────────── */

export default function HomePage() {
  const features = [
    { icon: IconZap, title: 'Lightweight', description: 'Minimal resource usage. Custom kernel optimized for speed and efficiency.' },
    { icon: IconTerminal, title: 'Terminal-Based', description: 'Full-featured shell with command history, tab completion, and 40+ built-in commands.' },
    { icon: IconPackage, title: 'Package Manager', description: 'Easy software installation and management with codix-pkg.' },
    { icon: IconMonitor, title: 'Desktop Environment', description: 'Optional GUI with window manager, file manager, and built-in applications.' },
    { icon: IconShield, title: 'Secure', description: 'Built with security in mind, featuring user permissions and sandboxing.' },
    { icon: IconHardDrive, title: 'Live USB', description: 'Boot directly from USB without installation. Perfect for testing and rescue.' },
  ]

  return (
    <main style={{ minHeight: '100vh' }}>
      <Navbar />

      {/* Hero */}
      <section className="min-h-screen flex items-center justify-center relative overflow-hidden pt-16 px-4">
        <div className="absolute inset-0 pointer-events-none">
          <div className="absolute -top-40 -right-40 w-80 h-80 rounded-full blur-3xl" style={{ backgroundColor: 'rgba(137,180,250,0.15)' }} />
          <div className="absolute -bottom-40 -left-40 w-80 h-80 rounded-full blur-3xl" style={{ backgroundColor: 'rgba(166,227,161,0.15)' }} />
        </div>

        <div className="max-w-5xl mx-auto relative z-10 text-center">
          <h1 className="text-5xl md:text-7xl font-extrabold mb-6 tracking-tight">
            <span className="text-codix-blue">Codix</span><span style={{ color: 'var(--codix-text)' }}>OS</span>
          </h1>
          <p className="text-lg md:text-xl max-w-2xl mx-auto mb-10" style={{ color: 'var(--codix-subtext)' }}>
            A lightweight, terminal-based operating system designed for simplicity and ease of use
          </p>

          <div className="flex flex-col sm:flex-row gap-4 justify-center mb-16">
            <a href="#download" className="btn-primary gap-2">
              <IconDownload size={18} /> Download Now
            </a>
            <a href="#demo" className="btn-secondary gap-2">
              <IconTerminal size={18} /> Live Demo
            </a>
          </div>

          {/* Stats */}
          <div className="grid grid-cols-2 md:grid-cols-4 gap-6 max-w-3xl mx-auto mb-16">
            {[
              { icon: IconCpu, label: 'CPU Usage', value: '< 1%', color: 'text-codix-blue' },
              { icon: IconActivity, label: 'RAM Usage', value: '32MB', color: 'text-codix-green' },
              { icon: IconHardDrive, label: 'Disk Size', value: '128MB', color: 'text-codix-yellow' },
              { icon: IconZap, label: 'Boot Time', value: '2s', color: 'text-codix-red' },
            ].map((s) => (
              <div key={s.label} className="text-center">
                <div className={`flex justify-center mb-2 ${s.color}`}><s.icon size={28} /></div>
                <div className="text-xl font-bold" style={{ color: 'var(--codix-text)' }}>{s.value}</div>
                <div className="text-xs" style={{ color: 'var(--codix-subtext)' }}>{s.label}</div>
              </div>
            ))}
          </div>

          {/* Terminal Demo */}
          <TerminalDemo />

          <div className="mt-12 animate-bounce">
            <IconChevronDown size={28} style={{ color: 'var(--codix-subtext)' }} />
          </div>
        </div>
      </section>

      {/* Features */}
      <section id="features" className="py-20 px-4" style={{ backgroundColor: 'rgba(49,50,68,0.3)' }}>
        <div className="max-w-6xl mx-auto">
          <div className="text-center mb-14">
            <h2 className="text-3xl md:text-4xl font-bold mb-3" style={{ color: 'var(--codix-text)' }}>Features</h2>
            <p className="max-w-xl mx-auto" style={{ color: 'var(--codix-subtext)' }}>
              Everything you need in a modern operating system, without the bloat
            </p>
          </div>
          <div className="grid md:grid-cols-2 lg:grid-cols-3 gap-6">
            {features.map((f, i) => <FeatureCard key={i} {...f} />)}
          </div>
        </div>
      </section>

      {/* Demo */}
      <section id="demo" className="py-20 px-4">
        <div className="max-w-6xl mx-auto">
          <div className="text-center mb-14">
            <h2 className="text-3xl md:text-4xl font-bold mb-3" style={{ color: 'var(--codix-text)' }}>See It In Action</h2>
            <p className="max-w-xl mx-auto" style={{ color: 'var(--codix-subtext)' }}>
              Try CodixOS directly in your browser
            </p>
          </div>
          <TerminalDemo />
        </div>
      </section>

      {/* Download */}
      <section id="download" className="py-20 px-4" style={{ backgroundColor: 'rgba(49,50,68,0.3)' }}>
        <div className="max-w-6xl mx-auto">
          <div className="text-center mb-14">
            <h2 className="text-3xl md:text-4xl font-bold mb-3" style={{ color: 'var(--codix-text)' }}>Download CodixOS</h2>
            <p className="max-w-xl mx-auto" style={{ color: 'var(--codix-subtext)' }}>
              Choose the option that works best for you
            </p>
          </div>
          <div className="grid md:grid-cols-3 gap-6">
            {[
              { icon: IconHardDrive, name: 'ISO Image', desc: 'Bootable ISO for VMs and live USB', size: '128 MB', href: 'https://github.com/itriedcoding/CodixOS' },
              { icon: IconGithub, name: 'Source Code', desc: 'Build from source', size: '15 MB', href: 'https://github.com/itriedcoding/CodixOS' },
              { icon: IconMonitor, name: 'Windows Installer', desc: 'Install via WSL', size: '50 MB', href: 'https://github.com/itriedcoding/CodixOS' },
            ].map((d, i) => (
              <div key={i} className="card text-center">
                <div className="flex justify-center mb-4 text-codix-blue"><d.icon size={40} /></div>
                <h3 className="text-lg font-semibold mb-1" style={{ color: 'var(--codix-text)' }}>{d.name}</h3>
                <p className="text-sm mb-2" style={{ color: 'var(--codix-subtext)' }}>{d.desc}</p>
                <p className="text-xs mb-4" style={{ color: 'var(--codix-subtext)' }}>{d.size}</p>
                <a href={d.href} target="_blank" rel="noopener noreferrer" className="btn-primary w-full text-sm">Download</a>
              </div>
            ))}
          </div>
        </div>
      </section>

      {/* Footer */}
      <footer className="py-10 px-4 border-t" style={{ borderColor: 'var(--codix-overlay)' }}>
        <div className="max-w-6xl mx-auto">
          <div className="grid md:grid-cols-4 gap-8 mb-8">
            <div>
              <div className="flex items-center gap-2 mb-3">
                <IconTerminal size={22} className="text-codix-blue" />
                <span className="text-lg font-bold" style={{ color: 'var(--codix-text)' }}>CodixOS</span>
              </div>
              <p className="text-sm" style={{ color: 'var(--codix-subtext)' }}>A lightweight, terminal-based operating system designed for simplicity.</p>
            </div>
            <div>
              <h4 className="font-semibold mb-3 text-sm" style={{ color: 'var(--codix-text)' }}>Product</h4>
              <ul className="space-y-2 text-sm" style={{ color: 'var(--codix-subtext)' }}>
                <li><a href="#features" className="hover:text-codix-blue transition-colors">Features</a></li>
                <li><a href="#download" className="hover:text-codix-blue transition-colors">Download</a></li>
                <li><a href="#demo" className="hover:text-codix-blue transition-colors">Demo</a></li>
              </ul>
            </div>
            <div>
              <h4 className="font-semibold mb-3 text-sm" style={{ color: 'var(--codix-text)' }}>Community</h4>
              <ul className="space-y-2 text-sm" style={{ color: 'var(--codix-subtext)' }}>
                <li><a href="https://github.com/itriedcoding/CodixOS" target="_blank" rel="noopener noreferrer" className="hover:text-codix-blue transition-colors">GitHub</a></li>
              </ul>
            </div>
            <div>
              <h4 className="font-semibold mb-3 text-sm" style={{ color: 'var(--codix-text)' }}>Legal</h4>
              <ul className="space-y-2 text-sm" style={{ color: 'var(--codix-subtext)' }}>
                <li><a href="https://github.com/itriedcoding/CodixOS/blob/master/LICENSE" target="_blank" rel="noopener noreferrer" className="hover:text-codix-blue transition-colors">License (MIT)</a></li>
              </ul>
            </div>
          </div>
          <div className="border-t pt-6 text-center text-xs" style={{ borderColor: 'var(--codix-overlay)', color: 'var(--codix-subtext)' }}>
            CodixOS. All rights reserved.
          </div>
        </div>
      </footer>
    </main>
  )
}
