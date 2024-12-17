#include "Editor/Precomplied/EditorPch.hpp"
#include "EditorNodeRegistration.hpp"

namespace Editor
{

	Fly::Flow EditorUpdate()
	{
		return Fly::Flow(true);
	}

	FLY_FUNCTION(EditorUpdate, Fly::Directory{ "Events" }, Fly::Event{});

	Fly::Flow ImGuiButton(const std::string& aLabel, Math::Vector2f aSize)
	{
		const std::string label = aLabel.empty() ? "Label" : aLabel;
		const bool wasClicked = ImGui::Button(label.c_str(), ImVec2{ aSize.x, aSize.y });
		return Fly::Flow(wasClicked);
	}

	FLY_FUNCTION(ImGuiButton, Fly::Directory{ "ImGui" }, Fly::InputNames{ "Label", "Size" }, Fly::OutputNames{ "On Click" }, Fly::DefaultValues{ std::string("Label") });


	struct ImGuiCheckboxState final
	{
		bool v = false;
	};

	std::tuple<Fly::Flow, Fly::Flow, Fly::Flow> ImGuiCheckbox(Fly::NodeState<ImGuiCheckboxState> aState, const std::string& aLabel)
	{
		const std::string label = aLabel.empty() ? "Label" : aLabel;
		bool wasClicked = ImGui::Checkbox(label.c_str(), &aState.mValue.v);

		return { Fly::Flow(wasClicked), Fly::Flow(aState.mValue.v), Fly::Flow(!aState.mValue.v) };
	}

	FLY_FUNCTION(ImGuiCheckbox, Fly::Directory{ "ImGui" }, Fly::InputNames{ "Label" }, Fly::OutputNames{ "On Click", "On Checked", "On Unchecked" }, Fly::DefaultValues{ std::string("Label") });

	FLY_FUNCTION(ImGui::SameLine, Fly::InputNames{ "Offset From Start", "Spacing" }, Fly::DefaultValues{ 0.f, -1.f });

	void ImGuiText(const std::string& aString)
	{
		ImGui::Text(aString.c_str());


		/*using StrT = std::decay_t<decltype(aString)>;
		constexpr bool i = Fly::IsProtectedType<StrT>;

		constexpr bool a = Fly::SameAsTemplateType<Fly::NodeState, std::decay_t<StrT>>;
		constexpr bool b = std::same_as<Fly::InternalExecutionContextPtr, std::decay_t<StrT>>;
		constexpr bool c = Fly::SameAsTemplateType<Fly::NodeExecutionContext, std::decay_t<StrT>>;

		using T = decltype(Fly::CreateTypeListFromFunctionWithoutProtectedTypes(ImGuiCheckbox));*/
	}

	FLY_FUNCTION(ImGuiText);

	struct ImGuiRadioButtonState final
	{
		bool isActive = false;
	};

	bool ImGuiRadioButton(Fly::NodeState<ImGuiRadioButtonState> aState, const std::string& aLabel)
	{
		const std::string label = aLabel.empty() ? "Label" : aLabel;
		const bool clicked = ImGui::RadioButton(label.c_str(), aState.mValue.isActive);
		if (clicked)
		{
			aState.mValue.isActive = !aState.mValue.isActive;
		}
		return aState.mValue.isActive;
	}

	FLY_FUNCTION(ImGuiRadioButton, Fly::Directory{ "ImGui" }, Fly::InputNames{ "Label" }, Fly::OutputNames{ "Is Active" }, Fly::DefaultValues{ std::string("Label") });
}