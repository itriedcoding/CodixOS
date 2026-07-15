import { NextRequest, NextResponse } from "next/server";
import prisma from "@/lib/prisma";
import { requireAuth } from "@/lib/auth";
import { execCommand } from "@/lib/docker";
import type { ApiResponse, FileEntry } from "@/types";

export async function GET(
  request: NextRequest,
  { params }: { params: { id: string } }
): Promise<NextResponse<ApiResponse<FileEntry[]>>> {
  try {
    const user = await requireAuth(request);
    const { searchParams } = new URL(request.url);
    const path = searchParams.get("path") || "/";

    const server = await prisma.server.findFirst({
      where: {
        id: params.id,
        user_id: user.id,
      },
    });

    if (!server) {
      return NextResponse.json(
        { success: false, error: "Server not found" },
        { status: 404 }
      );
    }

    const containerName = `codix-server-${server.uuid}`;

    try {
      const output = await execCommand(containerName, [
        "ls",
        "-la",
        "--time-style=long-iso",
        path,
      ]);

      const lines = output.split("\n").filter((line) => line.trim());
      const files: FileEntry[] = [];

      for (let i = 1; i < lines.length; i++) {
        const parts = lines[i].split(/\s+/);
        if (parts.length < 9) continue;

        const isDirectory = parts[0].startsWith("d");
        const name = parts.slice(8).join(" ");
        const size = parseInt(parts[4]) || 0;
        const modified = new Date(`${parts[5]} ${parts[6]}`);

        files.push({
          name,
          path: path === "/" ? `/${name}` : `${path}/${name}`,
          is_directory: isDirectory,
          size,
          modified,
          created: modified,
        });
      }

      return NextResponse.json({
        success: true,
        data: files,
      });
    } catch {
      return NextResponse.json({
        success: true,
        data: [],
      });
    }
  } catch (error) {
    if (error instanceof Error && error.message === "Unauthorized") {
      return NextResponse.json(
        { success: false, error: "Unauthorized" },
        { status: 401 }
      );
    }
    console.error("Get files error:", error);
    return NextResponse.json(
      { success: false, error: "Internal server error" },
      { status: 500 }
    );
  }
}

export async function POST(
  request: NextRequest,
  { params }: { params: { id: string } }
): Promise<NextResponse<ApiResponse>> {
  try {
    const user = await requireAuth(request);
    const body = await request.json();
    const { path: filePath, content, action } = body;

    if (!filePath) {
      return NextResponse.json(
        { success: false, error: "Path is required" },
        { status: 400 }
      );
    }

    const server = await prisma.server.findFirst({
      where: {
        id: params.id,
        user_id: user.id,
      },
    });

    if (!server) {
      return NextResponse.json(
        { success: false, error: "Server not found" },
        { status: 404 }
      );
    }

    const containerName = `codix-server-${server.uuid}`;

    if (action === "write" && content !== undefined) {
      try {
        const base64Content = Buffer.from(content).toString("base64");
        await execCommand(containerName, [
          "sh",
          "-c",
          `echo "${base64Content}" | base64 -d > ${filePath}`,
        ]);

        await prisma.activityLog.create({
          data: {
            user_id: user.id,
            event: `File "${filePath}" written on server "${server.name}"`,
            ip: request.headers.get("x-forwarded-for") || "unknown",
            data: { serverId: server.id, path: filePath },
          },
        });

        return NextResponse.json({
          success: true,
          message: "File written successfully",
        });
      } catch (writeError) {
        return NextResponse.json(
          { success: false, error: "Failed to write file" },
          { status: 500 }
        );
      }
    }

    if (action === "read") {
      try {
        const content = await execCommand(containerName, ["cat", filePath]);
        return NextResponse.json({
          success: true,
          data: { content },
        });
      } catch {
        return NextResponse.json(
          { success: false, error: "Failed to read file" },
          { status: 500 }
        );
      }
    }

    if (action === "delete") {
      try {
        await execCommand(containerName, ["rm", "-rf", filePath]);

        await prisma.activityLog.create({
          data: {
            user_id: user.id,
            event: `File "${filePath}" deleted on server "${server.name}"`,
            ip: request.headers.get("x-forwarded-for") || "unknown",
            data: { serverId: server.id, path: filePath },
          },
        });

        return NextResponse.json({
          success: true,
          message: "File deleted successfully",
        });
      } catch {
        return NextResponse.json(
          { success: false, error: "Failed to delete file" },
          { status: 500 }
        );
      }
    }

    return NextResponse.json(
      { success: false, error: "Invalid action" },
      { status: 400 }
    );
  } catch (error) {
    if (error instanceof Error && error.message === "Unauthorized") {
      return NextResponse.json(
        { success: false, error: "Unauthorized" },
        { status: 401 }
      );
    }
    console.error("File operation error:", error);
    return NextResponse.json(
      { success: false, error: "Internal server error" },
      { status: 500 }
    );
  }
}
