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
        'codix': {
          'bg': '#1e1e2e',
          'surface': '#313244',
          'overlay': '#45475a',
          'text': '#cdd6f4',
          'subtext': '#a6adc8',
          'blue': '#89b4fa',
          'green': '#a6e3a1',
          'yellow': '#f9e2af',
          'red': '#f38ba8',
          'magenta': '#f5c2e7',
          'teal': '#94e2d5',
          'peach': '#fab387',
        },
      },
      fontFamily: {
        'mono': ['Ubuntu Mono', 'monospace'],
        'sans': ['Ubuntu', 'sans-serif'],
      },
      animation: {
        'glow': 'glow 2s ease-in-out infinite alternate',
        'float': 'float 6s ease-in-out infinite',
        'matrix': 'matrix 20s linear infinite',
      },
      keyframes: {
        glow: {
          '0%': { boxShadow: '0 0 20px rgba(137, 180, 250, 0.3)' },
          '100%': { boxShadow: '0 0 40px rgba(137, 180, 250, 0.6)' },
        },
        float: {
          '0%, 100%': { transform: 'translateY(0)' },
          '50%': { transform: 'translateY(-10px)' },
        },
        matrix: {
          '0%': { transform: 'translateY(0)' },
          '100%': { transform: 'translateY(-100%)' },
        },
      },
    },
  },
  plugins: [],
}
