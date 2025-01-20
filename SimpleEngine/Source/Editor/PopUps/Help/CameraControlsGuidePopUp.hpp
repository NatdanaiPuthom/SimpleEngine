#pragma once
#include "Editor/Core/PopUp.hpp"

namespace Editor
{
	class CameraControlsGuidePopUp final : public PopUp
	{
	public:
		CameraControlsGuidePopUp(const std::string& aName);
		void Render() override;
	};
}
