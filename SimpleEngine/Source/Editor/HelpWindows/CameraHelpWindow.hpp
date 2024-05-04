#pragma once
#include "Editor/Template/ToolInterface.hpp"

namespace Editor
{
	class CameraHelpWindow final : public Editor::ToolInterface
	{
	public:
		CameraHelpWindow();
		void Draw() override;
	};
}