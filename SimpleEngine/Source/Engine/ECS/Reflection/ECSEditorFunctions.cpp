#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Reflection/ECSEditorFunctions.hpp"
#include "Engine/SimpleUtilities/FileManager/FileManager.hpp"
#include "Graphics/Defines.hpp"
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Texture/Texture.hpp"
#include "NodeScript/SimpleScript/Core/Global/ScriptGlobal.h"
#include "NodeScript/SimpleScript/Core/DataType/DataTypeManager.h"
#include "NodeScript/SimpleScript/Core/Script.h"
#include "NodeScript/SimpleScript/Core/Instance/ScriptInstance.h"
#include "NodeScript/SimpleScript/Core/ScriptFoundation.h"
#include "External/imgui.h"

namespace Editor
{
	static bool CustomDragFloat3(const char* aLabel, Math::Vector3f& aVector3)
	{
		bool edited = false;

		const float width = ImGui::GetContentRegionAvail().x / 5.0f;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, (ImVec2(0, 0)));
	
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		const std::string x = "X##" + std::string(aLabel);
		const char* xx = x.c_str();
		ImGui::Button(xx);
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		ImGui::SameLine();
		ImGui::SetNextItemWidth(width);
		const std::string floatX = "##X" + std::string(aLabel);
		const char* floatXX = floatX.c_str();
		if (ImGui::DragFloat(floatXX, &aVector3.x, 0.1f))
		{
			edited = true;
		}

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.60f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.60f, 0.0f, 1.0f));
		const std::string y = "Y##" + std::string(aLabel);
		const char* yy = y.c_str();
		ImGui::Button(yy);
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		ImGui::SameLine();
		ImGui::SetNextItemWidth(width);
		const std::string floatY = "##Y" + std::string(aLabel);
		const char* floatYY = floatY.c_str();
		if (ImGui::DragFloat(floatYY, &aVector3.y, 0.1f))
		{
			edited = true;
		}

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
		const std::string z = "Z##" + std::string(aLabel);
		const char* zz = z.c_str();
		ImGui::Button(zz);
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		ImGui::SameLine();
		ImGui::SetNextItemWidth(width);
		const std::string floatZ = "##Z" + std::string(aLabel);
		const char* floatZZ = floatZ.c_str();
		if (ImGui::DragFloat(floatZZ, &aVector3.z, 0.1f))
		{
			edited = true;
		}
		ImGui::PopStyleVar();

		ImGui::SameLine();
		const std::string label = std::string(aLabel) + "CustomDragFloat3";
		ImGui::PushID(label.c_str());
		ImGui::Text(aLabel);
		ImGui::PopID();

		return edited;
	}
}

namespace ECS
{
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
		if (Editor::CustomDragFloat3("Position", position))
		{
			edited = true;
			aValue.SetPosition(position);
		}

		Math::Vector3f rotation = aValue.GetRotation();
		if (Editor::CustomDragFloat3("Rotation", rotation))
		{
			edited = true;
			aValue.SetRotation(rotation);
		}

		Math::Vector3f scale = aValue.GetScale();
		if (Editor::CustomDragFloat3("Scale", scale))
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

	bool ViewAndEditValue(const Graphics::Mesh*& aMesh, const std::string& /*aVariableName*/)
	{
		std::string mesh;

		if (aMesh != nullptr)
		{
			mesh += aMesh->GetMeshName();
		}

		ImGui::AlignTextToFramePadding();

		ImGui::Text("Mesh:");
		ImGui::SameLine();
		ImGui::BeginDisabled();
		ImGui::InputText("", mesh.data(), mesh.size());
		ImGui::EndDisabled();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Browser"))
			{
				const std::string payloadData = reinterpret_cast<const char*>(payload->Data);
				const std::string extension = SimpleUtilities::FileManager::GetFileExtension(payloadData);

				if (extension == ".fbx")
				{
					aMesh = Global::GetModelFactory()->LoadMesh(payloadData);
				}
			}

			ImGui::EndDragDropTarget();
		}

