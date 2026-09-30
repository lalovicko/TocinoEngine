/**
 * @file Window.cpp
 * @brief Implementación de Window.
 *
 * La documentación de la interfaz (parámetros, valores de retorno, etc.) está en Window.h.
 * @see Window.h
 */


#include "Engine/Window.h"

Window::~Window() {
	Destroy();
}


bool
Window::Create(
	HINSTANCE Instance,
	const wchar_t* title,
	UINT clientWidth,
	UINT clientHeight) noexcept {
	if (m_handle ||
		!Instance ||
		!title ||
		clientWidth == 0 ||
		clientHeight == 0)
	{
		return false;
	}

	// Registro de la clase de ventana.
	WNDCLASSEXW windowClass{};

	windowClass.cbSize = sizeof(WNDCLASSEXW);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = Instance;
	windowClass.lpszClassName = ClassName;

	if (!RegisterClassExW(&windowClass))
	{
		return false;
	}

	m_instance = Instance;
	m_classRegistered = true;

	// Estilo fijo: sin redimensionar ni maximizar.
	constexpr DWORD Style =
		WS_OVERLAPPED |
		WS_CAPTION |
		WS_SYSMENU |
		WS_MINIMIZEBOX;

	// AdjustWindowRectEx convierte el tamaño del área cliente al tamaño total de la ventana.
	RECT windowRect{
		0,
		0,
		static_cast<LONG>(clientWidth),
		static_cast<LONG>(clientHeight)
	};

	if (!AdjustWindowRectEx(
		&windowRect,
		Style,
		FALSE,
		0))
	{
		Destroy();
		return false;
	}

	const int windowWidth =
		windowRect.right - windowRect.left;

	const int windowHeight =
		windowRect.bottom - windowRect.top;

	// Se pasa `this` como lpCreateParams: WindowProc lo recupera en WM_NCCREATE.
	m_handle = CreateWindowExW(
		0,
		ClassName,
		title,
		Style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowWidth,
		windowHeight,
		nullptr,
		nullptr,
		Instance,
		this
	);

	if (!m_handle)
	{
		Destroy();
		return false;
	}

	return true;
}
void
Window::Show(int showCommand) noexcept {
	if (m_handle)
	{
		ShowWindow(m_handle, showCommand);
		UpdateWindow(m_handle);
	}
}
bool
Window::ProcessMessages() noexcept {
	MSG message{};

	while (PeekMessageW(
		&message,
		nullptr,
		0,
		0,
		PM_REMOVE))
	{
		if (message.message == WM_QUIT)
		{
			return false;
		}

		TranslateMessage(&message);
		DispatchMessageW(&message);
	}

	return true;
}
bool
Window::IsMinimized() const noexcept {
	return m_handle && IsIconic(m_handle);
}
void
Window::Destroy() noexcept {
	if (m_handle)
	{
		DestroyWindow(m_handle);
		m_handle = nullptr;
	}

	if (m_classRegistered && m_instance)
	{
		UnregisterClassW(
			ClassName,
			m_instance
		);

		m_classRegistered = false;
		m_instance = nullptr;
	}
}
LRESULT CALLBACK Window::WindowProc(
	HWND handle,
	UINT message,
	WPARAM wParam,
	LPARAM lParam) {
	// Recupera el objeto Window asociado a este HWND (ver WM_NCCREATE).
	Window* window = nullptr;

	if (message == WM_NCCREATE)
	{
		const auto createStruct =
			reinterpret_cast<CREATESTRUCTW*>(lParam);

		window =
			static_cast<Window*>(
				createStruct->lpCreateParams
				);

		SetWindowLongPtrW(
			handle,
			GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(window)
		);

		window->m_handle = handle;
	}
	else
	{
		window =
			reinterpret_cast<Window*>(
				GetWindowLongPtrW(
					handle,
					GWLP_USERDATA
				)
				);
	}

	if (window)
	{
		switch (message)
		{
		case WM_CLOSE:
			DestroyWindow(handle);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
	}

	return DefWindowProcW(
		handle,
		message,
		wParam,
		lParam
	);
}