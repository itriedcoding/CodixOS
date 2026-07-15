import { NextRequest, NextResponse } from "next/server";
import prisma from "@/lib/prisma";
import { requireAdmin } from "@/lib/auth";
import type { ApiResponse } from "@/types";

export async function GET(
  request: NextRequest
): Promise<NextResponse<ApiResponse>> {
  try {
    await requireAdmin(request);

    const nodes = await prisma.node.findMany({
      include: {
        _count: {
          select: { servers: true, allocations: true },
        },
        allocations: {
          select: { id: true, server_id: true },
        },
      },
      orderBy: { created_at: "desc" },
    });

    const nodesWithStats = nodes.map((node) => ({
      ...node,
      servers_count: node._count.servers,
      allocations_count: node._count.allocations,
      allocated_ports: node.allocations.filter((a) => a.server_id !== null).length,
    }));

    return NextResponse.json({
      success: true,
      data: nodesWithStats,
    });
  } catch (error) {
    if (error instanceof Error && error.message === "Forbidden") {
      return NextResponse.json(
        { success: false, error: "Forbidden" },
        { status: 403 }
      );
    }
    console.error("Get nodes error:", error);
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
    const admin = await requireAdmin(request);
    const body = await request.json();
    const { name, location_id, fqdn, scheme, behind_proxy, memory, disk, cpu, io } = body;

    if (!name || !location_id || !fqdn) {
      return NextResponse.json(
        { success: false, error: "Name, location_id, and fqdn are required" },
        { status: 400 }
      );
    }

    const node = await prisma.node.create({
      data: {
        name,
        location_id,
        fqdn,
        scheme: scheme || "https",
        behind_proxy: behind_proxy || false,
        memory: memory || 8192,
        disk: disk || 102400,
        cpu: cpu || 100,
        io: io || 500,
      },
    });

    await prisma.activityLog.create({
      data: {
        user_id: admin.id,
        event: `Node "${name}" created`,
        ip: request.headers.get("x-forwarded-for") || "unknown",
        data: { nodeId: node.id },
      },
    });

    return NextResponse.json({
      success: true,
      data: node,
    });
  } catch (error) {
    if (error instanceof Error && error.message === "Forbidden") {
      return NextResponse.json(
        { success: false, error: "Forbidden" },
        { status: 403 }
      );
    }
    console.error("Create node error:", error);
    return NextResponse.json(
      { success: false, error: "Internal server error" },
      { status: 500 }
    );
  }
}

export async function DELETE(
  request: NextRequest
): Promise<NextResponse<ApiResponse>> {
  try {
    const admin = await requireAdmin(request);
    const { searchParams } = new URL(request.url);
    const nodeId = searchParams.get("id");

    if (!nodeId) {
      return NextResponse.json(
        { success: false, error: "Node ID is required" },
        { status: 400 }
      );
    }

    const node = await prisma.node.findUnique({
      where: { id: nodeId },
      include: { _count: { select: { servers: true } } },
    });

    if (!node) {
      return NextResponse.json(
        { success: false, error: "Node not found" },
        { status: 404 }
      );
    }

    if (node._count.servers > 0) {
      return NextResponse.json(
        { success: false, error: "Cannot delete node with active servers" },
        { status: 400 }
      );
    }

    await prisma.node.delete({ where: { id: nodeId } });

    await prisma.activityLog.create({
      data: {
        user_id: admin.id,
        event: `Node "${node.name}" deleted`,
        ip: request.headers.get("x-forwarded-for") || "unknown",
        data: { nodeId: node.id },
      },
    });

    return NextResponse.json({
      success: true,
      message: "Node deleted successfully",
    });
  } catch (error) {
    if (error instanceof Error && error.message === "Forbidden") {
      return NextResponse.json(
        { success: false, error: "Forbidden" },
        { status: 403 }
      );
    }
    console.error("Delete node error:", error);
    return NextResponse.json(
      { success: false, error: "Internal server error" },
      { status: 500 }
    );
  }
}
