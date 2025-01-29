#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/PopUps/Editor/AssetBrowserPopUp.hpp"

namespace Editor
{
	AssetBrowserPopUp2::AssetBrowserPopUp2(const std::string& aName)
		: PopUp(aName)
	{
	}

	void AssetBrowserPopUp2::Render()
	{
		if (ImGui::Begin(myImGuiName.c_str(), &myIsActive))
		{
			
		}

		ImGui::End();
	}
}
