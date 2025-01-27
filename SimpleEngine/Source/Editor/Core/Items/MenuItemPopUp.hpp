#pragma once
#include "Editor/Core/Base/MenuItemBase.hpp"
#include <functional>
#include <vector>

namespace Editor
{
	class MenuItemPopUp final : public MenuItemBase
	{
	public:
		MenuItemPopUp(const char* aName, std::function<void()> aCallback = nullptr);

		void Render() override final;
		void Invoke();

	public:
		void SetCallback(std::function<void()> aCallback);
		void SetIsActive(const bool aShouldActive);
	public:
		bool& GetIsActiveRef();
	private:
		std::vector< std::function<void()>> myCallback;
		bool myIsActive;
	};
}
