'use client'

import { useState, useRef, useEffect, useCallback } from 'react'

/* ── Color tokens ─────────────────────────────────────── */

const C = {
  bg: '#1e1e2e', surface: '#313244', overlay: '#45475a',
  text: '#cdd6f4', sub: '#a6adc8',
  blue: '#89b4fa', green: '#a6e3a1', yellow: '#f9e2af',
  red: '#f38ba8', magenta: '#f5c2e7', teal: '#94e2d5', peach: '#fab387',
}

/* ── SVG Icons (inline, zero deps) ────────────────────── */

function Svg({ d, size = 24, fill = 'none', stroke = 'currentColor', sw = 2, cl = '' }) {
  return <svg width={size} height={size} viewBox="0 0 24 24" fill={fill} stroke={stroke} strokeWidth={sw} strokeLinecap="round" strokeLinejoin="round" className={cl}><path d={d}/></svg>
}

const Ico = {
  Terminal: (p) => <Svg size={p.s||24} cl={p.c} d="polyline points='4 17 10 11 4 5' /><line x1='12' y1='19' x2='20' y2='19'/>" />,
  Download: (p) => <Svg size={p.s||24} cl={p.c} d="M21 15v4a2 2 0 01-2 2H5a2 2 0 01-2-2v-4M7 10l5 5 5-5M12 15V3" />,
  Github: (p) => <svg width={p.s||24} height={p.s||24} viewBox="0 0 24 24" fill="currentColor" className={p.c}><path d="M12 0c-6.626 0-12 5.373-12 12 0 5.302 3.438 9.8 8.207 11.387.599.111.793-.261.793-.577v-2.234c-3.338.726-4.033-1.416-4.033-1.416-.546-1.387-1.333-1.756-1.333-1.756-1.089-.745.083-.729.083-.729 1.205.084 1.839 1.237 1.839 1.237 1.07 1.834 2.807 1.304 3.492.997.107-.775.418-1.305.762-1.604-2.665-.305-5.467-1.334-5.467-5.931 0-1.311.469-2.381 1.236-3.221-.124-.303-.535-1.524.117-3.176 0 0 1.008-.322 3.301 1.23.957-.266 1.983-.399 3.003-.404 1.02.005 2.047.138 3.006.404 2.291-1.552 3.297-1.23 3.297-1.23.653 1.653.242 2.874.118 3.176.77.84 1.235 1.911 1.235 3.221 0 4.609-2.807 5.624-5.479 5.921.43.372.823 1.102.823 2.222v3.293c0 .319.192.694.801.576 4.765-1.589 8.199-6.086 8.199-11.386 0-6.627-5.373-12-12-12z"/></svg>,
  Cpu: (p) => <Svg size={p.s||24} cl={p.c} d="M4 4h16v16H4zM9 9h6v6H9zM9 1v3M15 1v3M9 20v3M15 20v3M20 9h3M20 14h3M1 9h3M1 14h3" />,
  HardDrive: (p) => <Svg size={p.s||24} cl={p.c} d="M22 12H2M5.45 5.11L2 12v6a2 2 0 002 2h16a2 2 0 002-2v-6l-3.45-6.89A2 2 0 0016.76 4H7.24a2 2 0 00-1.79 1.11zM6 16h.01M10 16h.01" />,
  Activity: (p) => <Svg size={p.s||24} cl={p.c} d="M22 12l-4 4-4-4M6 12l4-4 4 4" />,
  Monitor: (p) => <Svg size={p.s||24} cl={p.c} d="M2 3h20v14H2zM8 21h8M12 17v4" />,
  Zap: (p) => <Svg size={p.s||24} cl={p.c} d="M13 2L3 14h9l-1 8 10-12h-9l1-8" />,
  Shield: (p) => <Svg size={p.s||24} cl={p.c} d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z" />,
  Package: (p) => <Svg size={p.s||24} cl={p.c} d="M16.5 9.4L7.5 4.2M21 16V8a2 2 0 00-1-1.73l-7-4a2 2 0 00-2 0l-7 4A2 2 0 003 8v8a2 2 0 001 1.73l7 4a2 2 0 002 0l7-4A2 2 0 0021 16zM3.27 6.96L12 12.01l8.73-5.05M12 22.08V12" />,
  ChevronDown: (p) => <Svg size={p.s||24} cl={p.c} d="M6 9l6 6 6-6" />,
  Menu: (p) => <Svg size={p.s||24} cl={p.c} d="M3 12h18M3 6h18M3 18h18" />,
  X: (p) => <Svg size={p.s||24} cl={p.c} d="M18 6L6 18M6 6l12 12" />,
  Folder: (p) => <Svg size={p.s||24} cl={p.c} d="M22 19a2 2 0 01-2 2H4a2 2 0 01-2-2V5a2 2 0 012-2h5l2 3h9a2 2 0 012 2z" />,
  File: (p) => <Svg size={p.s||24} cl={p.c} d="M14 2H6a2 2 0 00-2 2v16a2 2 0 002 2h12a2 2 0 002-2V8z" />,
  Settings: (p) => <Svg size={p.s||24} cl={p.c} d="M12 15a3 3 0 100-6 3 3 0 000 6zM19.4 15a1.65 1.65 0 00.33 1.82l.06.06a2 2 0 01-2.83 2.83l-.06-.06a1.65 1.65 0 00-1.82-.33 1.65 1.65 0 00-1 1.51V21a2 2 0 01-4 0v-.09A1.65 1.65 0 009 19.4a1.65 1.65 0 00-1.82.33l-.06.06a2 2 0 01-2.83-2.83l.06-.06A1.65 1.65 0 004.68 15a1.65 1.65 0 00-1.51-1H3a2 2 0 010-4h.09A1.65 1.65 0 004.6 9a1.65 1.65 0 00-.33-1.82l-.06-.06a2 2 0 012.83-2.83l.06.06A1.65 1.65 0 009 4.68a1.65 1.65 0 001-1.51V3a2 2 0 014 0v.09a1.65 1.65 0 001 1.51 1.65 1.65 0 001.82-.33l.06-.06a2 2 0 012.83 2.83l-.06.06A1.65 1.65 0 0019.4 9a1.65 1.65 0 001.51 1H21a2 2 0 010 4h-.09a1.65 1.65 0 00-1.51 1z" />,
  TextEditor: (p) => <Svg size={p.s||24} cl={p.c} d="M17 3a2.83 2.83 0 114 4L7.5 20.5 2 22l1.5-5.5L17 3z" />,
  Grid: (p) => <Svg size={p.s||24} cl={p.c} d="M3 3h7v7H3zM14 3h7v7h-7zM3 14h7v7H3zM14 14h7v7h-7z" />,
  Wifi: (p) => <Svg size={p.s||24} cl={p.c} d="M1.42 9a16 16 0 0121.16 0M5 12.55a11 11 0 0114.08 0M8.53 16.11a6 6 0 016.95 0M12 20h.01" />,
  Battery: (p) => <Svg size={p.s||24} cl={p.c} d="M17 6H3a2 2 0 00-2 2v8a2 2 0 002 2h14a2 2 0 002-2V8a2 2 0 00-2-2zM23 10v4" />,
}

