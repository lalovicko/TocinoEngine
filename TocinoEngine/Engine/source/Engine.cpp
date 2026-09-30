#include <Engine/Engine.h>

#include <d3d11.h>
#include <wrl/client.h>

#include <CommonStates.h>

using Microsoft::WRL::ComPtr;

const wchar_t* Engine_GetName() noexcept
{
    return L"Game Engine DX11";
}

int Engine_RunSmokeTest() noexcept
{
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;

    HRESULT result = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        device.GetAddressOf(),
        nullptr,
        context.GetAddressOf()
    );

    // Utiliza el renderizador por software si no hay GPU disponible.
    if (FAILED(result))
    {
        device.Reset();
        context.Reset();

        result = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_WARP,
            nullptr,
            0,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            device.GetAddressOf(),
            nullptr,
            context.GetAddressOf()
        );
    }

    if (FAILED(result))
        return 0;

    try
    {
        // Obliga a compilar y enlazar DirectXTK.
        DirectX::CommonStates states(device.Get());

        if (states.LinearWrap() == nullptr)
            return 0;
    }
    catch (...)
    {
        return 0;
    }

    return 1;
}