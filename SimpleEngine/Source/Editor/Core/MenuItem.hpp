#pragma once
#include "External/imgui.h"
#include <string>
#include <functional>
#include <vector>
#include <memory>

namespace Editor
{
	class MenuItem;
}

namespace Editor
{
	template <typename T>
	concept DerivedFromMenuItem = std::is_base_of_v<MenuItem, T>&& std::is_class_v<T>;

	class MenuItem
	{
	public:
		MenuItem(const char* aName) : myName(aName) {}

		virtual ~MenuItem() = default;
		virtual void Render() = 0;

		const std::string& GetName() const { return myName; }
	protected:
		std::string myName;
	};


	class MenuItemButton final : public MenuItem
	{
	public:
		MenuItemButton(const char* aName, std::function<void()> aCallback = nullptr) : MenuItem(aName)
			, myCallback(std::move(aCallback))
		{
		}

		void SetCallback(std::function<void()> aCallback)
		{
			myCallback = aCallback;
		}

		void Render() override final
		{
			if (ImGui::MenuItem(myName.c_str()) && myCallback)
			{
				myCallback();
			}
		}

	private:
		std::function<void()> myCallback;
	};

	class MenuItemPopUp final : public MenuItem
	{
	public:
		MenuItemPopUp(const char* aName, std::function<void()> aCallback = nullptr) : MenuItem(aName)
		{

		}

		void Render() override final
		{
			if (ImGui::MenuItem(myName.c_str(), nullptr, &myTestBool) && !myCallback.empty() && myCallback.front())
			{
				for (auto& callback : myCallback)
				{
					callback();
				}
			}
		}

		void SetCallback(std::function<void()> aCallback)
		{
			myCallback.push_back(std::move(aCallback));
		}

		void Invoke()
		{
			if (!myCallback.empty() && myCallback.front())
			{
				for (auto& callback : myCallback)
				{
					callback();
				}
			}
		}

		bool myTestBool = false;

	private:
		std::vector< std::function<void()>> myCallback;
	};

	class MenuItemMenu final : public MenuItem
	{
	public:
		MenuItemMenu(const char* aName) : MenuItem(aName)
		{
		}

		template<DerivedFromMenuItem T>
		T* AddChild(std::unique_ptr<T> aChild)
		{
			T* ptr = aChild.get();
			myItems.push_back(std::move(aChild));
			return ptr;
		}

		void Render() override final
		{
			if (ImGui::BeginMenu(myName.c_str()))
			{
				for (const auto& child : myItems)
				{
					child->Render();
				}

				ImGui::EndMenu();
			}
		}

	private:
		std::vector<std::unique_ptr<MenuItem>> myItems;
	};


	class MenuItemSelectable final : public MenuItem
	{
	public:
		MenuItemSelectable(const char* aName, std::function<void(const std::string&)> aCallback) : MenuItem(aName)
			, myCallback(std::move(aCallback))
		{
		}

		void Render() override final
		{
			if (ImGui::BeginMenu(myName.c_str()))
			{
				for (const auto& name : myStrings)
				{
					if (ImGui::Selectable(name.c_str()) && myCallback)
					{
						myCallback(name);
					}
				}

				ImGui::EndMenu();
			}
		}

		void SetStrings(const std::vector<std::string> aNewStrings)
		{
			myStrings = aNewStrings;

		}

	private:
		std::function<void(const std::string&)> myCallback;
		std::vector<std::string> myStrings;
	};

}

namespace Editor
{
	class MainMenuTabBase
	{
	public:
		MainMenuTabBase(const char* aName) : myName(aName) {}

		virtual ~MainMenuTabBase() = default;
		virtual void Init() {};
		virtual void Render() = 0;

	protected:
		std::string myName;
	};

	class MenuTabWindow final : public MainMenuTabBase
	{
	public:
		MenuTabWindow(const char* aName) : MainMenuTabBase(aName) {}

		MenuItemPopUp* AddPopUp(std::unique_ptr<MenuItemPopUp> aButton)
		{
			MenuItemPopUp* buttonPointer = aButton.get();
			myButtons.push_back(std::move(aButton));
			return buttonPointer;
		}

		void Init() override final
		{
		}

		void Render() override final
		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu(myName.c_str()))
				{
					size_t currentButtonIndex = 0;

					for (currentButtonIndex; currentButtonIndex < myButtons.size(); ++currentButtonIndex)
					{
						auto& currentButton = myButtons[currentButtonIndex];

						if (ImGui::MenuItem(currentButton->GetName().c_str(), nullptr, &currentButton->myTestBool))
						{
							currentButton->Invoke();

							for (size_t i = 0; i < myButtons.size(); ++i)
							{
								if (i != currentButtonIndex)
								{
									auto& otherButton = myButtons[i];

									otherButton->myTestBool = false;
									otherButton->Invoke();
								}
							}

							break;
						}
					}

					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();
			}
		}

	private:
		std::vector<std::unique_ptr<MenuItemPopUp>> myButtons;
	};

	class MenuTabNormal final : public MainMenuTabBase
	{
	public:
		MenuTabNormal(const char* aName) : MainMenuTabBase(aName) {}

		MenuItemButton* AddButton(const char* aButtonName, std::function<void()> aCallback)
		{
			std::unique_ptr<MenuItemButton> button = std::make_unique<MenuItemButton>(aButtonName, aCallback);
			MenuItemButton* buttonPointer = button.get();
			myButtons.push_back(std::move(button));
			return buttonPointer;
		}

		MenuItemButton* AddButton(std::unique_ptr<MenuItemButton> aButton)
		{
			MenuItemButton* buttonPointer = aButton.get();
			myButtons.push_back(std::move(aButton));
			return buttonPointer;
		}

		MenuItemPopUp* AddPopUp(std::unique_ptr<MenuItemPopUp> aButton)
		{
			MenuItemPopUp* buttonPointer = aButton.get();
			myButtons.push_back(std::move(aButton));
			return buttonPointer;
		}

		MenuItemMenu* AddMenu(const char* aButtonName)
		{
			std::unique_ptr<MenuItemMenu> button = std::make_unique<MenuItemMenu>(aButtonName);
			MenuItemMenu* buttonPointer = button.get();
			myButtons.push_back(std::move(button));
			return buttonPointer;
		}

		MenuItemSelectable* AddSelectable(const char* aButtonName, std::function<void(const std::string&)> aCallback)
		{
			std::unique_ptr<MenuItemSelectable> button = std::make_unique<MenuItemSelectable>(aButtonName, std::move(aCallback));
			MenuItemSelectable* buttonPointer = button.get();
			myButtons.push_back(std::move(button));
			return buttonPointer;
		}

		void Render() override final
		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu(myName.c_str()))
				{
					for (const auto& button : myButtons)
					{
						button->Render();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();
			}
		}
	private:
		std::vector<std::unique_ptr<MenuItem>> myButtons;
	};

}