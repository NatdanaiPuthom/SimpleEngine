module;

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN //NOTE(v1.0.0): Fuck you Microsoft
#endif

#include <Windows.h>

export module MOD_Engine;

import MOD_Console;
import MOD_WinProc;

export namespace Simple
{
	class Engine final
	{
	public:
		Engine();
		~Engine();

		bool Init(HINSTANCE& hInstance, const int nCmdShow);
		void Update();
		bool IsRunning() const;

	private:
		HWND myHWND;
		Console myConsole;
		bool myIsRunning = false;
	};

	Engine::Engine()
	{
		myHWND = {};
	}

	Engine::~Engine()
	{
	}

	bool Engine::Init(HINSTANCE& hInstance, const int nCmdShow)
	{
		if (myIsRunning == true)
		{
			return false;
		}

		if (myConsole.Init() == false)
		{
			return false;
		}

		WNDCLASS wc = {};
		wc.lpfnWndProc = WndProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = L"SimpleWindowClass";

		if (!RegisterClass(&wc))
		{
			return false;
		}

		HWND hWnd = CreateWindow(
			L"SimpleWindowClass",  // Window class name
			L"Simple Window",      // Window title
			WS_OVERLAPPEDWINDOW,   // Window style
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, // Position and size
			NULL, NULL, hInstance, NULL);

		if (!hWnd)
		{
			return false;
		}

		ShowWindow(hWnd, nCmdShow);
		UpdateWindow(hWnd);

		myIsRunning = true;

		return true;
	}

	void Engine::Update()
	{
		MSG msg = { 0 };

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				myIsRunning = false;
				break;
			}
		}

		if (GetAsyncKeyState(VK_RETURN))
		{
			myIsRunning = false;
		}
	}

	bool Engine::IsRunning() const
	{
		return myIsRunning;
	}
}

#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif