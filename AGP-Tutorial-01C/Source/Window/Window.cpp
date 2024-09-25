#include "Window.h"

#include <cstdio>
#include <iostream>

Window::Window() : hInstance(nullptr), hWnd(nullptr), windowName(L"DirectX 11 Tutorial")
{
}

HRESULT Window::Init(HINSTANCE hInstance, int nCmdShow, int width, int height)
{
	this->hInstance = hInstance;

	// INFO: Create the window class
	WNDCLASSEX wc = {};

	// INFO: Fill in the window class structure with parameters that describe the main window
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"WindowClass1";
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

	// INFO: Register the window class
	if (!RegisterClassEx(&wc))
	{
		return E_FAIL;
	}

	// INFO: Adjust the window size
	RECT wr = { 0, 0, width, height };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	// INFO: Create the window and store a handle to it
	hWnd = CreateWindowEx(NULL,
		L"WindowClass1",
		windowName,
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, 0,
		wr.right - wr.left,		
		wr.bottom - wr.top,		
		NULL,					
		NULL,					
		hInstance,				
		NULL);

	if (hWnd == NULL)
	{
		return E_FAIL;
	}

	// INFO: Show the window on the screen
	ShowWindow(hWnd, nCmdShow);

	return S_OK;
}

LRESULT Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_KEYDOWN:
	{
		switch (wParam) 
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		case 'W':
			MessageBox(NULL, L"W was pressed!", L"Key Press", MB_ICONINFORMATION | MB_OK);
			break;
		}
	}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void Window::OpenConsole()
{
	if (AllocConsole())
	{
		// INFO: Redirect the standard input, output and error streams to our console
		FILE* fp = nullptr;
		freopen_s(&fp, "CONIN$", "r", stdin);
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);
		std::ios::sync_with_stdio(true);

		std::cout << "Console Opened!" << std::endl;
	}
}

void Window::Run()
{
	MSG msg = {};

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				break;
			}
		}
		else
		{
			// INFO: Game Code Here
		}
	}
}
