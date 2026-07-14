'use client'

import { useState, useEffect } from 'react'
import { motion, AnimatePresence } from 'framer-motion'
import { 
  Terminal, 
  Download, 
  Github, 
  Cpu, 
  HardDrive, 
  MemoryStick,
  Monitor,
  Zap,
  Shield,
  Package,
  ChevronDown,
  ExternalLink,
  Menu,
  X
} from 'lucide-react'

// Terminal Demo Component
function TerminalDemo() {
  const [lines, setLines] = useState<string[]>([])
  const [currentInput, setCurrentInput] = useState('')
  const [isTyping, setIsTyping] = useState(true)

  const demoCommands = [
    { input: 'neofetch', delay: 100 },
    { input: 'ls -la', delay: 200 },
    { input: 'pkg install vim', delay: 150 },
    { input: 'free -h', delay: 100 },
  ]

  useEffect(() => {
    let lineIndex = 0
    const addLine = () => {
      if (lineIndex < demoCommands.length) {
        setLines(prev => [...prev, `$ ${demoCommands[lineIndex].input}`])
        lineIndex++
        setTimeout(addLine, demoCommands[lineIndex - 1].delay)
      } else {
        setIsTyping(false)
      }
    }
    setTimeout(addLine, 1000)
  }, [])

  return (
    <div className="terminal-window max-w-3xl mx-auto">
      <div className="terminal-header">
        <div className="terminal-dot bg-codix-red"></div>
        <div className="terminal-dot bg-codix-yellow"></div>
        <div className="terminal-dot bg-codix-green"></div>
        <span className="ml-2 text-codix-subtext text-sm">codix@codixos:~</span>
      </div>
      <div className="terminal-body h-80 overflow-y-auto">
        <div className="text-codix-green mb-2">
          {`        _____      _               ____   _____ `}
        </div>
        <div className="text-codix-green mb-2">
          {`       / ____|    | |             |  _ \\ / ____|`}
        </div>
        <div className="text-codix-green mb-2">
          {`      | |     ___ | | ___  _ __   | |_) | (___  `}
        </div>
        <div className="text-codix-green mb-2">
          {`      | |    / _ \\| |/ _ \\| '__|  |  _ < \\___ \\ `}
        </div>
        <div className="text-codix-green mb-2">
          {`      | |___| (_) | | (_) | |     | |_) |____) |`}
        </div>
        <div className="text-codix-green mb-4">
          {`       \\_____\\___/|_|\\___/|_|     |____/|_____/ `}
        </div>
        <div className="text-codix-subtext mb-4">
          Welcome to CodixOS v1.0.0 - Type 'help' for commands
        </div>
        {lines.map((line, i) => (
          <div key={i} className="mb-1">
            <span className="text-codix-blue">$</span>
            <span className="text-codix-text ml-2">{line.slice(2)}</span>
            {line.includes('neofetch') && (
              <div className="text-codix-subtext ml-4 mt-2">
                <div>OS: CodixOS 1.0.0</div>
                <div>Kernel: codix-kernel 1.0.0</div>
                <div>Shell: codix-sh</div>
                <div>CPU: Virtual CPU @ 2.4GHz</div>
                <div>Memory: 128MB / 256MB</div>
              </div>
            )}
            {line.includes('ls -la') && (
              <div className="text-codix-subtext ml-4 mt-2">
                <div>total 24</div>
                <div>drwxr-xr-x 2 codix codix 4096 Jan 14 04:39 .</div>
                <div>-rw-r--r-- 1 codix codix  128 Jan 14 04:39 .bashrc</div>
                <div>drwxr-xr-x 2 codix codix 4096 Jan 14 04:39 Documents</div>
              </div>
            )}
          </div>
        ))}
        {!isTyping && (
          <div className="flex items-center">
            <span className="text-codix-blue">$</span>
            <span className="ml-2 animate-pulse">▌</span>
          </div>
        )}
      </div>
    </div>
  )
}

