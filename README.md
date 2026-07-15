# Codix Panel

Open-source game server management panel. Alternative to Pterodactyl Panel.

## Features

- Multi-game support (Minecraft, Rust, ARK, CS2, Terraria, Factorio, Palworld, Valheim)
- Docker-based server isolation
- Real-time console access
- File manager with code editor
- Database management (MySQL, PostgreSQL, MongoDB)
- User management with role-based access control
- Automated backups with restore
- Server scheduling (auto-start, auto-stop, tasks)
- REST API with API keys
- Server monitoring (CPU, RAM, disk, network)
- One-click mod/plugin installation
- Node (Wings) daemon for distributed hosting

## Architecture

- **Panel** - Web interface and API (Next.js + TypeScript + Prisma + PostgreSQL)
- **Wings** - Node daemon (Node.js + Docker API) runs on each game server node

## Quick Start

### Docker (Recommended)

```bash
docker compose up -d
```

### Manual

1. Clone the repo
2. `cd panel && npm install`
3. Configure `.env` (see `.env.example`)
4. `npx prisma db push`
5. `npm run build && npm start`

### Wings Daemon

1. `cd daemon && npm install`
2. Configure `.env` (see `.env.example`)
3. `npm run build && npm start`

## Supported Games

| Game | Eggs Available | Mods Supported |
|------|---------------|----------------|
| Minecraft | Vanilla, Paper, Spigot, Forge, Fabric, Purpur, BungeeCord, Velocity | Yes |
| Rust | Vanilla, Oxide, uMod | Yes |
| ARK | Vanilla, Mods | Yes |
| CS2 | Vanilla, Custom | Yes |
| Terraria | TModLoader | Yes |
| Factorio | Vanilla | Yes |
| Palworld | Vanilla | Yes |
| Valheim | Vanilla, BepInEx | Yes |

## API Documentation

See `/docs/api.md`

## Tech Stack

- Next.js 14 (App Router)
- TypeScript
- Prisma ORM
- PostgreSQL
- Docker
- Tailwind CSS
- JWT Authentication
- WebSocket

## Project Structure

```
CodixOS/
├── panel/                  # Web interface and API
│   ├── src/                # Next.js source
│   ├── prisma/             # Database schema
│   └── package.json
├── daemon/                 # Wings daemon
│   ├── src/                # TypeScript source
│   └── package.json
├── docker-compose.yml      # Full stack deployment
├── .github/workflows/      # CI/CD
└── README.md
```

## License

MIT License - see [LICENSE](LICENSE) for details.
