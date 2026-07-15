import { NextRequest, NextResponse } from "next/server";
import prisma from "@/lib/prisma";
import { requireAuth } from "@/lib/auth";
import type { ApiResponse, Backup } from "@/types";

export async function GET(
  request: NextRequest,
  { params }: { params: { id: string } }
): Promise<NextResponse<ApiResponse<Backup[]>>> {
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

    const backups = await prisma.backup.findMany({
      where: { server_id: params.id },
      orderBy: { created_at: "desc" },
    });

    return NextResponse.json({
      success: true,
      data: backups,
    });
  } catch (error) {
    if (error instanceof Error && error.message === "Unauthorized") {
      return NextResponse.json(
        { success: false, error: "Unauthorized" },
        { status: 401 }
      );
    }
    console.error("Get backups error:", error);
    return NextResponse.json(
      { success: false, error: "Internal server error" },
      { status: 500 }
    );
  }
}

export async function POST(
  request: NextRequest,
  { params }: { params: { id: string } }
): Promise<NextResponse<ApiResponse<Backup>>> {
  try {
    const user = await requireAuth(request);
    const body = await request.json();
    const { name } = body;

    if (!name) {
      return NextResponse.json(
        { success: false, error: "Backup name is required" },
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

    const backup = await prisma.backup.create({
      data: {
        server_id: params.id,
        name,
        size: 0,
      },
    });

    await prisma.activityLog.create({
      data: {
        user_id: user.id,
        event: `Backup "${name}" created for server "${server.name}"`,
        ip: request.headers.get("x-forwarded-for") || "unknown",
        data: { serverId: server.id, backupId: backup.id },
      },
    });

    return NextResponse.json({
      success: true,
      data: backup,
    });
  } catch (error) {
    if (error instanceof Error && error.message === "Unauthorized") {
      return NextResponse.json(
        { success: false, error: "Unauthorized" },
        { status: 401 }
      );
    }
    console.error("Create backup error:", error);
    return NextResponse.json(
      { success: false, error: "Internal server error" },
      { status: 500 }
    );
  }
}
