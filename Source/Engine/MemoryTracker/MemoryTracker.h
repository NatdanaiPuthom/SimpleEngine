#pragma once

namespace SimpleTracker
{
	//The Game Assembly stuff but modified a little bit

	struct MemoryTrackingSettings final
	{
		bool myShouldStoreStackTraces;
		bool myShouldTrackAllAllocations;
	};

	void StartMemoryTracking(const MemoryTrackingSettings& aTrackingSettings);
	void StopMemoryTrackingAndPrint();
}