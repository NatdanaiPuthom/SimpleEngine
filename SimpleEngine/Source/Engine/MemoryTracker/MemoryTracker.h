#pragma once
#include "Engine/MemoryTracker/MemoryTracker.h"
#include "External/TheGameAssembly/StackTrace/StackTrace.h"
#include <Windows.h>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <algorithm>

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
		struct Allocation final
		{
			size_t mySize = 0;
			StackTrace myStackTrace;
		};

	public:
		static int SimpleAllocHook(int aAllocType, void* aUserData, size_t aSize, int aBlockType, long aRequestNumber, const unsigned char*, int);
		static void StartMemoryTracking(const MemoryTrackingSettings& aTrackingSettings);
		static void PrintTopLeaks(std::unordered_map<StackTrace, int>& aMap);
		static void StopMemoryTrackingAndPrint();

		inline static std::mutex myAllocationMapMutex = {};
		inline static std::unordered_map<StackTrace, int> myStackTraceToAllocationCount = {};
		inline static std::unordered_map<long, Allocation> myAllocationMap = {};
		inline static int myTotalAllocationCount = 0;
		inline static thread_local bool myIsAllocationInProgress = false;
		inline static MemoryTrackingSettings myMemoryTrackingSettings = { false, true };
	};

	class SimpleMemoryTrackerWrapper final
	{
	public:
		inline SimpleMemoryTrackerWrapper()
		{
			SimpleMemoryTracker::myMemoryTrackingSettings = { false, true };
			_CrtSetAllocHook(&SimpleMemoryTracker::SimpleAllocHook);
		}

		inline ~SimpleMemoryTrackerWrapper()
		{
			SimpleMemoryTracker::StopMemoryTrackingAndPrint();
		}
	};

	class SimpleMemoryTrackerWrapperWrapper final
	{
		inline static SimpleMemoryTrackerWrapper globalStaticMemoryTrackerWrapper;
	};
}