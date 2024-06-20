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

//The Game Assembly stuff but modified a little bit. NOTE(v9.36.4): Modified a lot

namespace SimpleTracker
{
	struct MemoryTrackingSettings final
	{
		bool shouldStoreStackTraces;
		bool shouldTrackAllAllocations;
	};

	class SimpleMemoryTracker final
	{
		friend class SimpleMemoryTrackerWrapper;

		struct Allocation final
		{
			size_t size = 0;
			StackTrace stackTrace;
		};

		struct TemporaryScopedMemoryTrackerData
		{
			std::mutex allocationMapMutex;
			std::unordered_map<StackTrace, int> stackTraceToAllocationCount;
			std::unordered_map<long, Allocation> allocationMap;
			MemoryTrackingSettings memoryTrackingSettings = { false, false };
			std::atomic<bool> hasStarted = false;
			int totalAllocationCount = 0;
		};

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

		inline static TemporaryScopedMemoryTrackerData* myScopedTrackerData = nullptr;
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