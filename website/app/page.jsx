'use client'

import { useState, useRef, useEffect } from 'react'

/* ── SVG Icons ────────────────────────────────────────── */

const Icon = {
  Terminal: (p) => <svg width={p.size||24} height={p.size||24} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={p.className}><polyline points="4 17 10 11 4 5"/><line x1="12" y1="19" x2="20" y2="19"/></svg>,
  Download: (p) => <svg width={p.size||24} height={p.size||24} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={p.className}><path d="M21 15v4a2 2 0 01-2 2H5a2 2 0 01-2-2v-4"/><polyline points="7 10 12 15 17 10"/><line x1="12" y1="15" x2="12" y2="3"/></svg>,
  Github: (p) => <svg width={p.size||24} height={p.size||24} viewBox="0 0 24 24" fill="currentColor" className={p.className}><path d="M12 0c-6.626 0-12 5.373-12 12 0 5.302 3.438 9.8 8.207 11.387.599.111.793-.261.793-.577v-2.234c-3.338.726-4.033-1.416-4.033-1.416-.546-1.387-1.333-1.756-1.333-1.756-1.089-.745.083-.729.083-.729 1.205.084 1.839 1.237 1.839 1.237 1.07 1.834 2.807 1.304 3.492.997.107-.775.418-1.305.762-1.604-2.665-.305-5.467-1.334-5.467-5.931 0-1.311.469-2.381 1.236-3.221-.124-.303-.535-1.524.117-3.176 0 0 1.008-.322 3.301 1.23.957-.266 1.983-.399 3.003-.404 1.02.005 2.047.138 3.006.404 2.291-1.552 3.297-1.23 3.297-1.23.653 1.653.242 2.874.118 3.176.77.84 1.235 1.911 1.235 3.221 0 4.609-2.807 5.624-5.479 5.921.43.372.823 1.102.823 2.222v3.293c0 .319.192.694.801.576 4.765-1.589 8.199-6.086 8.199-11.386 0-6.627-5.373-12-12-12z"/></svg>,
  Cpu: (p) => <svg width={p.size||24} height={p.size||24} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={p.className}><rect x="4" y="4" width="16" height="16" rx="2"/><rect x="9" y="9" width="6" height="6"/><line x1="9" y1="1" x2="9" y2="4"/><line x1="15" y1="1" x2="15" y2="4"/><line x1="9" y1="20" x2="9" y2="23"/><line x1="15" y1="20" x2="15" y2="23"/><line x1="20" y1="9" x2="23" y2="9"/><line x1="20" y1="14" x2="23" y2="14"/><line x1="1" y1="9" x2="4" y2="9"/><line x1="1" y1="14" x2="4" y2="14"/></svg>,
  HardDrive: (p) => <svg width={p.size||24} height={p.size||24} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={p.className}><line x1="22" y1="12" x2="2" y2="12"/><path d="M5.45 5.11L2 12v6a2 2 0 002 2h16a2 2 0 002-2v-6l-3.45-6.89A2 2 0 0016.76 4H7.24a2 2 0 00-1.79 1.11z"/><line x1="6" y1="16" x2="6.01" y2="16"/><line x1="10" y1="16" x2="10.01" y2="16"/></svg>,
  Activity: (p) => <svg width={p.size||24} height={p.size||24} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={p.className}><polyline points="22 12 18 12 15 21 9 3 6 12 2 12"/></svg>,
  Monitor: (p) => <svg width={p.size||24} height={p.size||24} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={p.className}><rect x="2" y="3" width="20" height="14" rx="2"/><line x1="8" y1="21" x2="16" y2="21"/><line x1="12" y1="17" x2="12" y2="21"/></svg>,
  Zap: (p) => <svg width={p.size||24} height={p.size||24} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={p.className}><polygon points="13 2 3 14 12 14 11 22 21 10 12 10 13 2"/></svg>,
  Shield: (p) => <svg width={p.size||24} height={p.size||24} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={p.className}><path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"/></svg>,
  Package: (p) => <svg width={p.size||24} height={p.size||24} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={p.className}><line x1="16.5" y1="9.4" x2="7.5" y2="4.21"/><path d="M21 16V8a2 2 0 00-1-1.73l-7-4a2 2 0 00-2 0l-7 4A2 2 0 003 8v8a2 2 0 001 1.73l7 4a2 2 0 002 0l7-4A2 2 0 0021 16z"/><polyline points="3.27 6.96 12 12.01 20.73 6.96"/><line x1="12" y1="22.08" x2="12" y2="12"/></svg>,
  ChevronDown: (p) => <svg width={p.size||24} height={p.size||24} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={p.className}><polyline points="6 9 12 15 18 9"/></svg>,
  Menu: (p) => <svg width={p.size||24} height={p.size||24} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={p.className}><line x1="3" y1="12" x2="21" y2="12"/><line x1="3" y1="6" x2="21" y2="6"/><line x1="3" y1="18" x2="21" y2="18"/></svg>,
  X: (p) => <svg width={p.size||24} height={p.size||24} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" className={p.className}><line x1="18" y1="6" x2="6" y2="18"/><line x1="6" y1="6" x2="18" y2="18"/></svg>,
}

