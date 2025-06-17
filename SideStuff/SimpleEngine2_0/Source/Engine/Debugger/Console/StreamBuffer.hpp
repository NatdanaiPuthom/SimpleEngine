#pragma once
#include <sstream>

namespace Simple
{
	class CaptureStreamBuffer : public std::streambuf
	{
	public:
		CaptureStreamBuffer() = default;

		std::string GetContent();
	protected:
		int overflow(int aCharacter) override;
	private:
		std::stringstream myBuffer;
	};
}