#pragma once
#include "External/imgui.h"
#include <string>

namespace Editor
{
	class Window
	{
	public:
		Window(const std::string& aName, const std::string& aTag);

		virtual ~Window() = default;
		virtual void Render() = 0;

	public:
		void SetActive(bool aIsActive);
		void SetFocused(bool aIsFocused);
		void SetWindowName(const std::string& aNewName);
	public:
		bool IsActive() const;
		bool IsFocused() const;
	protected:
		std::string myName;
		std::string myImGuiTag;
		std::string myImGuiName;
		bool myIsActive;
		bool myIsFocus;
	};
}
