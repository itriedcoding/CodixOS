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
  Globe: (p) => <Svg size={p.s||24} cl={p.c} d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z" />,
  Calculator: (p) => <Svg size={p.s||24} cl={p.c} d="M4 7h16M4 7v10a2 2 0 002 2h12a2 2 0 002-2V7M8 7V5a2 2 0 012-2h4a2 2 0 012 2v2M8 11h.01M12 11h.01M16 11h.01M8 15h.01M12 15h.01" />,
  Image: (p) => <Svg size={p.s||24} cl={p.c} d="M21 16V8a2 2 0 00-1-1.73l-7-4a2 2 0 00-2 0l-7 4A2 2 0 003 8v8a2 2 0 001 1.73l7 4a2 2 0 002 0l7-4A2 2 0 0021 16z" />,
  Music: (p) => <Svg size={p.s||24} cl={p.c} d="M9 18V5l12-2v13M9 18a3 3 0 11-6 0 3 3 0 016 0zM21 16a3 3 0 11-6 0 3 3 0 016 0z" />,
  Video: (p) => <Svg size={p.s||24} cl={p.c} d="M23 7l-7 5 7 5V7zM1 5h14a2 2 0 012 2v10a2 2 0 01-2 2H1z" />,
  Mail: (p) => <Svg size={p.s||24} cl={p.c} d="M4 4h16c1.1 0 2 .9 2 2v12c0 1.1-.9 2-2 2H4c-1.1 0-2-.9-2-2V6c0-1.1.9-2 2-2zM22 6l-10 7L2 6" />,
  Lock: (p) => <Svg size={p.s||24} cl={p.c} d="M19 11H5a2 2 0 00-2 2v7a2 2 0 002 2h14a2 2 0 002-2v-7a2 2 0 00-2-2zM7 11V7a5 5 0 0110 0v4" />,
}

/* ── Terminal Engine ───────────────────────────────────── */

