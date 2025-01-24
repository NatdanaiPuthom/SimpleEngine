#pragma once
#include "Editor/Core/Base/MenuItemBase.hpp"
#include <memory>
#include <vector>

namespace Editor
{
	class MenuItemMenu final : public MenuItemBase
	{
	public:
		MenuItemMenu(const char* aName);

		void Render() override final;

	public:
		template<DerivedFromMenuItem T>
		T* AddChild(std::unique_ptr<T> aChild);
	private:
		std::vector<std::unique_ptr<MenuItemBase>> myItems;
	};

	template<DerivedFromMenuItem T>
	inline T* MenuItemMenu::AddChild(std::unique_ptr<T> aChild)
	{
		T* ptr = aChild.get();
		myItems.push_back(std::move(aChild));
		return ptr;
	}
}