		return true;
	}

	bool ViewAndEditValue(const Graphics::Shader*& aShader, const std::string& /*aVariableName*/)
	{
		bool isValid = false;

		std::string pixelShader;
		std::string vertexShader;

		if (aShader != nullptr)
		{
			pixelShader += aShader->GetPixelShaderName();
			vertexShader += aShader->GetVertexShaderName();
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

	bool ViewAndEditValue(const Graphics::Texture*& aTexture, const std::string& /*aVariableName*/)
	{
		bool isValid = false;

		std::string texture;

		if (aTexture != nullptr)
		{
			texture = aTexture->GetTextureName();
			isValid = true;
		}

		ImGui::AlignTextToFramePadding();

		ImGui::Text("Texture:");

		ImGui::SameLine();
		ImGui::BeginDisabled();
		ImGui::InputText("", texture.data(), texture.size());
		ImGui::EndDisabled();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Browser"))
			{
				const std::string payloadData = reinterpret_cast<const char*>(payload->Data);
				const std::string extension = SimpleUtilities::FileManager::GetFileExtension(payloadData);

				if (extension == ".dds")
				{
					const std::string fileName = SimpleUtilities::ConvertAbsolutePathToRelativePath(payloadData);
					aTexture = Global::GetGraphicsEngine()->GetTexture(fileName.c_str()).get();
				}
			}

			ImGui::EndDragDropTarget();
		}

		return isValid;
	}

	bool ViewAndEditValue(SCRIPT::ScriptInstance*& aScriptInstance, const std::string& /*aVariableName*/)
	{

		SCRIPT::DataTypeManager& dataTypeManager = SCRIPT::Global::GetDataTypeManager();
		SCRIPT::ScriptFoundation& scriptFoundation = SCRIPT::Global::GetFoundation();

		const auto& scripts = scriptFoundation.GetScripts();

		bool wasChanged = false;

		if (ImGui::BeginCombo("Script", "None"))
		{
			for (auto& [dataTypeID, scriptsByTypeID] : scripts)
			{
				const SCRIPT::DataType* dataType = dataTypeManager.Find(dataTypeID);
				if (dataType == nullptr)
				{
					continue;
				}
				ImGui::Text("%s Scripts:", dataType->name.c_str());
				for (auto& script : scriptsByTypeID)
				{
					bool isSelected = false;
					if (aScriptInstance != nullptr)
					{
						isSelected = aScriptInstance->myScript == script.get();
					}
					if (ImGui::Selectable(script->Name().c_str(), isSelected))
					{
						if (aScriptInstance != nullptr)
						{
							script->DestroyScriptInstance(*aScriptInstance);

						}

						aScriptInstance = &script->CreateScriptInstance();
						
						wasChanged = true;
					}
				}

				ImGui::Separator();


			}


			ImGui::EndCombo();
		}

		return wasChanged;
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
			case Graphics::Global_Slot_Albedo:
				ImGui::Text("Albedo:");
				break;
			case Graphics::Global_Slot_Normal:
				ImGui::Text("Normal:");
				break;
			case Graphics::Global_Slot_Material:
				ImGui::Text("Material:");
				break;
			}

			ImGui::SameLine();
			ImGui::BeginDisabled();
			ImGui::InputText("", texture.data(), texture.size());
			ImGui::EndDisabled();

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Browser"))
				{
					const std::string payloadData = reinterpret_cast<const char*>(payload->Data);
					const std::string extension = SimpleUtilities::FileManager::GetFileExtension(payloadData);

					if (extension == ".dds")
					{
						const std::string fileName = SimpleUtilities::ConvertAbsolutePathToRelativePath(payloadData);
						aTextures[i] = Global::GetGraphicsEngine()->GetTexture(fileName.c_str()).get();
					}
				}

				ImGui::EndDragDropTarget();
			}
		}

		return isValid;
	}
}