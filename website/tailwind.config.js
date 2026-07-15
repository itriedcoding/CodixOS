/** @type {import('tailwindcss').Config} */
module.exports = {
  content: [
    './pages/**/*.{js,ts,jsx,tsx,mdx}',
    './components/**/*.{js,ts,jsx,tsx,mdx}',
    './app/**/*.{js,ts,jsx,tsx,mdx}',
  ],
  theme: {
    extend: {
      colors: {
        'bg-primary': '#1e1e2e',
        'bg-secondary': '#181825',
        'bg-tertiary': '#313244',
        'bg-card': '#11111b',
        'accent-blue': '#89b4fa',
        'accent-green': '#a6e3a1',
        'accent-red': '#f38ba8',
        'accent-yellow': '#f9e2af',
        'accent-purple': '#cba6f7',
        'accent-pink': '#f5c2e7',
        'accent-teal': '#94e2d5',
        'accent-peach': '#fab387',
        'text-primary': '#cdd6f4',
        'text-secondary': '#a6adc8',
        'text-muted': '#6c7086',
        'border': '#45475a',
      },
      fontFamily: {
        'mono': ['JetBrains Mono', 'Fira Code', 'monospace'],
      },
    },
  },
  plugins: [],
}
