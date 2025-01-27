#pragma once
#include "External/imgui.h"

namespace Simple
{
	enum class eImGuiEditorStyle
	{
		Simple,
		Dark,
		Light,
		Count
	};

	enum class eImGuiEditorMode
	{
		Default,
		Playing,
		Count
	};

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
		static void SetEditorStyle(const eImGuiEditorStyle aStyle);
		static void SetEditorMode(const eImGuiEditorMode aMode);
	public:
		static void SameLineDummy(float aWidthOffset, float aHeightOffset);
		static void SeparatorDummy(float aWidthOffset, float aHeightOffset);
		static void SeparatorTextDummy(const char* aText = "", float aWidthOffset = 0.0f, float aHeightOffset = 0.0f);
	private:
		void LoadFronts();
		void LoadColors();
		void LoadStyle();
	private:
		static ImGuiStyle myStyle;
		static ImVec4 myColors[static_cast<size_t>(ImGuiCol_COUNT)];
		static ImVec4 myPlayModeBackgroundColor;
		static ImVec4 myInteractiveColor;
		static const ImVec4 myLegacyPinkColor;
		static const ImVec4 mySoftBlueColor;
		static const ImVec4 myDarkGrayColor;
		static eImGuiEditorStyle myCurrentColorStyle;
	};
}