const FILESYSTEM = {
  '~': { type: 'dir', children: ['Documents', 'Downloads', 'Pictures', '.bashrc', 'README.md', 'codixos.conf'] },
  '~/Documents': { type: 'dir', children: ['notes.txt', 'project-plan.md'] },
  '~/Downloads': { type: 'dir', children: [] },
  '~/Pictures': { type: 'dir', children: ['wallpaper.png', 'screenshot.png'] },
  '~/.bashrc': { type: 'file', content: '# CodixOS Shell Config\nexport PS1="codix@codixos:~$ "\nexport PATH=/usr/local/bin:$PATH\nalias ll="ls -la"' },
  '~/README.md': { type: 'file', content: '# CodixOS\nA lightweight, terminal-based operating system.\nBuilt from scratch with a custom kernel.\n\n## Features\n- Custom kernel with memory management\n- Built-in shell with 40+ commands\n- Package manager\n- Desktop environment\n- Full security suite' },
  '~/codixos.conf': { type: 'file', content: 'kernel.codix.version=1.0.0\ndisplay.resolution=1920x1080\ndisplay.theme=catppuccin-mocha\nsecurity.secureboot=enabled\nsecurity.encryption=enabled' },
  '~/Documents/notes.txt': { type: 'file', content: 'TODO:\n- Finish kernel memory manager\n- Add TCP/IP stack\n- Write man pages\n- Package Firefox' },
  '~/Documents/project-plan.md': { type: 'file', content: '# CodixOS Project Plan\n\n## Phase 1: Core OS\n- [x] Kernel\n- [x] Shell\n- [x] Utilities\n\n## Phase 2: Desktop\n- [x] Window Manager\n- [x] File Manager\n- [x] Text Editor\n\n## Phase 3: Security\n- [x] Secure Boot\n- [x] Encryption\n- [x] Authentication' },
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
  if (cmd === 'help') return `Commands:\n  System:    help, clear, echo, exit, reboot\n  Files:     ls, cat, pwd, cd, mkdir, cp, mv, rm\n  Info:      neofetch, whoami, hostname, uname, date, uptime\n  Resources: free, ps, df\n  Packages:  pkg install, pkg list, pkg update\n  Security:  secureboot, encrypt, auth, hardening, audit, sandbox, tls\n  Apps:      firefox, calculator, files, editor, settings\n  Theme:     theme`
  if (cmd === 'neofetch') return `${C.blue}        _____      _               ____   _____ ${C.text}\n${C.blue}       / ____|    | |             |  _ \\ / ____|${C.text}\n${C.blue}      | |     ___ | | ___  _ __   | |_) | (___  ${C.text}\n${C.blue}      | |    / _ \\| |/ _ \\| '__|  |  _ < \\___ \\ ${C.text}\n${C.blue}      | |___| (_) | | (_) | |     | |_) |____) |${C.text}\n${C.blue}       \\_____\\___/|_|\\___/|_|     |____/|_____/ ${C.text}\n\n${C.blue}OS${C.text}:       CodixOS 1.0.0 x86_64\n${C.blue}Kernel${C.text}:   codix-kernel 1.0.0\n${C.blue}Shell${C.text}:    codix-sh 1.0.0\n${C.blue}DE${C.text}:      codix-desktop 1.0.0\n${C.blue}CPU${C.text}:      Virtual CPU @ 2.4GHz\n${C.blue}Memory${C.text}:   32MiB / 256MiB\n${C.blue}Disk${C.text}:     128MB / 512MB\n${C.blue}Uptime${C.text}:   4h 23m\n\n${C.blue}Apps${C.text}:     Firefox, Terminal, File Manager,\n           Text Editor, Calculator, Settings,\n           System Monitor, Image Viewer\n\n${C.red}███${C.green}███${C.yellow}███${C.blue}███${C.magenta}███${C.teal}███${C.text}`
  if (cmd === 'ls') { const t = args[0] ? resolvePath(args[0]) : cwd; const e = FILESYSTEM[t]; if (!e) return `ls: cannot access '${args[0]}': No such file or directory`; if (e.type !== 'dir') return args[0]; return e.children.map(c => { const cp = t === '~' ? `~/${c}` : `${t}/${c}`; return FILESYSTEM[cp]?.type === 'dir' ? `${C.blue}${c}/${C.text}` : c }).join('  ') }
  if (cmd === 'cat') { if (!args[0]) return 'cat: missing operand'; const p = resolvePath(args[0]); const e = FILESYSTEM[p]; if (!e) return `cat: ${args[0]}: No such file`; if (e.type === 'dir') return `cat: ${args[0]}: Is a directory`; return e.content }
  if (cmd === 'pwd') return cwd
  if (cmd === 'whoami') return 'codix'
  if (cmd === 'hostname') return 'codixos'
  if (cmd === 'uname') return args[0] === '-a' ? 'CodixOS codixos 1.0.0 #1 SMP x86_64 GNU/Codix' : 'CodixOS'
  if (cmd === 'date') return new Date().toString()
  if (cmd === 'uptime') return ' 06:42:33 up 4:23, 1 user, load average: 0.12, 0.08, 0.05'
  if (cmd === 'free') return '              total        used        free      shared  buff/cache   available\nMem:        262144       32768      196608        4096       32768      225280\nSwap:       131072           0      131072'
  if (cmd === 'ps') return '  PID TTY          TIME CMD\n    1 ?        00:00:02 codix-init\n   12 ?        00:00:00 codix-sh\n   45 ?        00:00:01 codix-wm\n   67 ?        00:00:00 codix-term\n   89 ?        00:00:00 firefox\n   91 ?        00:00:00 codix-desktop'
  if (cmd === 'df') return 'Filesystem     1K-blocks   Used Available Use% Mounted on\n/dev/sda1       131072  32768    98304  25% /\n/dev/sda2       262144  12288   249856   5% /home\ntmpfs            16384     16     16368   1% /tmp'
  if (cmd === 'echo') return args.join(' ')
  if (cmd === 'clear') return '__CLEAR__'
  if (cmd === 'reboot') return 'Rebooting system...'
  if (cmd === 'exit') return 'Nice try. This terminal has no escape.'
  if (cmd === 'theme') return `Catppuccin Mocha Theme:\n  ${C.red}██${C.text} Red    #f38ba8\n  ${C.blue}██${C.text} Blue   #89b4fa\n  ${C.green}██${C.text} Green  #a6e3a1\n  ${C.yellow}██${C.text} Yellow #f9e2af\n  ${C.magenta}██${C.text} Mauve  #cba6f7\n  ${C.teal}██${C.text} Teal   #94e2d5\n  ${C.peach}██${C.text} Peach  #fab387`
  if (cmd === 'pkg') { if (args[0] === 'list') return 'Installed packages:\n  codix-kernel    1.0.0\n  codix-sh        1.0.0\n  codix-term      1.0.0\n  codix-pkg       1.0.0\n  codix-desktop   1.0.0\n  firefox         120.0\n  codix-utils     1.0.0\n  codix-security  1.0.0'; if (args[0] === 'install') return `Installing ${args[1] || 'package'}...`; if (args[0] === 'update') return 'Updating package lists...'; return 'Usage: pkg <install|list|update>' }
  if (cmd === 'firefox') return 'Launching Firefox browser...\n[Firefox would open here]'
  if (cmd === 'calculator') return 'Launching Calculator...\n[Calculator would open here]'
  if (cmd === 'files') return 'Launching File Manager...\n[File Manager would open here]'
  if (cmd === 'editor') return 'Launching Text Editor...\n[Text Editor would open here]'
  if (cmd === 'settings') return 'Launching Settings...\n[Settings would open here]'
  if (cmd === 'monitor') return 'Launching System Monitor...\n[CPU: 2% | RAM: 12% | Disk: 25%]'
  if (cmd === 'secureboot') return `\n${C.teal}=== Secure Boot Status ===${C.text}\n\n  State:             ${C.green}ENABLED${C.text}\n  Trusted Keys:      1\n\n  ${C.teal}Boot Chain Verification:${C.text}\n    Firmware:        ${C.green}VERIFIED${C.text}\n    Bootloader:      ${C.green}VERIFIED${C.text}\n    Kernel:          ${C.green}VERIFIED${C.text}\n    Initrd:          ${C.green}VERIFIED${C.text}\n\n  Overall Status:    ${C.green}SECURE${C.text}\n`
  if (cmd === 'encrypt') return `\n${C.teal}=== Encrypted Devices ===${C.text}\n\n  DEVICE               MAPPER         CIPHER     KEY      STATUS\n  ------               ------         ------     ---      ------\n  /dev/sda2            codix-data     aes-xts    256      ${C.green}UNLOCKED${C.text}\n  /dev/sda3            -              aes-xts    256      ${C.yellow}LOCKED${C.text}\n`
  if (cmd === 'auth') return `\n${C.teal}=== Authentication Status ===${C.text}\n\n  Users:       3\n  Sessions:    2\n  MFA Required: Yes\n\n  Password Policy:\n    Min Length:      12\n    Complexity:      Upper + Lower + Digits + Symbols\n    Expiry Days:     90\n    Max Attempts:    5\n    Lockout (sec):   300\n`
  if (cmd === 'hardening') return `\n${C.teal}=== System Hardening Status ===${C.text}\n\n  Level:           ${C.green}HIGH${C.text}\n  Security Score:  ${C.green}92${C.text}/100\n\n  Components:\n    Firewall:      ${C.green}ENABLED${C.text}\n    SELinux:       ${C.green}ENABLED${C.text}\n    Audit:         ${C.green}ENABLED${C.text}\n    Ptrace Scope:  2\n    No New Privs:  Yes\n\n  Services:  8/17 enabled\n  Open Ports: 5 (22, 80, 443, 53, 123)\n  Blacklisted: 3 apps\n`
  if (cmd === 'audit') return `\n${C.teal}=== Audit System Status ===${C.text}\n\n  Enabled:         Yes\n  Console Output:  Yes\n  Level Filter:    WARNING\n\n  Total Events:    1,247\n  Security Events: 23\n  Failed Logins:   7\n  Rules:           8\n  Recipients:      2\n\n  Recent Events:\n    [06:42:15] LOGIN codix 127.0.0.1\n    [06:42:10] BOOT system started\n    [06:41:55] CONFIG hardening applied\n`
  if (cmd === 'sandbox') return `\n${C.teal}=== Sandboxes ===${C.text}\n\n  ID   NAME                 TYPE        STATUS     CAPS\n  ---  ----                 ----        ------     ----\n  1    web-browser          container   RUNNING    2\n  2    terminal-app         namespace   RUNNING    5\n  3    file-manager         chroot      STOPPED    0\n  4    text-editor          namespace   RUNNING    3\n\n  Active Namespaces: PID, NET, MNT\n  Resource Limits:   512MB RAM, 256 PIDs\n`
  if (cmd === 'tls') return `\n${C.teal}=== Data Protection Status ===${C.text}\n\n  Key Pairs:    2\n    - RSA-2048 (server)\n    - ECDSA-P256 (client)\n\n  Certificates: 3\n    - CN=codixos (self-signed)\n    - CN=*.codixos.local\n    - CN=root-ca (CA)\n\n  Connections:  4 active\n\n  TLS Configuration:\n    Min Version:   1.2\n    Max Version:   1.3\n    Cipher Suites: 7\n    Client Certs:  Optional\n    Hostname Vfy:  Enabled\n`
  return `codix: command not found: ${cmd}\nType 'help' for available commands.`
}

