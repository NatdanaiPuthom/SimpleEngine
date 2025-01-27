#pragma once
#include <string>

namespace Editor
{
	class MenuItemBase;
}

namespace Editor
{
	template <typename T>
	concept DerivedFromMenuItem = std::is_base_of_v<MenuItemBase, T>&& std::is_class_v<T>;

	class MenuItemBase
	{
	public:
		MenuItemBase(const char* aName, const char* aImGuiTag = "##DefaultMenuItemBase")
			: myName(aName) 
			, myImGuiTag(aImGuiTag)
		{
			myImGuiName = myName + myImGuiTag;
		}

		virtual ~MenuItemBase() = default;
		virtual void Render() = 0;

	public:
		void SetImGuiTag(const char* aImGuiTag) 
		{
			myImGuiTag = aImGuiTag;
			myImGuiName = myName + myImGuiTag;
		}
	public:
		const std::string& GetName() const { return myName; }
	protected:
		std::string myName;
		std::string myImGuiTag;
		std::string myImGuiName;
	};
}