/* ── Terminal Engine ───────────────────────────────────── */

const FILESYSTEM = {
  '~': { type: 'dir', children: ['Documents', 'Downloads', '.bashrc', 'README.md', 'codixos.conf'] },
  '~/Documents': { type: 'dir', children: ['notes.txt'] },
  '~/Downloads': { type: 'dir', children: [] },
  '~/.bashrc': { type: 'file', content: '# CodixOS Shell Config\nexport PS1="codix@codixos:~$ "\nexport PATH=/usr/local/bin:$PATH' },
  '~/README.md': { type: 'file', content: '# CodixOS\nA lightweight, terminal-based operating system.\nBuilt from scratch with a custom kernel.' },
  '~/codixos.conf': { type: 'file', content: 'kernel.codix.version=1.0.0\ndisplay.resolution=1920x1080\ndisplay.theme=catppuccin-mocha' },
  '~/Documents/notes.txt': { type: 'file', content: 'TODO:\n- Finish kernel memory manager\n- Add TCP/IP stack\n- Write man pages' },
}

let cwd = '~'

function resolvePath(p) {
  if (p === '~' || p === '') return '~'
  if (p.startsWith('~/')) return p
  if (p === '..') { const parts = cwd.split('/'); parts.pop(); return parts.length ? parts.join('/') : '~' }
  return cwd === '~' ? `~/${p}` : `${cwd}/${p}`
}