/* ── Boot Sequence Animation ───────────────────────────── */

function BootSequence({ onComplete }) {
  const [lines, setLines] = useState([])
  const [done, setDone] = useState(false)
  const bootLines = [
    { text: 'CodixOS Bootloader v1.0.0', delay: 100 },
    { text: 'Verifying Secure Boot... OK', delay: 300 },
    { text: 'Loading kernel...', delay: 200 },
    { text: 'Initializing memory manager... OK', delay: 150 },
    { text: 'Mounting root filesystem... OK', delay: 200 },
    { text: 'Starting init system... OK', delay: 150 },
    { text: 'Loading security modules...', delay: 100 },
    { text: '  Secure Boot: ENABLED', delay: 100 },
    { text: '  Disk Encryption: ENABLED', delay: 100 },
    { text: '  Authentication: ENABLED', delay: 100 },
    { text: '  Firewall: ENABLED', delay: 100 },
    { text: 'Starting desktop environment...', delay: 200 },
    { text: 'Loading applications...', delay: 150 },
    { text: '', delay: 100 },
    { text: `${C.green}Welcome to CodixOS v1.0.0${C.text}`, delay: 200 },
  ]

  useEffect(() => {
    let idx = 0
    const timer = setInterval(() => {
      if (idx < bootLines.length) {
        setLines(p => [...p, bootLines[idx].text])
        idx++
      } else {
        clearInterval(timer)
        setDone(true)
        setTimeout(onComplete, 500)
      }
    }, 150)
    return () => clearInterval(timer)
  }, [])

  return (
    <div className="w-full h-full flex flex-col justify-center p-6 font-mono text-xs" style={{ backgroundColor: '#000', fontFamily: "'Courier New', monospace" }}>
      {lines.map((l, i) => (
        <div key={i} style={{ color: l.includes('OK') ? C.green : l.includes('Welcome') ? C.green : l.includes('ENABLED') ? C.teal : '#aaa' }}>{l}</div>
      ))}
      {!done && <span className="animate-pulse" style={{ color: C.green }}>_</span>}
    </div>
  )
}

