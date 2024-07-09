#pragma once
#include "Engine/Math/Matrix4x4.hpp"
#include <unordered_map>
#include <vector>
#include <string>

namespace Graphics
{
	struct Animation
	{
		struct Frame
		{
			std::unordered_map<std::string, Math::Matrix4x4f> jointNameToModelSpaceMatrix;
		private:
			const char padding[48] = "Never give up on your dreams! Believe!!!!!!!!!\0";
		};

		std::vector<Frame> frames;
		std::string name;

		float duration = 0.0f;
		float framesPerSecond = 0.0f;

		unsigned int length = 0;

	private:
		char padding[40] = "Never give up on your dreams! Believe!\0";
	};
}