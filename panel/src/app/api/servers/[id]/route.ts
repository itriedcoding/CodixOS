import { NextRequest, NextResponse } from "next/server";
import prisma from "@/lib/prisma";
import { requireAuth } from "@/lib/auth";
import type { ApiResponse } from "@/types";

export async function GET(
  request: NextRequest,
  { params }: { params: { id: string } }
): Promise<NextResponse<ApiResponse>> {
  try {
    const user = await requireAuth(request);

    const server = await prisma.server.findFirst({
      where: {
        id: params.id,
        user_id: user.id,
      },
      include: {
        node: true,
        egg: true,
        allocations: true,
      },
    });

    if (!server) {
      return NextResponse.json(
        { success: false, error: "Server not found" },
        { status: 404 }
      );
    }

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
    console.error("Get server error:", error);
    return NextResponse.json(
      { success: false, error: "Internal server error" },
      { status: 500 }
    );
  }
}

export async function PATCH(
  request: NextRequest,
  { params }: { params: { id: string } }
): Promise<NextResponse<ApiResponse>> {
  try {
    const user = await requireAuth(request);
    const body = await request.json();

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

    const allowedFields = ["name", "startup", "environment", "image"];
    const updateData: Record<string, unknown> = {};

    for (const field of allowedFields) {
      if (field in body) {
        updateData[field] = body[field];
      }
    }

    const updated = await prisma.server.update({
      where: { id: params.id },
      data: updateData,
      include: {
        node: true,
        egg: true,
      },
    });

    await prisma.activityLog.create({
      data: {
        user_id: user.id,
        event: `Server "${updated.name}" updated`,
        ip: request.headers.get("x-forwarded-for") || "unknown",
        data: { serverId: updated.id, changes: Object.keys(updateData) },
      },
    });

    return NextResponse.json({
      success: true,
      data: updated,
    });
  } catch (error) {
    if (error instanceof Error && error.message === "Unauthorized") {
      return NextResponse.json(
        { success: false, error: "Unauthorized" },
        { status: 401 }
      );
    }
    console.error("Update server error:", error);
    return NextResponse.json(
      { success: false, error: "Internal server error" },
      { status: 500 }
    );
  }
}

export async function DELETE(
  request: NextRequest,
  { params }: { params: { id: string } }
): Promise<NextResponse<ApiResponse>> {
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

    await prisma.server.delete({
      where: { id: params.id },
    });

    await prisma.activityLog.create({
      data: {
        user_id: user.id,
        event: `Server "${server.name}" deleted`,
        ip: request.headers.get("x-forwarded-for") || "unknown",
        data: { serverId: server.id },
      },
    });

    return NextResponse.json({
      success: true,
      message: "Server deleted successfully",
    });
  } catch (error) {
    if (error instanceof Error && error.message === "Unauthorized") {
      return NextResponse.json(
        { success: false, error: "Unauthorized" },
        { status: 401 }
      );
    }
    console.error("Delete server error:", error);
    return NextResponse.json(
      { success: false, error: "Internal server error" },
      { status: 500 }
    );
  }
}
