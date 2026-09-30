#include <Engine/Engine.h>
#include <DirectXMath.h>
#include <chrono>
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <cstddef>
#include <cstdint>
#include <new>
#define SAFE_RELEASE(x) if(x != nullptr) x->Release(); x = nullptr;
#define MESSAGE( classObj, method, state )   \
{                                            \
   std::wostringstream os_;                  \
   os_ << classObj << "::" << method << " : " << "[CREATION OF RESOURCE " << ": " << state << "] \n"; \
   OutputDebugStringW( os_.str().c_str() );  \
}

#define ERROR(classObj, method, errorMSG)                     \
{                                                             \
    try {                                                     \
        std::wostringstream os_;                              \
        os_ << L"ERROR : " << classObj << L"::" << method     \
            << L" : " << errorMSG << L"\n";                   \
        OutputDebugStringW(os_.str().c_str());                \
    } catch (...) {                                           \
        OutputDebugStringW(L"Failed to log error message.\n");\
    }                                                         \
}

template<typename T>
void SafeRelease(T*& object) noexcept
{
    if (object != nullptr)
    {
        object->Release();
        object = nullptr;
    }
}

struct
    Engine::Implementation {
    struct Vertex
    {
        float position[3];
        float color[4];
    };
	struct alignas(16) TransformBuffer
	{
		DirectX::XMFLOAT4X4 worldViewProjection;
	};

    HWND window = nullptr;

    std::uint32_t width = 0;
    std::uint32_t height = 0;

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView* renderTarget = nullptr;
	ID3D11Texture2D* depthStencilBuffer = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;

	ID3D11Buffer* transformBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	ID3D11RasterizerState* rasterizerState = nullptr;

    std::chrono::steady_clock::time_point startTime();

    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;
    ID3D11Buffer* vertexBuffer = nullptr;

    static bool
        CompileShader(const wchar_t* filename, const char* entryPoint,
            const char* shaderModel, ID3DBlob** shaderBlob) noexcept {
        if (!filename || !entryPoint || !shaderModel || !shaderBlob) {
            return false;
        }

        *shaderBlob = nullptr;

        UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
        compileFlags |= D3DCOMPILE_DEBUG;
        compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

        ID3DBlob* errors = nullptr;

        const HRESULT result = D3DCompileFromFile(
            filename,
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            entryPoint,
            shaderModel,
            compileFlags,
            0,
            shaderBlob,
            &errors
        );

        if (errors)
        {
            OutputDebugStringA(
                static_cast<const char*>(
                    errors->GetBufferPointer()
                    )
            );

            SafeRelease(errors);
        }

        if (FAILED(result))
        {
            SafeRelease(*shaderBlob);
            return false;
        }

        return true;
    }

    void ReleaseResources() noexcept
    {
        if (context)
        {
            context->ClearState();
            context->Flush();
        }

        SafeRelease(vertexBuffer);
        SafeRelease(inputLayout);
        SafeRelease(pixelShader);
        SafeRelease(vertexShader);
        SafeRelease(renderTarget);
        SafeRelease(swapChain);
        SafeRelease(context);
        SafeRelease(device);

        window = nullptr;
        width = 0;
        height = 0;
    }

};

Engine::Engine() noexcept
    : m_implementation(
        new (std::nothrow) Implementation{}
    )
{
}

Engine::~Engine() noexcept
{
    Shutdown();

    delete m_implementation;
    m_implementation = nullptr;
}

