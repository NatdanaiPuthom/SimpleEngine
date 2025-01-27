#pragma once
#include "Editor/Core/Base/MenuItemBase.hpp"
#include <functional>

namespace Editor
{
	class MenuItemButton final : public MenuItemBase
	{
	public:
		MenuItemButton(const char* aName, std::function<void()> aCallback = nullptr);

		void Render() override final;

	public:
		void SetCallback(std::function<void()> aCallback);
	private:
		std::function<void()> myCallback;
	};
}
