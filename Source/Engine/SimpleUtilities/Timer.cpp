#include "stdafx.h"
#include "Timer.h"

//#undef min
namespace SimpleUtilities
{
	Timer::Timer()
		: myDeltaTime(0.0f)
		, myTotalTime(0.0)
		, myStartTime(std::chrono::high_resolution_clock::now())
		, myCapDeltaTime(1.0 / 60)
	{
	}

	void Timer::Update()
	{
		const std::chrono::duration<double> deltaTime = std::chrono::high_resolution_clock::now() - myStartTime;

		myDeltaTime = static_cast<float>(std::min(deltaTime.count(), myCapDeltaTime));

		myTotalTime += deltaTime.count();
		myStartTime = std::chrono::high_resolution_clock::now();
	}

	void Timer::Reset()
	{
		myDeltaTime = 0.0f;
		myTotalTime = 0.0;
		myStartTime = std::chrono::high_resolution_clock::now();
	}

	float Timer::GetDeltaTime() const
	{
		return myDeltaTime;
	}

	double Timer::GetTotalTime() const
	{
		return myTotalTime;
	}
}
