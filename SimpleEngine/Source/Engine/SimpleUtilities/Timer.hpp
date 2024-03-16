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

		float GetDeltaTime() const;
		float GetTotalMinutes() const;
		float GetTotalHours() const;
		double GetTotalTime() const;
	private:
		std::chrono::high_resolution_clock::time_point myStartTime;
		const double myCapDeltaTime;
		double myTotalTime;
		float myDeltaTime;
	};

	Timer::Timer()
		: myStartTime(std::chrono::high_resolution_clock::now())
		, myCapDeltaTime(1.0 / 60)
		, myTotalTime(0.0)
		, myDeltaTime(0.0f)

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

	inline float Timer::GetTotalMinutes() const
	{
		return (static_cast<float>(myTotalTime) / 60.0f);
	}

	inline float Timer::GetTotalHours() const
	{
		return (GetTotalMinutes() / 60.0f);
	}

	double Timer::GetTotalTime() const
	{
		return myTotalTime;
	}
}