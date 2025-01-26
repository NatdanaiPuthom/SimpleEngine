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
		T* AddChild(const char* aName);
	private:
		std::vector<std::unique_ptr<MenuItemBase>> myItems;
	};

	template<DerivedFromMenuItem T>
	inline T* MenuItemMenu::AddChild(const char* aName)
	{
		std::unique_ptr<T> child = std::make_unique<T>(aName);

		T* ptr = child.get();
		ptr->SetImGuiTag(std::string("##" + myName).c_str());

		myItems.push_back(std::move(child));
		return ptr;
	}
}
