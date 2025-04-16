#include "StreamBuffer.hpp"

namespace Simple
{
	std::string CaptureStreamBuffer::GetContent()
	{
		return myBuffer.str();
	}

	int CaptureStreamBuffer::overflow(int aCharacter)
	{
		myBuffer.put(static_cast<char>(aCharacter));
		return aCharacter;
	}
}