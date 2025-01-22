#pragma once
#include "Editor/Core/PopUp.hpp"

namespace Editor
{
	class PostProcessPopUp final : public PopUp
	{
	public:
		PostProcessPopUp(const std::string& aName);
		void Render() override;
	};
}
