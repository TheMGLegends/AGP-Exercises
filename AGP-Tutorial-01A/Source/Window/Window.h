#pragma once

#include <Windows.h>

class Window
{
public:
	Window();

	HRESULT InitiWindow(HINSTANCE hInstance, int nCmdShow);
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Run();

private:
	HINSTANCE hInstance;
	HWND hWnd;
	const wchar_t* windowName;
};

