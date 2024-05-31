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
		bool myShouldStoreStackTraces;
		bool myShouldTrackAllAllocations;
	};

	class SimpleMemoryTracker final
	{
		friend class SimpleMemoryTrackerWrapper;

		struct Allocation final
		{
			size_t mySize = 0;
			StackTrace myStackTrace;
		};

	public:
		static void StartMemoryTracking(const bool aShowAdvanced = false, const std::string& aCallerName = "");
		static void StopMemoryTracking();
	private:
		static void Init(const MemoryTrackingSettings& aTrackingSettings);
		static void Destory();

		static int AllocHook(int aAllocType, void* aUserData, size_t aSize, int aBlockType, long aRequestNumber, const unsigned char*, int);
		static void PrintTopLeaks(std::unordered_map<StackTrace, int>& aMap);
		static void PrintShortLivedToOutput();
	private:
		inline static std::mutex myAllocationMapMutex = {};
		inline static std::unordered_map<StackTrace, int> myStackTraceToAllocationCount = {};
		inline static std::unordered_map<long, Allocation> myAllocationMap = {};

		inline static int myTotalAllocationCount = 0;
		inline static thread_local bool myIsAllocationInProgress = false;

		inline static MemoryTrackingSettings myMemoryTrackingSettings = { false, false };
		inline static MemoryTrackingSettings myShortLivedMemoryTrackingSettings = { false, false };

		static std::mutex myShortLivedAllocationMapMutex;
		static std::unordered_map<StackTrace, int> myShortLivedStackTraceToAllocationCount;
		static std::unordered_map<long, Allocation> myShortLivedAllocationMap;
		static std::atomic<bool> myHasStarted;
		static int myShortLivedTotalAllocationCount;
	};

	class SimpleMemoryTrackerWrapper final
	{
		friend class SimpleMemoryTracker;
	public:
		inline SimpleMemoryTrackerWrapper()
		{
			SimpleTracker::MemoryTrackingSettings memoryTrackingSettings = { false, true };
			SimpleTracker::SimpleMemoryTracker::Init(memoryTrackingSettings);
		}

		inline ~SimpleMemoryTrackerWrapper()
		{
			SimpleMemoryTracker::Destory();
		}

	private:
		static bool myShouldActive;
	};

	class SimpleMemoryTrackerWrapperWrapper final
	{
		inline static SimpleMemoryTrackerWrapper globalStaticMemoryTrackerWrapper;
	};
}

#define BeginMemoryTracking(aShowAdvanced) SimpleTracker::SimpleMemoryTracker::StartMemoryTracking(aShowAdvanced, std::source_location::current().function_name())
#define EndMemoryTracking() SimpleTracker::SimpleMemoryTracker::StopMemoryTracking()