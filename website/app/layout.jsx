import '../styles/globals.css'

export const metadata = {
  metadataBase: new URL('https://codixos-botzilla.vercel.app'),
  title: 'CodixOS - Lightweight Terminal Operating System',
  description: 'A lightweight, terminal-based operating system designed for simplicity and ease of use.',
  openGraph: {
    title: 'CodixOS - Lightweight Terminal Operating System',
    description: 'A lightweight, terminal-based operating system designed for simplicity and ease of use.',
    url: 'https://codixos-botzilla.vercel.app',
    siteName: 'CodixOS',
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
