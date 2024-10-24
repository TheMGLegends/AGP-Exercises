#pragma once

#include <Windows.h>
#include <memory>
#include <Keyboard.h>

#include "../Renderer/Renderer.h"

class Window
{
public:
	Window();
	HRESULT Init(HINSTANCE _hInstance, int nCmdShow, int width = 800, int height = 600);

	static LRESULT CALLBACK HandleMessageSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMessageRepeated(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void HandleInput();

	void OpenConsole();

	void Run();

	inline HWND GetWindowHandle() const { return hWnd; }
	inline Renderer& GetRenderer() const { return *renderer; }

private:
	HINSTANCE hInstance;
	HWND hWnd;
	const wchar_t* windowName;

	std::unique_ptr<Renderer> renderer;

	DirectX::Keyboard keyboard;
	DirectX::Keyboard::KeyboardStateTracker keyboardStateTracker;
};

