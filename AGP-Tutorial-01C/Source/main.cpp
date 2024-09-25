#include "Renderer/Renderer.h"
#include "Window/Window.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, 
				   _In_opt_ HINSTANCE hPrevInstance, 
				   _In_ LPSTR lpCmdLine, 
				   _In_ int nCmdShow)
{
	Window window{};
	
	window.OpenConsole();

	if (FAILED(window.Init(hInstance, nCmdShow)))
	{
		MessageBox(NULL, L"Window Initialization Failed", L"Critical Error", MB_ICONERROR | MB_OK);
	}

	Renderer renderer{};

	if (FAILED(renderer.Init(window.GetWindowHandle())))
	{
		MessageBox(NULL, L"Renderer Initialization Failed", L"Critical Error", MB_ICONERROR | MB_OK);
	}

	window.Run();

	renderer.Clean();

	return 0;
}