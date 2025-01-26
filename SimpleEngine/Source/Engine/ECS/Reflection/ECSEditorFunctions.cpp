#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Reflection/ECSEditorFunctions.hpp"
#include "Engine/SimpleUtilities/FileManager/FileManager.hpp"

#include "Graphics/Defines.hpp"
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Texture/Texture.hpp"
#include "Graphics/BufferData.hpp"
#include "Graphics/Camera/Camera.hpp"

#include "NodeScript/Fly.hpp"
#include "NodeScript/Instance/FlyClassInstance.hpp"

#include "External/imgui.h"

namespace Editor
{
	static ECS::ViewAndEditResult CustomDragFloat3(const char* aLabel, Math::Vector3f& aVector3)
	{
		ECS::ViewAndEditResult viewAndEditResult;

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
			viewAndEditResult.myIsEdited = true;
		}
		viewAndEditResult.myIsActive |= ImGui::IsItemActive();

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
			viewAndEditResult.myIsEdited = true;
		}
		viewAndEditResult.myIsActive |= ImGui::IsItemActive();

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
			viewAndEditResult.myIsEdited = true;
		}
		viewAndEditResult.myIsActive |= ImGui::IsItemActive();

		ImGui::PopStyleVar();

		ImGui::SameLine();
		const std::string label = std::string(aLabel) + "CustomDragFloat3";
		ImGui::PushID(label.c_str());
		ImGui::Text(aLabel);
		ImGui::PopID();

		return viewAndEditResult;
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

	ViewAndEditResult ViewAndEditValue(char& aValue, const std::string& aVariableName)
	{
		ViewAndEditResult viewAndEditResult;
		const std::string constCharToString(1, aValue);

		std::string variableNameWithoutImGuiID = ExtractStringFromImGuiIDFullName(aVariableName);
		variableNameWithoutImGuiID += ": " + constCharToString;

		char c[1]{ aValue };
		if (ImGui::InputText(variableNameWithoutImGuiID.c_str(), c, 1))
		{
			aValue = c[0];
			viewAndEditResult.myIsEdited;
		}

		return viewAndEditResult;
	}

	ViewAndEditResult ViewAndEditValue(std::string& aValue, const std::string& aVariableName)
	{
		ViewAndEditResult viewAndEditResult;
		char buffer[256]{};
		memset(buffer, '\0', sizeof(buffer));
		strncpy_s(buffer, aValue.c_str(), sizeof(buffer));
		buffer[sizeof(buffer) - 1] = '\0';

		if (ImGui::InputTextWithHint(aVariableName.c_str(), "Text", buffer, sizeof(buffer)))
		{
			aValue = std::string(buffer);
			viewAndEditResult.myIsEdited = true;
		}

		viewAndEditResult.myIsActive = ImGui::IsItemActive();

		return viewAndEditResult;
	}

	ViewAndEditResult ViewAndEditValue(int& aValue, const std::string& aVariableName)
	{
		ViewAndEditResult viewAndEditResult;
		viewAndEditResult.myIsEdited = ImGui::DragInt(aVariableName.c_str(), &aValue);
		viewAndEditResult.myIsActive = ImGui::IsItemActive();
		return viewAndEditResult;
	}

	ViewAndEditResult ViewAndEditValue(int*& aValue, const std::string& aVariableName)
	{
		ViewAndEditResult viewAndEditResult;
		if (aValue == nullptr)
		{
			int nullValue = INT_MIN;
			ImGui::DragInt(aVariableName.c_str(), &nullValue);

			return viewAndEditResult;
		}

		viewAndEditResult.myIsEdited |= ImGui::DragInt(aVariableName.c_str(), aValue);
		viewAndEditResult.myIsActive = ImGui::IsItemActive();
		return viewAndEditResult;
	}

	ViewAndEditResult ViewAndEditValue(float& aValue, const std::string& aVariableName)
	{
		ViewAndEditResult viewAndEditResult;
		viewAndEditResult.myIsEdited = ImGui::DragFloat(aVariableName.c_str(), &aValue);
		viewAndEditResult.myIsActive = ImGui::IsItemActive();
		return viewAndEditResult;
	}

	ViewAndEditResult ViewAndEditValue(bool& aValue, const std::string& aVariableName)
	{
		ViewAndEditResult viewAndEditResult;
		viewAndEditResult.myIsEdited = ImGui::Checkbox(aVariableName.c_str(), &aValue);
		viewAndEditResult.myIsActive = ImGui::IsItemActive();
		return viewAndEditResult;
	}

	ViewAndEditResult ViewAndEditValue(Math::Vector2f& aValue, const std::string& aVariableName)
	{
		ViewAndEditResult viewAndEditResult;
		viewAndEditResult.myIsEdited = ImGui::DragFloat2(aVariableName.c_str(), &aValue.x);
		viewAndEditResult.myIsActive = ImGui::IsItemActive();
		return viewAndEditResult;
	}

	ViewAndEditResult ViewAndEditValue(Math::Vector3f& aValue, const std::string& aVariableName)
	{
		ViewAndEditResult viewAndEditResult;
		viewAndEditResult.myIsEdited = ImGui::DragFloat3(aVariableName.c_str(), &aValue.x);
		viewAndEditResult.myIsActive = ImGui::IsItemActive();
		return viewAndEditResult;
	}

	ViewAndEditResult ViewAndEditValue(Math::Vector4f& aValue, const std::string& aVariableName)
	{
		ViewAndEditResult viewAndEditResult;
		viewAndEditResult.myIsEdited = ImGui::DragFloat4(aVariableName.c_str(), &aValue.x);
		viewAndEditResult.myIsActive = ImGui::IsItemActive();
		return viewAndEditResult;
	}

	ViewAndEditResult ViewAndEditValue(Math::Transform& aValue, const std::string& /*aVariableName*/)
	{
		ViewAndEditResult viewAndEditResult;

		{
			Math::Vector3f position = aValue.GetPosition();
			const ViewAndEditResult viewAndEditPosition = Editor::CustomDragFloat3("Position", position);
			viewAndEditResult |= viewAndEditPosition;
			if (viewAndEditPosition.myIsEdited)
			{
				aValue.SetPosition(position);
			}
		}

		{
			Math::Vector3f rotation = aValue.GetRotation();
			const ViewAndEditResult viewAndEditRotation = Editor::CustomDragFloat3("Rotation", rotation);
			viewAndEditResult |= viewAndEditRotation;
			if (viewAndEditRotation.myIsEdited)
			{
				aValue.SetRotation(rotation);
			}
		}

		{
			Math::Vector3f scale = aValue.GetScale();
			const ViewAndEditResult viewAndEditScale = Editor::CustomDragFloat3("Scale", scale);
			viewAndEditResult |= viewAndEditScale;
			if (viewAndEditScale.myIsEdited)
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

				aValue.SetScale(scale);
			}
		}

		return viewAndEditResult;
	}

	ViewAndEditResult ViewAndEditValue(Graphics::PointLightData& aPointLightData, const std::string& /*aVariableName*/)
	{
		ViewAndEditResult viewAndEditResult;
		const bool editedColor = ImGui::DragFloat3("Color", &aPointLightData.color.x, 0.1f, 0.0f);
		viewAndEditResult.myIsActive |= ImGui::IsItemActive();
		const bool editedIntensity = ImGui::DragFloat("Intensity", &aPointLightData.color.w, 0.1f, 0.0f);
		viewAndEditResult.myIsActive |= ImGui::IsItemActive();
		const bool editedRadius = ImGui::DragFloat("Radius", &aPointLightData.radius, 0.2f, 0.1f);
		viewAndEditResult.myIsActive |= ImGui::IsItemActive();

		viewAndEditResult.myIsEdited = (editedColor || editedIntensity || editedRadius);
		return viewAndEditResult;
	}

	ViewAndEditResult ViewAndEditValue(Graphics::Camera& aCamera, const std::string& /*aVariableName*/)
	{
		ViewAndEditResult viewAndEditResult;
		const Math::Vector2ui resolution = Global::GetResolution();

		float moveSpeed = aCamera.GetMoveSpeed();
		float horizontalFoV = aCamera.GetHorizontalFoV();
		float nearPlane = aCamera.GetNearPlane();
		float farPlane = aCamera.GetFarPlane();

		if (ImGui::DragFloat("Speed", &moveSpeed))
		{
			aCamera.SetMoveSpeed(moveSpeed);
			viewAndEditResult.myIsEdited = true;
			viewAndEditResult.myIsActive = ImGui::IsItemActive();
		}

		if (ImGui::DragFloat("HorizontalFoV", &horizontalFoV))
		{
			aCamera.SetHorizontalFoV(horizontalFoV, resolution);
			viewAndEditResult.myIsEdited = true;
			viewAndEditResult.myIsActive = ImGui::IsItemActive();
		}

		if (ImGui::DragFloat("NearPlane", &nearPlane))
		{
			aCamera.SetNearPlane(nearPlane, resolution);
			viewAndEditResult.myIsEdited = true;
			viewAndEditResult.myIsActive = ImGui::IsItemActive();
		}

		if (ImGui::DragFloat("FarPlane", &farPlane))
		{
			aCamera.SetFarPlane(farPlane, resolution);
			viewAndEditResult.myIsEdited = true;
			viewAndEditResult.myIsActive = ImGui::IsItemActive();
		}

		return viewAndEditResult;
	}

	ViewAndEditResult ViewAndEditValue(const Graphics::Mesh*& aMesh, const std::string& /*aVariableName*/)
	{
		ViewAndEditResult viewAndEditResult;
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
						const std::string meshRelativePath = SimpleUtilities::ConvertAbsolutePathToRelativePath(SimpleUtilities::CheckAndReturnAsAbsolutePath(payloadData));
						aMesh = Global::GetModelFactory()->LoadMesh(meshRelativePath);
						viewAndEditResult.myIsEdited = true;
					}
					ImGui::EndDragDropTarget();
				}
			}
		}

		return viewAndEditResult;
	}

	ViewAndEditResult ViewAndEditValue(const Graphics::Shader*& aShader, const std::string& /*aVariableName*/)
	{
		ViewAndEditResult viewAndEditResult;

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
			viewAndEditResult.myIsEdited = true;
			pixelShader = pixelShaderFileNames[selectedPixelShader];
		}

		ImGui::Text("VertexShader:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(200);

		if (ImGui::Combo("##VertexShaderECSEditorFunction", &selectedVertexShader, vertexShaderNames.c_str()))
		{
			viewAndEditResult.myIsEdited = true;
			vertexShader = vertexShaderFileNames[selectedVertexShader];
		}

		if (viewAndEditResult.myIsEdited == true)
		{
			SimpleUtilities::AppendStringInFront("Shaders\\", pixelShader);
			SimpleUtilities::AppendStringInFront("Shaders\\", vertexShader);

			const Graphics::Shader* shader = Global::GetGraphicsEngine()->GetShaderManager()->GetShader(pixelShader.c_str(), vertexShader.c_str()).get();

			if (shader != nullptr)
			{
				aShader = shader;
			}
		}

		return viewAndEditResult;
	}

	ViewAndEditResult ViewAndEditValue(const Graphics::Texture*& aTexture, const std::string& /*aVariableName*/)
	{
		ViewAndEditResult viewAndEditResult;

		std::string texture;

		if (aTexture != nullptr)
		{
			texture = aTexture->GetTextureName();
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
						aTexture = Global::GetGraphicsEngine()->GetTextureManager()->GetTexture(fileName.c_str()).get();
						viewAndEditResult.myIsEdited = true;
					}
					ImGui::EndDragDropTarget();
				}
			}
		}

		return viewAndEditResult;
	}

	ViewAndEditResult ViewAndEditValue(const Graphics::Skeleton*& aSkeleton, const std::string& /*aVariableName*/)
	{
		ViewAndEditResult viewAndEditResult;
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
						viewAndEditResult.myIsEdited = true;
					}
					ImGui::EndDragDropTarget();
				}
			}
		}

		return viewAndEditResult;
	}

	ViewAndEditResult ViewAndEditValue(const Graphics::Animation*& aAnimation, const std::string& /*aVariableName*/)
	{
		ViewAndEditResult viewAndEditResult;
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
						viewAndEditResult.myIsEdited = true;
					}
					ImGui::EndDragDropTarget();
				}
			}
		}

		return viewAndEditResult;
	}

	struct Combo
	{
		template<typename Func>
		Combo(const char* aLabel, const char* aPreview, Func&& aFunc)
		{
			isOpen = ImGui::BeginCombo(aLabel, aPreview);

			if (isOpen) aFunc();
		}

		~Combo()
		{
			if (isOpen)
			{
				ImGui::EndCombo();

			}
		}

		bool isOpen = false;
	};

	ViewAndEditResult ViewAndEditValue(Fly::ClassInstanceProxy& aClassInstance, [[maybe_unused]] const std::string& aVariableName)
	{

		ViewAndEditResult viewAndEditResult;

		Fly::DataTypeProxy entityDataType(Fly::GetDataTypeID<Entity*>());
		auto entityClasses = Fly::GetClassesByTargetDataType(entityDataType);

		std::string_view preview = aClassInstance ? aClassInstance.GetName() : "None";
		Combo classCombo("Entity Class", std::string(preview).c_str(), [&]() -> void
			{

				for (auto& entityClass : entityClasses)
				{
					const bool isSelected = aClassInstance ? aClassInstance.GetClassInstance().mClassID == entityClass.GetID() : false;
					if (ImGui::Selectable(std::string(entityClass.GetName()).c_str(), isSelected))
					{
						if (isSelected)
						{
							continue;
						}

						if (aClassInstance)
						{
							aClassInstance.Destroy();
						}

						aClassInstance = entityClass.CreateClassInstance();

						viewAndEditResult.myIsEdited = true;
					}
				}
			});

		if (!aClassInstance)
		{
			return viewAndEditResult;
		}

		aClassInstance.ViewAndEditVariableDefaultValues(nullptr);

		return viewAndEditResult;
	}

	ViewAndEditResult CustomViewAndEditValue(std::array<const Graphics::Texture*, 3>& aTextures, const std::string& /*aVariableName*/)
	{
		ViewAndEditResult viewAndEditResult;

		ImGui::AlignTextToFramePadding();

		ImTextureID albedoTextureID = aTextures[Graphics::Global_Slot_Albedo]->GetShaderResourceView().Get();
		ImTextureID normalTextureID = aTextures[Graphics::Global_Slot_Normal]->GetShaderResourceView().Get();
		ImTextureID materialTextureID = aTextures[Graphics::Global_Slot_Material]->GetShaderResourceView().Get();

		ImGui::Dummy(ImVec2(0, 10));

		ImVec2 windowSize = ImGui::GetWindowSize();

		const float imageWidth = 64.0f;
		const float imageHeight = 64.0f;
		const float totalSpacing = windowSize.x - (imageWidth * 3);
		const float spacing = totalSpacing / 4.0f;

		for (size_t i = 0; i < aTextures.size(); ++i)
		{
			std::string textureName;

			if (aTextures[i] != nullptr)
			{
				textureName = aTextures[i]->GetTextureName();
			}

			ImTextureID textureID = nullptr;
			const char* labelText = "";
			float xPosition = 0.0f;

			switch (i)
			{
			case Graphics::Global_Slot_Albedo:
				textureID = albedoTextureID;
				labelText = "Albedo";
				xPosition = spacing;
				break;
			case Graphics::Global_Slot_Normal:
				textureID = normalTextureID;
				labelText = "Normal";
				xPosition = spacing * 2 + imageWidth;
				break;
			case Graphics::Global_Slot_Material:
				textureID = materialTextureID;
				labelText = "Material";
				xPosition = windowSize.x - imageWidth - spacing;
				break;
			}

			ImGui::SetCursorPos(ImVec2(xPosition, ImGui::GetCursorPosY()));
			const ImVec2 imagePos = ImGui::GetCursorScreenPos();

			ImGui::BeginGroup();
			ImGui::Image(textureID, ImVec2(imageWidth, imageHeight));

			ImDrawList* drawList = ImGui::GetWindowDrawList();
			const ImVec2 imageMin = imagePos;
			const ImVec2 imageMax = ImVec2(imagePos.x + imageWidth, imagePos.y + imageHeight);
			drawList->AddRect(imageMin, imageMax, IM_COL32(0, 0, 0, 255), 0.0f, ImDrawFlags_None, 2.0f);

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
							const std::shared_ptr<const Graphics::Texture> texture = Global::GetGraphicsEngine()->GetTextureManager()->GetTexture(fileName.c_str());

							bool isCubeMap = false;

							ID3D11ShaderResourceView* shaderResourceView = texture.get()->GetShaderResourceView().Get();

							switch (i)
							{
							case Graphics::Global_Slot_Albedo:

								albedoTextureID = shaderResourceView;
								break;
							case Graphics::Global_Slot_Normal:
								normalTextureID = shaderResourceView;
								break;
							case Graphics::Global_Slot_Material:
								materialTextureID = shaderResourceView;
								break;
							}

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
								viewAndEditResult.myIsEdited = true;
								viewAndEditResult.myIsActive = true;
							}
						}

						ImGui::EndDragDropTarget();
					}

				}
			}

			ImGui::Text("%s", labelText);
			ImGui::EndGroup();

			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip(textureName.c_str());
			}

			if (i < aTextures.size() - 1)
			{
				ImGui::SameLine();
			}
		}

		return viewAndEditResult;
	}
}