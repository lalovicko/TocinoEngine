#include <Windows.h>

#include <engine/Engine.h>

#include <string>

int WINAPI
wWinMain(
    HINSTANCE, 
    HINSTANCE, 
    PWSTR, 
    int) {
    const int testResult = Engine_RunSmokeTest();

    std::wstring message = L"Engine cargado: ";
    message += Engine_GetName();

    if (testResult != 0)
    {
        message += L"\n\nDirectX 11: OK";
        message += L"\nDirectXTK: OK";
        message += L"\nEngine.dll: OK";
        message += L"\nEngine.lib: OK";
    }
    else
    {
        message += L"\n\nError durante la validacion.";
    }

    MessageBoxW(
        nullptr,
        message.c_str(),
        L"Sandbox - Smoke Test",
        testResult != 0
        ? MB_OK | MB_ICONINFORMATION
        : MB_OK | MB_ICONERROR
    );

    return testResult != 0 ? 0 : 1;
}