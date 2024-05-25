#include "Engine/Precomplied/EnginePch.hpp"
#include "MemoryTracker.h"

typedef struct _SimpleCrtMemBlockHeader
{
	struct _SimpleCrtMemBlockHeader* pBlockHeaderNext;
	struct _SimpleCrtMemBlockHeader* pBlockHeaderPrev;
	char* szFileName;
	int                         nLine;
#ifdef _WIN64
	/* These items are reversed on Win64 to eliminate gaps in the struct
	 * and ensure that sizeof(struct)%16 == 0, so 16-byte alignment is
	 * maintained in the debug heap.
	 */
	int                         nBlockUse;
	size_t                      nDataSize;
#else 
	size_t                      nDataSize;
	int                         nBlockUse;
#endif
	long                        lRequest;
	unsigned char               gap[4];
} _SimpleCrtMemBlockHeader;

namespace SimpleTracker
{
#ifdef _DEBUG
	int SimpleMemoryTracker::SimpleAllocHook(int aAllocType, void* aUserData, size_t aSize, int aBlockType, long aRequestNumber, const unsigned char*, int)
	{
		if (aBlockType == _CRT_BLOCK || SimpleMemoryTracker::myIsAllocationInProgress)
			return 1;

		// Do not track allocations by the memory tracker itself
		SimpleMemoryTracker::myIsAllocationInProgress = true;

		std::lock_guard<std::mutex> guard(SimpleMemoryTracker::myAllocationMapMutex);

		bool alloc = false;
		bool free = false;

		switch (aAllocType)
		{
		case _HOOK_ALLOC:
			alloc = true;
			break;
		case _HOOK_REALLOC:
			alloc = true;
			free = true;
			break;
		case _HOOK_FREE:
			free = true;
			break;
		}

		if (alloc)
		{
			Allocation& entry = SimpleMemoryTracker::myAllocationMap[aRequestNumber];

			if (SimpleMemoryTracker::myMemoryTrackingSettings.myShouldStoreStackTraces)
			{
				entry.myStackTrace = StackTrace::CaptureStackTrace(1);
				SimpleMemoryTracker::myStackTraceToAllocationCount[entry.myStackTrace]++;
			}

			entry.mySize = aSize;
			SimpleMemoryTracker::myTotalAllocationCount++;
		}

		if (free)
		{
			// read request number from header
			const size_t headerSize = sizeof(_SimpleCrtMemBlockHeader);
			const size_t ptr = (size_t)aUserData - headerSize;

			_SimpleCrtMemBlockHeader* pHead;
			pHead = (_SimpleCrtMemBlockHeader*)(ptr);

			long freeRequestNumber = pHead->lRequest;

			SimpleMemoryTracker::myAllocationMap.erase(freeRequestNumber);
		}

		SimpleMemoryTracker::myIsAllocationInProgress = false;
		return 1;
	}

	void SimpleMemoryTracker::StartMemoryTracking(const MemoryTrackingSettings& aTrackingSettings)
	{
		SimpleMemoryTracker::myMemoryTrackingSettings = aTrackingSettings;
		_CrtSetAllocHook(&SimpleMemoryTracker::SimpleAllocHook);
		std::atexit(StopMemoryTrackingAndPrint);
	}

	void SimpleMemoryTracker::PrintTopLeaks(std::unordered_map<StackTrace, int>& aMap)
	{
		std::vector<std::pair<StackTrace, int>> pairs;

		for (const auto& p : aMap)
		{
			pairs.push_back(p);
		}

		std::sort(pairs.begin(), pairs.end(), [](const auto& a, const auto& b) {return a.second > b.second; });

		OutputDebugStringA("================================================================================\n");
		int i = 0;
		for (const auto& p : pairs)
		{
			if (i >= 10)
				break;

			char buffer[100];
			sprintf_s(buffer, "Count: %d\n, Stack Trace:", p.second);
			OutputDebugStringA(buffer);
			p.first.Print();

			OutputDebugStringA("================================================================================\n");

			i++;
		}
	}

	void SimpleMemoryTracker::StopMemoryTrackingAndPrint()
	{
		std::lock_guard<std::mutex> guard(SimpleMemoryTracker::myAllocationMapMutex);

		_CrtSetAllocHook(nullptr);

		if (SimpleMemoryTracker::myMemoryTrackingSettings.myShouldTrackAllAllocations)
		{
			OutputDebugStringA("================================================================================\n");
			char buffer[100];
			sprintf_s(buffer, "== Total Allocation Count: %d\n", SimpleMemoryTracker::myTotalAllocationCount);
			OutputDebugStringA(buffer);

			if (SimpleMemoryTracker::myMemoryTrackingSettings.myShouldStoreStackTraces)
			{
				OutputDebugStringA("== Top 10 Allocations: \n");
				PrintTopLeaks(SimpleMemoryTracker::myStackTraceToAllocationCount);
			}
			else
			{
				OutputDebugStringA("================================================================================\n");
			}
		}

		if (SimpleMemoryTracker::myAllocationMap.size() == 0)
		{
			OutputDebugStringA("================================================================================\n");
			OutputDebugStringA("== No memory leaks found! \n");
			OutputDebugStringA("================================================================================\n");
		}
		else
		{
			OutputDebugStringA("================================================================================\n");
			char buffer[100];
			sprintf_s(buffer, "== Total Number of Memory Leaks: %d\n", (int)SimpleMemoryTracker::myAllocationMap.size());
			OutputDebugStringA(buffer);

			if (SimpleMemoryTracker::myMemoryTrackingSettings.myShouldStoreStackTraces)
			{
				OutputDebugStringA("================================================================================\n");
				std::unordered_map<StackTrace, int> stackTraceToLeakCountMap;

				for (const auto& p : SimpleMemoryTracker::myAllocationMap)
				{
					stackTraceToLeakCountMap[p.second.myStackTrace]++;
				}

				OutputDebugStringA("== Top 10 Leaks: \n");
				PrintTopLeaks(stackTraceToLeakCountMap);
			}
			else
			{
				OutputDebugStringA("================================================================================\n");
				OutputDebugStringA("== For more details modify SimpleTracker::StartMemoryTracking\n");
				OutputDebugStringA("== SimpleTracker::StartMemoryTracking({ StoreStackTraces(Very slow, gives precise info), TrackAllAllocations(decently fast, lots of spam) });\n");
				OutputDebugStringA("================================================================================\n");
			}
		}

		SimpleMemoryTracker::myAllocationMap.clear();
	}

#else 
	void SimpleMemoryTracker::StartMemoryTracking(const MemoryTrackingSettings&) {}
	void SimpleMemoryTracker::StopMemoryTrackingAndPrint() {}
#endif 
}