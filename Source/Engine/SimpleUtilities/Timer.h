#pragma once
#include <chrono>

namespace SimpleUtilities
{
	class Timer
	{
	public:
		Timer();
		Timer(const Timer& aTimer) = delete;
		Timer& operator=(const Timer& aTimer) = delete;

		void Update();
		void Reset();

		float GetDeltaTime() const;
		double GetTotalTime() const;
	private:
		std::chrono::high_resolution_clock::time_point myStartTime;
		const double myCapDeltaTime;
		double myTotalTime;
		float myDeltaTime;
	};
}
