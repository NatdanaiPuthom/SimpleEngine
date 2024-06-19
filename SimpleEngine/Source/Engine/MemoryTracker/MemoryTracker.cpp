#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/MemoryTracker/MemoryTracker.h"
#include "External/nlohmann/json.hpp"
#include <fstream>
#include <iostream>

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
	char localBuffer[255];
	bool SimpleMemoryTrackerWrapper::myShouldActive = false;

	std::unordered_map<StackTrace, int> SimpleMemoryTracker::myStaticShortLivedStackTraceToAllocationCount;
	std::unordered_map<long, SimpleMemoryTracker::Allocation> SimpleMemoryTracker::myStaticShortLivedAllocationMap;
	std::mutex SimpleMemoryTracker::myStaticShortLivedAllocationMapMutex;
	int SimpleMemoryTracker::myStaticShortLivedTotalAllocationCount;
	std::atomic<bool> SimpleMemoryTracker::myStaticHasStarted = false;

#ifdef _DEBUG
	int SimpleMemoryTracker::AllocHook(int aAllocType, void* aUserData, size_t aSize, int aBlockType, long aRequestNumber, const unsigned char*, int)
	{
		if (aBlockType == _CRT_BLOCK || SimpleMemoryTracker::myStaticIsAllocationInProgress)
			return 1;

		// Do not track allocations by the memory tracker itself
		SimpleMemoryTracker::myStaticIsAllocationInProgress = true;

		std::lock_guard<std::mutex> guard(SimpleMemoryTracker::myStaticAllocationMapMutex);

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
			Allocation& entry = SimpleMemoryTracker::myStaticAllocationMap[aRequestNumber];

			if (SimpleMemoryTracker::myStaticMemoryTrackingSettings.shouldStoreStackTraces)
			{
				entry.stackTrace = StackTrace::CaptureStackTrace(1);
				SimpleMemoryTracker::myStaticStackTraceToAllocationCount[entry.stackTrace]++;
			}

			entry.size = aSize;
			SimpleMemoryTracker::myStaticTotalAllocationCount++;

			if (myStaticHasStarted)
			{
				Allocation& entry2 = SimpleMemoryTracker::myStaticShortLivedAllocationMap[aRequestNumber];

				if (SimpleMemoryTracker::myStaticShortLivedMemoryTrackingSettings.shouldStoreStackTraces)
				{
					entry2.stackTrace = StackTrace::CaptureStackTrace(1);
					SimpleMemoryTracker::myStaticShortLivedStackTraceToAllocationCount[entry2.stackTrace]++;
				}

				entry2.size = aSize;
				SimpleMemoryTracker::myStaticShortLivedTotalAllocationCount++;
			}
		}

		if (free)
		{
			// read request number from header
			const size_t headerSize = sizeof(_SimpleCrtMemBlockHeader);
			const size_t ptr = (size_t)aUserData - headerSize;

			_SimpleCrtMemBlockHeader* pHead;
			pHead = (_SimpleCrtMemBlockHeader*)(ptr);

			long freeRequestNumber = pHead->lRequest;

			SimpleMemoryTracker::myStaticStackTraceToAllocationCount.erase(SimpleMemoryTracker::myStaticAllocationMap[freeRequestNumber].stackTrace);
			SimpleMemoryTracker::myStaticAllocationMap.erase(freeRequestNumber);

			if (myStaticHasStarted)
			{
				if (SimpleMemoryTracker::myStaticMemoryTrackingSettings.shouldStoreStackTraces)
				{
					SimpleMemoryTracker::myStaticShortLivedStackTraceToAllocationCount.erase(SimpleMemoryTracker::myStaticShortLivedAllocationMap[freeRequestNumber].stackTrace);
				}

				SimpleMemoryTracker::myStaticShortLivedAllocationMap.erase(freeRequestNumber);
			}
		}

		SimpleMemoryTracker::myStaticIsAllocationInProgress = false;
		return 1;
	}

	void SimpleMemoryTracker::Init()
	{
		const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_DEBUG);

		std::ifstream file(filename);

		if (file.is_open() == false)
		{
			const std::string fileName = "debug_settings.json";
			const std::string source = SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_DEPENDENCIES_SETTINGS) + fileName;
			const std::string destination = SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_SETTINGS) + fileName;
			std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing);

			std::cout << "Copied: " << fileName << std::endl;
		}

		file.open(filename);
		assert(file.is_open() && "Failed to open file");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		const nlohmann::json memoryTrackerSettings = json["Debug_Settings"]["MemoryTracker"];

		SimpleMemoryTrackerWrapper::myShouldActive = memoryTrackerSettings["Active"];

		if (SimpleMemoryTrackerWrapper::myShouldActive == false)
		{
			return;
		}

		const MemoryTrackingSettings& trackingSettings = { memoryTrackerSettings["Advanced"], true };

		SimpleMemoryTracker::myStaticMemoryTrackingSettings = trackingSettings;
		_CrtSetAllocHook(&SimpleMemoryTracker::AllocHook);
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
			if (i >= 5)
				break;

			char buffer[100];
			sprintf_s(buffer, "Count: %d\n, Stack Trace:", p.second);
			OutputDebugStringA(buffer);
			p.first.Print();

			OutputDebugStringA("================================================================================\n");

			i++;
		}
	}

	void SimpleMemoryTracker::Destory()
	{
		if (SimpleMemoryTrackerWrapper::myShouldActive == false)
		{
			return;
		}

		std::lock_guard<std::mutex> guard(SimpleMemoryTracker::myStaticAllocationMapMutex);

		_CrtSetAllocHook(nullptr);

		if (SimpleMemoryTracker::myStaticAllocationMap.size() == 0)
		{
			if (SimpleMemoryTracker::myStaticMemoryTrackingSettings.shouldTrackAllAllocations)
			{
				OutputDebugStringA("================================================================================\n");
				char buffer[100];
				sprintf_s(buffer, "== Total Allocation Count: %d\n", SimpleMemoryTracker::myStaticTotalAllocationCount);
				OutputDebugStringA(buffer);
			}

			OutputDebugStringA("================================================================================\n");
			OutputDebugStringA("== No memory leaks found! \n");
			OutputDebugStringA("================================================================================\n");
		}
		else
		{
			if (SimpleMemoryTracker::myStaticMemoryTrackingSettings.shouldStoreStackTraces)
			{
				OutputDebugStringA("================================================================================\n");
				std::unordered_map<StackTrace, int> stackTraceToLeakCountMap;

				for (const auto& p : SimpleMemoryTracker::myStaticAllocationMap)
				{
					stackTraceToLeakCountMap[p.second.stackTrace]++;
				}

				OutputDebugStringA("== Top 5 Leaks: \n");
				PrintTopLeaks(stackTraceToLeakCountMap);
			}

			if (SimpleMemoryTracker::myStaticMemoryTrackingSettings.shouldTrackAllAllocations)
			{
				OutputDebugStringA("================================================================================\n");
				char buffer[100];
				sprintf_s(buffer, "== Total Allocation Count: %d\n", SimpleMemoryTracker::myStaticTotalAllocationCount);
				OutputDebugStringA(buffer);
			}

			OutputDebugStringA("================================================================================\n");
			char buffer[100];
			sprintf_s(buffer, "== Total Number of Memory Leaks: %d\n", (int)SimpleMemoryTracker::myStaticAllocationMap.size());
			OutputDebugStringA(buffer);
			OutputDebugStringA("================================================================================\n");
			OutputDebugStringA("== For more details modify SimpleTracker::MemoryTrackingSettings\n");
			OutputDebugStringA("== SimpleTracker::MemoryTrackingSettings({ StoreStackTraces(Very slow, gives precise info), TrackAllAllocations(decently fast, lots of spam) });\n");
			OutputDebugStringA("================================================================================\n");
		}

		SimpleMemoryTracker::myStaticAllocationMap.clear();
		SimpleMemoryTracker::myStaticStackTraceToAllocationCount.clear();
	}

	void SimpleMemoryTracker::StartMemoryTracking(const bool aShowAdvanced, const std::string& aCallerName)
	{
		if (myStaticHasStarted == true) //TO-DO(v10.0.0): Better error and warning messages
		{
			return;
		}

		sprintf_s(localBuffer, std::string("== " + aCallerName).c_str());
		myStaticHasStarted = true;
		myStaticShortLivedMemoryTrackingSettings = { aShowAdvanced, true };
	}

	void SimpleMemoryTracker::StopMemoryTracking()
	{
		if (myStaticHasStarted == false)  //TO-DO(v10.0.0): Better error and warning messages
		{
			return;
		}

		if (SimpleMemoryTrackerWrapper::myShouldActive == false)
		{
			return;
		}

		myStaticHasStarted = false;
		PrintShortLivedToOutput();
	}

	void SimpleMemoryTracker::PrintShortLivedToOutput()
	{
		SimpleMemoryTracker::myStaticIsAllocationInProgress = true;

		std::lock_guard<std::mutex> guard(SimpleMemoryTracker::myStaticAllocationMapMutex);

		if (SimpleMemoryTracker::myStaticShortLivedAllocationMap.size() == 0)
		{
			OutputDebugStringA("\n================================================================================\n");
			OutputDebugStringA(localBuffer);
			char buffer[100];
			sprintf_s(buffer, "\n== Total Allocation Count: %d\n", SimpleMemoryTracker::myStaticShortLivedTotalAllocationCount);
			OutputDebugStringA(buffer);
			OutputDebugStringA("== No memory leaks found! \n");
			OutputDebugStringA("================================================================================\n");
		}
		else
		{
			if (SimpleMemoryTracker::myStaticShortLivedMemoryTrackingSettings.shouldStoreStackTraces)
			{
				OutputDebugStringA("\n================================================================================\n");
				std::unordered_map<StackTrace, int> stackTraceToLeakCountMap;

				for (const auto& p : SimpleMemoryTracker::myStaticShortLivedAllocationMap)
				{
					stackTraceToLeakCountMap[p.second.stackTrace]++;
				}

				OutputDebugStringA("== Top 10 Leaks: \n");
				PrintTopLeaks(stackTraceToLeakCountMap);
			}

			OutputDebugStringA(localBuffer);

			if (SimpleMemoryTracker::myStaticShortLivedMemoryTrackingSettings.shouldTrackAllAllocations)
			{
				OutputDebugStringA("\n");
				char buffer[100];
				sprintf_s(buffer, "== Total Allocation Count: %d\n", SimpleMemoryTracker::myStaticShortLivedTotalAllocationCount);
				OutputDebugStringA(buffer);
			}

			char buffer[100];
			sprintf_s(buffer, "== Number of Memory Leaks: %d\n", (int)SimpleMemoryTracker::myStaticShortLivedAllocationMap.size());
			OutputDebugStringA(buffer);
			OutputDebugStringA("================================================================================\n\n");
		}

		SimpleMemoryTracker::myStaticShortLivedAllocationMap.clear();
		SimpleMemoryTracker::myStaticShortLivedStackTraceToAllocationCount.clear();
		SimpleMemoryTracker::myStaticShortLivedTotalAllocationCount = 0;

		SimpleMemoryTracker::myStaticIsAllocationInProgress = false;
	}
#else 
	void SimpleMemoryTracker::Init(const MemoryTrackingSettings& /*aTrackingSettings*/) {};
	void SimpleMemoryTracker::Destory() {};
	void SimpleMemoryTracker::StartMemoryTracking(const bool /*aShowAdvanced*/, const std::string& /*aCallerName*/) {}
	void SimpleMemoryTracker::StopMemoryTracking() {}
#endif 
}