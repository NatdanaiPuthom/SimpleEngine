#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <string_view>
#include <array>

#include <External/TheGameAssembly/StackWalker/StackWalker.h>
#include <External/TheGameAssembly/StackTrace/StackTrace.h>

using namespace SimpleTracker;

constexpr std::size_t STACK_TRACE_MAX_LINES = 32;
constexpr std::size_t STACK_TRACE_MAX_LINE_LENGTH = 1024;

namespace SimpleTracker
{
	struct StackTraceImpl final
	{
		std::array<const char*, STACK_TRACE_MAX_LINES> myLines = {};

		bool operator==(const StackTraceImpl& aStackTraceImpl) const noexcept
		{
			return memcmp(myLines.data(), aStackTraceImpl.myLines.data(), sizeof(const char*) * STACK_TRACE_MAX_LINES) == 0;
		}
	};
}

namespace std
{
	template<> struct hash<SimpleTracker::StackTraceImpl>
	{
		std::size_t operator()(const SimpleTracker::StackTraceImpl& aStackTrace) const noexcept
		{
			size_t result = 0;
			for (auto e : aStackTrace.myLines)
			{
				// Magic function to combine hashes. Should be switched if we add a dependency on a library with hash utility functions
				result ^= std::hash<const char*>{}(e)+0x9e3779b9 + (result << 6) + (result >> 2);
			}
			return result;
		}
	};
}

struct StackTraceLineHash 
{
	size_t operator() (const std::array<char, STACK_TRACE_MAX_LINE_LENGTH>& aLine) const
	{
		return std::hash<std::string_view>()(std::string_view(aLine.data(), std::strlen(aLine.data())));
	}
};

class ToStringStackWalker : public StackWalker
{
public:
	ToStringStackWalker() : StackWalker() 
	{
		LoadModules();
	}
	void SetBuffer(std::array<const char*, STACK_TRACE_MAX_LINES>* aBuffer, const int aSkipCount = 0);
protected:
	virtual void OnOutput(LPCSTR szText);
	virtual void OnDbgHelpErr(LPCSTR, DWORD, DWORD64) {}; // supress error output
private:
	std::array<const char*, STACK_TRACE_MAX_LINES>* myBuffer = 0;
	int myCurrentIndex = 0;
};

static std::unordered_set<std::array<char, STACK_TRACE_MAX_LINE_LENGTH>, StackTraceLineHash> localStackTraceLineCache;
static std::unordered_set<StackTraceImpl> localStackTraceCache;
static ToStringStackWalker localStackWalker;
static std::mutex localStackTraceMutex;

void ToStringStackWalker::SetBuffer(std::array<const char*, STACK_TRACE_MAX_LINES>* aBuffer,const int aSkipCount)
{
	myBuffer = aBuffer;
	myCurrentIndex = -aSkipCount;
	m_MaxRecursionCount = STACK_TRACE_MAX_LINES + aSkipCount;
}

void ToStringStackWalker::OnOutput(LPCSTR szText)
{
	if (!myBuffer)
		return;

	if (myCurrentIndex < 0)
	{
		myCurrentIndex++;
		return;
	}

	if (myCurrentIndex >= STACK_TRACE_MAX_LINES)
		return;

	std::array<char, STACK_TRACE_MAX_LINE_LENGTH> myLine{};
	strncpy_s(myLine.data(), STACK_TRACE_MAX_LINE_LENGTH, szText, STACK_TRACE_MAX_LINE_LENGTH);
	auto pair = localStackTraceLineCache.insert(myLine);
	const char* cachedLine = pair.first->data();

	(*myBuffer)[myCurrentIndex] = cachedLine;
	myCurrentIndex++;
}

StackTrace::StackTrace(const StackTraceImpl& aStackTraceImpl) 
	: myImpl(&aStackTraceImpl)
{
}

SimpleTracker::StackTrace::StackTrace() 
	: myImpl(nullptr)
{
}

StackTrace StackTrace::CaptureStackTrace(int aSkipDepth)
{
	std::lock_guard<std::mutex> guard(localStackTraceMutex);

	StackTraceImpl impl;
	localStackWalker.SetBuffer(&impl.myLines, 2+aSkipDepth);
	localStackWalker.ShowCallstack();
	localStackWalker.SetBuffer(nullptr);

	const auto& it = localStackTraceCache.insert(impl).first;
	return StackTrace(*it);
}

void StackTrace::Print() const
{
	if (myImpl == nullptr)
	{
		OutputDebugStringA("Empty Stack Trace\n");
		return;
	}

	for (const auto& line : myImpl->myLines)
	{
		if (line == nullptr)
			break;

		OutputDebugStringA(line);
	}
}

std::size_t StackTrace::ComputeHash() const
{
	if (myImpl == nullptr)
		return 0;

	return std::hash<StackTraceImpl>()(*myImpl);
}
