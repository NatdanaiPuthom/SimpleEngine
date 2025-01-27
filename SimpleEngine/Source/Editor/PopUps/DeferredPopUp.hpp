#pragma once
#include "Editor/Core/PopUp.hpp"

namespace Editor
{
	class DeferredPopUp final : public PopUp
	{
	public:
		DeferredPopUp(const std::string& aName);
		void Render() override;
	};
}
