#pragma once
#include "Editor/Core/PopUp.hpp"
#include <functional>
#include <memory>

namespace Editor
{
	class EditorCallbacks
	{
	public:
		static std::function<void()> SetPopUpActive(std::shared_ptr<PopUp> aPopUp, bool* aBoolean)
		{
			return [=]() -> void
				{
					aPopUp->SetActive(*aBoolean);
				};
		}
	};
}