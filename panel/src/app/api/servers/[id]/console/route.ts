import { NextRequest, NextResponse } from "next/server";
import prisma from "@/lib/prisma";
import { requireAuth } from "@/lib/auth";
import { getContainerStats } from "@/lib/docker";
import type { ApiResponse, ServerStats } from "@/types";

export async function GET(
  request: NextRequest,
  { params }: { params: { id: string } }
): Promise<NextResponse<ApiResponse<ServerStats>>> {
  try {
    const user = await requireAuth(request);

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
      const stats = await getContainerStats(containerName);
      return NextResponse.json({
        success: true,
        data: {
          ...stats,
          disk_usage: 0,
          disk_limit: 0,
          uptime: 0,
        },
      });
    } catch {
      return NextResponse.json({
        success: true,
        data: {
          cpu_percent: 0,
          memory_usage: 0,
          memory_limit: 0,
          disk_usage: 0,
          disk_limit: 0,
          network_rx: 0,
          network_tx: 0,
          uptime: 0,
        },
      });
    }
  } catch (error) {
    if (error instanceof Error && error.message === "Unauthorized") {
      return NextResponse.json(
        { success: false, error: "Unauthorized" },
        { status: 401 }
      );
    }
    console.error("Get console stats error:", error);
    return NextResponse.json(
      { success: false, error: "Internal server error" },
      { status: 500 }
    );
  }
}
