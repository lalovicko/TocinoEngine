/**
 * @file Main.cpp
 * @brief Punto de entrada del Sandbox: crea la ventana, inicializa el Engine y corre el bucle principal.
 * @see Window, Engine
 */

#include <Windows.h>
#include <cstdint>
#include "Engine/Window.h"
#include <Engine/Engine.h>

 /**
  * @brief Punto de entrada de la aplicación (subsistema Windows).
  *
  * Crea una ventana de 1280x720, inicializa el Engine y repite "procesar mensajes y
  * dibujar un frame" hasta que se cierra la ventana. Si la ventana está minimizada
  * no dibuja.
  *
  * @param[in] hInstance     Instancia del ejecutable.
  * @param[in] hPrevInstance Siempre nullptr en Win32 moderno (no se usa).
  * @param[in] lpCmdLine     Argumentos de la línea de comandos (no se usa).
  * @param[in] nCmdShow      Cómo debe mostrarse la ventana al inicio (`SW_*`).
  * @return 0 si la aplicación terminó con normalidad; -1 si falló la creación de la
  *         ventana o la inicialización del Engine.
  */
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

    // Ventana Win32 (RAII: se destruye sola al salir de wWinMain).
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

    // El Engine dibuja sobre el HWND de la ventana.
    Engine graphicsEngine;

    if (!graphicsEngine.Initialize(
        window.GetNativeWindow(),
        windowWidth,
        windowHeight))
    {
        graphicsEngine.Shutdown();
        return -1;
    }

    // Bucle principal: atender mensajes de Windows y dibujar un frame.
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