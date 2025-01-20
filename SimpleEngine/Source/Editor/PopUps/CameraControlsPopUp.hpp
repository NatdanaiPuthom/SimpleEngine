#pragma once
#include "Editor/Core/PopUp.hpp"

namespace Editor
{
	class CameraControlsPopUp final : public PopUp
	{
	public:
		CameraControlsPopUp(const std::string& aName);
		void Render() override;
	};
}