/* ── Terminal Engine ───────────────────────────────────── */

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

/* ── Firefox Browser App ───────────────────────────────── */

function FirefoxApp() {
  const [url, setUrl] = useState('https://www.google.com')
  const [tabs, setTabs] = useState([{ title: 'New Tab', url: 'https://www.google.com' }])
  const [activeTab, setActiveTab] = useState(0)

  return (
    <div className="h-full flex flex-col" style={{ backgroundColor: C.bg }}>
      {/* Tab bar */}
      <div className="flex items-center gap-1 px-2 py-1 border-b" style={{ backgroundColor: C.surface, borderColor: C.overlay }}>
        {tabs.map((tab, i) => (
          <div key={i} className="flex items-center gap-1 px-2 py-1 rounded text-[10px] cursor-pointer" style={{ backgroundColor: i === activeTab ? C.overlay : 'transparent', color: i === activeTab ? C.text : C.sub }}>
            <Ico.Globe s={10} />
            <span className="max-w-[80px] truncate">{tab.title}</span>
          </div>
        ))}
        <button className="ml-1 text-[10px]" style={{ color: C.sub }}>+</button>
      </div>
      {/* URL bar */}
      <div className="flex items-center gap-2 px-2 py-1 border-b" style={{ borderColor: C.overlay }}>
        <Ico.Globe s={12} c="text-codix-blue" />
        <div className="flex-1 px-2 py-0.5 rounded text-[10px]" style={{ backgroundColor: C.overlay, color: C.text }}>{url}</div>
      </div>
      {/* Content */}
      <div className="flex-1 flex items-center justify-center" style={{ backgroundColor: '#fff' }}>
        <div className="text-center">
          <div className="text-2xl font-bold mb-2" style={{ color: '#4285f4' }}>Google</div>
          <div className="w-64 h-8 rounded-full border px-3 flex items-center" style={{ borderColor: '#ddd' }}>
            <Ico.Terminal s={12} c="#9aa0a6" />
          </div>
        </div>
      </div>
    </div>
  )
}

/* ── Calculator App ────────────────────────────────────── */

function CalculatorApp() {
  const [display, setDisplay] = useState('0')
  const [expr, setExpr] = useState('')

  function btn(val) {
    if (val === 'C') { setDisplay('0'); setExpr(''); return }
    if (val === '=') { try { setDisplay(String(eval(expr))) } catch { setDisplay('Error') }; setExpr(''); return }
    setExpr(e => e + val)
    setDisplay(d => d === '0' ? val : d + val)
  }

  return (
    <div className="h-full flex flex-col p-3" style={{ backgroundColor: C.bg }}>
      <div className="mb-3 p-3 rounded text-right text-2xl font-mono" style={{ backgroundColor: C.surface, color: C.text, fontFamily: "'Courier New', monospace" }}>{display}</div>
      <div className="grid grid-cols-4 gap-1 flex-1">
        {['C', '(', ')', '/', '7', '8', '9', '*', '4', '5', '6', '-', '1', '2', '3', '+', '0', '.', '=', ''].map((b, i) => (
          b ? <button key={i} onClick={() => btn(b)} className="rounded text-sm font-medium transition-colors" style={{ backgroundColor: ['+', '-', '*', '/', '='].includes(b) ? C.blue : C.surface, color: C.text }}>{b}</button> : <div key={i} />
        ))}
      </div>
    </div>
  )
}

/* ── System Monitor App ────────────────────────────────── */

