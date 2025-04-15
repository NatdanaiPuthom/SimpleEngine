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
		Engine();
		~Engine();

		void Init();
		void Update();
	private:
		Timer* myTimer;
		Console* myConsole;
	};
}