function processCommand(input) {
  const parts = input.trim().split(/\s+/)
  const cmd = parts[0]; const args = parts.slice(1)
  if (!cmd) return ''
  if (cmd === 'cd') { const t = args[0] ? resolvePath(args[0]) : '~'; if (FILESYSTEM[t]?.type === 'dir') { cwd = t; return null }; return `cd: ${args[0]}: No such directory` }
  if (cmd === 'help') return 'Commands: help, neofetch, ls, cat, pwd, whoami, hostname, uname, date, uptime, free, ps, df, echo, clear, pkg, theme, secureboot, encrypt, exit'
  if (cmd === 'neofetch') return `       _____      _               ____   _____ \n      / ____|    | |             |  _ \\ / ____|\n     | |     ___ | | ___  _ __   | |_) | (___  \n     | |    / _ \\| |/ _ \\| '__|  |  _ < \\___ \\ \n     | |___| (_) | | (_) | |     | |_) |____) |\n      \\_____|\\___/|_|\\___/|_|     |____/|_____/ \n\n${C.blue}OS${C.text}:       CodixOS 1.0.0 x86_64\n${C.blue}Kernel${C.text}:   codix-kernel 1.0.0\n${C.blue}Shell${C.text}:    codix-sh 1.0.0\n${C.blue}CPU${C.text}:      Virtual CPU @ 2.4GHz\n${C.blue}Memory${C.text}:   32MiB / 256MiB\n\n${C.red}███${C.green}███${C.yellow}███${C.blue}███${C.magenta}███${C.teal}███${C.text}`
  if (cmd === 'ls') { const t = args[0] ? resolvePath(args[0]) : cwd; const e = FILESYSTEM[t]; if (!e) return `ls: cannot access '${args[0]}': No such file or directory`; if (e.type !== 'dir') return args[0]; return e.children.map(c => { const cp = t === '~' ? `~/${c}` : `${t}/${c}`; return FILESYSTEM[cp]?.type === 'dir' ? `${C.blue}${c}/${C.text}` : c }).join('  ') }
  if (cmd === 'cat') { if (!args[0]) return 'cat: missing operand'; const p = resolvePath(args[0]); const e = FILESYSTEM[p]; if (!e) return `cat: ${args[0]}: No such file`; if (e.type === 'dir') return `cat: ${args[0]}: Is a directory`; return e.content }
  if (cmd === 'pwd') return cwd
  if (cmd === 'whoami') return 'codix'
  if (cmd === 'hostname') return 'codixos'
  if (cmd === 'uname') return args[0] === '-a' ? 'CodixOS codixos 1.0.0 #1 SMP x86_64 GNU/Codix' : 'CodixOS'
  if (cmd === 'date') return new Date().toString()
  if (cmd === 'uptime') return ' 06:42:33 up 4:23, 1 user, load average: 0.12, 0.08, 0.05'
  if (cmd === 'free') return '              total        used        free\nMem:        262144       32768      196608\nSwap:       131072           0      131072'
  if (cmd === 'ps') return '  PID TTY          TIME CMD\n    1 ?        00:00:02 codix-init\n   12 ?        00:00:00 codix-sh\n   45 ?        00:00:01 codix-wm\n   67 ?        00:00:00 codix-term'
  if (cmd === 'df') return 'Filesystem     1K-blocks   Used Available Use% Mounted on\n/dev/sda1       131072  32768    98304  25% /\n/dev/sda2       262144  12288   249856   5% /home'
  if (cmd === 'echo') return args.join(' ')
  if (cmd === 'clear') return '__CLEAR__'
  if (cmd === 'theme') return `Catppuccin Mocha:\n  ${C.red}██${C.text} Red #f38ba8\n  ${C.blue}██${C.text} Blue #89b4fa\n  ${C.green}██${C.text} Green #a6e3a1\n  ${C.yellow}██${C.text} Yellow #f9e2af\n  ${C.magenta}██${C.text} Magenta #f5c2e7`
  if (cmd === 'exit') return 'Nice try. This terminal has no escape.'
  if (cmd === 'pkg') { if (args[0] === 'list') return 'Installed:\n  codix-kernel 1.0.0\n  codix-sh 1.0.0\n  codix-term 1.0.0\n  codix-pkg 1.0.0'; return 'Usage: pkg <install|list>' }
  if (cmd === 'secureboot') return `\n${C.teal}=== Secure Boot Status ===${C.text}\n\n  State:             ${C.green}ENABLED${C.text}\n  Trusted Keys:      1\n\n  ${C.teal}Boot Chain Verification:${C.text}\n    Firmware:        ${C.green}VERIFIED${C.text}\n    Bootloader:      ${C.green}VERIFIED${C.text}\n    Kernel:          ${C.green}VERIFIED${C.text}\n    Initrd:          ${C.yellow}NOT VERIFIED${C.text}\n\n  Overall Status:    ${C.green}SECURE${C.text}\n`
  if (cmd === 'encrypt') return `\n${C.teal}=== Encrypted Devices ===${C.text}\n\n  DEVICE               MAPPER         CIPHER     KEY      STATUS\n  ------               ------         ------     ---      ------\n  /dev/sda2            codix-data     aes-xts    256      ${C.green}UNLOCKED${C.text}\n  /dev/sda3            -              aes-xts    256      ${C.yellow}LOCKED${C.text}\n`
  return `codix: command not found: ${cmd}\nType 'help' for available commands.`
}

/* ── Interactive Terminal ──────────────────────────────── */

function TermApp({ onOutput }) {
  const [lines, setLines] = useState([{ type: 'o', text: 'Welcome to CodixOS Terminal v1.0.0\nType help for available commands.\n' }])
  const [input, setInput] = useState('')
  const [hist, setHist] = useState([])
  const [hIdx, setHIdx] = useState(-1)
  const bottom = useRef(null)
  const inp = useRef(null)

  useEffect(() => { bottom.current?.scrollIntoView({ behavior: 'smooth' }) }, [lines])

  const focus = () => inp.current?.focus()

  function exec(e) {
    e.preventDefault()
    const raw = input.trim(); setInput('')
    if (!raw) { setLines(p => [...p, { type: 'p', text: '' }]); return }
    setHist(h => [...h, raw]); setHIdx(-1)
    const res = processCommand(raw)
    if (res === '__CLEAR__') { setLines([]); return }
    setLines(p => [...p, { type: 'p', text: raw }, ...(res !== null ? [{ type: 'o', text: res }] : [])])
  }

  function keyDown(e) {
    if (e.key === 'ArrowUp') { e.preventDefault(); const i = hIdx < hist.length - 1 ? hIdx + 1 : hIdx; setHIdx(i); setInput(hist[hist.length - 1 - i] || '') }
    if (e.key === 'ArrowDown') { e.preventDefault(); const i = hIdx > 0 ? hIdx - 1 : -1; setHIdx(i); setInput(i >= 0 ? hist[hist.length - 1 - i] : '') }
  }

  return (
    <div onClick={focus} className="h-full flex flex-col" style={{ backgroundColor: C.bg }}>
      <div className="flex-1 p-3 font-mono text-xs leading-relaxed overflow-y-auto" style={{ fontFamily: "'Courier New', monospace" }}>
        {lines.map((l, i) => (
          <div key={i}>
            {l.type === 'p' ? (
              <div><span style={{ color: C.green }}>codix</span><span style={{ color: C.text }}>@</span><span style={{ color: C.blue }}>codixos</span><span style={{ color: C.text }}>:{cwd} $ </span><span style={{ color: C.text }}>{l.text}</span></div>
            ) : <pre className="whitespace-pre-wrap" style={{ color: C.sub }}>{l.text}</pre>}
          </div>
        ))}
        <div className="flex">
          <span style={{ color: C.green }}>codix</span>
          <span style={{ color: C.text }}>@</span>
          <span style={{ color: C.blue }}>codixos</span>
          <span style={{ color: C.text }}>:{cwd} $ </span>
          <form onSubmit={exec} className="flex-1"><input ref={inp} autoFocus value={input} onChange={e => setInput(e.target.value)} onKeyDown={keyDown} className="flex-1 bg-transparent outline-none font-mono text-xs w-full" style={{ color: C.text, fontFamily: "'Courier New', monospace" }} spellCheck={false} autoComplete="off" /></form>
        </div>
        <div ref={bottom} />
      </div>
    </div>
  )
}

