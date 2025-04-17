#include "MemoryTracker.h"
//#include "Engine/SimpleUtilities/Utility.hpp"
//#include "External/nlohmann/json.hpp"
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

#ifdef _DEBUG
	int SimpleMemoryTracker::AllocHook(int aAllocType, void* aUserData, size_t aSize, int aBlockType, long aRequestNumber, const unsigned char*, int)
	{
		if (aBlockType == _CRT_BLOCK || myStaticIsAllocationInProgress)
			return 1;

		// Do not track allocations by the memory tracker itself
		myStaticIsAllocationInProgress = true;

		std::lock_guard<std::mutex> guard(myStaticAllocationMapMutex);

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
			Allocation& entry = myStaticAllocationMap[aRequestNumber];

			if (myStaticMemoryTrackingSettings.shouldStoreStackTraces)
			{
				entry.stackTrace = StackTrace::CaptureStackTrace(1);
				myStaticStackTraceToAllocationCount[entry.stackTrace]++;
			}

			entry.size = aSize;
			SimpleMemoryTracker::myStaticTotalAllocationCount++;

			if (TemporaryScopedMemoryTrackerData::GetInstance().hasStarted)
			{
				Allocation& entry2 = TemporaryScopedMemoryTrackerData::GetInstance().allocationMap[aRequestNumber];

				if (TemporaryScopedMemoryTrackerData::GetInstance().memoryTrackingSettings.shouldStoreStackTraces)
				{
					entry2.stackTrace = StackTrace::CaptureStackTrace(1);
					TemporaryScopedMemoryTrackerData::GetInstance().stackTraceToAllocationCount[entry2.stackTrace]++;
				}

				entry2.size = aSize;
				TemporaryScopedMemoryTrackerData::GetInstance().totalAllocationCount++;
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

			myStaticStackTraceToAllocationCount.erase(myStaticAllocationMap[freeRequestNumber].stackTrace);
			myStaticAllocationMap.erase(freeRequestNumber);

			if (TemporaryScopedMemoryTrackerData::GetInstance().hasStarted)
			{
				if (myStaticMemoryTrackingSettings.shouldStoreStackTraces)
				{
					TemporaryScopedMemoryTrackerData::GetInstance().stackTraceToAllocationCount.erase(TemporaryScopedMemoryTrackerData::GetInstance().allocationMap[freeRequestNumber].stackTrace);
				}

				TemporaryScopedMemoryTrackerData::GetInstance().allocationMap.erase(freeRequestNumber);
			}
		}

		myStaticIsAllocationInProgress = false;
		return 1;
	}

	void SimpleMemoryTracker::Init()
	{
		/*const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_DEBUG);

		std::ifstream file(filename);

		if (file.is_open() == false)
		{
			const std::string fileName = SIMPLE_FILENAME_DEBUGJSON;
			const std::string source = SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_DEPENDENCIES_SETTINGS) + fileName;
			const std::string destination = SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_SETTINGS) + fileName;
			std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing);
		}

		file.open(filename);
		assert(file.is_open() && "Failed to open file");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		const nlohmann::json memoryTrackerSettings = json["Debug_Settings"]["MemoryTracker"];

		SimpleMemoryTrackerWrapper::myShouldActive = memoryTrackerSettings["Active"];*/

		SimpleMemoryTrackerWrapper::myShouldActive = true; //Temp

		if (SimpleMemoryTrackerWrapper::myShouldActive == false)
		{
			return;
		}

		//const MemoryTrackingSettings& trackingSettings = { memoryTrackerSettings["Advanced"], true };
		const MemoryTrackingSettings& trackingSettings = { false , true }; //Temp

		myStaticMemoryTrackingSettings = trackingSettings;
		_CrtSetAllocHook(&SimpleMemoryTracker::AllocHook);
	}

	void SimpleMemoryTracker::PrintTopLeaks(std::unordered_map<StackTrace, int>& aMap, const bool aScopedTracking)
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

		WriteToTxtFile(pairs, aScopedTracking);
	}

	void SimpleMemoryTracker::Destory()
	{
		if (SimpleMemoryTrackerWrapper::myShouldActive == false)
		{
			return;
		}

		TemporaryScopedMemoryTrackerData::GetInstance().hasStarted = false;
		TemporaryScopedMemoryTrackerData::GetInstance().allocationMap.clear();
		TemporaryScopedMemoryTrackerData::GetInstance().stackTraceToAllocationCount.clear();
		TemporaryScopedMemoryTrackerData::GetInstance().totalAllocationCount = 0;

		std::lock_guard<std::mutex> guard(myStaticAllocationMapMutex);

		_CrtSetAllocHook(nullptr);

		if (myStaticAllocationMap.size() == 0)
		{
			if (myStaticMemoryTrackingSettings.shouldTrackAllAllocations)
			{
				OutputDebugStringA("================================================================================\n");
				char buffer[100];
				sprintf_s(buffer, "== Total Allocation Count: %d\n", myStaticTotalAllocationCount);
				OutputDebugStringA(buffer);
			}

			OutputDebugStringA("================================================================================\n");
			OutputDebugStringA("== No memory leaks found! \n");
			OutputDebugStringA("================================================================================\n");
		}
		else
		{
			if (myStaticMemoryTrackingSettings.shouldStoreStackTraces)
			{
				OutputDebugStringA("================================================================================\n");
				std::unordered_map<StackTrace, int> stackTraceToLeakCountMap;

				for (const auto& p : myStaticAllocationMap)
				{
					stackTraceToLeakCountMap[p.second.stackTrace]++;
				}

				OutputDebugStringA("== Top 5 Leaks: \n");
				PrintTopLeaks(stackTraceToLeakCountMap, false);
			}

			if (myStaticMemoryTrackingSettings.shouldTrackAllAllocations)
			{
				OutputDebugStringA("================================================================================\n");
				char buffer[100];
				sprintf_s(buffer, "== Total Allocation Count: %d\n", myStaticTotalAllocationCount);
				OutputDebugStringA(buffer);
			}

			OutputDebugStringA("================================================================================\n");
			char buffer[100];
			sprintf_s(buffer, "== Total Number of Memory Leaks: %d\n", (int)myStaticAllocationMap.size());
			OutputDebugStringA(buffer);
			OutputDebugStringA("================================================================================\n");
			OutputDebugStringA("== For more details modify SimpleTracker::MemoryTrackingSettings\n");
			OutputDebugStringA("== SimpleTracker::MemoryTrackingSettings({ StoreStackTraces(Very slow, gives precise info), TrackAllAllocations(decently fast, lots of spam) });\n");
			OutputDebugStringA("================================================================================\n");
		}

		myStaticAllocationMap.clear();
		myStaticStackTraceToAllocationCount.clear();
	}

	void SimpleMemoryTracker::StartMemoryTracking(const bool aShowAdvanced, const std::string& aCallerName)
	{
		if (SimpleMemoryTrackerWrapper::myShouldActive == false)
		{
			return;
		}

		TemporaryScopedMemoryTrackerData::GetInstance();

		//if (myScopedTrackerData == nullptr)
		//{
		//	myScopedTrackerData = new TemporaryScopedMemoryTrackerData();
		//}
		//else
		//{
		//	return; //TO-DO(v10.0.0): Better error and warning messages
		//}

		sprintf_s(localBuffer, std::string("== " + aCallerName).c_str());

		TemporaryScopedMemoryTrackerData::GetInstance().hasStarted = true;
		TemporaryScopedMemoryTrackerData::GetInstance().memoryTrackingSettings = { aShowAdvanced, true };
	}

	void SimpleMemoryTracker::StopMemoryTracking()
	{
		if (SimpleMemoryTrackerWrapper::myShouldActive == false)
		{
			return;
		}

		if (TemporaryScopedMemoryTrackerData::GetInstance().hasStarted == false)
		{
			return;
		}

		PrintShortLivedToOutput();

		TemporaryScopedMemoryTrackerData::GetInstance().hasStarted = false;
	}


	void SimpleMemoryTracker::PrintShortLivedToOutput()
	{
		myStaticIsAllocationInProgress = true;

		std::lock_guard<std::mutex> guard(myStaticAllocationMapMutex);

		if (TemporaryScopedMemoryTrackerData::GetInstance().allocationMap.size() == 0)
		{
			OutputDebugStringA("\n================================================================================\n");
			OutputDebugStringA(localBuffer);
			char buffer[100];
			sprintf_s(buffer, "\n== Total Allocation Count: %d\n", TemporaryScopedMemoryTrackerData::GetInstance().totalAllocationCount);
			OutputDebugStringA(buffer);
			OutputDebugStringA("== No memory leaks found! \n");
			OutputDebugStringA("================================================================================\n");
		}
		else
		{
			if (TemporaryScopedMemoryTrackerData::GetInstance().memoryTrackingSettings.shouldStoreStackTraces)
			{
				OutputDebugStringA("\n================================================================================\n");
				std::unordered_map<StackTrace, int> stackTraceToLeakCountMap;

				for (const auto& p : TemporaryScopedMemoryTrackerData::GetInstance().allocationMap)
				{
					stackTraceToLeakCountMap[p.second.stackTrace]++;
				}

				OutputDebugStringA("== Top 5 Leaks: \n");
				PrintTopLeaks(stackTraceToLeakCountMap, true);
			}

			OutputDebugStringA(localBuffer);

			if (TemporaryScopedMemoryTrackerData::GetInstance().memoryTrackingSettings.shouldTrackAllAllocations)
			{
				OutputDebugStringA("\n");
				char buffer[100];
				sprintf_s(buffer, "== Total Allocation Count: %d\n", TemporaryScopedMemoryTrackerData::GetInstance().totalAllocationCount);
				OutputDebugStringA(buffer);
			}

			char buffer[100];
			sprintf_s(buffer, "== Number of Memory Leaks: %d\n", (int)TemporaryScopedMemoryTrackerData::GetInstance().allocationMap.size());
			OutputDebugStringA(buffer);
			OutputDebugStringA("================================================================================\n\n");
		}

		TemporaryScopedMemoryTrackerData::GetInstance().allocationMap.clear();
		TemporaryScopedMemoryTrackerData::GetInstance().stackTraceToAllocationCount.clear();
		TemporaryScopedMemoryTrackerData::GetInstance().totalAllocationCount = 0;

		myStaticIsAllocationInProgress = false;
	}

	void SimpleMemoryTracker::WriteToTxtFile(const std::vector<std::pair<StackTrace, int>>& aStackTraces, const bool aScopedTracking)
	{
		(void)aStackTraces;
		(void)aScopedTracking;
		//std::string filePath = SimpleUtilities::GetAbsolutePath(SIMPLE_FILENAME_STACKTRACES);
		//const size_t dotPosition = filePath.find_last_of('.');
		//const std::string extension = filePath.substr(dotPosition);

		//if (aScopedTracking)
		//{
		//	//std::string test(localBuffer); test; //TO-DO(v11.2.1): find a way to get pretty name for function name
		//	filePath = filePath.substr(0, dotPosition) + "_scoped";
		//}
		//else
		//{
		//	filePath = filePath.substr(0, dotPosition) + "_full";
		//}

		//filePath += extension;

		//const std::string fileNameWithCounter = SimpleUtilities::AppendCounterIfAlreadyExist(filePath);

		//std::ofstream writeFile(fileNameWithCounter);
		//assert(writeFile.is_open() && "Failed to open the file");

		//writeFile << "================================================================================" << "\n";

		//for (const auto& traces : aStackTraces)
		//{
		//	char buffer[100];
		//	sprintf_s(buffer, "Count: %d\n, Stack Trace:", traces.second);
		//	writeFile << buffer << "\n";

		//	const std::vector<const char*> lines = traces.first.GetLines(); //NOTE(v11.2.0): Guaranteed to not be nullptr

		//	for (const auto& line : lines)
		//	{
		//		writeFile << line << "\n";
		//	}

		//	writeFile << "================================================================================" << "\n";
		//}

		//writeFile.close();
	}

#else 
	void SimpleMemoryTracker::Init() {};
	void SimpleMemoryTracker::Destory() {};
	void SimpleMemoryTracker::StartMemoryTracking(const bool /*aShowAdvanced*/, const std::string& /*aCallerName*/) {}
	void SimpleMemoryTracker::StopMemoryTracking() {}
#endif 
	}