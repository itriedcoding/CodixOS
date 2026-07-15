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
          DEFAULT: "#1e1e2e",
          50: "#181825",
          100: "#1e1e2e",
          200: "#313244",
          300: "#45475a",
          400: "#585b70",
        },
        secondary: "#181825",
        tertiary: "#313244",
        accent: {
          blue: "#89b4fa",
          green: "#a6e3a1",
          red: "#f38ba8",
          yellow: "#f9e2af",
          mauve: "#cba6f7",
          peach: "#fab387",
          teal: "#94e2d5",
          lavender: "#b4befe",
        },
        text: {
          primary: "#cdd6f4",
          secondary: "#a6adc8",
          muted: "#6c7086",
        },
      },
    },
  },
  plugins: [],
};
