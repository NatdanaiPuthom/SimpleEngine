#include "stdafx.h"
#include "Engine/engine.h"
#include "Engine/SimpleUtilities/InputManager.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (SimpleUtilities::InputManager::GetInstance().UpdateEvents(message, wParam, lParam))
		return 0;

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int /*nCmdShow*/)
{
	Engine engine;
	engine.Init(hInstance, 1280, 720);

	SimpleUtilities::InputManager* input = &SimpleUtilities::InputManager::GetInstance();
	input->SetHWND(engine.GetHWND());

	while (engine.BeginFrame())
	{
		input->Update();
		engine.EndFrame();;
	}

	return 0;
}
