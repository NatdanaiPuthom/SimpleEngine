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
		void Draw() override;

	private:
		std::string myCurrentDirectory;
	};
}