module;

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN //NOTE(v1.0.0): Fuck you Microsoft
#endif

#include <Windows.h>
#include <cassert>

export module MOD_Engine;

import MOD_Console;
import MOD_WinProc;
import MOD_Timer;

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
		HWND SetUpMainWindow(HINSTANCE& hInstance);

	private:
		HWND myHWND;
		Timer myTimer;
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
		if (myIsRunning)
		{
			return false;
		}

		if (!myConsole.Init())
		{
			return false;
		}

		myHWND = SetUpMainWindow(hInstance);

		if (!myHWND)
		{
			return false;
		}

		ShowWindow(myHWND, nCmdShow);
		UpdateWindow(myHWND);

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

	HWND Engine::SetUpMainWindow(HINSTANCE& hInstance)
	{
		WNDCLASSEXW wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wcex.style &= ~CS_DROPSHADOW;
		wcex.lpfnWndProc = WndProc;
		wcex.hInstance = hInstance;
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
		wcex.lpszClassName = L"Natdanai";
		//wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		//wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));
		//wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(101));

		//assert(wcex.hCursor && "Failed to load cursor");
		//assert(wcex.hIcon != NULL && "Failed to load icon");
		//assert(wcex.hIconSm != NULL && "Failed to load small icon");

		if (!RegisterClassExW(&wcex))
		{
			MessageBox(NULL, L"Failed to register window class", L"Error", MB_ICONERROR);
			return nullptr;
		}

		const int width = 1600;
		const int height = 900;

		RECT wr = {};
		wr.left = 0;
		wr.right = width + wr.left;
		wr.top = 0;
		wr.bottom = height + wr.top;

		AdjustWindowRect(&wr, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

		return CreateWindow(
			L"Natdanai",
			L"v1.0.0",
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			wr.right - wr.left,
			wr.bottom - wr.top,
			nullptr,
			nullptr,
			hInstance,
			nullptr);
	}
}

#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif