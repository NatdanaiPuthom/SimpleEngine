#pragma once
#include "Editor/Core/MenuItem.hpp"
#include <functional>
#include <vector>

namespace Editor
{
	class MenuItemPopUp final : public MenuItem
	{
	public:
		bool myTestBool = false;
	public:
		MenuItemPopUp(const char* aName, std::function<void()> aCallback = nullptr);

		void Render() override final;
		void Invoke();

	public:
		void SetCallback(std::function<void()> aCallback);
	private:
		std::vector< std::function<void()>> myCallback;
	};
}