bool Engine::Initialize(
    void* nativeWindow,
    std::uint32_t width,
    std::uint32_t height
) noexcept
{
    if (!m_implementation ||
        !nativeWindow ||
        width == 0 ||
        height == 0)
    {
        return false;
    }

    Implementation& engine = *m_implementation;


    engine.ReleaseResources();

    engine.window = static_cast<HWND>(nativeWindow);
    engine.width = width;
    engine.height = height;
    DXGI_SWAP_CHAIN_DESC swapChainDescription{};

    swapChainDescription.BufferCount = 2;
    swapChainDescription.BufferDesc.Width =     engine.width;
    swapChainDescription.BufferDesc.Height =      engine.height;
    swapChainDescription.BufferDesc.Format =      DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDescription.BufferDesc.RefreshRate.Numerator =       60;
    swapChainDescription.BufferDesc.RefreshRate.Denominator =        1;
    swapChainDescription.BufferUsage =       DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDescription.OutputWindow =     engine.window;
    swapChainDescription.SampleDesc.Count = 1;
    swapChainDescription.SampleDesc.Quality = 0; 
    swapChainDescription.Windowed = TRUE;

    swapChainDescription.SwapEffect =
        DXGI_SWAP_EFFECT_DISCARD;

    constexpr D3D_FEATURE_LEVEL featureLevels[]
    {
        D3D_FEATURE_LEVEL_11_0
    };

    D3D_FEATURE_LEVEL selectedFeatureLevel{};

    HRESULT result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &swapChainDescription,
        &engine.swapChain,
        &engine.device,
        &selectedFeatureLevel,
        &engine.context
    );

    if (FAILED(result))
    {
        SafeRelease(engine.swapChain);
        SafeRelease(engine.context);
        SafeRelease(engine.device);

        result = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_WARP,
            nullptr,
            0,
            featureLevels,
            ARRAYSIZE(featureLevels),
            D3D11_SDK_VERSION,
            &swapChainDescription,
            &engine.swapChain,
            &engine.device,
            &selectedFeatureLevel,
            &engine.context
        );
    }

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    ID3D11Texture2D* backBuffer = nullptr;

    result = engine.swapChain->GetBuffer(
        0,
        __uuidof(ID3D11Texture2D),
        reinterpret_cast<void**>(&backBuffer)
    );

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    result = engine.device->CreateRenderTargetView(
        backBuffer,
        nullptr,
        &engine.renderTarget
    );

    SafeRelease(backBuffer);

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    D3D11_VIEWPORT viewport{};

    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    viewport.Width =
        static_cast<float>(engine.width);

    viewport.Height =
        static_cast<float>(engine.height);

    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    engine.context->RSSetViewports(
        1,
        &viewport
    );

    ID3DBlob* vertexShaderBlob = nullptr;
    ID3DBlob* pixelShaderBlob = nullptr;

    if (!Implementation::CompileShader(
        L"bin\\Shaders\\Triangle.hlsl",
        "VSMain",
        "vs_5_0",
        &vertexShaderBlob))
    {
        engine.ReleaseResources();
        return false;
    }

    if (!Implementation::CompileShader(
        L"bin\\Shaders\\Triangle.hlsl",
        "PSMain",
        "ps_5_0",
        &pixelShaderBlob))
    {
        SafeRelease(vertexShaderBlob);
        engine.ReleaseResources();
        return false;
    }

    result = engine.device->CreateVertexShader(
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        nullptr,
        &engine.vertexShader
    );

    if (FAILED(result))
    {
        SafeRelease(pixelShaderBlob);
        SafeRelease(vertexShaderBlob);
        engine.ReleaseResources();
        return false;
    }

    result = engine.device->CreatePixelShader(
        pixelShaderBlob->GetBufferPointer(),
        pixelShaderBlob->GetBufferSize(),
        nullptr,
        &engine.pixelShader
    );

    if (FAILED(result))
    {
        SafeRelease(pixelShaderBlob);
        SafeRelease(vertexShaderBlob);
        engine.ReleaseResources();
        return false;
    }

    constexpr D3D11_INPUT_ELEMENT_DESC inputElements[]{
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, static_cast<UINT>(offsetof(Implementation::Vertex, position)), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, static_cast<UINT>(offsetof(Implementation::Vertex, color)),D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    result = engine.device->CreateInputLayout(
        inputElements,
        ARRAYSIZE(inputElements),
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        &engine.inputLayout
    );

    SafeRelease(pixelShaderBlob);
    SafeRelease(vertexShaderBlob);

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    constexpr Implementation::Vertex vertices[]
    {
        {
            { 0.0f, 0.6f, 0.0f },
            { 1.0f, 0.0f, 0.0f, 1.0f }
        },
        {
            { 0.6f, -0.6f, 0.0f },
            { 0.0f, 1.0f, 0.0f, 1.0f }
        },
        {
            { -0.6f, -0.6f, 0.0f },
            { 0.0f, 0.3f, 1.0f, 1.0f }
        }
    };

    D3D11_BUFFER_DESC vertexBufferDescription{};

    vertexBufferDescription.ByteWidth =
        static_cast<UINT>(sizeof(vertices));

    vertexBufferDescription.Usage =
        D3D11_USAGE_IMMUTABLE;

    vertexBufferDescription.BindFlags =
        D3D11_BIND_VERTEX_BUFFER;

    vertexBufferDescription.CPUAccessFlags = 0;
    vertexBufferDescription.MiscFlags = 0;
    vertexBufferDescription.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA initialVertexData{};
    initialVertexData.pSysMem = vertices;

    result = engine.device->CreateBuffer(
        &vertexBufferDescription,
        &initialVertexData,
        &engine.vertexBuffer
    );

    if (FAILED(result))
    {
        engine.ReleaseResources();
        return false;
    }

    return true;
}

void Engine::Render() noexcept
{
    if (!m_implementation)
        return;

    Implementation& engine = *m_implementation;

    if (!engine.context ||
        !engine.swapChain ||
        !engine.renderTarget ||
        !engine.vertexBuffer ||
        !engine.inputLayout ||
        !engine.vertexShader ||
        !engine.pixelShader)
    {
        return;
    }

    constexpr float clearColor[]
    {
        0.03f,
        0.04f,
        0.08f,
        1.0f
    };

    engine.context->OMSetRenderTargets(
        1,
        &engine.renderTarget,
        nullptr
    );

    engine.context->ClearRenderTargetView(
        engine.renderTarget,
        clearColor
    );

    constexpr UINT stride =
        sizeof(Implementation::Vertex);

    constexpr UINT offset = 0;

    engine.context->IASetVertexBuffers(
        0,
        1,
        &engine.vertexBuffer,
        &stride,
        &offset
    );

    engine.context->IASetInputLayout(
        engine.inputLayout
    );

    engine.context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );

    engine.context->VSSetShader(
        engine.vertexShader,
        nullptr,
        0
    );

    engine.context->PSSetShader(
        engine.pixelShader,
        nullptr,
        0
    );

    engine.context->Draw(3, 0);

    engine.swapChain->Present(1, 0);
}

void Engine::Shutdown() noexcept
{
    if (m_implementation)
        m_implementation->ReleaseResources();
}