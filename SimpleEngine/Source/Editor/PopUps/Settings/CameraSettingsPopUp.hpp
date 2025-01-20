#pragma once
#include "Editor/Core/PopUp.hpp"

namespace Editor
{
	class CameraSettingsPopUp final : public PopUp
	{
	public:
		CameraSettingsPopUp(const std::string& aName);
		void Render() override;
	};
}