/* ── Color tokens ─────────────────────────────────────── */

const C = {
  bg: '#1e1e2e', surface: '#313244', overlay: '#45475a',
  text: '#cdd6f4', sub: '#a6adc8',
  blue: '#89b4fa', green: '#a6e3a1', yellow: '#f9e2af',
  red: '#f38ba8', magenta: '#f5c2e7', teal: '#94e2d5', peach: '#fab387',
}

/* ── Terminal Engine ───────────────────────────────────── */

const FILESYSTEM = {
  '~': { type: 'dir', children: ['Documents', 'Downloads', '.bashrc', 'README.md', 'codixos.conf'] },
  '~/Documents': { type: 'dir', children: ['notes.txt'] },
  '~/Downloads': { type: 'dir', children: [] },
  '~/.bashrc': { type: 'file', content: '# CodixOS Shell Config\nexport PS1="codix@codixos:~$ "\nexport PATH=/usr/local/bin:$PATH\nalias ll="ls -la"' },
  '~/README.md': { type: 'file', content: '# CodixOS\nA lightweight, terminal-based operating system.\nBuilt from scratch with a custom kernel.' },
  '~/codixos.conf': { type: 'file', content: 'kernel.codix.version=1.0.0\nshell.codix.version=1.0.0\ndisplay.resolution=1920x1080\ndisplay.theme=catppuccin-mocha' },
  '~/Documents/notes.txt': { type: 'file', content: 'TODO:\n- Finish kernel memory manager\n- Add TCP/IP stack\n- Write man pages' },
}

let cwd = '~'

function resolvePath(p) {
  if (p === '~' || p === '') return '~'
  if (p.startsWith('~/')) return p
  if (p === '..') {
    const parts = cwd.split('/')
    parts.pop()
    return parts.length ? parts.join('/') : '~'
  }
  if (cwd === '~') return `~/${p}`
  return `${cwd}/${p}`
}

function cmdHelp() {
  return `Available commands:

  help              Show this help message
  neofetch          Display system information
  ls [path]         List directory contents
  cat <file>        Display file contents
  pwd               Print working directory
  whoami            Display current user
  hostname          Display hostname
  uname [-a]        Display system information
  date              Display current date and time
  uptime            Display system uptime
  free              Display memory usage
  ps                Display running processes
  df                Display disk usage
  echo <text>       Print text
  clear             Clear the terminal
  pkg install <pkg> Install a package
  pkg list          List installed packages
  history           Show command history
  theme             Show color palette
  exit              Close terminal (just kidding)`
}

