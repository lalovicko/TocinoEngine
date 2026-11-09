#pragma once
#include "Engine/API.h"
#include <cstdint>
#include <directxmath.h>
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <string>
#include <new>

extern "C" {
    ENGINE_API bool
        Engine_Initialize(HWND hwnd, int width, int height) noexcept;

    ENGINE_API void
        Engine_Update() noexcept;

    ENGINE_API void
        Engine_Render() noexcept;

    ENGINE_API void
        Engine_Shutdown() noexcept;
}