/**
 * @file Prerequisites.h
 * @brief Incluye dependencias comunes y declara tipos, macros y funciones compartidas por el motor.
 */
#pragma once
#include "API.h"
#include <cstdint>
#include <DirectXMath.h>
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <sstream>
#include <cstddef>
#include <chrono>
#include <new>

 /**
  * @def SAFE_RELEASE(x)
  * @brief Libera una interfaz COM si no es nula y asigna @c nullptr a la variable.
  * @param x Variable que contiene el puntero COM.
  * @warning El argumento debe ser una variable puntero modificable; la macro lo evalúa
  *          más de una vez y no debe usarse con expresiones que tengan efectos secundarios.
  */
#define SAFE_RELEASE(x) if(x != nullptr) x->Release(); x = nullptr;

  /**
   * @def MESSAGE(classObj, method, state)
   * @brief Escribe un mensaje de estado de creación en la ventana de depuración de Windows.
   * @param classObj Nombre de la clase o subsistema que genera el mensaje.
   * @param method Nombre del método que informa el estado.
   * @param state Texto descriptivo del estado.
   */
#define MESSAGE( classObj, method, state )   \
{                                            \
   std::wostringstream os_;                  \
   os_ << classObj << "::" << method << " : " << "[CREATION OF RESOURCE " << ": " << state << "] \n"; \
   OutputDebugStringW( os_.str().c_str() );  \
}

   /**
    * @def ERROR(classObj, method, errorMSG)
    * @brief Escribe un mensaje de error en la ventana de depuración de Windows.
    * @param classObj Nombre de la clase o subsistema que genera el error.
    * @param method Nombre del método donde ocurrió.
    * @param errorMSG Descripción del problema.
    */
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


    /**
     * @enum ExtensionType
     * @brief Tipos de archivo de textura reconocidos por la interfaz de texturas.
     * @note La enumeración define DDS, PNG y JPG. La implementación actual de
     *       @c Texture2D::init solo delega la carga para PNG y JPG.
     */
enum
    ExtensionType {
    DDS = 0,
    PNG = 1,
    JPG = 2
};

/**
 * @struct Vertex
 * @brief Formato de vértice utilizado por la geometría del motor.
 * @details Cada vértice contiene una posición 3D y un color RGBA.
 */
struct
    Vertex {
    /** @brief Coordenadas de posición X, Y y Z. */
    float position[3];
    /** @brief Componentes de color rojo, verde, azul y alfa. */
    float color[4];
};


/**
 * @brief Libera una interfaz COM y deja el puntero en estado nulo.
 * @tparam T Tipo de la interfaz COM.
 * @param object Referencia al puntero que se liberará.
 * @note A diferencia de la macro @c SAFE_RELEASE, esta función evalúa el puntero
 *       una sola vez y acepta una referencia a puntero.
 */
template<typename T>
void SafeRelease(T*& object) noexcept
{
    if (object != nullptr)
    {
        object->Release();
        object = nullptr;
    }
}

/**
 * @brief Funciones de entrada C para controlar el motor desde una aplicación anfitriona.
 * @details Se exportan con enlace C para evitar el decorado de nombres de C++.
 */
extern "C" {
    /**
     * @brief Inicializa el motor asociado a una ventana Win32.
     * @param hwnd Identificador nativo de la ventana.
     * @param width Ancho del área de renderizado en píxeles.
     * @param height Alto del área de renderizado en píxeles.
     * @return @c true si la inicialización termina correctamente; de lo contrario, @c false.
     */
    ENGINE_API bool
        Engine_Initialize(HWND hwnd, int width, int height) noexcept;

    /** @brief Actualiza el estado del motor para el siguiente cuadro. */
    ENGINE_API void
        Engine_Update() noexcept;

    /** @brief Renderiza y presenta un cuadro. */
    ENGINE_API void
        Engine_Render() noexcept;

    /** @brief Libera los recursos globales del motor y finaliza su ejecución. */
    ENGINE_API void
        Engine_Shutdown() noexcept;
}