// Feature Card Component
function FeatureCard({ icon: Icon, title, description }: { 
  icon: any, 
  title: string, 
  description: string 
}) {
  return (
    <motion.div 
      className="card group"
      whileHover={{ scale: 1.02 }}
      transition={{ duration: 0.2 }}
    >
      <div className="text-codix-blue mb-4 group-hover:text-codix-green transition-colors">
        <Icon size={32} />
      </div>
      <h3 className="text-xl font-semibold mb-2 text-codix-text">{title}</h3>
      <p className="text-codix-subtext">{description}</p>
    </motion.div>
  )
}

// Stat Card Component
function StatCard({ icon: Icon, label, value, color }: {
  icon: any,
  label: string,
  value: string,
  color: string
}) {
  return (
    <div className="text-center">
      <div className={`text-3xl mb-2 ${color}`}>
        <Icon size={32} className="mx-auto" />
      </div>
      <div className="text-2xl font-bold text-codix-text">{value}</div>
      <div className="text-codix-subtext">{label}</div>
    </div>
  )
}

// Navigation
function Navbar() {
  const [isOpen, setIsOpen] = useState(false)

  return (
    <nav className="fixed top-0 left-0 right-0 z-50 bg-codix-bg/80 backdrop-blur-lg border-b border-codix-overlay">
      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
        <div className="flex items-center justify-between h-16">
          <div className="flex items-center gap-2">
            <Terminal className="text-codix-blue" size={28} />
            <span className="text-xl font-bold text-codix-text">CodixOS</span>
          </div>
          
          <div className="hidden md:flex items-center gap-8">
            <a href="#features" className="text-codix-subtext hover:text-codix-blue transition-colors">Features</a>
            <a href="#demo" className="text-codix-subtext hover:text-codix-blue transition-colors">Demo</a>
            <a href="#download" className="text-codix-subtext hover:text-codix-blue transition-colors">Download</a>
            <a href="#docs" className="text-codix-subtext hover:text-codix-blue transition-colors">Docs</a>
            <a 
              href="https://github.com/codixos" 
              target="_blank" 
              rel="noopener noreferrer"
              className="text-codix-subtext hover:text-codix-blue transition-colors"
            >
              <Github size={20} />
            </a>
            <a href="#download" className="btn-primary">Download</a>
          </div>

          <button 
            className="md:hidden text-codix-text"
            onClick={() => setIsOpen(!isOpen)}
          >
            {isOpen ? <X size={24} /> : <Menu size={24} />}
          </button>
        </div>
      </div>

      <AnimatePresence>
        {isOpen && (
          <motion.div 
            initial={{ opacity: 0, height: 0 }}
            animate={{ opacity: 1, height: 'auto' }}
            exit={{ opacity: 0, height: 0 }}
            className="md:hidden bg-codix-surface border-t border-codix-overlay"
          >
            <div className="px-4 py-4 space-y-3">
              <a href="#features" className="block text-codix-subtext hover:text-codix-blue">Features</a>
              <a href="#demo" className="block text-codix-subtext hover:text-codix-blue">Demo</a>
              <a href="#download" className="block text-codix-subtext hover:text-codix-blue">Download</a>
              <a href="#docs" className="block text-codix-subtext hover:text-codix-blue">Docs</a>
              <a href="#download" className="block btn-primary text-center">Download</a>
            </div>
          </motion.div>
        )}
      </AnimatePresence>
    </nav>
  )
}

