#include "App.h"

#include <stdlib.h>
#include <time.h>

App::App(HINSTANCE hInstance, int nCmdShow, int width, int height)
{
	// INFO: Initialize the random number generator
	srand(static_cast<unsigned int>(time(nullptr)));

	// INFO: Creates custom console for iostream output
	window.OpenConsole();

	if (FAILED(window.Init(hInstance, nCmdShow)))
	{
		MessageBox(NULL, L"Window Initialization Failed", L"Critical Error", MB_ICONERROR | MB_OK);
	}

	window.Run();

	window.GetRenderer().Clean();
}
