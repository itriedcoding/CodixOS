# CodixOS

A lightweight, terminal-based operating system designed for simplicity and ease of use.

## Features

- **Custom Kernel**: Lightweight kernel with memory management and process scheduling
- **Custom Shell**: Feature-rich shell with command history and tab completion
- **Package Manager**: Simple package management system
- **Desktop Environment**: Optional GUI with window manager and applications
- **Live USB**: Boot directly from USB without installation
- **Windows Support**: Run via WSL or standalone installer

## Quick Start

### Prerequisites

- GCC/G++ compiler
- NASM assembler
- GNU Make
- QEMU (for testing)

### Building from Source

```bash
# Clone the repository
git clone https://github.com/codixos/CodixOS.git
cd CodixOS

# Build everything
./build-user.sh all

# Or build individual components
./build-user.sh shell      # Build shell only
./build-user.sh utils      # Build utilities only
./build-user.sh desktop    # Build desktop environment
```

### Running CodixOS

#### Terminal Mode

```bash
# Run the shell
./build/codix-sh
```

#### Desktop Mode (requires SDL2 and X11)

```bash
# Install dependencies
sudo apt-get install libsdl2-dev xorg

# Build and start desktop
./build-user.sh desktop
./scripts/start-desktop.sh
```

#### Using QEMU

```bash
# Build ISO
./build-iso.sh

# Run in QEMU
qemu-system-x86_64 -cdrom codixos-1.0.0.iso -m 512M
```

## Project Structure

```
CodixOS/
├── codixos/                 # Main OS source code
│   ├── kernel/             # Custom kernel
│   ├── shell/              # Command shell
│   ├── utils/              # System utilities
│   ├── terminal/           # Terminal emulator
│   ├── pkgmanager/         # Package manager
│   ├── init/               # Init system
│   └── desktop/            # Desktop environment
│       ├── wm/             # Window manager
│       ├── shell/          # Desktop shell
│       └── apps/           # GUI applications
├── iso/                    # ISO build configuration
├── website/                # Project website
├── build-user.sh           # Build script (Linux/WSL)
├── build.bat               # Build script (Windows)
├── build-iso.sh            # ISO builder
└── README.md               # This file
```

## Documentation

- [Installation Guide](docs/installation.md)
- [User Guide](docs/user-guide.md)
- [Developer Guide](docs/developer.md)
- [API Reference](docs/api.md)

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by Linux, Minix, and other Unix-like operating systems
- Built with simplicity and learning in mind
- Thanks to all contributors