/* ── File Manager App ──────────────────────────────────── */

function FileManagerApp() {
  const [path, setPath] = useState('~')
  const entry = FILESYSTEM[path] || { type: 'dir', children: [] }

  return (
    <div className="h-full flex flex-col" style={{ backgroundColor: C.bg }}>
      <div className="flex items-center gap-1 px-2 py-1.5 text-xs border-b" style={{ borderColor: C.overlay, backgroundColor: C.surface }}>
        {path !== '~' && <button onClick={() => setPath('..')} className="px-2 py-0.5 rounded hover:bg-codix-overlay transition-colors" style={{ color: C.sub }}>Back</button>}
        <span className="ml-2 font-mono" style={{ color: C.text }}>{path}</span>
      </div>
      <div className="flex-1 p-2 overflow-y-auto">
        {entry.children.length === 0 ? (
          <p className="text-xs p-2" style={{ color: C.sub }}>Empty folder</p>
        ) : (
          <div className="space-y-0.5">
            {entry.children.map(c => {
              const cp = path === '~' ? `~/${c}` : `${path}/${c}`
              const isDir = FILESYSTEM[cp]?.type === 'dir'
              return (
                <button key={c} onClick={() => isDir && setPath(cp)} className="flex items-center gap-2 w-full px-2 py-1 rounded text-xs text-left hover:bg-codix-overlay transition-colors" style={{ color: C.text }}>
                  {isDir ? <Icon.Folder size={14} c="text-codix-blue" /> : <Icon.File size={14} c="text-codix-sub" />}
                  {c}{isDir ? '/' : ''}
                </button>
              )
            })}
          </div>
        )}
      </div>
    </div>
  )
}

function Icon({ size, c, children }) {
  return <span className={c} style={{ fontSize: size }}>{children}</span>
}

/* ── Text Editor App ───────────────────────────────────── */

function TextEditorApp() {
  const [text, setText] = useState('# Welcome to CodixOS Text Editor\n# Start typing to edit this file.\n\nfunction main() {\n  print("Hello from CodixOS!")\n}\n\nmain()')
  return (
    <div className="h-full flex flex-col" style={{ backgroundColor: C.bg }}>
      <div className="flex items-center gap-2 px-3 py-1.5 text-xs border-b" style={{ borderColor: C.overlay, backgroundColor: C.surface }}>
        <span style={{ color: C.text }}>untitled.txt</span>
        <span style={{ color: C.sub }}>— 8 lines</span>
      </div>
      <textarea value={text} onChange={e => setText(e.target.value)} className="flex-1 p-3 font-mono text-xs resize-none outline-none leading-relaxed" style={{ backgroundColor: C.bg, color: C.text, fontFamily: "'Courier New', monospace", tabSize: 2 }} spellCheck={false} />
    </div>
  )
}

/* ── Settings App ──────────────────────────────────────── */

function SettingsApp() {
  return (
    <div className="h-full p-4 overflow-y-auto" style={{ backgroundColor: C.bg }}>
      <h3 className="text-sm font-semibold mb-3" style={{ color: C.text }}>System Settings</h3>
      <div className="space-y-3 text-xs" style={{ color: C.sub }}>
        <div className="flex justify-between p-2 rounded" style={{ backgroundColor: C.surface }}><span>Theme</span><span style={{ color: C.blue }}>Catppuccin Mocha</span></div>
        <div className="flex justify-between p-2 rounded" style={{ backgroundColor: C.surface }}><span>Resolution</span><span>1920x1080</span></div>
        <div className="flex justify-between p-2 rounded" style={{ backgroundColor: C.surface }}><span>Font</span><span>Ubuntu Mono</span></div>
        <div className="flex justify-between p-2 rounded" style={{ backgroundColor: C.surface }}><span>Kernel</span><span>codix-kernel 1.0.0</span></div>
        <div className="flex justify-between p-2 rounded" style={{ backgroundColor: C.surface }}><span>Shell</span><span>codix-sh 1.0.0</span></div>
        <div className="flex justify-between p-2 rounded" style={{ backgroundColor: C.surface }}><span>Window Manager</span><span>codix-wm 1.0.0</span></div>
      </div>
    </div>
  )
}

/* ── Window Manager (Draggable Windows) ────────────────── */

const APPS = {
  terminal: { title: 'Terminal', icon: '>', component: TermApp, w: 580, h: 380, x: 60, y: 40 },
  files: { title: 'File Manager', icon: '\u{1F4C1}', component: FileManagerApp, w: 400, h: 340, x: 100, y: 60 },
  editor: { title: 'Text Editor', icon: '\u{270E}', component: TextEditorApp, w: 480, h: 360, x: 160, y: 30 },
  settings: { title: 'Settings', icon: '\u{2699}', component: SettingsApp, w: 340, h: 320, x: 200, y: 50 },
}