// Hero Section
function Hero() {
  return (
    <section className="min-h-screen flex items-center justify-center relative overflow-hidden pt-16">
      {/* Background Effects */}
      <div className="absolute inset-0 overflow-hidden">
        <div className="absolute -top-40 -right-40 w-80 h-80 bg-codix-blue/20 rounded-full blur-3xl"></div>
        <div className="absolute -bottom-40 -left-40 w-80 h-80 bg-codix-green/20 rounded-full blur-3xl"></div>
      </div>

      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 relative z-10">
        <div className="text-center">
          <motion.div
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.6 }}
          >
            <h1 className="text-5xl md:text-7xl font-bold mb-6">
              <span className="text-codix-blue">Codix</span>
              <span className="text-codix-text">OS</span>
            </h1>
            <p className="text-xl md:text-2xl text-codix-subtext mb-8 max-w-2xl mx-auto">
              A lightweight, terminal-based operating system designed for simplicity and ease of use
            </p>
          </motion.div>

          <motion.div 
            className="flex flex-col sm:flex-row gap-4 justify-center"
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.6, delay: 0.2 }}
          >
            <a href="#download" className="btn-primary flex items-center justify-center gap-2">
              <Download size={20} />
              Download Now
            </a>
            <a href="#demo" className="btn-secondary flex items-center justify-center gap-2">
              <Terminal size={20} />
              Live Demo
            </a>
          </motion.div>

          <motion.div 
            className="mt-12 grid grid-cols-2 md:grid-cols-4 gap-8 max-w-3xl mx-auto"
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.6, delay: 0.4 }}
          >
            <StatCard icon={Cpu} label="CPU Usage" value="< 1%" color="text-codix-blue" />
            <StatCard icon={MemoryStick} label="RAM Usage" value="32MB" color="text-codix-green" />
            <StatCard icon={HardDrive} label="Disk Size" value="128MB" color="text-codix-yellow" />
            <StatCard icon={Zap} label="Boot Time" value="2s" color="text-codix-red" />
          </motion.div>
        </div>

        <motion.div 
          className="mt-16"
          initial={{ opacity: 0, y: 40 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.8, delay: 0.6 }}
        >
          <TerminalDemo />
        </motion.div>
      </div>

      <motion.div 
        className="absolute bottom-8 left-1/2 transform -translate-x-1/2"
        animate={{ y: [0, 10, 0] }}
        transition={{ duration: 2, repeat: Infinity }}
      >
        <ChevronDown className="text-codix-subtext" size={32} />
      </motion.div>
    </section>
  )
}

// Features Section
function Features() {
  const features = [
    {
      icon: Zap,
      title: 'Lightweight',
      description: 'Minimal resource usage with a custom kernel optimized for speed and efficiency.'
    },
    {
      icon: Terminal,
      title: 'Terminal-Based',
      description: 'Full-featured shell with command history, tab completion, and powerful utilities.'
    },
    {
      icon: Package,
      title: 'Package Manager',
      description: 'Easy software installation and management with codix-pkg.'
    },
    {
      icon: Monitor,
      title: 'Desktop Environment',
      description: 'Optional GUI with window manager, file manager, and built-in applications.'
    },
    {
      icon: Shield,
      title: 'Secure',
      description: 'Built with security in mind, featuring user permissions and sandboxing.'
    },
    {
      icon: HardDrive,
      title: 'Live USB',
      description: 'Boot directly from USB without installation. Perfect for testing and rescue.'
    },
  ]

  return (
    <section id="features" className="py-20 bg-codix-surface/30">
      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
        <motion.div 
          className="text-center mb-16"
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          viewport={{ once: true }}
        >
          <h2 className="text-4xl font-bold mb-4 text-codix-text">Features</h2>
          <p className="text-codix-subtext max-w-2xl mx-auto">
            Everything you need in a modern operating system, without the bloat
          </p>
        </motion.div>

        <div className="grid md:grid-cols-2 lg:grid-cols-3 gap-6">
          {features.map((feature, i) => (
            <motion.div
              key={i}
              initial={{ opacity: 0, y: 20 }}
              whileInView={{ opacity: 1, y: 0 }}
              viewport={{ once: true }}
              transition={{ delay: i * 0.1 }}
            >
              <FeatureCard {...feature} />
            </motion.div>
          ))}
        </div>
      </div>
    </section>
  )
}

// Demo Section
function Demo() {
  return (
    <section id="demo" className="py-20">
      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
        <motion.div 
          className="text-center mb-16"
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          viewport={{ once: true }}
        >
          <h2 className="text-4xl font-bold mb-4 text-codix-text">See It In Action</h2>
          <p className="text-codix-subtext max-w-2xl mx-auto">
            Try CodixOS directly in your browser
          </p>
        </motion.div>

        <TerminalDemo />
      </div>
    </section>
  )
}

