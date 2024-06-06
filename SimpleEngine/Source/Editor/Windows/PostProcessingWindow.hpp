#pragma once
#include "Editor/Template/ToolInterface.hpp"

namespace Editor
{
	class PostProcessingWindow final : public Editor::ToolInterface
	{
	public:
		PostProcessingWindow();

		void Init() override;
		void Update() override;
		void Draw() override;
	};
}