function cmdNeofetch() {
  return `       _____      _               ____   _____ 
      / ____|    | |             |  _ \\ / ____|
     | |     ___ | | ___  _ __   | |_) | (___  
     | |    / _ \\| |/ _ \\| '__|  |  _ < \\___ \\ 
     | |___| (_) | | (_) | |     | |_) |____) |
      \\_____|\\___/|_|\\___/|_|     |____/|_____/ 

${C.blue}OS${C.text}:       CodixOS 1.0.0 x86_64
${C.blue}Host${C.text}:     CodixOS Virtual Machine
${C.blue}Kernel${C.text}:   codix-kernel 1.0.0
${C.blue}Uptime${C.text}:   4 hours, 23 mins
${C.blue}Shell${C.text}:    codix-sh 1.0.0
${C.blue}Terminal${C.text}: codix-term 1.0.0
${C.blue}CPU${C.text}:      Virtual CPU @ 2.4GHz
${C.blue}Memory${C.text}:   32MiB / 256MiB

${C.red}███${C.green}███${C.yellow}███${C.blue}███${C.magenta}███${C.teal}███${C.text}`
}

function cmdLs(args) {
  const target = args[0] ? resolvePath(args[0]) : cwd
  const entry = FILESYSTEM[target]
  if (!entry) return `ls: cannot access '${args[0] || target}': No such file or directory`
  if (entry.type !== 'dir') return args[0] || target
  if (entry.children.length === 0) return ''
  return entry.children.map(c => {
    const childPath = target === '~' ? `~/${c}` : `${target}/${c}`
    const child = FILESYSTEM[childPath]
    const isDir = child && child.type === 'dir'
    return isDir ? `${C.blue}${c}/${C.text}` : c
  }).join('  ')
}

function cmdCat(args) {
  if (!args[0]) return 'cat: missing operand'
  const path = resolvePath(args[0])
  const entry = FILESYSTEM[path]
  if (!entry) return `cat: ${args[0]}: No such file or directory`
  if (entry.type === 'dir') return `cat: ${args[0]}: Is a directory`
  return entry.content
}

function cmdPkg(args) {
  const pkgs = { vim: '9.0', nano: '7.2', gcc: '13.1', git: '2.42', python: '3.11', node: '20.5', htop: '3.3' }
  if (args[0] === 'install') {
    const name = args[1]
    if (!name) return 'Usage: pkg install <package>'
    if (pkgs[name]) return `Resolving dependencies... done\nFetching ${name}... done\nInstalling ${name} ${pkgs[name]}... done\n${name} installed successfully.`
    return `error: package '${name}' not found in registry.\nAvailable: ${Object.keys(pkgs).join(', ')}`
  }
  if (args[0] === 'list') {
    return `Installed packages:\n  codix-kernel  1.0.0\n  codix-sh      1.0.0\n  codix-term    1.0.0\n  codix-pkg     1.0.0\n  base-utils    1.0.0`
  }
  return 'Usage: pkg <install|list>'
}

function cmdFree() {
  return `              total        used        free      shared  buff/cache   available
Mem:        262144       32768      196608        2048       32768      229376
Swap:       131072           0      131072`
}

function cmdPs() {
  return `  PID TTY          TIME CMD
    1 ?        00:00:02 codix-init
   12 ?        00:00:00 codix-sh
   45 ?        00:00:01 codix-wm
   67 ?        00:00:00 codix-term
  102 ?        00:00:00 ps`
}

function cmdDf() {
  return `Filesystem     1K-blocks   Used Available Use% Mounted on
/dev/sda1       131072  32768    98304  25% /
tmpfs            65536       0     65536   0% /dev/shm
/dev/sda2       262144  12288   249856   5% /home`
}

function cmdTheme() {
  return `Catppuccin Mocha Palette:

  ${C.red}██${C.text} Red       #f38ba8
  ${C.peach}██${C.text} Peach     #fab387
  ${C.yellow}██${C.text} Yellow    #f9e2af
  ${C.green}██${C.text} Green     #a6e3a1
  ${C.teal}██${C.text} Teal      #94e2d5
  ${C.blue}██${C.text} Blue      #89b4fa
  ${C.magenta}██${C.text} Magenta   #f5c2e7
  ${C.text}██${C.text} Text      #cdd6f4
  ${C.sub}██${C.text} Subtext   #a6adc8
  ${C.surface}██${C.text} Surface   #313244
  ${C.overlay}██${C.text} Overlay   #45475a
  ${C.bg}██${C.text} BG        #1e1e2e`
}

