/**
 * @file Engine.h
 * @brief Interfaz pública del motor gráfico (Direct3D 11).
 *
 * Expone dos interfaces:
 *  - Un grupo de funciones con enlace C (`extern "C"`), pensado para usar el motor
 *    desde otros lenguajes (@ref engine_c_api).
 *  - La clase Engine, que es la que usa hoy el Sandbox.
 *
 * @see API.h, Window.h
 */

#pragma once

#include "API.h"
#include <cstdint>
#include <Windows.h>

 /**
  * @defgroup engine_c_api API en C
  * @brief Funciones exportadas con enlace C.
  *
  * @warning Estas cuatro funciones están declaradas y marcadas con ENGINE_API, pero
  * **todavía no están definidas** en Engine.cpp. Si algún código las llama, el enlazador
  * marcará un error de símbolo externo sin resolver (LNK2019). Por ahora solo se usa
  * la clase Engine.
  * @{
  */
extern "C" {
	/**
	 * @brief Inicializa el motor a través de la API en C.
	 * @param hwnd   Ventana (HWND) donde se dibujará.
	 * @param width  Ancho del área cliente, en píxeles.
	 * @param height Alto del área cliente, en píxeles.
	 * @return true si se inicializó correctamente.
	 * @note Pendiente de implementar. Equivale a Engine::Initialize().
	 */
	ENGINE_API bool
		Engine_Initialize(HWND hwnd, int width, int height) noexcept;

	/**
	 * @brief Actualiza la lógica del motor (un paso de simulación).
	 * @note Pendiente de implementar. La clase Engine aún no tiene Update(): la animación
	 * del cubo se calcula dentro de Engine::Render().
	 */
	ENGINE_API void
		Engine_Update() noexcept;

	/**
	 * @brief Dibuja un frame.
	 * @note Pendiente de implementar. Equivale a Engine::Render().
	 */
	ENGINE_API void
		Engine_Render() noexcept;

	/**
	 * @brief Libera todos los recursos del motor.
	 * @note Pendiente de implementar. Equivale a Engine::Shutdown().
	 */
	ENGINE_API void
		Engine_Shutdown() noexcept;
}
/**
 * @}
 */

 /**
  * @class Engine
  * @brief Motor gráfico Direct3D 11: crea el dispositivo y dibuja la escena en una ventana.
  *
  * Hoy la escena es un cubo con un color distinto por vértice que gira sobre los ejes X e Y.
  *
  * Usa el patrón PImpl: todos los objetos de Direct3D viven en una estructura privada
  * (definida en Engine.cpp), de modo que quien use la DLL no necesita incluir `d3d11.h`.
  *
  * La clase posee recursos de la GPU, por lo que no se puede copiar ni mover.
  *
  * @par Ejemplo
  * @code
  * Engine engine;
  * if (!engine.Initialize(window.GetNativeWindow(), 1280, 720))
  *     return -1;
  *
  * while (window.ProcessMessages())
  *     engine.Render();
  *
  * engine.Shutdown();
  * @endcode
  *
  * @see Window
  */
class ENGINE_API
	Engine final {
public:
	/**
	 * @brief Construye el motor vacío (aún sin dispositivo ni recursos).
	 *
	 * Reserva la estructura interna con `new (std::nothrow)`. Llama a Initialize() para
	 * poder dibujar.
	 */
	Engine() noexcept;

	/**
	 * @brief Destruye el motor y libera todos sus recursos (llama a Shutdown()).
	 */
	~Engine() noexcept;

	/**
	 * @name Copia y movimiento
	 * Deshabilitados: la clase posee recursos de GPU y un puntero PImpl.
	 * @{
	 */
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	Engine(Engine&&) = delete;
	Engine& operator=(Engine&&) = delete;
	/**
	 * @}
	 */

	 /**
	  * @brief Crea el dispositivo Direct3D 11 y todos los recursos necesarios para dibujar.
	  *
	  * Pasos, en orden:
	  *  -# Crea el dispositivo y la swap chain (doble búfer, RGBA de 8 bits). Usa la GPU y,
	  *     si eso falla, cae a WARP (renderizado por software).
	  *  -# Crea el render target a partir del back buffer y fija el viewport.
	  *  -# Crea el depth buffer (`D24_UNORM_S8_UINT`) y su vista.
	  *  -# Compila `Cube.hlsl` (VSMain y PSMain, shader model 5.0) y crea el input layout.
	  *  -# Crea el vertex buffer (8 vértices), el index buffer (36 índices) y el constant
	  *     buffer con la matriz de transformación.
	  *  -# Guarda el instante inicial que usa Render() para animar.
	  *
	  * Si el motor ya estaba inicializado, primero libera los recursos anteriores.
	  *
	  * @param[in] nativeWindow Ventana donde se presentará la imagen: un `HWND` en Windows.
	  * @param[in] width        Ancho del área cliente en píxeles (mayor que 0).
	  * @param[in] height       Alto del área cliente en píxeles (mayor que 0).
	  * @return true si todo se creó bien. false si algún argumento es inválido o falla
	  *         cualquier paso; en ese caso los recursos parciales ya fueron liberados.
	  *
	  * @warning El shader se busca en `bin\Shaders\Cube.hlsl`, una ruta **relativa al
	  * directorio de trabajo** del proceso. En Visual Studio ese directorio es `$(SolutionDir)`
	  * (ver `Sandbox.vcxproj.user`). Si ejecutas el `.exe` desde otra carpeta, Initialize()
	  * devuelve false.
	  * @note Los mensajes de error del compilador de shaders salen por `OutputDebugString`
	  * (ventana Output de Visual Studio).
	  */
	bool Initialize(
		void* nativeWindow,
		std::uint32_t width,
		std::uint32_t height
	) noexcept;

	/**
	 * @brief Dibuja un frame y lo presenta en pantalla.
	 *
	 * Limpia el color y la profundidad, calcula la matriz World * View * Projection y la
	 * sube al constant buffer, y dibuja el cubo con `DrawIndexed(36)`.
	 *
	 *  - **World**: rotación sobre X a 0.4 rad/s y sobre Y a 0.8 rad/s (tiempo desde Initialize()).
	 *  - **View**: cámara en (0, 0, -3) mirando al origen, con +Y hacia arriba.
	 *  - **Projection**: perspectiva de 45 grados, planos cercano 0.1 y lejano 100.
	 *
	 * Presenta con `Present(1, 0)`, o sea sincronizado con el refresco del monitor (vsync).
	 *
	 * @note No hace nada si el motor no está inicializado. Debe llamarse una vez por frame.
	 */
	void Render() noexcept;

	/**
	 * @brief Libera todos los recursos de Direct3D.
	 *
	 * Se puede llamar varias veces sin problema; también la invoca el destructor.
	 * Después de llamarla hay que volver a llamar a Initialize() para dibujar de nuevo.
	 */
	void Shutdown() noexcept;

private:
	/**
	 * @brief Estructura interna (PImpl); su definición está en Engine.cpp.
	 */
	struct Implementation;
	Implementation* m_implementation = nullptr;  ///< Datos internos. Es nullptr solo si falló la reserva de memoria en el constructor.
};