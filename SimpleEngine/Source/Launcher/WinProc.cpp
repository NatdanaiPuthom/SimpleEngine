#include "MainSingleton/MainSingleton.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Global.hpp"
#include "Engine/SimpleUtilities/FileManager/FileManager.hpp"
#include <External/imgui.h>
#include <dwmapi.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return 0;

	if (MainSingleton::GetInputManager().UpdateEvents(message, wParam, lParam))
		return 0;

	switch (message)
	{
	case WM_SETCURSOR:
		::SetCursor(Global::GetCurrentCustomCursor());
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			MainSingleton::GetInputManager().ResetKeyStates();

			if (MainSingleton::GetInputManager().GetMouseIsHidden() && Global::GetGraphicsEngine()->GetCurrentCamera()->IsFreeFlyActive())
			{
				Global::GetGraphicsEngine()->GetCurrentCamera()->InactiveFreeFly();
			}
		}
		break;
	case WM_DWMNCRENDERINGCHANGED:
	{
		//const COLORREF COLOR = 0xFF00FF; //0xBGR Pink
		const COLORREF COLOR = 0x252525; //0xBGR DarkGray
		DwmSetWindowAttribute(hWnd, DWMWINDOWATTRIBUTE::DWMWA_CAPTION_COLOR, &COLOR, sizeof(COLOR));
		break;
	}
	case WM_DROPFILES:
	{
		HDROP hDrop = reinterpret_cast<HDROP>(wParam);
		SimpleUtilities::FileManager::DropFiles(hDrop);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}