function Window({ id, app, onClose, onFocus, focused, zIndex }) {
  const [pos, setPos] = useState({ x: app.x, y: app.y })
  const [size, setSize] = useState({ w: app.w, h: app.h })
  const [maximized, setMaximized] = useState(false)
  const [minimized, setMinimized] = useState(false)
  const dragRef = useRef(null)
  const prev = useRef({ x: app.x, y: app.y, w: app.w, h: app.h })

  function onDragStart(e) {
    e.preventDefault()
    onFocus()
    const startX = e.clientX - pos.x; const startY = e.clientY - pos.y
    function onMove(ev) { setPos({ x: ev.clientX - startX, y: Math.max(0, ev.clientY - startY) }) }
    function onUp() { document.removeEventListener('mousemove', onMove); document.removeEventListener('mouseup', onUp) }
    document.addEventListener('mousemove', onMove)
    document.addEventListener('mouseup', onUp)
  }

  function toggleMax() {
    if (maximized) { setPos({ x: prev.current.x, y: prev.current.y }); setSize({ w: prev.current.w, h: prev.current.h }); setMaximized(false) }
    else { prev.current = { ...pos, ...size }; setPos({ x: 0, y: 0 }); setSize({ w: window.innerWidth, h: window.innerHeight - 48 }); setMaximized(true) }
  }

  if (minimized) return null

  const Comp = app.component

  return (
    <div
      onMouseDown={onFocus}
      className="absolute flex flex-col rounded-lg overflow-hidden border shadow-2xl"
      style={{ left: pos.x, top: pos.y, width: size.w, height: size.h, zIndex, borderColor: focused ? C.blue : C.overlay, backgroundColor: C.surface }}
    >
      {/* Title bar */}
      <div onMouseDown={onDragStart} onDoubleClick={toggleMax} className="flex items-center justify-between px-3 py-1.5 cursor-move select-none shrink-0" style={{ backgroundColor: focused ? C.surface : C.overlay }}>
        <span className="text-xs font-medium" style={{ color: focused ? C.text : C.sub }}>{app.title}</span>
        <div className="flex items-center gap-1.5">
          <button onClick={() => setMinimized(true)} className="w-3 h-3 rounded-full" style={{ backgroundColor: C.yellow }} />
          <button onClick={toggleMax} className="w-3 h-3 rounded-full" style={{ backgroundColor: C.green }} />
          <button onClick={onClose} className="w-3 h-3 rounded-full" style={{ backgroundColor: C.red }} />
        </div>
      </div>
      {/* Content */}
      <div className="flex-1 overflow-hidden">
        <Comp onOutput={() => {}} />
      </div>
    </div>
  )
}

/* ── Desktop Demo ──────────────────────────────────────── */

