#pragma once
#include <string>

namespace Editor
{
	class MainMenuTabBase
	{
	public:
		MainMenuTabBase(const char* aName, const char* aImGuiTag)
			: myName(aName) 
			, myImGuiTag(aImGuiTag)
		{
			myImGuiName = myName + "##" + myImGuiTag;
		}

		virtual ~MainMenuTabBase() = default;
		virtual void Init() {};
		virtual void Render() = 0;

	protected:
		std::string myName;
		std::string myImGuiTag;
		std::string myImGuiName;
	};
}

namespace Editor
{
	template <typename T>
	concept DerivedFromMainMenuTabBase = std::is_base_of_v<MainMenuTabBase, T>&& std::is_class_v<T>;
}