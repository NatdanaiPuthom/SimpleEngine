#pragma once
#include "Editor/Template/ToolInterface.hpp"
#include <string>

namespace Editor
{
	class AssetWindow final : public Editor::ToolInterface
	{
	public:
		AssetWindow();

		void Init() override;
		void Update() override;
		void Draw() override;
	};
}