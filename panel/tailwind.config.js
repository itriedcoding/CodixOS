/** @type {import('tailwindcss').Config} */
module.exports = {
  content: [
    "./src/pages/**/*.{js,ts,jsx,tsx,mdx}",
    "./src/components/**/*.{js,ts,jsx,tsx,mdx}",
    "./src/app/**/*.{js,ts,jsx,tsx,mdx}",
  ],
  theme: {
    extend: {
      colors: {
        primary: {
          DEFAULT: "#ffffff",
          50: "#f8fafc",
          100: "#f1f5f9",
          200: "#e2e8f0",
          300: "#cbd5e1",
          400: "#94a3b8",
        },
        secondary: "#f8fafc",
        tertiary: "#f1f5f9",
        accent: {
          blue: "#0ea5e9",
          green: "#22c55e",
          red: "#ef4444",
          yellow: "#eab308",
          mauve: "#a855f7",
          peach: "#f97316",
          teal: "#14b8a6",
          lavender: "#818cf8",
        },
        text: {
          primary: "#0f172a",
          secondary: "#475569",
          muted: "#94a3b8",
        },
      },
    },
  },
  plugins: [],
};
