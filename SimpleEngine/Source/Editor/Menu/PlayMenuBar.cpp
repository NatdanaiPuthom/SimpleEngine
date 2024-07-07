#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Menu/PlayMenuBar.hpp"

namespace Editor
{
	PlayMenuBar::PlayMenuBar()
	{
	}

	PlayMenuBar::~PlayMenuBar()
	{
	}

	void PlayMenuBar::Init()
	{
	}

	void PlayMenuBar::Update()
	{
		if (ImGui::BeginMainMenuBar())
		{
			const float distanceFromStart = ImGui::GetWindowWidth() - ImGui::GetContentRegionAvail().x;

			ImGui::Dummy(ImVec2(-distanceFromStart + ImGui::GetWindowWidth() * 0.5f - 38.0f, 0));

			Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();

			const bool isPlaying = sceneManager.GetIsPlaying();
			if (isPlaying == true)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImColor(1.0f, 0.0f, 0.0f, 1.0f).Value);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(0.6f, 0.0f, 0.0f, 1.0f).Value);
			}

			if (ImGui::Button(ICON_FA_PLAY))
			{
				sceneManager.TogglePlay();
			}

			if (isPlaying == true)
			{
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
			}

			ImGui::BeginDisabled();
			if (ImGui::Button(ICON_FA_STOP))
			{

			}
			ImGui::EndDisabled();

			ImGui::EndMainMenuBar();
		}
	}

	void PlayMenuBar::Draw()
	{
	}
}