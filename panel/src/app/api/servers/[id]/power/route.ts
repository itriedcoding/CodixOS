import { NextRequest, NextResponse } from "next/server";
import prisma from "@/lib/prisma";
import { requireAuth } from "@/lib/auth";
import {
  startContainer,
  stopContainer,
  getContainerStats,
} from "@/lib/docker";
import type { ApiResponse } from "@/types";

export async function POST(
  request: NextRequest,
  { params }: { params: { id: string } }
): Promise<NextResponse<ApiResponse>> {
  try {
    const user = await requireAuth(request);
    const body = await request.json();
    const { action } = body;

    if (!action || !["start", "stop", "restart"].includes(action)) {
      return NextResponse.json(
        { success: false, error: "Invalid action. Must be start, stop, or restart" },
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
      switch (action) {
        case "start":
          await startContainer(containerName);
          break;
        case "stop":
          await stopContainer(containerName);
          break;
        case "restart":
          await stopContainer(containerName);
          await startContainer(containerName);
          break;
      }

      await prisma.activityLog.create({
        data: {
          user_id: user.id,
          event: `Server "${server.name}" ${action === "start" ? "started" : action === "stop" ? "stopped" : "restarted"}`,
          ip: request.headers.get("x-forwarded-for") || "unknown",
          data: { serverId: server.id, action },
        },
      });

      return NextResponse.json({
        success: true,
        message: `Server ${action} successful`,
      });
    } catch (dockerError) {
      console.error("Docker error:", dockerError);
      return NextResponse.json(
        { success: false, error: `Failed to ${action} server container` },
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
    console.error("Power action error:", error);
    return NextResponse.json(
      { success: false, error: "Internal server error" },
      { status: 500 }
    );
  }
}