// Download Section
function Download() {
  const downloads = [
    { name: 'ISO Image', description: 'Bootable ISO for VMs and live USB', size: '128 MB', icon: HardDrive },
    { name: 'Source Code', description: 'Build from source', size: '15 MB', icon: Github },
    { name: 'Windows Installer', description: 'Install via WSL', size: '50 MB', icon: Monitor },
  ]

  return (
    <section id="download" className="py-20 bg-codix-surface/30">
      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
        <motion.div 
          className="text-center mb-16"
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          viewport={{ once: true }}
        >
          <h2 className="text-4xl font-bold mb-4 text-codix-text">Download CodixOS</h2>
          <p className="text-codix-subtext max-w-2xl mx-auto">
            Choose the option that works best for you
          </p>
        </motion.div>

        <div className="grid md:grid-cols-3 gap-6">
          {downloads.map((dl, i) => (
            <motion.div
              key={i}
              className="card text-center"
              initial={{ opacity: 0, y: 20 }}
              whileInView={{ opacity: 1, y: 0 }}
              viewport={{ once: true }}
              transition={{ delay: i * 0.1 }}
            >
              <dl.icon className="mx-auto text-codix-blue mb-4" size={48} />
              <h3 className="text-xl font-semibold mb-2 text-codix-text">{dl.name}</h3>
              <p className="text-codix-subtext mb-4">{dl.description}</p>
              <p className="text-codix-subtext text-sm mb-4">{dl.size}</p>
              <button className="btn-primary w-full">Download</button>
            </motion.div>
          ))}
        </div>
      </div>
    </section>
  )
}

// Footer
function Footer() {
  return (
    <footer className="py-12 border-t border-codix-overlay">
      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
        <div className="grid md:grid-cols-4 gap-8">
          <div>
            <div className="flex items-center gap-2 mb-4">
              <Terminal className="text-codix-blue" size={24} />
              <span className="text-lg font-bold text-codix-text">CodixOS</span>
            </div>
            <p className="text-codix-subtext text-sm">
              A lightweight, terminal-based operating system designed for simplicity.
            </p>
          </div>
          
          <div>
            <h4 className="font-semibold mb-4 text-codix-text">Product</h4>
            <ul className="space-y-2 text-codix-subtext text-sm">
              <li><a href="#features" className="hover:text-codix-blue transition-colors">Features</a></li>
              <li><a href="#download" className="hover:text-codix-blue transition-colors">Download</a></li>
              <li><a href="#docs" className="hover:text-codix-blue transition-colors">Documentation</a></li>
            </ul>
          </div>
          
          <div>
            <h4 className="font-semibold mb-4 text-codix-text">Community</h4>
            <ul className="space-y-2 text-codix-subtext text-sm">
              <li><a href="https://github.com/codixos" className="hover:text-codix-blue transition-colors">GitHub</a></li>
              <li><a href="#" className="hover:text-codix-blue transition-colors">Discord</a></li>
              <li><a href="#" className="hover:text-codix-blue transition-colors">Forum</a></li>
            </ul>
          </div>
          
          <div>
            <h4 className="font-semibold mb-4 text-codix-text">Legal</h4>
            <ul className="space-y-2 text-codix-subtext text-sm">
              <li><a href="#" className="hover:text-codix-blue transition-colors">Privacy</a></li>
              <li><a href="#" className="hover:text-codix-blue transition-colors">Terms</a></li>
              <li><a href="#" className="hover:text-codix-blue transition-colors">License (MIT)</a></li>
            </ul>
          </div>
        </div>
        
        <div className="mt-8 pt-8 border-t border-codix-overlay text-center text-codix-subtext text-sm">
          <p>&copy; {new Date().getFullYear()} CodixOS. All rights reserved.</p>
        </div>
      </div>
    </footer>
  )
}

// Main Page
export default function Home() {
  return (
    <main className="min-h-screen">
      <Navbar />
      <Hero />
      <Features />
      <Demo />
      <Download />
      <Footer />
    </main>
  )
}
