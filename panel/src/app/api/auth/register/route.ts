import { NextRequest, NextResponse } from "next/server";
import prisma from "@/lib/prisma";
import { hashPassword, signToken } from "@/lib/auth";
import type { ApiResponse, JWTPayload } from "@/types";

export async function POST(
  request: NextRequest
): Promise<NextResponse<ApiResponse>> {
  try {
    const body = await request.json();
    const { email, username, password } = body;

    if (!email || !username || !password) {
      return NextResponse.json(
        { success: false, error: "Email, username, and password are required" },
        { status: 400 }
      );
    }

    if (password.length < 8) {
      return NextResponse.json(
        { success: false, error: "Password must be at least 8 characters" },
        { status: 400 }
      );
    }

    const existingUser = await prisma.user.findFirst({
      where: {
        OR: [{ email }, { username }],
      },
    });

    if (existingUser) {
      return NextResponse.json(
        {
          success: false,
          error: existingUser.email === email
            ? "Email already in use"
            : "Username already taken",
        },
        { status: 409 }
      );
    }

    const hashedPassword = await hashPassword(password);

    const userCount = await prisma.user.count();
    const isFirstUser = userCount === 0;

    const user = await prisma.user.create({
      data: {
        email,
        username,
        password: hashedPassword,
        root_admin: isFirstUser,
      },
    });

    const payload: JWTPayload = {
      userId: user.id,
      email: user.email,
      username: user.username,
      root_admin: user.root_admin,
    };

    const token = signToken(payload);

    await prisma.activityLog.create({
      data: {
        user_id: user.id,
        event: isFirstUser ? "Admin account created" : "User account created",
        ip: request.headers.get("x-forwarded-for") || request.headers.get("x-real-ip") || "unknown",
        data: { isFirstUser },
      },
    });

    const response = NextResponse.json({
      success: true,
      data: {
        user: {
          id: user.id,
          email: user.email,
          username: user.username,
          root_admin: user.root_admin,
        },
        token,
      },
    });

    response.cookies.set("token", token, {
      httpOnly: true,
      secure: process.env.NODE_ENV === "production",
      sameSite: "lax",
      maxAge: 7 * 24 * 60 * 60,
      path: "/",
    });

    return response;
  } catch (error) {
    console.error("Registration error:", error);
    return NextResponse.json(
      { success: false, error: "Internal server error" },
      { status: 500 }
    );
  }
}
