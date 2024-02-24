#include "Engine/Precomplied/EnginePch.hpp"
#include <mutex>
#include <debugapi.h>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "Engine/MemoryTracker/MemoryTracker.h"
#include <External/TheGameAssembly/StackTrace/StackTrace.h>

//The Game Assembly Stuff but modified a little bit
using namespace SimpleTracker;

#ifdef _DEBUG

struct AllocationEntry
{
	size_t mySize = 0;
	StackTrace myStackTrace;
};

static int localTotalAllocationCount = 0;
static thread_local bool localIsAllocationInProgress = false;
static std::mutex localAllocationMapMutex;
static std::unordered_map<long, AllocationEntry> localAllocationMap;
static std::unordered_map<StackTrace, int> localStackTraceToAllocationCount;
static MemoryTrackingSettings localMemoryTrackingSettings;

typedef struct _CrtMemBlockHeader
{
	struct _CrtMemBlockHeader* pBlockHeaderNext;
	struct _CrtMemBlockHeader* pBlockHeaderPrev;
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
} _CrtMemBlockHeader;

int AllocHook(int aAllocType, void* aUserData, size_t aSize, int aBlockType, long aRequestNumber, const unsigned char*, int)
{	if (aBlockType == _CRT_BLOCK || localIsAllocationInProgress)
		return true;

	// Do not track allocations by the memory tracker itself
	localIsAllocationInProgress = true;

	std::lock_guard<std::mutex> guard(localAllocationMapMutex);

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
		AllocationEntry& entry = localAllocationMap[aRequestNumber];

		if (localMemoryTrackingSettings.myShouldStoreStackTraces)
		{
			entry.myStackTrace = StackTrace::CaptureStackTrace(1);
			localStackTraceToAllocationCount[entry.myStackTrace]++;
		}

		entry.mySize = aSize;
		localTotalAllocationCount++;
	}

	if (free)
	{
		// read request number from header
		const size_t headerSize = sizeof(_CrtMemBlockHeader);
		const size_t ptr = (size_t)aUserData - headerSize;

		_CrtMemBlockHeader* pHead;
		pHead = (_CrtMemBlockHeader*)(ptr);

		long freeRequestNumber = pHead->lRequest;

		localAllocationMap.erase(freeRequestNumber);
	}

	localIsAllocationInProgress = false;
	return true;
}

void SimpleTracker::StartMemoryTracking(const MemoryTrackingSettings& aTrackingSettings)
{
	localMemoryTrackingSettings = aTrackingSettings;
	_CrtSetAllocHook(&AllocHook);
}

void PrintTopLeaks(std::unordered_map<StackTrace, int>& aMap)
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

void SimpleTracker::StopMemoryTrackingAndPrint()
{
	std::lock_guard<std::mutex> guard(localAllocationMapMutex);

	_CrtSetAllocHook(nullptr);

	if (localMemoryTrackingSettings.myShouldTrackAllAllocations)
	{
		OutputDebugStringA("================================================================================\n");
		char buffer[100];
		sprintf_s(buffer, "== Total Allocation Count: %d\n", localTotalAllocationCount);
		OutputDebugStringA(buffer);

		if (localMemoryTrackingSettings.myShouldStoreStackTraces)
		{
			OutputDebugStringA("== Top 10 Allocations: \n");
			PrintTopLeaks(localStackTraceToAllocationCount);
		}
		else
		{
			OutputDebugStringA("================================================================================\n");
		}
	}

	if (localAllocationMap.size() == 0)
	{
		OutputDebugStringA("================================================================================\n");
		OutputDebugStringA("== No memory leaks found! \n");
		OutputDebugStringA("================================================================================\n");
	}
	else
	{
		OutputDebugStringA("================================================================================\n");
		char buffer[100];
		sprintf_s(buffer, "== Total Number of Memory Leaks: %d\n", (int)localAllocationMap.size());
		OutputDebugStringA(buffer);

		if (localMemoryTrackingSettings.myShouldStoreStackTraces)
		{
			OutputDebugStringA("================================================================================\n");
			std::unordered_map<StackTrace, int> stackTraceToLeakCountMap;

			for (const auto& p : localAllocationMap)
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

	localAllocationMap.clear();
}

#else 
void SimpleTracker::StartMemoryTracking(const MemoryTrackingSettings&) {}
void SimpleTracker::StopMemoryTrackingAndPrint() {}
#endif 
