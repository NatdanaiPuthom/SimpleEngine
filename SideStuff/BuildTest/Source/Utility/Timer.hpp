#pragma once
#include <chrono>

namespace Simple
{
	class Timer final
	{
	public:
		Timer();
		~Timer();

		void Update();
		void Reset() noexcept;

		void ResetTimer() noexcept;
		void StartTimer();
		float EndTimer();
	public:
		void SetMaximumDeltaTime(const float aMaximumDeltaTimeInSecond) noexcept;
	public:
		float GetDeltaTime() const noexcept;
		float GetTotalHours() const noexcept;
		float GetTotalMinutes() const noexcept;
		float GetTotalSeconds() const noexcept;
	private:
		std::chrono::high_resolution_clock::time_point myStartTime;
		std::chrono::high_resolution_clock::time_point myTimerStartTime;
		float myTotalTimeInSeconds;
		float myMaximumDeltaTimeInSeconds;
		float myDeltaTimeInSeconds;
		bool myTimerIsRunning;
	};

	inline Timer::Timer()
		: myTotalTimeInSeconds(0.0f)
		, myMaximumDeltaTimeInSeconds(1.0f / 60.0f)
		, myDeltaTimeInSeconds(0.0f)
		, myTimerIsRunning(false)
	{
		myStartTime = std::chrono::high_resolution_clock::time_point{};
		myTimerStartTime = std::chrono::high_resolution_clock::time_point{};
	}

	inline Timer::~Timer()
	{
		myStartTime = std::chrono::high_resolution_clock::time_point{};
		myTimerStartTime = std::chrono::high_resolution_clock::time_point{};

		myTotalTimeInSeconds = 0.0f;
		myMaximumDeltaTimeInSeconds = 0.0f;
		myDeltaTimeInSeconds = 0.0f;
		myTimerIsRunning = false;
	}

	inline void Timer::Update()
	{
		const float deltaTime = static_cast<float>((std::chrono::high_resolution_clock::now() - myStartTime).count());

		myDeltaTimeInSeconds = std::min(deltaTime, myMaximumDeltaTimeInSeconds);
		myTotalTimeInSeconds += deltaTime;

		myStartTime = std::chrono::high_resolution_clock::now();
	}

	inline void Timer::StartTimer()
	{
		myTimerIsRunning = true;
		myTimerStartTime = std::chrono::high_resolution_clock::now();
	}

	inline float Timer::EndTimer()
	{
		if (myTimerIsRunning == false)
		{
			return 0.0f;
		}

		const auto delta = std::chrono::high_resolution_clock::now() - myTimerStartTime;
		const float time = static_cast<float>(delta.count());

		myTimerStartTime = std::chrono::high_resolution_clock::time_point{};
		myTimerIsRunning = false;

		return time;
	}

	inline void Timer::Reset() noexcept
	{
		myDeltaTimeInSeconds = 0.0f;
		myTotalTimeInSeconds = 0.0;
		myStartTime = std::chrono::high_resolution_clock::now();
	}

	inline void Timer::SetMaximumDeltaTime(const float aMaximumDeltaTimeInSecond) noexcept
	{
		myMaximumDeltaTimeInSeconds = aMaximumDeltaTimeInSecond;
	}

	inline float Timer::GetDeltaTime() const noexcept
	{
		return myDeltaTimeInSeconds;
	}

	inline float Timer::GetTotalHours() const noexcept
	{
		return (GetTotalMinutes() / 60.0f);
	}

	inline float Timer::GetTotalMinutes() const noexcept
	{
		return (static_cast<float>(GetTotalSeconds()) / 60.0f);
	}

	inline float Timer::GetTotalSeconds() const noexcept
	{
		return myTotalTimeInSeconds;
	}

	inline void Timer::ResetTimer() noexcept
	{
		myTimerStartTime = std::chrono::high_resolution_clock::now();
	}
}