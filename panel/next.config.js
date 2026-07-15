/** @type {import('next').NextConfig} */
const nextConfig = {
  webpack: (config, { isServer }) => {
    if (isServer) {
      config.externals = [
        ...(config.externals || []),
        { canvas: "canvas" },
        "dockerode",
        "ssh2",
      ];
    }
    return config;
  },
};

module.exports = nextConfig;
