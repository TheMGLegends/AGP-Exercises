#include "Window/Window.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, 
				   _In_opt_ HINSTANCE hPrevInstance, 
				   _In_ LPSTR lpCmdLine, 
				   _In_ int nCmdShow)
{
	Window window;

	if (FAILED(window.InitiWindow(hInstance, nCmdShow)))
	{
		MessageBox(NULL, L"Window Initialization Failed", L"Error", MB_ICONERROR | MB_OK);
	}

	window.Run();

	return 0;
}