#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/ECSEditorFunctions.hpp"
#include "External/imgui.h"

static std::string ExtractStringFromImGuiIDFullName(const std::string& aString)
{
	const size_t position = aString.find("##");

	std::string variableNameWithoutImGuiID;

	if (position != std::string::npos)
	{
		return aString.substr(0, position);
	}

	return aString;
}

bool ViewAndEditValue(char& aValue, const std::string& aVariableName)
{
	const std::string constCharToString(1, aValue);

	std::string variableNameWithoutImGuiID = ExtractStringFromImGuiIDFullName(aVariableName);
	variableNameWithoutImGuiID += ": " + constCharToString;

	ImGui::Text(variableNameWithoutImGuiID.c_str());

	return true;
}

bool ViewAndEditValue(const std::string& aValue, const std::string& aVariableName)
{
	std::string variableNameWithoutImGuiID = ExtractStringFromImGuiIDFullName(aVariableName);
	variableNameWithoutImGuiID += ": " + aValue;

	ImGui::Text(variableNameWithoutImGuiID.c_str());

	return true;
}

bool ViewAndEditValue(int& aValue, const std::string& aVariableName)
{
	return ImGui::DragInt(aVariableName.c_str(), &aValue);
}

bool ViewAndEditValue(int*& aValue, const std::string& aVariableName)
{
	if (aValue == nullptr)
	{
		//assert(false && "Value is nullptr");

		int nullValue = INT_MIN;
		ImGui::DragInt(aVariableName.c_str(), &nullValue);

		return false;
	}

	return ImGui::DragInt(aVariableName.c_str(), aValue);
}

bool ViewAndEditValue(float& aValue, const std::string& aVariableName)
{
	return ImGui::DragFloat(aVariableName.c_str(), &aValue);
}

bool ViewAndEditValue(bool& aValue, const std::string& aVariableName)
{
	return ImGui::Checkbox(aVariableName.c_str(), &aValue);
}

bool ViewAndEditValue(Math::Vector2f& aValue, const std::string& aVariableName)
{
	return ImGui::DragFloat2(aVariableName.c_str(), &aValue.x);
}

bool ViewAndEditValue(Math::Vector3f& aValue, const std::string& aVariableName)
{
	return ImGui::DragFloat3(aVariableName.c_str(), &aValue.x);
}

bool ViewAndEditValue(Math::Vector4f& aValue, const std::string& aVariableName)
{
	return ImGui::DragFloat4(aVariableName.c_str(), &aValue.x);
}

bool ViewAndEditValue(Math::Transform& aValue, const std::string& /*aVariableName*/)
{
	bool edited = false;

	Math::Vector3f position = aValue.GetPosition();
	if (ImGui::DragFloat3("Position##Transform", &position.x, 0.1f))
	{
		edited = true;
		aValue.SetPosition(position);
	}

	Math::Vector3f rotation = aValue.GetRotation();
	if (ImGui::DragFloat3("Rotation##Transform", &rotation.x, 0.01f))
	{
		edited = true;
		aValue.SetRotation(rotation);
	}

	Math::Vector3f scale = aValue.GetScale();
	if (ImGui::DragFloat3("Scale##Transform", &scale.x, 0.01f, 0.001f))
	{
		if (scale.x < 0.001f)
		{
			scale.x = 0.001f;
		}

		if (scale.y < 0.001f)
		{
			scale.y = 0.001f;
		}

		if (scale.z < 0.001f)
		{
			scale.z = 0.001f;
		}

		edited = true;
		aValue.SetScale(scale);
	}

	return edited;
}

bool ViewAndEditValue(const Graphics::Mesh*& aValue, const std::string& /*aVariableName*/)
{
	std::string mesh;

	if (aValue != nullptr)
	{
		mesh += aValue->GetMeshName();
	}

	ImGui::AlignTextToFramePadding();

	ImGui::Text("Mesh:");
	ImGui::SameLine();
	ImGui::BeginDisabled();
	ImGui::InputText("", mesh.data(), mesh.size());
	ImGui::EndDisabled();

	return true;
}

bool ViewAndEditValue(const Graphics::Shader*& aValue, const std::string& /*aVariableName*/)
{
	bool isValid = false;

	std::string pixelShader;
	std::string vertexShader;

	if (aValue != nullptr)
	{
		pixelShader += aValue->GetPixelShaderName();
		vertexShader += aValue->GetVertexShaderName();
		isValid = true;
	}

	ImGui::AlignTextToFramePadding();

	ImGui::Text("PixelShader:");
	ImGui::SameLine();
	ImGui::BeginDisabled();
	ImGui::InputText("", pixelShader.data(), pixelShader.size());
	ImGui::EndDisabled();

	ImGui::Text("VertexShader:");
	ImGui::SameLine();
	ImGui::BeginDisabled();
	ImGui::InputText("", vertexShader.data(), vertexShader.size());
	ImGui::EndDisabled();

	return isValid;
}

bool CustomViewAndEditValue(std::array<const Graphics::Texture*, 3>& aTextures, const std::string& /*aVariableName*/)
{
	bool isValid = false;

	ImGui::AlignTextToFramePadding();

	if (aTextures[0] != nullptr)
	{
		isValid = true;
	}

	for (size_t i = 0; i < aTextures.size(); ++i)
	{
		std::string texture;

		if (aTextures[i] != nullptr)
		{
			texture = aTextures[i]->GetTextureName();
		}

		switch (i)
		{
		case 0:
			ImGui::Text("Albedo:");
			break;
		case 1:
			ImGui::Text("Normal:");
			break;
		case 2:
			ImGui::Text("Material:");
			break;
		}

		ImGui::SameLine();
		ImGui::BeginDisabled();
		ImGui::InputText("", texture.data(), texture.size());
		ImGui::EndDisabled();
	}

	return isValid;
}