#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/Base/Window.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"

namespace Editor
{
	Window::Window(const std::string& aName, const std::string& aTag)
		: myName(aName)
		, myIsActive(true)
		, myIsFocus(false)
	{
		myImGuiTag = "##" + SimpleUtilities::ConvertTypeIndexNameToPrettyName(aTag);
		myImGuiName = myName + myImGuiTag;
	}

	void Window::SetActive(bool aIsActive)
	{
		myIsActive = aIsActive;
	}

	void Window::SetFocused(bool aIsFocused)
	{
		myIsFocus = aIsFocused;
	}

	void Window::SetWindowName(const std::string& aNewName)
	{
		myName = aNewName;
		myImGuiName = myName + myImGuiTag;
	}

	bool Window::IsActive() const
	{
		return myIsActive;
	}

	bool Window::IsFocused() const
	{
		return myIsFocus;
	}
}
