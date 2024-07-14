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

		if (const ImGuiPayload* currentPayload = ImGui::GetDragDropPayload())
		{
			const std::string payloadData = reinterpret_cast<const char*>(currentPayload->Data);
			const std::string extension = SimpleUtilities::FileManager::GetFileExtension(payloadData);

			if (extension == ".fbx")
			{
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Browser"))
					{
						aMesh = Global::GetModelFactory()->LoadMesh(payloadData);
					}
					ImGui::EndDragDropTarget();
				}
			}
		}

		return true;
	}

	bool ViewAndEditValue(const Graphics::Shader*& aShader, const std::string& /*aVariableName*/)
	{
		bool isValid = false;
		bool changed = false;

		static int selectedPixelShader = 0;
		static int selectedVertexShader = 0;

		std::string pixelShader;
		std::string vertexShader;
		std::string pixelShaderNames;
		std::string vertexShaderNames;

		std::vector<std::string> shaderFileNames = SimpleUtilities::FileManager::GetFileNamesFromDirectory(SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_SHADERS));
		std::vector<std::string> pixelShaderFileNames = SimpleUtilities::ReturnOnlyStringContaining("PS", shaderFileNames);
		std::vector<std::string> vertexShaderFileNames = SimpleUtilities::ReturnOnlyStringContaining("VS", shaderFileNames);

		if (aShader != nullptr)
		{
			pixelShader += aShader->GetPixelShaderName();
			vertexShader += aShader->GetVertexShaderName();
			isValid = true;
		}

		for (size_t i = 0; i < pixelShaderFileNames.size(); ++i)
		{
			if (pixelShader == pixelShaderFileNames[i])
			{
				selectedPixelShader = static_cast<int>(i);
				break;
			}
		}

		for (size_t i = 0; i < vertexShaderFileNames.size(); ++i)
		{
			if (vertexShader == vertexShaderFileNames[i])
			{
				selectedVertexShader = static_cast<int>(i);
				break;
			}
		}

		for (const std::string& name : pixelShaderFileNames)
		{
			pixelShaderNames += name;
			pixelShaderNames += '\0';
		}

		for (const std::string& name : vertexShaderFileNames)
		{
			vertexShaderNames += name;
			vertexShaderNames += '\0';
		}

		pixelShaderNames += '\0';
		vertexShaderNames += '\0';

		SimpleUtilities::AppendStringsInFront("Shaders\\", shaderFileNames);

		ImGui::AlignTextToFramePadding();

		ImGui::Text("PixelShader:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(200);

		if (ImGui::Combo("##PixelShaderECSEditorFunction", &selectedPixelShader, pixelShaderNames.c_str()))
		{
			changed = true;
			pixelShader = pixelShaderFileNames[selectedPixelShader];
		}

		ImGui::Text("VertexShader:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(200);

		if (ImGui::Combo("##VertexShaderECSEditorFunction", &selectedVertexShader, vertexShaderNames.c_str()))
		{
			changed = true;
			vertexShader = vertexShaderFileNames[selectedVertexShader];
		}

		if (changed == true)
		{
			SimpleUtilities::AppendStringInFront("Shaders\\", pixelShader);
			SimpleUtilities::AppendStringInFront("Shaders\\", vertexShader);

			const Graphics::Shader* shader = Global::GetGraphicsEngine()->GetShader(pixelShader.c_str(), vertexShader.c_str()).get();

			if (shader != nullptr)
			{
				aShader = shader;
			}
		}

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

		if (const ImGuiPayload* currentPayload = ImGui::GetDragDropPayload())
		{
			const std::string payloadData = reinterpret_cast<const char*>(currentPayload->Data);
			const std::string extension = SimpleUtilities::FileManager::GetFileExtension(payloadData);

			if (extension == ".dds")
			{
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Browser"))
					{
						const std::string fileName = SimpleUtilities::ConvertAbsolutePathToRelativePath(payloadData);
						aTexture = Global::GetGraphicsEngine()->GetTexture(fileName.c_str()).get();
					}
					ImGui::EndDragDropTarget();
				}
			}
		}

		return isValid;
	}

	bool ViewAndEditValue(const Graphics::Skeleton*& aSkeleton, const std::string& /*aVariableName*/)
	{
		std::string name;

		if (aSkeleton != nullptr)
		{
			name = aSkeleton->myName;
		}

		ImGui::AlignTextToFramePadding();

		ImGui::Text("Skeleton:");
		ImGui::SameLine();
		ImGui::BeginDisabled();
		ImGui::InputText("", name.data(), name.size());
		ImGui::EndDisabled();

		if (const ImGuiPayload* currentPayload = ImGui::GetDragDropPayload())
		{
			const std::string payloadData = reinterpret_cast<const char*>(currentPayload->Data);
			const std::string extension = SimpleUtilities::FileManager::GetFileExtension(payloadData);

			if (extension == ".fbx")
			{
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Browser"))
					{
						const std::string relativePath = SimpleUtilities::ConvertAbsolutePathToRelativePath(payloadData);
						aSkeleton = Global::GetModelFactory()->LoadSkeleton(relativePath);
					}
					ImGui::EndDragDropTarget();
				}
			}
		}

		return true;
	}

	bool ViewAndEditValue(const Graphics::Animation*& aAnimation, const std::string& /*aVariableName*/)
	{
		std::string name;

		if (aAnimation != nullptr)
		{
			name = SimpleUtilities::FileManager::GetFileName(aAnimation->relativePath);
		}

		ImGui::AlignTextToFramePadding();

		ImGui::Text("Animation:");
		ImGui::SameLine();
		ImGui::BeginDisabled();
		ImGui::InputText("", name.data(), name.size());
		ImGui::EndDisabled();

		if (const ImGuiPayload* currentPayload = ImGui::GetDragDropPayload())
		{
			const std::string payloadData = reinterpret_cast<const char*>(currentPayload->Data);
			const std::string extension = SimpleUtilities::FileManager::GetFileExtension(payloadData);

			if (extension == ".fbx")
			{
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Browser"))
					{
						const std::string relativePath = SimpleUtilities::ConvertAbsolutePathToRelativePath(payloadData);
						aAnimation = Global::GetModelFactory()->LoadAnimationFBX(relativePath);
					}
					ImGui::EndDragDropTarget();
				}
			}
		}

		return true;
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
			std::string textureName;

			if (aTextures[i] != nullptr)
			{
				textureName = aTextures[i]->GetTextureName();
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
			ImGui::InputText("", textureName.data(), textureName.size());
			ImGui::EndDisabled();

			if (const ImGuiPayload* currentPayload = ImGui::GetDragDropPayload())
			{
				const std::string payloadData = reinterpret_cast<const char*>(currentPayload->Data);
				const std::string extension = SimpleUtilities::FileManager::GetFileExtension(payloadData);

				if (extension == ".dds")
				{
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Browser"))
						{
							const std::string fileName = SimpleUtilities::ConvertAbsolutePathToRelativePath(payloadData);
							const std::shared_ptr<const Graphics::Texture> texture = Global::GetGraphicsEngine()->GetTexture(fileName.c_str());

							bool isCubeMap = false;

							ID3D11ShaderResourceView* shaderResourceView = texture.get()->GetShaderResourceView().Get();

							ID3D11Resource* resource = nullptr;
							shaderResourceView->GetResource(&resource);

							if (resource)
							{
								ID3D11Texture2D* texture2D = nullptr;
								if (SUCCEEDED(resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&texture2D)))
								{
									D3D11_TEXTURE2D_DESC desc;
									texture2D->GetDesc(&desc);

									if (desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
									{
										isCubeMap = true;
									}

									texture2D->Release();
								}

								resource->Release();
							}

							if (isCubeMap == false)
							{
								aTextures[i] = texture.get();
							}
						}

						ImGui::EndDragDropTarget();
					}
				}
			}
		}

		return isValid;
	}
}