const PROCESSORS = {
  help: () => cmdHelp(),
  neofetch: () => cmdNeofetch(),
  ls: (a) => cmdLs(a),
  cat: (a) => cmdCat(a),
  pwd: () => cwd,
  whoami: () => 'codix',
  hostname: () => 'codixos',
  uname: (a) => a[0] === '-a' ? 'CodixOS codixos 1.0.0 #1 SMP PREEMPT_DYNAMIC x86_64 GNU/Codix' : 'CodixOS',
  date: () => new Date().toString(),
  uptime: () => ' 06:42:33 up 4:23, 1 user, load average: 0.12, 0.08, 0.05',
  free: () => cmdFree(),
  ps: () => cmdPs(),
  df: () => cmdDf(),
  echo: (a) => a.join(' '),
  clear: () => '__CLEAR__',
  pkg: (a) => cmdPkg(a),
  history: () => null,
  theme: () => cmdTheme(),
  exit: () => 'Nice try. This terminal has no escape.',
}

function processCommand(input) {
  const parts = input.trim().split(/\s+/)
  const cmd = parts[0]
  const args = parts.slice(1)

  if (!cmd) return ''

  // Handle cd
  if (cmd === 'cd') {
    const target = args[0] ? resolvePath(args[0]) : '~'
    if (FILESYSTEM[target] && FILESYSTEM[target].type === 'dir') {
      cwd = target
      return null
    }
    return `cd: ${args[0]}: No such directory`
  }

  const handler = PROCESSORS[cmd]
  if (handler) return handler(args)
  return `codix: command not found: ${cmd}\nType 'help' for available commands.`
}

/* ── Interactive Terminal Component ────────────────────── */

function InteractiveTerminal() {
  const [lines, setLines] = useState([
    { type: 'output', text: `Welcome to CodixOS Terminal v1.0.0\nType 'help' for available commands.\n` },
  ])
  const [input, setInput] = useState('')
  const [history, setHistory] = useState([])
  const [historyIdx, setHistoryIdx] = useState(-1)
  const bottomRef = useRef(null)
  const inputRef = useRef(null)

  useEffect(() => {
    bottomRef.current?.scrollIntoView({ behavior: 'smooth' })
  }, [lines])

  const focus = () => inputRef.current?.focus()

  function exec(e) {
    e.preventDefault()
    const raw = input.trim()
    setInput('')

    if (!raw) {
      setLines(p => [...p, { type: 'prompt', text: raw }])
      return
    }

    setHistory(h => [...h, raw])
    setHistoryIdx(-1)

    const result = processCommand(raw)

    if (result === '__CLEAR__') {
      setLines([])
      return
    }

    setLines(p => [
      ...p,
      { type: 'prompt', text: raw },
      ...(result !== null ? [{ type: 'output', text: result }] : []),
    ])
  }

  function keyDown(e) {
    if (e.key === 'ArrowUp') {
      e.preventDefault()
      const newIdx = historyIdx < history.length - 1 ? historyIdx + 1 : historyIdx
      setHistoryIdx(newIdx)
      setInput(history[history.length - 1 - newIdx] || '')
    }
    if (e.key === 'ArrowDown') {
      e.preventDefault()
      const newIdx = historyIdx > 0 ? historyIdx - 1 : -1
      setHistoryIdx(newIdx)
      setInput(newIdx >= 0 ? history[history.length - 1 - newIdx] : '')
    }
  }

  return (
    <div
      onClick={focus}
      className="max-w-3xl mx-auto rounded-xl overflow-hidden border shadow-2xl cursor-text"
      style={{ borderColor: C.overlay, backgroundColor: C.bg }}
    >
      {/* Title bar */}
      <div className="flex items-center gap-2 px-4 py-2.5" style={{ backgroundColor: C.surface }}>
        <div className="w-3 h-3 rounded-full" style={{ backgroundColor: C.red }} />
        <div className="w-3 h-3 rounded-full" style={{ backgroundColor: C.yellow }} />
        <div className="w-3 h-3 rounded-full" style={{ backgroundColor: C.green }} />
        <span className="ml-2 text-sm select-none" style={{ color: C.sub }}>codix@codixos — codix-term</span>
      </div>

      {/* Body */}
      <div className="p-4 font-mono text-sm leading-relaxed h-96 overflow-y-auto" style={{ fontFamily: "'Courier New', Consolas, monospace" }}>
        {lines.map((line, i) => (
          <div key={i}>
            {line.type === 'prompt' ? (
              <div>
                <span style={{ color: C.green }}>codix</span>
                <span style={{ color: C.text }}>@</span>
                <span style={{ color: C.blue }}>codixos</span>
                <span style={{ color: C.text }}>:{cwd} $ </span>
                <span style={{ color: C.text }}>{line.text}</span>
              </div>
            ) : (
              <pre className="whitespace-pre-wrap" style={{ color: C.sub }}>{line.text}</pre>
            )}
          </div>
        ))}

        {/* Active prompt */}
        <div className="flex items-center">
          <span style={{ color: C.green }}>codix</span>
          <span style={{ color: C.text }}>@</span>
          <span style={{ color: C.blue }}>codixos</span>
          <span style={{ color: C.text }}>:{cwd} $ </span>
          <form onSubmit={exec} className="flex-1">
            <input
              ref={inputRef}
              autoFocus
              value={input}
              onChange={(e) => setInput(e.target.value)}
              onKeyDown={keyDown}
              className="flex-1 bg-transparent outline-none font-mono text-sm w-full"
              style={{ color: C.text, fontFamily: "'Courier New', Consolas, monospace" }}
              spellCheck={false}
              autoComplete="off"
            />
          </form>
        </div>
        <div ref={bottomRef} />
      </div>
    </div>
  )
}

