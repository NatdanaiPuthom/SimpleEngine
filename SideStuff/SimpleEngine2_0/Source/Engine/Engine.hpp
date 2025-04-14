#pragma once
#include <memory>

namespace Simple
{
	class Timer;
}

namespace Simple
{
	class Engine final
	{
	public:
		Engine();
		~Engine();

		void Init(void* aHandleInstance, const int aCmdShow);
		void Update();

		bool IsRunning() const;
	private:
		Timer* myTimer;
		bool myIsRunning;
	};
}