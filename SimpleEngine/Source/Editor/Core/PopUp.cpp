#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/PopUp.hpp"

namespace Editor
{
	PopUp::PopUp(const std::string& aWindowName)
		: Window(aWindowName, typeid(PopUp).name())
	{
	}

	void PopUp::Init()
	{
	}

	void PopUp::Update()
	{
		if (myIsActive)
		{
			if (ImGui::Begin(myImGuiName.c_str(), &myIsActive))
			{
				myIsFocus = ImGui::IsWindowFocused();
			}

			ImGui::End();
		}
	}
}
