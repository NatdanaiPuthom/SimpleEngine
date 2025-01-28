#pragma once
#include <chrono>

namespace SimpleUtilities
{
	class Timer final
	{
	public:
		Timer();
		Timer(const Timer& aTimer) = delete;
		Timer& operator=(const Timer& aTimer) = delete;

		void Update();
		void Reset();

		void StartTimer();
		double EndTimer();

		float GetDeltaTime() const;
		float GetTotalMinutes() const;
		float GetTotalHours() const;
		double GetTotalTime() const;
	private:
		void ResetTimer();
	private:
		std::chrono::high_resolution_clock::time_point myStartTimerTime;
		std::chrono::high_resolution_clock::time_point myEndTimerTime;
		std::chrono::high_resolution_clock::time_point myStartTime;
		const double myCapDeltaTime;
		double myTotalTime;
		float myDeltaTime;
		bool myTimerIsRunning;
	};

	inline Timer::Timer()
		: myStartTime(std::chrono::high_resolution_clock::now())
		, myCapDeltaTime(1.0 / 60)
		, myTotalTime(0.0)
		, myDeltaTime(0.0f)
		, myTimerIsRunning(false)

	{
	}

#undef min //NOTE(v10.0.0): Where did this come from?

	inline void Timer::Update()
	{
		const std::chrono::duration<double> deltaTime = std::chrono::high_resolution_clock::now() - myStartTime;

		myDeltaTime = static_cast<float>(std::min(deltaTime.count(), myCapDeltaTime));

		myTotalTime += deltaTime.count();
		myStartTime = std::chrono::high_resolution_clock::now();
	}

	inline void Timer::Reset()
	{
		myDeltaTime = 0.0f;
		myTotalTime = 0.0;
		myStartTime = std::chrono::high_resolution_clock::now();
	}

	inline void Timer::StartTimer()
	{
		myStartTimerTime = std::chrono::high_resolution_clock::now();
		myTimerIsRunning = true;
	}

	inline double Timer::EndTimer()
	{
		if (myTimerIsRunning)
		{
			myEndTimerTime = std::chrono::high_resolution_clock::now();
			const double time = std::chrono::duration<double>(myEndTimerTime - myStartTimerTime).count();

			ResetTimer();

			return time;
		}
		else
		{
			return 0;
		}
	}

	inline float Timer::GetDeltaTime() const
	{
		return myDeltaTime;
	}

	inline float Timer::GetTotalMinutes() const
	{
		return (static_cast<float>(myTotalTime) / 60.0f);
	}

	inline float Timer::GetTotalHours() const
	{
		return (GetTotalMinutes() / 60.0f);
	}

	inline double Timer::GetTotalTime() const
	{
		return myTotalTime;
	}

	inline void Timer::ResetTimer()
	{
		myStartTimerTime = std::chrono::high_resolution_clock::time_point{};
		myEndTimerTime = std::chrono::high_resolution_clock::time_point{};
		myTimerIsRunning = false;
	}
}