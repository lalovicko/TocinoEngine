/**
 * @file SwapChain.h
 * @brief Declara la interfaz de administración de una cadena de intercambio DXGI.
 */
#pragma once
#include "Engine/Prerequisites.h"

 // Forward declarations of classes used in the SwapChain class
class Window;
class Device;
class Texture2D;

/**
 * @class SwapChain
 * @brief Encapsula una cadena de intercambio de DXGI (@c IDXGISwapChain).
 * @details La cadena de intercambio administra los buffers que se presentan en una
 *          ventana y permite presentar cuadros, obtener el back buffer y redimensionarlo.
 */
class
	SwapChain {
public:
	/** @brief Construye una cadena de intercambio sin recursos asociados. */
	SwapChain() = default;

	/** @brief Libera la cadena de intercambio. */
	~SwapChain();

	/** @brief Deshabilita la copia de la cadena de intercambio. */
	SwapChain(const SwapChain&) = delete;
	SwapChain& operator=(const SwapChain&) = delete;

	/** @brief Deshabilita el movimiento de la cadena de intercambio. */
	SwapChain(SwapChain&&) = delete;
	SwapChain& operator=(SwapChain&&) = delete;

	/**
	 * @brief Inicializa la cadena de intercambio para una ventana y dimensiones dadas.
	 * @param device Dispositivo Direct3D 11 que se utilizará para la creación.
	 * @param window Ventana Win32 asociada a la presentación.
	 * @param width Ancho del back buffer en píxeles.
	 * @param height Alto del back buffer en píxeles.
	 * @return Código @c HRESULT que indica el resultado de la inicialización.
	 * @note La firma recibe @p device y @p window por valor, tal como está declarada en la API.

	 */
	HRESULT
		init(Device device, Window window,
			unsigned int width, unsigned int height);

	/**
	 * @brief Destruye la cadena de intercambio y libera el recurso COM asociado.
	 */
	void
		destroy();

	/**
	 * @brief Presenta en pantalla el contenido del back buffer.
	 * @param syncInterval Cantidad de intervalos verticales que se esperarán antes de presentar.
	 * @param flags Banderas de presentación de DXGI.
	 * @return Código @c HRESULT de la operación.
	 */
	HRESULT
		Present(unsigned int syncInterval = 1,
			unsigned int flags = 0);

	/**
	 * @brief Obtiene uno de los buffers de la cadena de intercambio.
	 * @param bufferIndex Índice del buffer que se desea recuperar (normalmente 0).
	 * @param backBuffer Objeto @c Texture2D que recibirá la referencia al recurso.
	 * @return Código @c HRESULT de la operación.
	 * @note El parámetro @p backBuffer se recibe por valor en la declaración actual.
	 */
	HRESULT
		GetBuffer(unsigned int bufferIndex,
			Texture2D backBuffer);

	/**
	 * @brief Redimensiona los buffers de la cadena de intercambio.
	 * @param width Nuevo ancho en píxeles.
	 * @param height Nuevo alto en píxeles.
	 * @return Código @c HRESULT de la operación.
	 */
	HRESULT
		ResizeBuffers(unsigned int width,
			unsigned int height);

	/**
	 * @brief Obtiene el puntero a la interfaz DXGI interna.
	 * @return Puntero a @c IDXGISwapChain; no transfiere la propiedad del recurso.
	 */
	IDXGISwapChain*
		getSwapChain() { return m_swapChain; }

private:
	/** @brief Referencia COM a la cadena de intercambio. */
	IDXGISwapChain* m_swapChain = nullptr;
};