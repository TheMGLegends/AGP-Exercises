#pragma once

#include <Windows.h>

class Window
{
public:
	Window();

	HRESULT Init(HINSTANCE hInstance, int nCmdShow, int width = 800, int height = 600);
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Run();

	inline HWND GetWindowHandle() const { return hWnd; }

private:
	HINSTANCE hInstance;
	HWND hWnd;
	const wchar_t* windowName;
};

