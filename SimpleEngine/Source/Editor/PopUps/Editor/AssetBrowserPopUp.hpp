#pragma once
#include "Editor/Core/PopUp.hpp"

namespace Editor
{
	class AssetBrowserPopUp2 final : public PopUp
	{
	public:
		AssetBrowserPopUp2(const std::string& aName);

		void Render() override;
	};
}
