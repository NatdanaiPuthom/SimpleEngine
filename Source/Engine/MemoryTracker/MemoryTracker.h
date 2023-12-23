#pragma once

namespace SimpleTracker
{
	struct MemoryTrackingSettings final
	{
		bool myShouldStoreStackTraces;
		bool myShouldTrackAllAllocations;
	};

	void StartMemoryTracking(const MemoryTrackingSettings& aTrackingSettings);
	void StopMemoryTrackingAndPrint();
}