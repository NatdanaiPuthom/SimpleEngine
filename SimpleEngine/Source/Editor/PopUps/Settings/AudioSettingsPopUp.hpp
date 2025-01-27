#pragma once
#include "Editor/Core/PopUp.hpp"

namespace Editor
{
	class AudioSettingsPopUp final : public PopUp
	{
	public:
		AudioSettingsPopUp(const std::string& aName);

		void Init() override;
		void Render() override;
	private:
		bool myMusicIsActive;
	};
}
