#pragma once
#include "Editor/Template/ToolInterface.hpp"
#include "Engine/Math/Vector2.hpp"
#include <string>
#include <vector>
#include <windef.h>

namespace Graphics
{
	class GraphicsEngine;
}

namespace Editor
{
	class SettingsWindow final : public Editor::ToolInterface
	{
	public:
		SettingsWindow();

		void Init() override;
		void Update() override;
		void Draw() override;

	private:
		void ToggleVSync(Graphics::GraphicsEngine* aGraphicsEngine);
		void AdjustWindowSize();
	private:
		void UpdateAndFetchCurrentMonitorResolution();
	private:
		Math::Vector2ui myMonitorResolution;

		int mySelectedWindowSize;
		bool myConsoleIsOpen;

		std::vector<Math::Vector2ui> myWindowSizes;
	};
}