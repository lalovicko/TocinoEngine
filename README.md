# TocinoEngine

Motor gráfico 3D escrito en **C++20** sobre **Direct3D 11**, para Windows. El repositorio tiene dos proyectos:

- **Engine**: el motor, compilado como DLL (`Engine.dll`).
- **Sandbox**: aplicación de prueba que abre una ventana y usa el motor.

Hoy el motor dibuja un **cubo con un color distinto por vértice que gira** sobre los ejes X e Y.

## Características actuales

- Dispositivo y swap chain de Direct3D 11 (doble búfer, RGBA de 8 bits), con respaldo a **WARP** (renderizado por software) si no hay GPU compatible.
- Depth buffer `D24_UNORM_S8_UINT`.
- Shaders HLSL (shader model 5.0) compilados en tiempo de ejecución desde `bin/Shaders/Cube.hlsl`.
- Cámara en perspectiva (45°) y animación basada en el tiempo transcurrido.
- Ventana Win32 mínima con patrón RAII (`Window`).
- Interfaz pública del motor con patrón PImpl (los headers no exponen `d3d11.h`).
- Headers documentados con comentarios Doxygen.

## Estructura del repositorio

```
TocinoEngine/                  <- raíz del repositorio
├── README.md
├── .gitignore
└── TocinoEngine/              <- carpeta de la solución ($(SolutionDir))
    ├── TocinoEngine.slnx
    ├── Engine/                <- DLL del motor
    │   ├── include/engine/    (API.h, Engine.h, Window.h)
    │   └── source/            (Engine.cpp)
    ├── Sandbox/               <- aplicación de prueba
    │   └── source/            (Main.cpp, Window.cpp)
    ├── ThirdParties/
    │   └── DirectXTK/         <- DirectX Tool Kit (Microsoft, licencia MIT)
    ├── bin/
    │   └── Shaders/           <- Cube.hlsl, Triangle.hlsl (versionados)
    ├── lib/                   <- Engine.lib (generado al compilar, no versionado)
    └── intermediate/          <- objetos intermedios (generado, no versionado)
```

La salida de la compilación (`Engine.dll`, `Sandbox.exe`) va a `bin/x64/<Configuración>/`.

## Requisitos

- Windows 10 u 11 (x64).
- Visual Studio con la carga de trabajo **Desarrollo para el escritorio con C++**, con el toolset **v145** (el que usan `Engine` y `Sandbox`) y el SDK de Windows 10/11.
- GPU compatible con Direct3D 11 (feature level 11.0). Sin ella se usa WARP, más lento.

> `DirectXTK_Desktop_2022.vcxproj` usa el toolset **v143**. Si Visual Studio se queja de que no lo encuentra, instala el componente *MSVC v143* desde el instalador de Visual Studio o retargetea el proyecto.

## Compilar y ejecutar

1. Clona el repositorio:
   ```bash
   git clone https://github.com/lalovicko/TocinoEngine.git
   ```
2. Abre `TocinoEngine/TocinoEngine.slnx` en Visual Studio.
3. Elige la configuración **Debug | x64** y marca **Sandbox** como proyecto de inicio.
4. **Antes de ejecutar**, configura el directorio de trabajo: clic derecho en *Sandbox* → *Propiedades* → *Depuración* → *Directorio de trabajo* = `$(SolutionDir)`.
5. Compila y ejecuta (**F5** o **Ctrl+F5**).

### Por qué el directorio de trabajo importa

El motor busca el shader en la ruta relativa `bin\Shaders\Cube.hlsl`, así que el proceso debe arrancar con la carpeta de la solución como directorio de trabajo. Si no, `Engine::Initialize()` devuelve `false` y el Sandbox se cierra sin abrir ventana.

Esa configuración vive en `Sandbox.vcxproj.user`, que **no se versiona** (está en `.gitignore`), por eso hay que repetir el paso 4 después de clonar.

Para ejecutar el `.exe` fuera de Visual Studio, hazlo desde la carpeta de la solución:

```bat
cd TocinoEngine\TocinoEngine
bin\x64\Debug\Sandbox.exe
```

## Uso del motor

Así se usa desde una aplicación (resumen de `Sandbox/source/Main.cpp`):

```cpp
#include <Engine/Engine.h>
#include "Engine/Window.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
    Window window;
    if (!window.Create(hInstance, L"Mi ventana", 1280, 720))
        return -1;

    window.Show(nCmdShow);

    Engine engine;
    if (!engine.Initialize(window.GetNativeWindow(), 1280, 720))
        return -1;

    while (window.ProcessMessages())
    {
        if (window.IsMinimized())
            continue;

        engine.Render();
    }

    engine.Shutdown();
    return 0;
}
```

Todo símbolo público del motor se marca con `ENGINE_API` (`API.h`), que se resuelve a `dllexport` al compilar `Engine` (macro `ENGINE_BUILD_DLL`) y a `dllimport` en los clientes.

## Estado y limitaciones

- La API con enlace C (`Engine_Initialize`, `Engine_Update`, `Engine_Render`, `Engine_Shutdown`) está **declarada pero aún no implementada** en `Engine.cpp`; por ahora se usa la clase `Engine`.
- La clase `Window` tiene su header en `Engine/include/engine/`, pero su implementación (`Window.cpp`) se compila dentro de **Sandbox** y no se exporta desde la DLL.
- `Triangle.hlsl` es el shader de la prueba inicial del triángulo; el motor actual ya no lo carga.
- DirectXTK está incluido como dependencia del proyecto; por ahora el código del motor usa `DirectXMath` directamente.

## Dependencias

- [DirectX Tool Kit](https://github.com/microsoft/DirectXTK) (Microsoft, licencia MIT), incluido en `ThirdParties/DirectXTK`.
- Direct3D 11, DXGI, D3DCompiler y WIC (vienen con el SDK de Windows).