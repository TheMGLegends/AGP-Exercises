#pragma once

#include "../Window/Window.h"

class App
{
public:
	App(HINSTANCE hInstance, int nCmdShow, int width = 800, int height = 600);

private:
	Window window;
};

