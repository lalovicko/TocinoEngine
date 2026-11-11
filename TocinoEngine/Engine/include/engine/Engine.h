/**
 * @file Engine.h
 * @brief Declara la interfaz pública del motor gráfico.
 */
#pragma once
#include "Prerequisites.h"

 /**
  * @class Engine
  * @brief Fachada principal del motor de renderizado basado en Direct3D 11.
  * @details Administra la inicialización de la GPU, los recursos gráficos, el renderizado
  *          de cada cuadro y la liberación de recursos. La implementación se mantiene
  *          oculta mediante el patrón PImpl.
  * @note La clase no permite copia ni movimiento.
  */
class ENGINE_API
    Engine final {
public:
    /** @brief Construye el motor y reserva su estructura de implementación. */
    Engine() noexcept;

    /** @brief Apaga el motor y libera la estructura de implementación. */
    ~Engine() noexcept;

    /** @brief Deshabilita la copia para evitar compartir la propiedad de los recursos. */
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    /** @brief Deshabilita el movimiento de la instancia. */
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

    /**
     * @brief Inicializa Direct3D 11 y crea los recursos necesarios para renderizar.
     * @details Intenta crear el dispositivo con hardware y utiliza WARP como alternativa.
     *          También crea la swap chain, las vistas de render/depth, shaders, buffers
     *          y la geometría de prueba.
     * @param nativeWindow Ventana Win32 expresada como puntero opaco (@c HWND).
     * @param width Ancho del área de renderizado en píxeles; debe ser mayor que cero.
     * @param height Alto del área de renderizado en píxeles; debe ser mayor que cero.
     * @return @c true si todos los recursos requeridos se crearon; de lo contrario, @c false.
     * @note La ruta del shader utilizada por la implementación es
     *       @c bin\Shaders\Cube.hlsl, relativa al directorio de trabajo.
     */
    bool
        Initialize(void* nativeWindow,
            std::uint32_t width,
            std::uint32_t height) noexcept;

    /**
     * @brief Dibuja el cuadro actual y lo presenta en la ventana.
     * @details Actualiza la matriz World-View-Projection, enlaza buffers y shaders,
     *          ejecuta el dibujo indexado del cubo y llama a @c Present.
     * @note No realiza ninguna operación si la instancia o los recursos necesarios
     *       no están inicializados.
     */
    void
        Render() noexcept;

    /**
     * @brief Libera los recursos asociados al motor.
     * @details La operación puede invocarse antes del destructor; la implementación
     *          reinicia los identificadores y dimensiones internos.
     */
    void
        Shutdown() noexcept;

private:
    /** @brief Declaración adelantada de la implementación privada (PImpl). */
    struct Implementation;

    /** @brief Puntero propietario a la implementación interna del motor. */
    Implementation* m_implementation = nullptr;
};