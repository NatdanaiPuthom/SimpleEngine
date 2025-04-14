#pragma once
#include "Utility/Timer.hpp"

namespace Simple
{
	class Console;
}

namespace Simple
{
	class Engine final
	{
		using HWNDPtr = void*; //NOTE: To avoid having to include Windows.h in headers
		using HINSTANCEPtr = void*; //NOTE: To avoid having to include Windows.h in headers
	public:
		Engine();
		~Engine();

		void Init(HINSTANCEPtr aHInstance, const int aCmdShow);
		void Update();

		bool IsRunning() const noexcept;
	private:
		HWNDPtr myHWND;
		Timer myTimer;
		Console* myConsole;
		bool myIsRunning;
	};
}