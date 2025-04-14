#include "ApplicationWindow.hpp"
#include <dwmapi.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

namespace Simple
{
	HWND ApplicationWindow::SetUpMainWindow(HINSTANCE& aHandleInstance, const int aWidth, const int aHeight, const int aCmdShow)
	{
		WNDCLASSEXW wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_OWNDC;
		wcex.lpfnWndProc = WndProc;
		wcex.hInstance = aHandleInstance;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszClassName = L"Natdanai";
		//wcex.hIcon = LoadIcon(aHandleInstance, MAKEINTRESOURCE(101));
		//wcex.hIconSm = LoadIcon(aHandleInstance, MAKEINTRESOURCE(101));

		if (!RegisterClassExW(&wcex))
		{
			MessageBox(NULL, L"Failed to register window class", L"Error", MB_ICONERROR);
			return nullptr;
		}

		RECT wr = { 0, 0, aWidth, aHeight };
		AdjustWindowRect(&wr, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

		HWND hwnd = CreateWindowEx(
			0,
			L"Natdanai",
			L"SimpleEngine",
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			wr.right - wr.left,
			wr.bottom - wr.top,
			nullptr,
			nullptr,
			aHandleInstance,
			nullptr);

		if (hwnd)
		{
			//NOTE(v11.4.5): Disable rounded corners
			enum DWM_WINDOW_CORNER_PREFERENCE {
				DWMWCP_DEFAULT = 0,
				DWMWCP_DONOTROUND = 1,
				DWMWCP_ROUND = 2,
				DWMWCP_ROUNDSMALL = 3
			};

			DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_DONOTROUND;
			DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

			ShowWindow(hwnd, aCmdShow);
			UpdateWindow(hwnd);
		}

		return hwnd;
	}
}