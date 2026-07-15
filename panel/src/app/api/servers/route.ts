import { NextRequest, NextResponse } from "next/server";
import { v4 as uuidv4 } from "uuid";
import prisma from "@/lib/prisma";
import { requireAuth } from "@/lib/auth";
import type { ApiResponse } from "@/types";

export async function GET(
  request: NextRequest
): Promise<NextResponse<ApiResponse>> {
  try {
    const user = await requireAuth(request);

    const servers = await prisma.server.findMany({
      where: { user_id: user.id },
      include: {
        node: true,
        egg: true,
      },
      orderBy: { created_at: "desc" },
    });

    return NextResponse.json({
      success: true,
      data: servers,
    });
  } catch (error) {
    if (error instanceof Error && error.message === "Unauthorized") {
      return NextResponse.json(
        { success: false, error: "Unauthorized" },
        { status: 401 }
      );
    }
    console.error("Get servers error:", error);
    return NextResponse.json(
      { success: false, error: "Internal server error" },
      { status: 500 }
    );
  }
}

export async function POST(
  request: NextRequest
): Promise<NextResponse<ApiResponse>> {
  try {
    const user = await requireAuth(request);
    const body = await request.json();
    const { name, node_id, egg_id, startup, environment, image, skip_egg_install } = body;

    if (!name || !node_id || !egg_id) {
      return NextResponse.json(
        { success: false, error: "Name, node_id, and egg_id are required" },
        { status: 400 }
      );
    }

    const node = await prisma.node.findUnique({ where: { id: node_id } });
    if (!node) {
      return NextResponse.json(
        { success: false, error: "Node not found" },
        { status: 404 }
      );
    }

    const egg = await prisma.egg.findUnique({ where: { id: egg_id } });
    if (!egg) {
      return NextResponse.json(
        { success: false, error: "Egg not found" },
        { status: 404 }
      );
    }

    const server = await prisma.server.create({
      data: {
        user_id: user.id,
        name,
        node_id,
        egg_id,
        uuid: uuidv4(),
        startup: startup || egg.startup,
        environment: environment || {},
        image: image || egg.docker_image,
        skip_egg_install: skip_egg_install || false,
      },
      include: {
        node: true,
        egg: true,
      },
    });

    await prisma.activityLog.create({
      data: {
        user_id: user.id,
        event: `Server "${name}" created`,
        ip: request.headers.get("x-forwarded-for") || "unknown",
        data: { serverId: server.id },
      },
    });

    return NextResponse.json({
      success: true,
      data: server,
    });
  } catch (error) {
    if (error instanceof Error && error.message === "Unauthorized") {
      return NextResponse.json(
        { success: false, error: "Unauthorized" },
        { status: 401 }
      );
    }
    console.error("Create server error:", error);
    return NextResponse.json(
      { success: false, error: "Internal server error" },
      { status: 500 }
    );
  }
}
