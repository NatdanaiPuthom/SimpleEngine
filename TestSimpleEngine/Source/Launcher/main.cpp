#define WIN32_LEAN_AND_MEAN //NOTE(v1.0.0): Fuck you Microsoft
#include <Windows.h>

import MOD_Console;
import MOD_WinProc;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	// Define and register the window class
	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"SimpleWindowClass";

	if (!RegisterClass(&wc)) return -1;

	HWND hWnd = CreateWindow(
		L"SimpleWindowClass",  // Window class name
		L"Simple Window",      // Window title
		WS_OVERLAPPEDWINDOW,   // Window style
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, // Position and size
		NULL, NULL, hInstance, NULL);

	if (!hWnd) return -1;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg = { 0 };
	bool isRunning = true;

	Simple::Console console;
	console.Init();

	while (isRunning)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				isRunning = false;
				break;
			}
		}

		if (GetAsyncKeyState(VK_RETURN))
		{
			isRunning = false;
			break;
		}
	}

	return (int)msg.wParam;
}
