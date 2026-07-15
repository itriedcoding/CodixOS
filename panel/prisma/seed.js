const { PrismaClient } = require("@prisma/client");
const prisma = new PrismaClient();

async function main() {
  console.log("Seeding database...");

  const admin = await prisma.user.upsert({
    where: { email: "admin@codixpanel.dev" },
    update: {},
    create: {
      email: "admin@codixpanel.dev",
      username: "admin",
      password: "$2a$12$LJ3m4ys3Lz0YBNOURq0Y3OjCfWjFZnGzQxQxQxQxQxQxQxQxQxQxQ",
      root_admin: true,
    },
  });
  console.log("Admin user:", admin.id);

  const node1 = await prisma.node.create({
    data: {
      name: "US-East-1",
      location_id: "00000000-0000-0000-0000-000000000001",
      fqdn: "node1.codixpanel.dev",
      scheme: "https",
      behind_proxy: false,
      memory: 32768,
      disk: 512000,
      cpu: 2400,
      io: 500,
    },
  });

  const node2 = await prisma.node.create({
    data: {
      name: "EU-West-1",
      location_id: "00000000-0000-0000-0000-000000000002",
      fqdn: "node2.codixpanel.dev",
      scheme: "https",
      behind_proxy: false,
      memory: 65536,
      disk: 1024000,
      cpu: 4800,
      io: 500,
    },
  });
  console.log("Nodes created");

  const mcEgg = await prisma.egg.create({
    data: {
      name: "Minecraft Java",
      category: "Minecraft",
      description: "Minecraft Java Edition server with Paper, Spigot, or Fabric support",
      docker_image: "ghcr.io/pterodactyl/yolks:java_21",
      startup: "java -Xms${SERVER_MEMORY}M -Xmx${SERVER_MEMORY}M -jar server.jar nogui",
      config_files: ["server.properties", "eula.txt"],
    },
  });

  const rustEgg = await prisma.egg.create({
    data: {
      name: "Rust",
      category: "Rust",
      description: "Rust dedicated server with Oxide/uMod support",
      docker_image: "ghcr.io/pterodactyl/yolks:rust",
      startup: "./rust_server -batchmode +server.port ${SERVER_PORT} +server.identity ${SERVER_IDENTITY}",
      config_files: ["server.cfg"],
    },
  });

  const cs2Egg = await prisma.egg.create({
    data: {
      name: "CS2",
      category: "Counter-Strike",
      description: "Counter-Strike 2 dedicated server",
      docker_image: "ghcr.io/pterodactyl/yolks:source",
      startup: "./srcds_run -game csgo +ip ${SERVER_IP} +port ${SERVER_PORT} +map ${START_MAP}",
      config_files: ["server.cfg", "autoexec.cfg"],
    },
  });

  const arkEgg = await prisma.egg.create({
    data: {
      name: "ARK: Survival Evolved",
      category: "ARK",
      description: "ARK: Survival Evolved dedicated server",
      docker_image: "ghcr.io/pterodactyl/yolks:ark",
      startup: "./ShooterGameServer TheIsland?listen?bIsApiActive=true?RCONPort=${RCON_PORT}?QueryPort=${SERVER_PORT}",
      config_files: ["Game.ini", "GameUserSettings.ini"],
    },
  });

  const terrariaEgg = await prisma.egg.create({
    data: {
      name: "Terraria",
      category: "Terraria",
      description: "Terraria dedicated server with tModLoader support",
      docker_image: "ghcr.io/pterodactyl/yolks:terraria",
      startup: "dotnet tModLoader.dll -port ${SERVER_PORT} -world ${WORLD_NAME}",
      config_files: ["serverconfig.txt"],
    },
  });

  const valheimEgg = await prisma.egg.create({
    data: {
      name: "Valheim",
      category: "Valheim",
      description: "Valheim dedicated server",
      docker_image: "ghcr.io/pterodactyl/yolks:valheim",
      startup: "./valheim_server.x86_64 -name ${SERVER_NAME} -port ${SERVER_PORT} -world ${WORLD_NAME} -password ${SERVER_PASSWORD}",
      config_files: [],
    },
  });

  const palworldEgg = await prisma.egg.create({
    data: {
      name: "Palworld",
      category: "Palworld",
      description: "Palworld dedicated server",
      docker_image: "ghcr.io/pterodactyl/yolks:palworld",
      startup: "./PalServer.sh -port=${SERVER_PORT} -players=${MAX_PLAYERS}",
      config_files: ["PalWorldSettings.ini"],
    },
  });

  const factorioEgg = await prisma.egg.create({
    data: {
      name: "Factorio",
      category: "Factorio",
      description: "Factorio headless dedicated server",
      docker_image: "ghcr.io/pterodactyl/yolks:factorio",
      startup: "./factorio --start-server ${SAVE_NAME} --port ${SERVER_PORT} --server-settings settings.json",
      config_files: ["settings.json"],
    },
  });

  console.log("Eggs created");

  const server1 = await prisma.server.create({
    data: {
      user_id: admin.id,
      name: "Survival MC",
      node_id: node1.id,
      egg_id: mcEgg.id,
      uuid: "a1b2c3d4-e5f6-7890-abcd-ef1234567890",
      startup: "java -Xms2G -Xmx4G -jar server.jar nogui",
      environment: { SERVER_MEMORY: "4096", SERVER_PORT: "25565", JAVA_VERSION: "21" },
      image: "ghcr.io/pterodactyl/yolks:java_21",
      skip_egg_install: false,
    },
  });

  const server2 = await prisma.server.create({
    data: {
      user_id: admin.id,
      name: "Creative MC",
      node_id: node1.id,
      egg_id: mcEgg.id,
      uuid: "b2c3d4e5-f6a7-8901-bcde-f12345678901",
      startup: "java -Xms1G -Xmx2G -jar server.jar nogui",
      environment: { SERVER_MEMORY: "2048", SERVER_PORT: "25566", JAVA_VERSION: "21" },
      image: "ghcr.io/pterodactyl/yolks:java_21",
      skip_egg_install: false,
    },
  });

  const server3 = await prisma.server.create({
    data: {
      user_id: admin.id,
      name: "Rust EU Official",
      node_id: node2.id,
      egg_id: rustEgg.id,
      uuid: "c3d4e5f6-a7b8-9012-cdef-123456789012",
      startup: "./rust_server -batchmode +server.port 28015 +server.identity rust_main",
      environment: { SERVER_PORT: "28015", SERVER_IDENTITY: "rust_main", RCON_PORT: "28016" },
      image: "ghcr.io/pterodactyl/yolks:rust",
      skip_egg_install: false,
    },
  });

  const server4 = await prisma.server.create({
    data: {
      user_id: admin.id,
      name: "CS2 Competitive",
      node_id: node1.id,
      egg_id: cs2Egg.id,
      uuid: "d4e5f6a7-b8c9-0123-defa-234567890123",
      startup: "./srcds_run -game csgo +ip 0.0.0.0 +port 27015 +map de_dust2",
      environment: { SERVER_PORT: "27015", SERVER_IP: "0.0.0.0", START_MAP: "de_dust2" },
      image: "ghcr.io/pterodactyl/yolks:source",
      skip_egg_install: false,
    },
  });

  const server5 = await prisma.server.create({
    data: {
      user_id: admin.id,
      name: "ARK Island",
      node_id: node2.id,
      egg_id: arkEgg.id,
      uuid: "e5f6a7b8-c9d0-1234-efab-345678901234",
      startup: "./ShooterGameServer TheIsland?listen?QueryPort=7777",
      environment: { SERVER_PORT: "7777", RCON_PORT: "27020" },
      image: "ghcr.io/pterodactyl/yolks:ark",
      skip_egg_install: false,
    },
  });

  const server6 = await prisma.server.create({
    data: {
      user_id: admin.id,
      name: "Terraria Calamity",
      node_id: node1.id,
      egg_id: terrariaEgg.id,
      uuid: "f6a7b8c9-d0e1-2345-fabc-456789012345",
      startup: "dotnet tModLoader.dll -port 7777 -world World1",
      environment: { SERVER_PORT: "7777", WORLD_NAME: "World1" },
      image: "ghcr.io/pterodactyl/yolks:terraria",
      skip_egg_install: false,
    },
  });

  const server7 = await prisma.server.create({
    data: {
      user_id: admin.id,
      name: "Valheim Plains",
      node_id: node2.id,
      egg_id: valheimEgg.id,
      uuid: "a7b8c9d0-e1f2-3456-abcd-567890123456",
      startup: "./valheim_server.x86_64 -name MyServer -port 2456 -world Dedicated1",
      environment: { SERVER_PORT: "2456", SERVER_NAME: "MyServer", WORLD_NAME: "Dedicated1", SERVER_PASSWORD: "valheim123" },
      image: "ghcr.io/pterodactyl/yolks:valheim",
      skip_egg_install: false,
    },
  });

  const server8 = await prisma.server.create({
    data: {
      user_id: admin.id,
      name: "Palworld Fun",
      node_id: node1.id,
      egg_id: palworldEgg.id,
      uuid: "b8c9d0e1-f2a3-4567-bcde-678901234567",
      startup: "./PalServer.sh -port=8211 -players=16",
      environment: { SERVER_PORT: "8211", MAX_PLAYERS: "16" },
      image: "ghcr.io/pterodactyl/yolks:palworld",
      skip_egg_install: false,
    },
  });

  console.log("8 Servers created");

  await prisma.activityLog.createMany({
    data: [
      { user_id: admin.id, event: 'Server "Survival MC" was created', ip: "192.168.1.1", data: { serverId: server1.id } },
      { user_id: admin.id, event: 'Server "Rust EU Official" was started', ip: "192.168.1.1", data: { serverId: server3.id } },
      { user_id: admin.id, event: 'Node "US-East-1" was registered', ip: "192.168.1.1", data: { nodeId: node1.id } },
      { user_id: admin.id, event: 'Backup created for "CS2 Competitive"', ip: "192.168.1.1", data: { serverId: server4.id } },
      { user_id: admin.id, event: 'Server "ARK Island" was updated', ip: "192.168.1.1", data: { serverId: server5.id } },
      { user_id: admin.id, event: 'Server "Terraria Calamity" was installed', ip: "192.168.1.1", data: { serverId: server6.id } },
      { user_id: admin.id, event: 'Server "Palworld Fun" was created', ip: "192.168.1.1", data: { serverId: server8.id } },
      { user_id: admin.id, event: 'Node "EU-West-1" was registered', ip: "10.0.0.1", data: { nodeId: node2.id } },
    ],
  });

  await prisma.allocation.createMany({
    data: [
      { node_id: node1.id, ip: "192.168.1.10", port: 25565, server_id: server1.id },
      { node_id: node1.id, ip: "192.168.1.10", port: 25566, server_id: server2.id },
      { node_id: node1.id, ip: "192.168.1.10", port: 27015, server_id: server4.id },
      { node_id: node1.id, ip: "192.168.1.10", port: 7777, server_id: server6.id },
      { node_id: node1.id, ip: "192.168.1.10", port: 8211, server_id: server8.id },
      { node_id: node2.id, ip: "10.0.0.10", port: 28015, server_id: server3.id },
      { node_id: node2.id, ip: "10.0.0.10", port: 7777, server_id: server5.id },
      { node_id: node2.id, ip: "10.0.0.10", port: 2456, server_id: server7.id },
      { node_id: node1.id, ip: "192.168.1.10", port: 25570, server_id: null },
      { node_id: node2.id, ip: "10.0.0.10", port: 25565, server_id: null },
    ],
  });

  await prisma.backup.createMany({
    data: [
      { server_id: server1.id, name: "Daily Backup - Jul 14", size: 524288000 },
      { server_id: server1.id, name: "Pre-Update Backup", size: 498073600 },
      { server_id: server3.id, name: "Full Wipe Backup", size: 1073741824 },
      { server_id: server4.id, name: "Config Backup", size: 52428800 },
    ],
  });

  await prisma.database.createMany({
    data: [
      { server_id: server1.id, name: "minecraft_plugins", host: "db.codixpanel.dev", port: 5432, username: "mc_user", password: "secure_pass_123" },
      { server_id: server3.id, name: "rust_data", host: "db.codixpanel.dev", port: 5432, username: "rust_user", password: "secure_pass_456" },
    ],
  });

  console.log("Activity logs, allocations, backups, and databases created");
  console.log("Seed complete!");
}

main()
  .catch((e) => { console.error(e); process.exit(1); })
  .finally(async () => { await prisma.$disconnect(); });
