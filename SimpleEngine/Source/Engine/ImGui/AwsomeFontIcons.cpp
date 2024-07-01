#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ImGui/AwsomeFontIcons.hpp"
#include "External/imgui.h"
#include "External/AwsomeFontIcons/IconFontDefines.h"

namespace Simple
{
	void AwsomeFontIcons::ShowIcons()
	{
		if (ImGui::Begin("FAIcons##DemoFaIcons"))
		{
			ImGui::Button(ICON_FA_AD);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ADDRESS_BOOK);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ADDRESS_CARD);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ADJUST);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_AIR_FRESHENER);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ALIGN_CENTER);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ALIGN_JUSTIFY);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ALIGN_LEFT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ALIGN_RIGHT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ALLERGIES);

			ImGui::Button(ICON_FA_AMBULANCE);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_AMERICAN_SIGN_LANGUAGE_INTERPRETING);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANCHOR);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANGLE_DOUBLE_DOWN);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANGLE_DOUBLE_LEFT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANGLE_DOUBLE_RIGHT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANGLE_DOUBLE_UP);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANGLE_DOWN);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANGLE_LEFT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANGLE_RIGHT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANGLE_UP);

			ImGui::Button(ICON_FA_ANGRY);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ANKH);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_APPLE_ALT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARCHIVE);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARCHWAY);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_ALT_CIRCLE_DOWN);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_ALT_CIRCLE_LEFT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_ALT_CIRCLE_RIGHT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_ALT_CIRCLE_UP);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_CIRCLE_DOWN);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_CIRCLE_LEFT);

			ImGui::Button(ICON_FA_ARROW_CIRCLE_RIGHT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_CIRCLE_UP);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_DOWN);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_LEFT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_RIGHT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROW_UP);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROWS_ALT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROWS_ALT_H);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ARROWS_ALT_V);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ASSISTIVE_LISTENING_SYSTEMS);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ASTERISK);

			ImGui::Button(ICON_FA_AT);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ATLAS);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_ATOM);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_AUDIO_DESCRIPTION);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_AWARD);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_BABY);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_BABY_CARRIAGE);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_BACKSPACE);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_BACKWARD);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_BACON);
			ImGui::SameLine();
			ImGui::Button(ICON_FA_BACTERIA);

			//NOTE(v11.2.4): Fuck this, I will try to automate this one day but not today
		}
		ImGui::End();
	}
}
