#pragma once
#include "Editor/Core/PopUp.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Graphics/GraphicsDeclarations.hpp"
#include <vector>
#include <string>

namespace Graphics
{
	class GraphicsEngine;
}

namespace Editor
{
	class GraphicsSettingsPopUp final : public PopUp
	{
	public:
		GraphicsSettingsPopUp(const std::string& aName);

		void Init() override;
		void Render() override;
	private:
		void InitRasterizerStatesStrings();
		void UpdateFPSCapSettings(Graphics::GraphicsEngine* aGraphicsEngine);
		void UpdateConsoleSettings();
		void UpdateCursorSettings();
		void UpdateWindowsSizeSettings();
		void UpdateAndFetchCurrentMonitorResolution();
		void UpdateAndFetchFPSCapStrings();
		void UpdateAndFetchCurrentCursorSettings();
	private:
		std::array<const char*, static_cast<int>(Graphics::eRasterizerState::Count)> myRasterizerStatesConstChar;

		std::vector<std::string> myFPSCapAsString;
		std::vector<std::string> myCursorNames;
		std::vector<const char*> myFPSCapAsConstChar;
		std::vector<Math::Vector2ui> myWindowSizes;

		std::string myLongCursorStringName;

		Math::Vector2ui myMonitorResolution;

		int mySelectedWindowSize;
		int mySelectedRasterizerState;
		int mySelectedCursor;
		bool myConsoleIsOpen;
	};
}