/* ── Navbar ────────────────────────────────────────────── */

function Navbar() {
  const [open, setOpen] = useState(false)
  return (
    <nav className="fixed top-0 left-0 right-0 z-50 border-b" style={{ backgroundColor: 'rgba(30,30,46,0.9)', backdropFilter: 'blur(12px)', borderColor: C.overlay }}>
      <div className="max-w-6xl mx-auto px-4 sm:px-6 lg:px-8">
        <div className="flex items-center justify-between h-14">
          <a href="#" className="flex items-center gap-2">
            <Icon.Terminal size={24} className="text-codix-blue" />
            <span className="text-lg font-bold" style={{ color: C.text }}>CodixOS</span>
          </a>
          <div className="hidden md:flex items-center gap-6">
            <a href="#features" className="text-sm hover:text-codix-blue transition-colors" style={{ color: C.sub }}>Features</a>
            <a href="#terminal" className="text-sm hover:text-codix-blue transition-colors" style={{ color: C.sub }}>Terminal</a>
            <a href="#download" className="text-sm hover:text-codix-blue transition-colors" style={{ color: C.sub }}>Download</a>
            <a href="https://github.com/itriedcoding/CodixOS" target="_blank" rel="noopener noreferrer" className="hover:text-codix-blue transition-colors" style={{ color: C.sub }}><Icon.Github size={18} /></a>
            <a href="#download" className="btn-primary text-sm py-2 px-5">Download</a>
          </div>
          <button className="md:hidden" onClick={() => setOpen(!open)} style={{ color: C.text }}>
            {open ? <Icon.X size={22} /> : <Icon.Menu size={22} />}
          </button>
        </div>
      </div>
      {open && (
        <div className="md:hidden border-t px-4 py-3 space-y-2" style={{ backgroundColor: C.surface, borderColor: C.overlay }}>
          <a href="#features" className="block text-sm py-1" style={{ color: C.sub }} onClick={() => setOpen(false)}>Features</a>
          <a href="#terminal" className="block text-sm py-1" style={{ color: C.sub }} onClick={() => setOpen(false)}>Terminal</a>
          <a href="#download" className="block text-sm py-1" style={{ color: C.sub }} onClick={() => setOpen(false)}>Download</a>
        </div>
      )}
    </nav>
  )
}