function MonitorApp() {
  const [cpu, setCpu] = useState(12)
  const [ram, setRam] = useState(32)
  const [disk, setDisk] = useState(25)

  useEffect(() => {
    const id = setInterval(() => {
      setCpu(p => Math.max(1, Math.min(99, p + (Math.random() - 0.5) * 10)))
      setRam(p => Math.max(10, Math.min(80, p + (Math.random() - 0.5) * 5)))
    }, 1000)
    return () => clearInterval(id)
  }, [])

  function Bar({ value, color }) {
    return (
      <div className="w-full h-3 rounded-full overflow-hidden" style={{ backgroundColor: C.overlay }}>
        <div className="h-full rounded-full transition-all" style={{ width: `${value}%`, backgroundColor: color }} />
      </div>
    )
  }

  return (
    <div className="h-full p-4 overflow-y-auto" style={{ backgroundColor: C.bg }}>
      <h3 className="text-sm font-semibold mb-4" style={{ color: C.text }}>System Monitor</h3>
      <div className="space-y-4">
        <div>
          <div className="flex justify-between text-xs mb-1" style={{ color: C.sub }}><span>CPU</span><span>{Math.round(cpu)}%</span></div>
          <Bar value={cpu} color={cpu > 80 ? C.red : C.green} />
        </div>
        <div>
          <div className="flex justify-between text-xs mb-1" style={{ color: C.sub }}><span>Memory</span><span>{Math.round(ram)}%</span></div>
          <Bar value={ram} color={ram > 80 ? C.red : C.blue} />
        </div>
        <div>
          <div className="flex justify-between text-xs mb-1" style={{ color: C.sub }}><span>Disk</span><span>{disk}%</span></div>
          <Bar value={disk} color={C.yellow} />
        </div>
        <div className="pt-2 border-t" style={{ borderColor: C.overlay }}>
          <h4 className="text-xs font-semibold mb-2" style={{ color: C.text }}>Processes</h4>
          <div className="text-[10px] space-y-1" style={{ color: C.sub }}>
            <div className="flex justify-between"><span>codix-init</span><span>1.2%</span></div>
            <div className="flex justify-between"><span>codix-wm</span><span>0.8%</span></div>
            <div className="flex justify-between"><span>firefox</span><span>3.2%</span></div>
            <div className="flex justify-between"><span>codix-term</span><span>0.4%</span></div>
          </div>
        </div>
      </div>
    </div>
  )
}

/* ── Image Viewer App ──────────────────────────────────── */

function ImageViewerApp() {
  return (
    <div className="h-full flex items-center justify-center" style={{ backgroundColor: C.bg }}>
      <div className="text-center">
        <Ico.Image s={48} c="text-codix-sub" />
        <p className="text-xs mt-2" style={{ color: C.sub }}>Image Viewer</p>
        <p className="text-[10px]" style={{ color: C.overlay }}>Supports PNG, JPG, GIF, SVG</p>
      </div>
    </div>
  )
}

/* ── File Manager App ──────────────────────────────────── */

function FileManagerApp() {
  return (
    <div className="h-full flex" style={{ backgroundColor: C.bg }}>
      <div className="w-32 p-2 border-r" style={{ borderColor: C.overlay }}>
        <div className="text-[10px] font-semibold mb-2" style={{ color: C.sub }}>PLACES</div>
        {['Home', 'Documents', 'Downloads', 'Pictures', 'Desktop'].map(p => (
          <div key={p} className="flex items-center gap-1 px-1 py-0.5 rounded text-[10px] cursor-pointer hover:bg-codix-overlay/30" style={{ color: C.text }}>
            <Ico.Folder s={10} c="text-codix-blue" />{p}
          </div>
        ))}
      </div>
      <div className="flex-1 p-2">
        <div className="flex gap-2 flex-wrap">
          {['Documents', 'Downloads', 'Pictures', '.bashrc', 'README.md'].map(f => (
            <div key={f} className="flex flex-col items-center gap-1 p-2 rounded w-16 cursor-pointer hover:bg-codix-overlay/30">
              {f.startsWith('.') ? <Ico.File s={24} c="text-codix-sub" /> : <Ico.Folder s={24} c="text-codix-blue" />}
              <span className="text-[9px] text-center leading-tight" style={{ color: C.text }}>{f}</span>
            </div>
          ))}
        </div>
      </div>
    </div>
  )
}

/* ── Text Editor App ───────────────────────────────────── */

function TextEditorApp() {
  return (
    <div className="h-full flex flex-col" style={{ backgroundColor: C.bg }}>
      <div className="flex items-center gap-2 px-2 py-1 border-b text-[10px]" style={{ borderColor: C.overlay, color: C.sub }}>
        <span className="px-1">File</span>
        <span className="px-1">Edit</span>
        <span className="px-1">View</span>
      </div>
      <div className="flex-1 p-2 font-mono text-xs overflow-y-auto" style={{ backgroundColor: C.surface, color: C.text, fontFamily: "'Courier New', monospace" }}>
        <div style={{ color: C.overlay }}> 1  </div>
        <div><span style={{ color: C.blue }}>#</span> Welcome to CodixOS</div>
        <div style={{ color: C.overlay }}> 2  </div>
        <div><span style={{ color: C.blue }}>##</span> Getting Started</div>
        <div style={{ color: C.overlay }}> 3  </div>
        <div>This is a lightweight, terminal-based OS.</div>
        <div style={{ color: C.overlay }}> 4  </div>
        <div></div>
        <div style={{ color: C.overlay }}> 5  </div>
        <div><span style={{ color: C.green }}>Features:</span></div>
        <div style={{ color: C.overlay }}> 6  </div>
        <div>  - Custom kernel</div>
        <div style={{ color: C.overlay }}> 7  </div>
        <div>  - Built-in shell</div>
        <div style={{ color: C.overlay }}> 8  </div>
        <div>  - Desktop environment</div>
        <div style={{ color: C.overlay }}> 9  </div>
        <div>  - Firefox browser</div>
      </div>
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
        <div className="flex justify-between p-2 rounded" style={{ backgroundColor: C.surface }}><span>Security Level</span><span style={{ color: C.green }}>High</span></div>
      </div>
    </div>
  )
}

