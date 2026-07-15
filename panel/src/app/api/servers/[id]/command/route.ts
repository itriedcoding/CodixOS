import { NextRequest, NextResponse } from "next/server";
import prisma from "@/lib/prisma";
import { requireAuth } from "@/lib/auth";
import { execCommand } from "@/lib/docker";
import type { ApiResponse } from "@/types";

export async function POST(
  request: NextRequest,
  { params }: { params: { id: string } }
): Promise<NextResponse<ApiResponse<{ output: string }>>> {
  try {
    const user = await requireAuth(request);
    const body = await request.json();
    const { command } = body;

    if (!command) {
      return NextResponse.json(
        { success: false, error: "Command is required" },
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

    try {
      const output = await execCommand(containerName, ["sh", "-c", command]);

      await prisma.activityLog.create({
        data: {
          user_id: user.id,
          event: `Command executed on server "${server.name}"`,
          ip: request.headers.get("x-forwarded-for") || "unknown",
          data: { serverId: server.id, command },
        },
      });

      return NextResponse.json({
        success: true,
        data: { output },
      });
    } catch (execError) {
      return NextResponse.json(
        { success: false, error: "Failed to execute command" },
        { status: 500 }
      );
    }
  } catch (error) {
    if (error instanceof Error && error.message === "Unauthorized") {
      return NextResponse.json(
        { success: false, error: "Unauthorized" },
        { status: 401 }
      );
    }
    console.error("Command error:", error);
    return NextResponse.json(
      { success: false, error: "Internal server error" },
      { status: 500 }
    );
  }
}
