#pragma once
#include "Engine/MemoryTracker/MemoryTracker.h"
#include "External/TheGameAssembly/StackTrace/StackTrace.h"
#include <Windows.h>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <atomic>
#include <string>
#include <source_location>
#include <memory>

//The Game Assembly stuff but modified a little bit. NOTE(v9.36.4): Modified a lot

namespace SimpleTracker
{
	class SimpleMemoryTracker;
	class SimpleMemoryTrackerWrapper;

	struct MemoryTrackingSettings final
	{
		bool shouldStoreStackTraces;
		bool shouldTrackAllAllocations;
	};

	struct Allocation final
	{
		size_t size = 0;
		StackTrace stackTrace;
	};

	class TemporaryScopedMemoryTrackerData final
	{
		friend class SimpleTracker::SimpleMemoryTracker;
	public:
		std::unordered_map<StackTrace, int> stackTraceToAllocationCount;
		std::unordered_map<long, Allocation> allocationMap;
		MemoryTrackingSettings memoryTrackingSettings = { false, false };
		std::atomic<bool> hasStarted = false;
		int totalAllocationCount = 0;

	public:

		~TemporaryScopedMemoryTrackerData() = default;

	private:
		static TemporaryScopedMemoryTrackerData& GetInstance()
		{
			std::call_once(myInitInstanceFlag, &TemporaryScopedMemoryTrackerData::Init); //TO-DO(v11.2.1): learn more about this, as this was guided by ChatGPT and I have no clue how this work but it works
			return *myInstance;
		}

		TemporaryScopedMemoryTrackerData() : myPadding{ INT_MIN } {};

		static void Init()
		{
			myInstance.reset(new TemporaryScopedMemoryTrackerData());
		}

	private:
		inline static std::unique_ptr<TemporaryScopedMemoryTrackerData> myInstance;
		inline static std::once_flag myInitInstanceFlag;
	private:
		int myPadding[6];
	};

	class SimpleMemoryTracker final
	{
		friend class SimpleTracker::SimpleMemoryTrackerWrapper;

	public:
		static void StartMemoryTracking(const bool aShowAdvanced = false, const std::string& aCallerName = "");
		static void StopMemoryTracking();
	private:
		static void Init();
		static void Destory();

		static int AllocHook(int aAllocType, void* aUserData, size_t aSize, int aBlockType, long aRequestNumber, const unsigned char*, int);
		static void PrintTopLeaks(std::unordered_map<StackTrace, int>& aMap, const bool aScopedTracking);
		static void PrintShortLivedToOutput();
		static void WriteToTxtFile(const std::vector<std::pair<StackTrace, int>>& aStackTraces, const bool aScopedTracking);
	private:
		inline static std::mutex myStaticAllocationMapMutex{};
		inline static std::unordered_map<StackTrace, int> myStaticStackTraceToAllocationCount{};
		inline static std::unordered_map<long, Allocation> myStaticAllocationMap{};
		inline static int myStaticTotalAllocationCount = 0;
		inline static thread_local bool myStaticIsAllocationInProgress = false;
		inline static MemoryTrackingSettings myStaticMemoryTrackingSettings = { false, false };
	};

	class SimpleMemoryTrackerWrapper final
	{
		friend class SimpleMemoryTracker;
	public:
		inline SimpleMemoryTrackerWrapper()
		{
			SimpleTracker::SimpleMemoryTracker::Init();
		}

		inline ~SimpleMemoryTrackerWrapper()
		{
			SimpleMemoryTracker::Destory();
		}

	private:
		inline static bool myShouldActive = false;
	};

	class SimpleMemoryTrackerWrapperWrapper final
	{
		inline static SimpleMemoryTrackerWrapper myStaticMemoryTrackerWrapper;
	};
}

#define BeginMemoryTracking(aShowAdvanced) SimpleTracker::SimpleMemoryTracker::StartMemoryTracking(aShowAdvanced, std::source_location::current().function_name())
#define EndMemoryTracking() SimpleTracker::SimpleMemoryTracker::StopMemoryTracking()