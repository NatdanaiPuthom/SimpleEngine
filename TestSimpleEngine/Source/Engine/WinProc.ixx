module;

#include <dwmapi.h>

export module MOD_WinProc;

export LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SETCURSOR:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_ACTIVATE:
		break;
	case WM_DWMNCRENDERINGCHANGED:
	{
		break;
	}
	case WM_DROPFILES:
	{
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