function DesktopDemo() {
  const [windows, setWindows] = useState({})
  const [topZ, setTopZ] = useState(10)
  const [focusedWin, setFocusedWin] = useState(null)
  const [clock, setClock] = useState('')
  const [startOpen, setStartOpen] = useState(false)

  useEffect(() => {
    function tick() { const d = new Date(); setClock(d.toLocaleTimeString('en-US', { hour: '2-digit', minute: '2-digit', hour12: false })) }
    tick(); const id = setInterval(tick, 1000); return () => clearInterval(id)
  }, [])

  function openApp(id) {
    setWindows(w => ({ ...w, [id]: true }))
    setTopZ(z => z + 1)
    setFocusedWin(id)
    setStartOpen(false)
  }

  function closeApp(id) {
    setWindows(w => { const n = { ...w }; delete n[id]; return n })
    if (focusedWin === id) setFocusedWin(null)
  }

  function focusApp(id) {
    setTopZ(z => z + 1)
    setFocusedWin(id)
  }

  const openApps = Object.keys(windows).filter(k => windows[k])

  return (
    <div className="relative w-full rounded-xl overflow-hidden border shadow-2xl" style={{ borderColor: C.overlay, height: 500 }}>
      {/* Desktop background */}
      <div className="absolute inset-0" style={{ backgroundColor: C.bg, backgroundImage: `radial-gradient(ellipse at 30% 20%, rgba(137,180,250,0.08) 0%, transparent 50%), radial-gradient(ellipse at 70% 80%, rgba(166,227,161,0.06) 0%, transparent 50%)` }} />

      {/* Desktop icons */}
      <div className="absolute top-4 left-4 flex flex-col gap-3 z-[1]">
        {[
          { id: 'terminal', label: 'Terminal', icon: <Ico.Terminal s={28} c="text-codix-green" /> },
          { id: 'files', label: 'Files', icon: <Ico.Folder s={28} c="text-codix-blue" /> },
          { id: 'editor', label: 'Editor', icon: <Ico.TextEditor s={28} c="text-codix-yellow" /> },
          { id: 'settings', label: 'Settings', icon: <Ico.Settings s={28} c="text-codix-sub" /> },
        ].map(d => (
          <button key={d.id} onDoubleClick={() => openApp(d.id)} className="flex flex-col items-center gap-1 p-2 rounded-lg hover:bg-codix-overlay/30 transition-colors w-16">
            {d.icon}
            <span className="text-[10px] leading-tight text-center" style={{ color: C.sub }}>{d.label}</span>
          </button>
        ))}
      </div>

      {/* Windows */}
      {Object.entries(windows).map(([id, open]) => {
        if (!open) return null
        const app = APPS[id]
        return <Window key={id} id={id} app={app} onClose={() => closeApp(id)} onFocus={() => focusApp(id)} focused={focusedWin === id} zIndex={focusedWin === id ? topZ : 5} />
      })}

      {/* Start Menu */}
      {startOpen && (
        <div className="absolute bottom-12 left-0 w-56 rounded-t-lg border overflow-hidden z-[100]" style={{ borderColor: C.overlay, backgroundColor: C.surface }}>
          <div className="p-3 border-b" style={{ borderColor: C.overlay }}>
            <p className="text-xs font-semibold" style={{ color: C.text }}>CodixOS</p>
            <p className="text-[10px]" style={{ color: C.sub }}>v1.0.0</p>
          </div>
          <div className="p-1">
            {[
              { id: 'terminal', label: 'Terminal', icon: <Ico.Terminal s={16} c="text-codix-green" /> },
              { id: 'files', label: 'File Manager', icon: <Ico.Folder s={16} c="text-codix-blue" /> },
              { id: 'editor', label: 'Text Editor', icon: <Ico.TextEditor s={16} c="text-codix-yellow" /> },
              { id: 'settings', label: 'Settings', icon: <Ico.Settings s={16} c="text-codix-sub" /> },
            ].map(a => (
              <button key={a.id} onClick={() => openApp(a.id)} className="flex items-center gap-2 w-full px-2 py-1.5 rounded text-xs hover:bg-codix-overlay/40 transition-colors" style={{ color: C.text }}>
                {a.icon} {a.label}
              </button>
            ))}
          </div>
        </div>
      )}

      {/* Taskbar */}
      <div className="absolute bottom-0 left-0 right-0 h-10 flex items-center px-2 border-t z-50" style={{ backgroundColor: C.surface, borderColor: C.overlay }}>
        {/* Start button */}
        <button onClick={() => setStartOpen(!startOpen)} className="flex items-center gap-1.5 px-2.5 py-1 rounded hover:bg-codix-overlay/40 transition-colors" style={{ backgroundColor: startOpen ? C.overlay : 'transparent' }}>
          <Ico.Grid s={16} c="text-codix-blue" />
          <span className="text-xs font-medium" style={{ color: C.text }}>CodixOS</span>
        </button>

        {/* Divider */}
        <div className="w-px h-5 mx-1.5" style={{ backgroundColor: C.overlay }} />

        {/* Open apps */}
        <div className="flex-1 flex items-center gap-1 overflow-x-auto">
          {Object.keys(APPS).map(id => {
            const isOpen = windows[id]
            const isFocused = focusedWin === id
            return (
              <button key={id} onClick={() => isOpen ? (isFocused ? setFocusedWin(null) : focusApp(id)) : openApp(id)} className="flex items-center gap-1.5 px-2 py-1 rounded text-[11px] transition-colors" style={{ backgroundColor: isFocused ? C.overlay : isOpen ? 'rgba(69,71,90,0.3)' : 'transparent', color: isFocused ? C.text : C.sub }}>
                {id === 'terminal' && <span className="text-codix-green">{'>'}</span>}
                {id === 'files' && <span className="text-codix-blue">{'\u{1F4C1}'}</span>}
                {id === 'editor' && <span className="text-codix-yellow">{'\u{270E}'}</span>}
                {id === 'settings' && <span className="text-codix-sub">{'\u{2699}'}</span>}
                <span className="hidden sm:inline">{APPS[id].title}</span>
              </button>
            )
          })}
        </div>

        {/* System tray */}
        <div className="flex items-center gap-2 px-2 text-[11px]" style={{ color: C.sub }}>
          <Ico.Wifi s={13} c="text-codix-green" />
          <Ico.Battery s={13} />
          <span className="font-mono" style={{ fontFamily: "'Courier New', monospace" }}>{clock}</span>
        </div>
      </div>
    </div>
  )
}

/* ── Terminal Demo (standalone, for terminal section) ──── */