/* ── Window Manager (Draggable Windows) ────────────────── */

const APPS = {
  terminal: { title: 'Terminal', icon: '>', component: TermApp, w: 580, h: 380, x: 60, y: 40 },
  firefox: { title: 'Firefox', icon: '\u{1F310}', component: FirefoxApp, w: 640, h: 420, x: 120, y: 50 },
  files: { title: 'File Manager', icon: '\u{1F4C1}', component: FileManagerApp, w: 400, h: 340, x: 180, y: 60 },
  editor: { title: 'Text Editor', icon: '\u{270E}', component: TextEditorApp, w: 480, h: 360, x: 240, y: 30 },
  calculator: { title: 'Calculator', icon: '\u{1F5A9}', component: CalculatorApp, w: 240, h: 320, x: 300, y: 70 },
  monitor: { title: 'System Monitor', icon: '\u{1F4CA}', component: MonitorApp, w: 320, h: 360, x: 80, y: 80 },
  image: { title: 'Image Viewer', icon: '\u{1F5BC}', component: ImageViewerApp, w: 400, h: 320, x: 160, y: 90 },
  settings: { title: 'Settings', icon: '\u{2699}', component: SettingsApp, w: 340, h: 320, x: 200, y: 50 },
}

function Window({ id, app, onClose, onFocus, focused, zIndex }) {
  const [pos, setPos] = useState({ x: app.x, y: app.y })
  const [size, setSize] = useState({ w: app.w, h: app.h })
  const [maximized, setMaximized] = useState(false)
  const [minimized, setMinimized] = useState(false)
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
      <div onMouseDown={onDragStart} onDoubleClick={toggleMax} className="flex items-center justify-between px-3 py-1.5 cursor-move select-none shrink-0" style={{ backgroundColor: focused ? C.surface : C.overlay }}>
        <span className="text-xs font-medium" style={{ color: focused ? C.text : C.sub }}>{app.title}</span>
        <div className="flex items-center gap-1.5">
          <button onClick={() => setMinimized(true)} className="w-3 h-3 rounded-full" style={{ backgroundColor: C.yellow }} />
          <button onClick={toggleMax} className="w-3 h-3 rounded-full" style={{ backgroundColor: C.green }} />
          <button onClick={onClose} className="w-3 h-3 rounded-full" style={{ backgroundColor: C.red }} />
        </div>
      </div>
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
  const [booted, setBooted] = useState(false)

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

  if (!booted) {
    return (
      <div className="relative w-full rounded-xl overflow-hidden border shadow-2xl" style={{ borderColor: C.overlay, height: 500 }}>
        <BootSequence onComplete={() => setBooted(true)} />
      </div>
    )
  }

  return (
    <div className="relative w-full rounded-xl overflow-hidden border shadow-2xl" style={{ borderColor: C.overlay, height: 500 }}>
      <div className="absolute inset-0" style={{ backgroundColor: C.bg, backgroundImage: `radial-gradient(ellipse at 30% 20%, rgba(137,180,250,0.08) 0%, transparent 50%), radial-gradient(ellipse at 70% 80%, rgba(166,227,161,0.06) 0%, transparent 50%)` }} />

      {/* Desktop icons */}
      <div className="absolute top-4 left-4 flex flex-col gap-3 z-[1]">
        {[
          { id: 'terminal', label: 'Terminal', icon: <Ico.Terminal s={28} c="text-codix-green" /> },
          { id: 'firefox', label: 'Firefox', icon: <Ico.Globe s={28} c="text-codix-peach" /> },
          { id: 'files', label: 'Files', icon: <Ico.Folder s={28} c="text-codix-blue" /> },
          { id: 'editor', label: 'Editor', icon: <Ico.TextEditor s={28} c="text-codix-yellow" /> },
          { id: 'calculator', label: 'Calculator', icon: <Ico.Calculator s={28} c="text-codix-teal" /> },
          { id: 'monitor', label: 'Monitor', icon: <Ico.Activity s={28} c="text-codix-magenta" /> },
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
              { id: 'firefox', label: 'Firefox', icon: <Ico.Globe s={16} c="text-codix-peach" /> },
              { id: 'files', label: 'File Manager', icon: <Ico.Folder s={16} c="text-codix-blue" /> },
              { id: 'editor', label: 'Text Editor', icon: <Ico.TextEditor s={16} c="text-codix-yellow" /> },
              { id: 'calculator', label: 'Calculator', icon: <Ico.Calculator s={16} c="text-codix-teal" /> },
              { id: 'monitor', label: 'System Monitor', icon: <Ico.Activity s={16} c="text-codix-magenta" /> },
              { id: 'image', label: 'Image Viewer', icon: <Ico.Image s={16} c="text-codix-blue" /> },
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
        <button onClick={() => setStartOpen(!startOpen)} className="flex items-center gap-1.5 px-2.5 py-1 rounded hover:bg-codix-overlay/40 transition-colors" style={{ backgroundColor: startOpen ? C.overlay : 'transparent' }}>
          <Ico.Grid s={16} c="text-codix-blue" />
          <span className="text-xs font-medium" style={{ color: C.text }}>CodixOS</span>
        </button>
        <div className="w-px h-5 mx-1.5" style={{ backgroundColor: C.overlay }} />
        <div className="flex-1 flex items-center gap-1 overflow-x-auto">
          {Object.keys(APPS).map(id => {
            const isOpen = windows[id]
            const isFocused = focusedWin === id
            return (
              <button key={id} onClick={() => isOpen ? (isFocused ? setFocusedWin(null) : focusApp(id)) : openApp(id)} className="flex items-center gap-1.5 px-2 py-1 rounded text-[11px] transition-colors" style={{ backgroundColor: isFocused ? C.overlay : isOpen ? 'rgba(69,71,90,0.3)' : 'transparent', color: isFocused ? C.text : C.sub }}>
                <span>{APPS[id].icon}</span>
                <span className="hidden sm:inline">{APPS[id].title}</span>
              </button>
            )
          })}
        </div>
        <div className="flex items-center gap-2 px-2 text-[11px]" style={{ color: C.sub }}>
          <Ico.Wifi s={13} c="text-codix-green" />
          <Ico.Battery s={13} />
          <span className="font-mono" style={{ fontFamily: "'Courier New', monospace" }}>{clock}</span>
        </div>
      </div>
    </div>
  )
}

/* ── Interactive Terminal (standalone) ─────────────────── */

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
    <div onClick={focus} className="w-full rounded-xl overflow-hidden border shadow-2xl" style={{ borderColor: C.overlay, height: 400 }}>
      <div className="flex items-center gap-2 px-3 py-2 border-b" style={{ backgroundColor: C.surface, borderColor: C.overlay }}>
        <div className="flex gap-1.5"><div className="w-3 h-3 rounded-full" style={{ backgroundColor: C.red }} /><div className="w-3 h-3 rounded-full" style={{ backgroundColor: C.yellow }} /><div className="w-3 h-3 rounded-full" style={{ backgroundColor: C.green }} /></div>
        <span className="text-[11px] ml-2" style={{ color: C.sub }}>codix@codixos: ~</span>
      </div>
      <div className="flex-1 p-3 font-mono text-xs leading-relaxed overflow-y-auto" style={{ backgroundColor: C.bg, height: 340, fontFamily: "'Courier New', monospace" }}>
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
            <a href="#security" className="text-sm hover:text-codix-blue transition-colors" style={{ color: C.sub }}>Security</a>
            <a href="#apps" className="text-sm hover:text-codix-blue transition-colors" style={{ color: C.sub }}>Apps</a>
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
          <a href="#security" className="block text-sm py-1" style={{ color: C.sub }} onClick={() => setOpen(false)}>Security</a>
          <a href="#apps" className="block text-sm py-1" style={{ color: C.sub }} onClick={() => setOpen(false)}>Apps</a>
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
              { ico: <Ico.Terminal s={24} c="text-codix-peach" />, t: 'Firefox Pre-installed', d: 'Full-featured web browser ready to use out of the box.' },
              { ico: <Ico.HardDrive s={24} c="text-codix-blue" />, t: 'Bootable ISO', d: 'Create a bootable USB. Run live without installation.' },
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

      {/* Security Features */}
      <section id="security" className="py-16 px-4">
        <div className="max-w-6xl mx-auto">
          <h2 className="text-2xl md:text-3xl font-bold text-center mb-3" style={{ color: C.text }}>Security Features</h2>
          <p className="text-center mb-10 text-sm" style={{ color: C.sub }}>Enterprise-grade security built into the kernel</p>
          <div className="grid md:grid-cols-2 lg:grid-cols-3 gap-5">
            {[
              { ico: <Ico.Shield s={28} c="text-codix-green" />, t: 'Secure Boot', d: 'Verify digital signatures of firmware and bootloaders. Trusted boot chain from hardware to OS.', color: C.green },
              { ico: <Ico.HardDrive s={28} c="text-codix-yellow" />, t: 'Full-Disk Encryption', d: 'LUKS-based AES-256-XTS encryption. Protect data at rest with strong cryptographic algorithms.', color: C.yellow },
              { ico: <Ico.Shield s={28} c="text-codix-blue" />, t: 'User Authentication', d: 'Password policies, MFA/TOTP support, account lockout, and session management.', color: C.blue },
              { ico: <Ico.Settings s={28} c="text-codix-red" />, t: 'System Hardening', d: 'Disable unnecessary services, control ports, blacklist software, and apply security rules.', color: C.red },
              { ico: <Ico.Activity s={28} c="text-codix-magenta" />, t: 'Audit Logging', d: 'Comprehensive event tracking, filter rules, alerting, and security monitoring.', color: C.magenta },
              { ico: <Ico.Terminal s={28} c="text-codix-teal" />, t: 'Process Isolation', d: 'Capability-based sandboxing, namespaces, resource limits, and seccomp filters.', color: C.teal },
              { ico: <Ico.Lock s={28} c="text-codix-peach" />, t: 'TLS Encryption', d: 'TLS 1.2/1.3 support. RSA, ECDSA, Ed25519 key pairs. Certificate management.', color: C.peach },
            ].map((f, i) => (
              <div key={i} className="card group">
                <div className="mb-3 group-hover:scale-110 transition-transform">{f.ico}</div>
                <h3 className="text-base font-semibold mb-1" style={{ color: f.color }}>{f.t}</h3>
                <p className="text-sm leading-relaxed" style={{ color: C.sub }}>{f.d}</p>
              </div>
            ))}
          </div>
        </div>
      </section>

      {/* Pre-installed Apps */}
      <section id="apps" className="py-16 px-4" style={{ backgroundColor: 'rgba(49,50,68,0.2)' }}>
        <div className="max-w-6xl mx-auto">
          <h2 className="text-2xl md:text-3xl font-bold text-center mb-3" style={{ color: C.text }}>Pre-installed Applications</h2>
          <p className="text-center mb-10 text-sm" style={{ color: C.sub }}>Everything you need, ready to use out of the box</p>
          <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
            {[
              { ico: <Ico.Globe s={32} c="text-codix-peach" />, name: 'Firefox', desc: 'Web browser', category: 'Internet' },
              { ico: <Ico.Terminal s={32} c="text-codix-green" />, name: 'Terminal', desc: 'Command line', category: 'System' },
              { ico: <Ico.Folder s={32} c="text-codix-blue" />, name: 'File Manager', desc: 'Browse files', category: 'System' },
              { ico: <Ico.TextEditor s={32} c="text-codix-yellow" />, name: 'Text Editor', desc: 'Edit code & text', category: 'Utilities' },
              { ico: <Ico.Calculator s={32} c="text-codix-teal" />, name: 'Calculator', desc: 'Math & conversions', category: 'Utilities' },
              { ico: <Ico.Activity s={32} c="text-codix-magenta" />, name: 'System Monitor', desc: 'CPU, RAM, disk', category: 'System' },
              { ico: <Ico.Image s={32} c="text-codix-blue" />, name: 'Image Viewer', desc: 'View pictures', category: 'Media' },
              { ico: <Ico.Settings s={32} c="text-codix-sub" />, name: 'Settings', desc: 'System config', category: 'System' },
            ].map((app, i) => (
              <div key={i} className="card text-center group hover:scale-105 transition-transform">
                <div className="flex justify-center mb-3">{app.ico}</div>
                <h3 className="text-sm font-semibold mb-1" style={{ color: C.text }}>{app.name}</h3>
                <p className="text-[11px] mb-2" style={{ color: C.sub }}>{app.desc}</p>
                <span className="text-[10px] px-2 py-0.5 rounded" style={{ backgroundColor: C.overlay, color: C.sub }}>{app.category}</span>
              </div>
            ))}
          </div>
        </div>
      </section>

      {/* Desktop Environment Demo */}
      <section id="desktop" className="py-16 px-4">
        <div className="max-w-5xl mx-auto">
          <h2 className="text-2xl md:text-3xl font-bold text-center mb-3" style={{ color: C.text }}>Desktop Environment</h2>
          <p className="text-center mb-8 text-sm" style={{ color: C.sub }}>Watch it boot, then double-click icons to open apps. Drag windows. Use the taskbar.</p>
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
            <code className="px-1.5 py-0.5 rounded text-xs" style={{ backgroundColor: C.surface, color: C.blue }}>secureboot</code>{' '}
            <code className="px-1.5 py-0.5 rounded text-xs" style={{ backgroundColor: C.surface, color: C.blue }}>pkg list</code>
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
            <a href="#security" className="hover:text-codix-blue transition-colors">Security</a>
            <a href="#apps" className="hover:text-codix-blue transition-colors">Apps</a>
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
