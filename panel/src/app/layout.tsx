import type { Metadata } from "next";
import "./globals.css";

export const metadata: Metadata = {
  title: "Codix Panel",
  description: "Open-source game server management panel",
};

export default function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <html lang="en">
      <body className="bg-primary text-text-primary antialiased">
        {children}
      </body>
    </html>
  );
}