function InteractiveTerminal() {
  const [lines, setLines] = useState([{ type: 'o', text: 'Welcome to CodixOS Terminal v1.0.0\nType help for available commands.\n' }])
  const [input, setInput] = useState('')
  const [hist, setHist] = useState([])
  const [hIdx, setHIdx] = useState(-1)
  const bottom = useRef(null)
  const inp = useRef(null)

  useEffect(() => { bottom.current?.scrollIntoView({ behavior: 'smooth' }) }, [lines])

  const focus = () => inp.current?.focus()

  function exec(e) {
    e.preventDefault()
    const raw = input.trim(); setInput('')
    if (!raw) { setLines(p => [...p, { type: 'p', text: '' }]); return }
    setHist(h => [...h, raw]); setHIdx(-1)
    const res = processCommand(raw)
    if (res === '__CLEAR__') { setLines([]); return }
    setLines(p => [...p, { type: 'p', text: raw }, ...(res !== null ? [{ type: 'o', text: res }] : [])])
  }

  function keyDown(e) {
    if (e.key === 'ArrowUp') { e.preventDefault(); const i = hIdx < hist.length - 1 ? hIdx + 1 : hIdx; setHIdx(i); setInput(hist[hist.length - 1 - i] || '') }
    if (e.key === 'ArrowDown') { e.preventDefault(); const i = hIdx > 0 ? hIdx - 1 : -1; setHIdx(i); setInput(i >= 0 ? hist[hist.length - 1 - i] : '') }
  }

  return (
    <div onClick={focus} className="max-w-3xl mx-auto rounded-xl overflow-hidden border shadow-2xl cursor-text" style={{ borderColor: C.overlay, backgroundColor: C.bg }}>
      <div className="flex items-center gap-2 px-4 py-2" style={{ backgroundColor: C.surface }}>
        <div className="w-3 h-3 rounded-full" style={{ backgroundColor: C.red }} />
        <div className="w-3 h-3 rounded-full" style={{ backgroundColor: C.yellow }} />
        <div className="w-3 h-3 rounded-full" style={{ backgroundColor: C.green }} />
        <span className="ml-2 text-sm select-none" style={{ color: C.sub }}>codix@codixos — codix-term</span>
      </div>
      <div className="p-4 font-mono text-sm leading-relaxed h-80 overflow-y-auto" style={{ fontFamily: "'Courier New', monospace" }}>
        {lines.map((l, i) => (
          <div key={i}>
            {l.type === 'p' ? (
              <div><span style={{ color: C.green }}>codix</span><span style={{ color: C.text }}>@</span><span style={{ color: C.blue }}>codixos</span><span style={{ color: C.text }}>:{cwd} $ </span><span style={{ color: C.text }}>{l.text}</span></div>
            ) : <pre className="whitespace-pre-wrap" style={{ color: C.sub }}>{l.text}</pre>}
          </div>
        ))}
        <div className="flex items-center">
          <span style={{ color: C.green }}>codix</span>
          <span style={{ color: C.text }}>@</span>
          <span style={{ color: C.blue }}>codixos</span>
          <span style={{ color: C.text }}>:{cwd} $ </span>
          <form onSubmit={exec} className="flex-1"><input ref={inp} autoFocus value={input} onChange={e => setInput(e.target.value)} onKeyDown={keyDown} className="flex-1 bg-transparent outline-none font-mono text-sm w-full" style={{ color: C.text, fontFamily: "'Courier New', monospace" }} spellCheck={false} autoComplete="off" /></form>
        </div>
        <div ref={bottom} />
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
          <a href="#" className="flex items-center gap-2"><Ico.Terminal s={24} c="text-codix-blue" /><span className="text-lg font-bold" style={{ color: C.text }}>CodixOS</span></a>
          <div className="hidden md:flex items-center gap-6">
            <a href="#features" className="text-sm hover:text-codix-blue transition-colors" style={{ color: C.sub }}>Features</a>
            <a href="#desktop" className="text-sm hover:text-codix-blue transition-colors" style={{ color: C.sub }}>Desktop</a>
            <a href="#terminal" className="text-sm hover:text-codix-blue transition-colors" style={{ color: C.sub }}>Terminal</a>
            <a href="#download" className="text-sm hover:text-codix-blue transition-colors" style={{ color: C.sub }}>Download</a>
            <a href="https://github.com/itriedcoding/CodixOS" target="_blank" rel="noopener noreferrer" className="hover:text-codix-blue transition-colors" style={{ color: C.sub }}><Ico.Github s={18} /></a>
            <a href="#download" className="btn-primary text-sm py-2 px-5">Download</a>
          </div>
          <button className="md:hidden" onClick={() => setOpen(!open)} style={{ color: C.text }}>{open ? <Ico.X s={22} /> : <Ico.Menu s={22} />}</button>
        </div>
      </div>
      {open && (
        <div className="md:hidden border-t px-4 py-3 space-y-2" style={{ backgroundColor: C.surface, borderColor: C.overlay }}>
          <a href="#features" className="block text-sm py-1" style={{ color: C.sub }} onClick={() => setOpen(false)}>Features</a>
          <a href="#desktop" className="block text-sm py-1" style={{ color: C.sub }} onClick={() => setOpen(false)}>Desktop</a>
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
          <h1 className="text-5xl md:text-7xl font-extrabold mb-5 tracking-tight"><span className="text-codix-blue">Codix</span><span style={{ color: C.text }}>OS</span></h1>
          <p className="text-lg md:text-xl max-w-xl mx-auto mb-8" style={{ color: C.sub }}>A lightweight, terminal-based operating system built from scratch</p>
          <div className="flex flex-col sm:flex-row gap-3 justify-center mb-14">
            <a href="#download" className="btn-primary gap-2"><Ico.Download s={18} /> Download</a>
            <a href="#desktop" className="btn-secondary gap-2"><Ico.Monitor s={18} /> See Desktop</a>
          </div>
          <div className="grid grid-cols-2 md:grid-cols-4 gap-4 max-w-2xl mx-auto mb-14">
            {[
              { ico: <Ico.Cpu s={22} c="text-codix-blue" />, label: 'CPU', value: '< 1%' },
              { ico: <Ico.Activity s={22} c="text-codix-green" />, label: 'RAM', value: '32MB' },
              { ico: <Ico.HardDrive s={22} c="text-codix-yellow" />, label: 'Disk', value: '128MB' },
              { ico: <Ico.Zap s={22} c="text-codix-red" />, label: 'Boot', value: '2s' },
            ].map(s => (
              <div key={s.label} className="text-center p-3 rounded-lg" style={{ backgroundColor: 'rgba(49,50,68,0.4)' }}>
                <div className="flex justify-center mb-1">{s.ico}</div>
                <div className="text-lg font-bold" style={{ color: C.text }}>{s.value}</div>
                <div className="text-xs" style={{ color: C.sub }}>{s.label}</div>
              </div>
            ))}
          </div>
          <div className="animate-bounce opacity-40"><Ico.ChevronDown s={24} c="text-codix-sub" /></div>
        </div>
      </section>

      {/* Features */}
      <section id="features" className="py-16 px-4" style={{ backgroundColor: 'rgba(49,50,68,0.2)' }}>
        <div className="max-w-6xl mx-auto">
          <h2 className="text-2xl md:text-3xl font-bold text-center mb-10" style={{ color: C.text }}>Features</h2>
          <div className="grid md:grid-cols-2 lg:grid-cols-3 gap-5">
            {[
              { ico: <Ico.Zap s={24} c="text-codix-blue" />, t: 'Lightweight', d: 'Minimal resource footprint. Runs on as little as 256MB RAM.' },
              { ico: <Ico.Terminal s={24} c="text-codix-blue" />, t: 'Terminal-First', d: '40+ built-in commands. Tab completion. Command history.' },
              { ico: <Ico.Package s={24} c="text-codix-blue" />, t: 'Package Manager', d: 'Install software with codix-pkg. Simple dependency resolution.' },
              { ico: <Ico.Monitor s={24} c="text-codix-blue" />, t: 'Desktop Environment', d: 'Optional GUI with window manager and built-in applications.' },
              { ico: <Ico.Shield s={24} c="text-codix-green" />, t: 'Secure Boot', d: 'Verify digital signatures of firmware and bootloaders for trusted startup.' },
              { ico: <Ico.Shield s={24} c="text-codix-yellow" />, t: 'Full-Disk Encryption', d: 'LUKS-based encryption. AES-256-XTS. Protect your data at rest.' },
              { ico: <Ico.HardDrive s={24} c="text-codix-blue" />, t: 'Bootable ISO', d: 'Create a bootable USB. Run live without installation.' },
              { ico: <Ico.Terminal s={24} c="text-codix-peach" />, t: 'Firefox Pre-installed', d: 'Full-featured web browser ready to use out of the box.' },
            ].map((f, i) => (
              <div key={i} className="card group">
                <div className="mb-3 group-hover:text-codix-green transition-colors">{f.ico}</div>
                <h3 className="text-base font-semibold mb-1" style={{ color: C.text }}>{f.t}</h3>
                <p className="text-sm leading-relaxed" style={{ color: C.sub }}>{f.d}</p>
              </div>
            ))}
          </div>
        </div>
      </section>

      {/* Desktop Environment Demo */}
      <section id="desktop" className="py-16 px-4">
        <div className="max-w-5xl mx-auto">
          <h2 className="text-2xl md:text-3xl font-bold text-center mb-3" style={{ color: C.text }}>Desktop Environment</h2>
          <p className="text-center mb-8 text-sm" style={{ color: C.sub }}>Double-click icons to open apps. Drag windows. Use the taskbar.</p>
          <DesktopDemo />
        </div>
      </section>

      {/* Terminal */}
      <section id="terminal" className="py-16 px-4" style={{ backgroundColor: 'rgba(49,50,68,0.2)' }}>
        <div className="max-w-6xl mx-auto">
          <h2 className="text-2xl md:text-3xl font-bold text-center mb-3" style={{ color: C.text }}>Interactive Terminal</h2>
          <p className="text-center mb-8 text-sm" style={{ color: C.sub }}>
            Try commands: <code className="px-1.5 py-0.5 rounded text-xs" style={{ backgroundColor: C.surface, color: C.blue }}>help</code>{' '}
            <code className="px-1.5 py-0.5 rounded text-xs" style={{ backgroundColor: C.surface, color: C.blue }}>neofetch</code>{' '}
            <code className="px-1.5 py-0.5 rounded text-xs" style={{ backgroundColor: C.surface, color: C.blue }}>ls</code>{' '}
            <code className="px-1.5 py-0.5 rounded text-xs" style={{ backgroundColor: C.surface, color: C.blue }}>cat README.md</code>
          </p>
          <InteractiveTerminal />
        </div>
      </section>

      {/* Download */}
      <section id="download" className="py-16 px-4">
        <div className="max-w-4xl mx-auto">
          <h2 className="text-2xl md:text-3xl font-bold text-center mb-10" style={{ color: C.text }}>Download CodixOS</h2>
          <div className="grid md:grid-cols-3 gap-5">
            {[
              { ico: <Ico.HardDrive s={36} c="text-codix-blue" />, name: 'ISO Image', desc: 'Bootable ISO for VMs and live USB', size: '128 MB', href: 'https://github.com/itriedcoding/CodixOS' },
              { ico: <Ico.Github s={36} c="text-codix-blue" />, name: 'Source Code', desc: 'Build from source', size: '15 MB', href: 'https://github.com/itriedcoding/CodixOS' },
              { ico: <Ico.Monitor s={36} c="text-codix-blue" />, name: 'WSL Installer', desc: 'Run inside Windows Subsystem for Linux', size: '50 MB', href: 'https://github.com/itriedcoding/CodixOS' },
            ].map((d, i) => (
              <div key={i} className="card text-center">
                <div className="flex justify-center mb-3">{d.ico}</div>
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
          <div className="flex items-center gap-2"><Ico.Terminal s={18} c="text-codix-blue" /><span className="text-sm font-semibold" style={{ color: C.text }}>CodixOS</span></div>
          <div className="flex items-center gap-6 text-xs" style={{ color: C.sub }}>
            <a href="#features" className="hover:text-codix-blue transition-colors">Features</a>
            <a href="#desktop" className="hover:text-codix-blue transition-colors">Desktop</a>
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