/* ── Page ──────────────────────────────────────────────── */

export default function HomePage() {
  return (
    <main style={{ minHeight: '100vh' }}>
      <Navbar />

      {/* Hero */}
      <section className="min-h-screen flex items-center justify-center relative overflow-hidden pt-14 px-4">
        <div className="absolute inset-0 pointer-events-none">
          <div className="absolute -top-32 -right-32 w-72 h-72 rounded-full blur-3xl" style={{ backgroundColor: 'rgba(137,180,250,0.12)' }} />
          <div className="absolute -bottom-32 -left-32 w-72 h-72 rounded-full blur-3xl" style={{ backgroundColor: 'rgba(166,227,161,0.12)' }} />
        </div>
        <div className="max-w-4xl mx-auto relative z-10 text-center">
          <h1 className="text-5xl md:text-7xl font-extrabold mb-5 tracking-tight">
            <span className="text-codix-blue">Codix</span><span style={{ color: C.text }}>OS</span>
          </h1>
          <p className="text-lg md:text-xl max-w-xl mx-auto mb-8" style={{ color: C.sub }}>
            A lightweight, terminal-based operating system built from scratch
          </p>
          <div className="flex flex-col sm:flex-row gap-3 justify-center mb-14">
            <a href="#download" className="btn-primary gap-2"><Icon.Download size={18} /> Download</a>
            <a href="#terminal" className="btn-secondary gap-2"><Icon.Terminal size={18} /> Try Terminal</a>
          </div>

          {/* Stats */}
          <div className="grid grid-cols-2 md:grid-cols-4 gap-4 max-w-2xl mx-auto mb-14">
            {[
              { icon: Icon.Cpu, label: 'CPU', value: '< 1%', c: 'text-codix-blue' },
              { icon: Icon.Activity, label: 'RAM', value: '32MB', c: 'text-codix-green' },
              { icon: Icon.HardDrive, label: 'Disk', value: '128MB', c: 'text-codix-yellow' },
              { icon: Icon.Zap, label: 'Boot', value: '2s', c: 'text-codix-red' },
            ].map((s) => (
              <div key={s.label} className="text-center p-3 rounded-lg" style={{ backgroundColor: 'rgba(49,50,68,0.4)' }}>
                <div className={`flex justify-center mb-1 ${s.c}`}><s.icon size={22} /></div>
                <div className="text-lg font-bold" style={{ color: C.text }}>{s.value}</div>
                <div className="text-xs" style={{ color: C.sub }}>{s.label}</div>
              </div>
            ))}
          </div>

          <div className="animate-bounce opacity-40">
            <Icon.ChevronDown size={24} style={{ color: C.sub }} />
          </div>
        </div>
      </section>

      {/* Features */}
      <section id="features" className="py-16 px-4" style={{ backgroundColor: 'rgba(49,50,68,0.2)' }}>
        <div className="max-w-6xl mx-auto">
          <h2 className="text-2xl md:text-3xl font-bold text-center mb-10" style={{ color: C.text }}>Features</h2>
          <div className="grid md:grid-cols-2 lg:grid-cols-3 gap-5">
            {[
              { icon: Icon.Zap, title: 'Lightweight', desc: 'Minimal resource footprint. Runs on as little as 256MB RAM.' },
              { icon: Icon.Terminal, title: 'Terminal-First', desc: '40+ built-in commands. Tab completion. Command history.' },
              { icon: Icon.Package, title: 'Package Manager', desc: 'Install software with codix-pkg. Simple dependency resolution.' },
              { icon: Icon.Monitor, title: 'Desktop Environment', desc: 'Optional GUI with window manager and built-in applications.' },
              { icon: Icon.Shield, title: 'Secure by Default', desc: 'User permissions, process isolation, and sandboxing built in.' },
              { icon: Icon.HardDrive, title: 'Bootable ISO', desc: 'Create a bootable USB. Run live without installation.' },
            ].map((f, i) => (
              <div key={i} className="card group">
                <div className="mb-3 text-codix-blue group-hover:text-codix-green transition-colors"><f.icon size={24} /></div>
                <h3 className="text-base font-semibold mb-1" style={{ color: C.text }}>{f.title}</h3>
                <p className="text-sm leading-relaxed" style={{ color: C.sub }}>{f.desc}</p>
              </div>
            ))}
          </div>
        </div>
      </section>

      {/* Interactive Terminal */}
      <section id="terminal" className="py-16 px-4">
        <div className="max-w-6xl mx-auto">
          <h2 className="text-2xl md:text-3xl font-bold text-center mb-3" style={{ color: C.text }}>Try It Yourself</h2>
          <p className="text-center mb-8 text-sm" style={{ color: C.sub }}>Interactive terminal — type commands like <code className="px-1.5 py-0.5 rounded text-xs" style={{ backgroundColor: C.surface, color: C.blue }}>help</code> <code className="px-1.5 py-0.5 rounded text-xs" style={{ backgroundColor: C.surface, color: C.blue }}>neofetch</code> <code className="px-1.5 py-0.5 rounded text-xs" style={{ backgroundColor: C.surface, color: C.blue }}>ls</code> <code className="px-1.5 py-0.5 rounded text-xs" style={{ backgroundColor: C.surface, color: C.blue }}>cat</code></p>
          <InteractiveTerminal />
        </div>
      </section>

      {/* Download */}
      <section id="download" className="py-16 px-4" style={{ backgroundColor: 'rgba(49,50,68,0.2)' }}>
        <div className="max-w-4xl mx-auto">
          <h2 className="text-2xl md:text-3xl font-bold text-center mb-10" style={{ color: C.text }}>Download CodixOS</h2>
          <div className="grid md:grid-cols-3 gap-5">
            {[
              { icon: Icon.HardDrive, name: 'ISO Image', desc: 'Bootable ISO for VMs and live USB', size: '128 MB', href: 'https://github.com/itriedcoding/CodixOS' },
              { icon: Icon.Github, name: 'Source Code', desc: 'Build from source', size: '15 MB', href: 'https://github.com/itriedcoding/CodixOS' },
              { icon: Icon.Monitor, name: 'WSL Installer', desc: 'Run inside Windows Subsystem for Linux', size: '50 MB', href: 'https://github.com/itriedcoding/CodixOS' },
            ].map((d, i) => (
              <div key={i} className="card text-center">
                <div className="flex justify-center mb-3 text-codix-blue"><d.icon size={36} /></div>
                <h3 className="text-base font-semibold mb-1" style={{ color: C.text }}>{d.name}</h3>
                <p className="text-xs mb-1" style={{ color: C.sub }}>{d.desc}</p>
                <p className="text-xs mb-3" style={{ color: C.overlay }}>{d.size}</p>
                <a href={d.href} target="_blank" rel="noopener noreferrer" className="btn-primary w-full text-sm py-2">Download</a>
              </div>
            ))}
          </div>
        </div>
      </section>

      {/* Footer */}
      <footer className="py-8 px-4 border-t" style={{ borderColor: C.overlay }}>
        <div className="max-w-6xl mx-auto flex flex-col md:flex-row items-center justify-between gap-4">
          <div className="flex items-center gap-2">
            <Icon.Terminal size={18} className="text-codix-blue" />
            <span className="text-sm font-semibold" style={{ color: C.text }}>CodixOS</span>
          </div>
          <div className="flex items-center gap-6 text-xs" style={{ color: C.sub }}>
            <a href="#features" className="hover:text-codix-blue transition-colors">Features</a>
            <a href="#terminal" className="hover:text-codix-blue transition-colors">Terminal</a>
            <a href="#download" className="hover:text-codix-blue transition-colors">Download</a>
            <a href="https://github.com/itriedcoding/CodixOS" target="_blank" rel="noopener noreferrer" className="hover:text-codix-blue transition-colors">GitHub</a>
          </div>
          <p className="text-xs" style={{ color: C.overlay }}>MIT License</p>
        </div>
      </footer>
    </main>
  )
}
