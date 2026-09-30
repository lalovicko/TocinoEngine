#include <Windows.h>
#include <cstdint>
#include "Engine/Window.h"
#include <Engine/Engine.h>

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow
)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    constexpr UINT windowWidth = 1280;
    constexpr UINT windowHeight = 720;

    Window window;

    if (!window.Create(
        hInstance,
        L"OOH-Yea",
        windowWidth,
        windowHeight))
    {
        return -1;
    }

    window.Show(nCmdShow);

    Engine graphicsEngine;

    if (!graphicsEngine.Initialize(
        window.GetNativeWindow(),
        windowWidth,
        windowHeight))
    {
        graphicsEngine.Shutdown();
        return -1;
    }

    while
        (window.ProcessMessages())
    {
        if (window.IsMinimized())
        {
            continue;
        }
        graphicsEngine.Render();
    }

    graphicsEngine.Shutdown();

    return 0;
}