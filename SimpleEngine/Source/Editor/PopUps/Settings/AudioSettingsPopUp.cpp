#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/PopUps/Settings/AudioSettingsPopUp.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	AudioSettingsPopUp::AudioSettingsPopUp(const std::string& aName)
		: PopUp(aName)
		, myMusicIsActive(false)
	{
	}

	void AudioSettingsPopUp::Render()
	{
		if (ImGui::Begin(myImGuiName.c_str(), &myIsActive))
		{
			Simpleton::AudioManager& audioManager = MainSingleton::GetAudioManager();

			const std::string musicName = audioManager.GetMainMusicName();

			if (ImGui::Checkbox(std::string("Play Music").append(myImGuiTag).c_str(), &myMusicIsActive))
			{
				if (myMusicIsActive == true)
				{
					audioManager.PlayMusic(musicName);
				}
				else
				{
					audioManager.StopAllMusic();
				}
			}

			char buffer[256];
			memset(buffer, '\0', sizeof(buffer));
			strncpy_s(buffer, musicName.c_str(), sizeof(buffer));
			buffer[sizeof(buffer) - 1] = '\0';

			ImGui::AlignTextToFramePadding();

			ImGui::Text("Music:");
			ImGui::SameLine();
			ImGui::PushItemWidth(200);

			ImGui::BeginDisabled();
			if (ImGui::InputTextWithHint(myImGuiTag.c_str(), "Drag Drop Music File", buffer, sizeof(buffer)))
			{
			}
			ImGui::EndDisabled();

			if (const ImGuiPayload* currentPayload = ImGui::GetDragDropPayload())
			{
				const std::string payloadData = reinterpret_cast<const char*>(currentPayload->Data);
				const std::string extension = SimpleUtilities::FileManager::GetFileExtension(payloadData);

				if (extension == ".mp3")
				{
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Browser"))
						{
							const std::string newMusicName = SimpleUtilities::FileManager::GetFileName(payloadData);
							audioManager.SetMainMusic(newMusicName);
						}
						ImGui::EndDragDropTarget();
					}
				}
			}

			float musicVolume = audioManager.GetMusicVolume();
			if (ImGui::DragFloat("Music Volume##SettingWindow", &musicVolume, 0.01f, 0.0f, 1.0f))
			{
				audioManager.ChangeMusicVolume(musicVolume);
			}
		}

		ImGui::End();
	}
}
