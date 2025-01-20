#pragma once
#include "Editor/Core/PopUp.hpp"
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
		void SameLineDummy(float aWidthOffset, float aHeightOffset);
		void AdjustFPSCap(Graphics::GraphicsEngine* aGraphicsEngine);
	private:
		std::vector<std::string> myFPSCapAsString;
		std::vector<const char*> myFPSCapAsConstChar;
		int mySelectedRasterizerState;
	};
}
