#pragma once
#include "External/imgui.h"

namespace Simple
{
	class ImGuiEngine final
	{
	public:
		ImGuiEngine();
		~ImGuiEngine();

		void Init();
		void BeginFrame();
		void EndFrame();
		void Save();

	public:
		static void SetSimpleStyle();
		static void SetDarkStyle();
		static void SetLightStyle();
	private:
		void LoadFronts();
		void LoadColors();
		void LoadStyle();
	private:
		static ImGuiStyle myStyle;
		static ImVec4 myColors;
	};
}