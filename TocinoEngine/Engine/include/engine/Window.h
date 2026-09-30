/**
 * @file Window.h
 * @brief Ventana Win32 mínima (RAII) sobre la que dibuja el Engine.
 */

#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>

 /**
  * @class Window
  * @brief Envoltorio de una ventana Win32 de tamaño fijo.
  *
  * Registra su propia clase de ventana, crea la ventana y ambas se destruyen
  * automáticamente en el destructor (RAII).
  *
  * El estilo es `WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX`: tiene barra
  * de título y botón de minimizar, pero no se puede redimensionar ni maximizar.
  *
  * @par Ejemplo
  * @code
  * Window window;
  * if (!window.Create(hInstance, L"Mi ventana", 1280, 720))
  *     return -1;
  *
  * window.Show(nCmdShow);
  *
  * while (window.ProcessMessages())
  * {
  *     // ...dibujar un frame...
  * }
  * @endcode
  *
  * @see Engine
  */
class Window final {
public:

	/**
	 * @brief Crea un objeto vacío, todavía sin ventana Win32.
	 */
	Window() = default;


	/**
	 * @brief Destruye la ventana y des-registra su clase, si existen.
	 */
	~Window();

	/**
	 * @brief No copiable: cada objeto es dueño de su `HWND`.
	 */
	Window(const Window&) = delete;

	/**
	 * @brief Registra la clase de ventana y crea la ventana (oculta).
	 *
	 * El tamaño pedido es el del **área cliente**: internamente se usa `AdjustWindowRectEx`
	 * para sumar la barra de título y los bordes, así que la superficie donde dibuja el
	 * Engine mide exactamente @p clientWidth x @p clientHeight.
	 *
	 * @param[in] Instance     `HINSTANCE` de la aplicación (el que recibe `wWinMain`).
	 * @param[in] title        Título de la ventana.
	 * @param[in] clientWidth  Ancho del área cliente en píxeles (mayor que 0).
	 * @param[in] clientHeight Alto del área cliente en píxeles (mayor que 0).
	 * @return true si la ventana se creó. false si ya existía una ventana, si algún
	 *         argumento es inválido o si falla alguna llamada de Win32 (en ese caso no
	 *         queda nada registrado).
	 * @note La ventana no se ve hasta que llames a Show().
	 */
	bool
		Create(
			HINSTANCE Instance,
			const wchar_t* title,
			UINT clientWidth,
			UINT clientHeight
		) noexcept;


	/**
	 * @brief Muestra la ventana y fuerza su primer repintado.
	 * @param[in] showCommand Modo de visualización (`SW_*`); normalmente el `nCmdShow`
	 *                        que recibe `wWinMain`.
	 * @note No hace nada si la ventana no fue creada.
	 */
	void
		Show(int showCommand) noexcept;

	/**
	 * @brief Atiende los mensajes pendientes de Windows, sin bloquear.
	 *
	 * Vacía la cola con `PeekMessage`, así que regresa de inmediato aunque no haya
	 * mensajes. Pensada como condición del bucle principal.
	 *
	 * @return true mientras la aplicación deba seguir corriendo; false cuando llega
	 *         `WM_QUIT` (se cerró la ventana).
	 */
	bool
		ProcessMessages() noexcept;


	/**
	 * @brief Devuelve el identificador nativo de la ventana.
	 * @return El `HWND`, o nullptr si la ventana no se ha creado o ya se destruyó.
	 */
	HWND GetNativeWindow() const noexcept {
		return m_handle;
	}


	/**
	 * @brief Indica si la ventana está minimizada.
	 * @return true si existe y está minimizada (`IsIconic`).
	 */
	bool
		IsMinimized() const noexcept;

private:


	/**
	 * @brief Destruye la ventana y des-registra la clase. Es seguro llamarla varias veces.
	 */
	void
		Destroy() noexcept;

	/**
	 * @brief Procedimiento de ventana (callback que invoca Windows).
	 *
	 * Al recibir `WM_NCCREATE` guarda el puntero `this` (que llegó en `lpCreateParams`) en
	 * `GWLP_USERDATA` y lo recupera en los demás mensajes.
	 *
	 * Mensajes que atiende:
	 *  - `WM_CLOSE`: destruye la ventana (`DestroyWindow`).
	 *  - `WM_DESTROY`: publica `WM_QUIT` (`PostQuitMessage`), que termina el bucle principal.
	 *  - Cualquier otro: lo delega en `DefWindowProcW`.
	 *
	 * @param handle  Ventana que recibe el mensaje.
	 * @param message Código del mensaje.
	 * @param wParam  Parámetro del mensaje (depende de @p message).
	 * @param lParam  Parámetro del mensaje (depende de @p message).
	 * @return Resultado del procesamiento, según el mensaje.
	 */
	static
		LRESULT CALLBACK
		WindowProc(
			HWND handle,
			UINT message,
			WPARAM wParam,
			LPARAM lParam
		);


	static
		constexpr const wchar_t* ClassName =  ///< Nombre con el que se registra la clase de ventana en Win32.
		L"Grafica3DEngineWindowClass";

	HINSTANCE m_instance = nullptr;  ///< Instancia con la que se registró la clase (necesaria para des-registrarla).
	HWND m_handle = nullptr;  ///< Ventana nativa; nullptr si no existe.
	bool
		m_classRegistered = false;  ///< true mientras la clase de ventana esté registrada.
};