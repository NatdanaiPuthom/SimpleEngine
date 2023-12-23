#pragma once
#include <functional>

namespace SimpleTracker
{
	struct StackTraceImpl;

	class StackTrace final
	{
	public:
		StackTrace();

		bool operator==(const StackTrace& aStackTrace) const noexcept
		{
			return myImpl == aStackTrace.myImpl;
		}

		void Print() const;
		std::size_t ComputeHash() const;
		static StackTrace CaptureStackTrace(int aSkipDepth);
	private:
		StackTrace(const StackTraceImpl&);
		const StackTraceImpl* myImpl;
	};
}

namespace std
{
	template<> struct hash<SimpleTracker::StackTrace>
	{
		std::size_t operator()(const SimpleTracker::StackTrace& aStackTrace) const noexcept
		{
			return aStackTrace.ComputeHash();
		}
	};
}
