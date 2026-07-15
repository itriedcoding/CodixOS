import '../styles/globals.css'

export const metadata = {
  metadataBase: new URL('https://codixpanel.dev'),
  title: 'Codix Panel - Open-Source Game Server Management',
  description: 'Modern, fast, and secure game server management panel. Alternative to Pterodactyl Panel with Docker isolation, real-time console, and more.',
  openGraph: {
    title: 'Codix Panel - Open-Source Game Server Management',
    description: 'Modern, fast, and secure game server management panel. Alternative to Pterodactyl Panel with Docker isolation, real-time console, and more.',
    url: 'https://codixpanel.dev',
    siteName: 'Codix Panel',
    type: 'website',
  },
}

export default function RootLayout({ children }) {
  return (
    <html lang="en">
      <body>{children}</body>
    </html>
  )
}
