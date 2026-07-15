import { NextRequest, NextResponse } from "next/server";
import { v4 as uuidv4 } from "uuid";
import prisma from "@/lib/prisma";
import { requireAuth } from "@/lib/auth";
import type { ApiResponse, Database } from "@/types";

export async function GET(
  request: NextRequest,
  { params }: { params: { id: string } }
): Promise<NextResponse<ApiResponse<Database[]>>> {
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

    const databases = await prisma.database.findMany({
      where: { server_id: params.id },
      orderBy: { created_at: "desc" },
    });

    return NextResponse.json({
      success: true,
      data: databases,
    });
  } catch (error) {
    if (error instanceof Error && error.message === "Unauthorized") {
      return NextResponse.json(
        { success: false, error: "Unauthorized" },
        { status: 401 }
      );
    }
    console.error("Get databases error:", error);
    return NextResponse.json(
      { success: false, error: "Internal server error" },
      { status: 500 }
    );
  }
}

export async function POST(
  request: NextRequest,
  { params }: { params: { id: string } }
): Promise<NextResponse<ApiResponse<Database>>> {
  try {
    const user = await requireAuth(request);
    const body = await request.json();
    const { name } = body;

    if (!name) {
      return NextResponse.json(
        { success: false, error: "Database name is required" },
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

    const existingDb = await prisma.database.findFirst({
      where: {
        server_id: params.id,
        name,
      },
    });

    if (existingDb) {
      return NextResponse.json(
        { success: false, error: "Database with this name already exists" },
        { status: 409 }
      );
    }

    const dbPassword = uuidv4().replace(/-/g, "").substring(0, 16);

    const database = await prisma.database.create({
      data: {
        server_id: params.id,
        name,
        host: "localhost",
        port: 5432,
        username: `codix_${name}`,
        password: dbPassword,
      },
    });

    await prisma.activityLog.create({
      data: {
        user_id: user.id,
        event: `Database "${name}" created for server "${server.name}"`,
        ip: request.headers.get("x-forwarded-for") || "unknown",
        data: { serverId: server.id, databaseId: database.id },
      },
    });

    return NextResponse.json({
      success: true,
      data: database,
    });
  } catch (error) {
    if (error instanceof Error && error.message === "Unauthorized") {
      return NextResponse.json(
        { success: false, error: "Unauthorized" },
        { status: 401 }
      );
    }
    console.error("Create database error:", error);
    return NextResponse.json(
      { success: false, error: "Internal server error" },
      { status: 500 }
    );
  }
}
