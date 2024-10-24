#include "Application/App.h"

#include <Mouse.h>

int WINAPI WinMain(_In_ HINSTANCE hInstance, 
				   _In_opt_ HINSTANCE hPrevInstance, 
				   _In_ LPSTR lpCmdLine, 
				   _In_ int nCmdShow)
{
	// INFO: Testing DirectX Toolkit
	DirectX::Mouse m;

	App app{ hInstance, nCmdShow };

	return 0;
}