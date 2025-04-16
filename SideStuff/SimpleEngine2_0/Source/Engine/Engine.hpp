#pragma once

namespace Simple
{
	class Timer;
	class Console;
}

namespace Simple
{
	class Engine final
	{
	public:
		static Engine* GetInstance();
		static Engine* CreateInstance();
		static void DestroyInstance();
	public:
		Engine();
		~Engine();

		void Init();
		void Update();
	private:
	private:
		Timer* myTimer;
		Console* myConsole;
	private:
		inline static Engine* sInstance = nullptr;
	};
}