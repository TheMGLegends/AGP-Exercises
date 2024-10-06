#include "Window.h"

#include <cstdio>
#include <iostream>

#include <DirectXMath.h>

Window::Window() : hInstance(nullptr), hWnd(nullptr), windowName(L"DirectX 11 Tutorial")
{
}

HRESULT Window::Init(HINSTANCE _hInstance, int nCmdShow, int width, int height)
{
	hInstance = _hInstance;

	// INFO: Create the window class
	WNDCLASSEX wc = {};

	// INFO: Fill in the window class structure with parameters that describe the main window
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = HandleMessageSetup;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"WindowClass1";
	wc.hIcon = LoadIcon(NULL, IDI_SHIELD);
	wc.hCursor = LoadCursor(NULL, IDC_HAND);

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
		this); // INFO: Needed to be this rather than NULL so that can call renderer.SetColor() in HandleMessage()

	if (hWnd == NULL)
	{
		return E_FAIL;
	}

	// INFO: Show the window on the screen
	ShowWindow(hWnd, nCmdShow);

	// INFO: Create the renderer
	renderer = std::make_unique<Renderer>();
	if (FAILED(renderer->Init(hWnd)))
	{
		MessageBox(NULL, L"Renderer Initialization Failed", L"Critical Error", MB_ICONERROR | MB_OK);
	}

	return S_OK;
}

LRESULT Window::HandleMessageSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCCREATE)
	{
		// INFO: Extract the Window Class from the Creation Data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

		// INFO: Store the Window Pointer in the User Data of the Window
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));

		// INFO: Set the Message Procedure to Handle Message Repeated
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMessageRepeated));

		// INFO: Call the User Defined Message Handling
		return pWnd->HandleMessage(hWnd, uMsg, wParam, lParam);
	}

	// INFO: Default Message Handling for the Window
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT Window::HandleMessageRepeated(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// INFO: Extract the Window Pointer from the User Data of the Window
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	// INFO: Call the User Defined Message Handling
	return pWnd->HandleMessage(hWnd, uMsg, wParam, lParam);
}

LRESULT Window::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
	{
		//MessageBeep(MB_ICONSTOP);
		PostQuitMessage(0);
		return 0;
	}
	case WM_KEYUP:
	{
		switch (wParam)
		{
		case 'W':
			renderer->moveForward = false;
			break;
		case 'A':
			renderer->moveLeft = false;
			break;
		case 'S':
			renderer->moveBackward = false;
			break;
		case 'D':
			renderer->moveRight = false;
			break;
		case VK_UP:
			renderer->pitchUp = false;
			break;
		case VK_DOWN:
			renderer->pitchDown = false;
			break;
		case VK_LEFT:
			renderer->yawLeft = false;
			break;
		case VK_RIGHT:
			renderer->yawRight = false;
			break;
		}
		break;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		case 'W':
		{
			renderer->AddYPosition(0.1f);
			//renderer->camera.z += 0.25f;
			renderer->moveForward = true;
			//MessageBox(NULL, L"W was pressed!", L"Key Press", MB_ICONINFORMATION | MB_OK);
			break;
		}
		case 'A':
		{
			renderer->AddXPosition(-0.1f);
			//renderer->camera.x -= 0.25f;
			renderer->moveLeft = true;
			break;
		}
		case 'S':
		{
			renderer->AddYPosition(-0.1f);
			//renderer->camera.z -= 0.25f;
			renderer->moveBackward = true;
			break;
		}
		case 'D':
		{
			renderer->AddXPosition(0.1f);
			//renderer->camera.x += 0.25f;
			renderer->moveRight = true;
			break;
		}
		case VK_UP:
		{
			renderer->pitchUp = true;
			//renderer->camera.pitch -= DirectX::XM_PI / 8;
			//renderer->SetColor(1.0f, 0.0f, 0.0f);
			break;
		}
		case VK_DOWN:
		{
			renderer->pitchDown = true;
			//renderer->camera.pitch += DirectX::XM_PI / 8;
			//renderer->SetColor(0.0f, 1.0f, 0.0f);
			break;
		}
		case VK_LEFT:
		{
			renderer->yawLeft = true;
			//renderer->camera.yaw -= DirectX::XM_PI / 8;
			//renderer->SetColor(0.0f, 0.0f, 1.0f);
			break;
		}
		case VK_RIGHT:
		{
			renderer->yawRight = true;
			//renderer->camera.yaw += DirectX::XM_PI / 8;
			//renderer->SetColor(1.0f, 1.0f, 0.0f);
			break;
		}
		}
		break;
	}

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
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
			renderer->RenderFrame();
		}
	}
}
