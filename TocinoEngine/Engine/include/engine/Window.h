#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>

class Window final {
public:

	Window() = default;


	~Window();	
				
	Window(const Window&) = delete;

	bool
		Create(
			HINSTANCE Instance,
			const wchar_t* title,
			UINT clientWidth,
			UINT clientHeight
		) noexcept;


	void
		Show(int showCommand) noexcept;

	bool
		ProcessMessages() noexcept;


	HWND GetNativeWindow() const noexcept {
		return m_handle;
	}


	bool
		IsMinimized() const noexcept;

private:


	void
		Destroy() noexcept;

	static
		LRESULT CALLBACK
		WindowProc(
			HWND handle,
			UINT message,
			WPARAM wParam,
			LPARAM lParam
		);


	static
		constexpr const wchar_t* ClassName =
		L"Grafica3DEngineWindowClass";

	HINSTANCE m_instance = nullptr; 
	HWND m_handle = nullptr;       
	bool
		m_classRegistered = false; 
};