#include <External/TheGameAssembly/StackTrace/StackTrace.h>

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
	auto pair = ToStrongStackWalkerWrapper::localStackTraceLineCache.insert(myLine);
	const char* cachedLine = pair.first->data();

	(*myBuffer)[myCurrentIndex] = cachedLine;
	myCurrentIndex++;
}

SimpleTracker::StackTrace::StackTrace(const StackTraceImpl& aStackTraceImpl) 
	: myImpl(&aStackTraceImpl)
{
}

SimpleTracker::StackTrace::StackTrace() 
	: myImpl(nullptr)
{
}

SimpleTracker::StackTrace SimpleTracker::StackTrace::CaptureStackTrace(int aSkipDepth)
{
	std::lock_guard<std::mutex> guard(ToStrongStackWalkerWrapper::localStackTraceMutex);

	StackTraceImpl impl;
	ToStrongStackWalkerWrapper::localStackWalker.SetBuffer(&impl.myLines, 2+aSkipDepth);
	ToStrongStackWalkerWrapper::localStackWalker.ShowCallstack();
	ToStrongStackWalkerWrapper::localStackWalker.SetBuffer(nullptr);

	const auto& it = ToStrongStackWalkerWrapper::localStackTraceCache.insert(impl).first;
	return StackTrace(*it);
}

const std::vector<const char*> SimpleTracker::StackTrace::GetLines() const
{
	std::vector<const char*> lines;

	if (myImpl == nullptr)
	{
		return lines;
	}

	for (const auto& line : myImpl->myLines)
	{
		if (line == nullptr)
		{
			break;
		}

		lines.push_back(line);
	}

	return lines;
}

void SimpleTracker::StackTrace::Print() const
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

std::size_t SimpleTracker::StackTrace::ComputeHash() const
{
	if (myImpl == nullptr)
		return 0;

	return std::hash<StackTraceImpl>()(*myImpl);
}
