#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/Tools/RenderTool.hpp"

namespace Tool
{
	RenderTool::RenderTool()
	{
	}

	void RenderTool::Draw()
	{
		WaterReflectionModel();
		ImGuiImage();
	}

	void RenderTool::ImGuiImage()
	{
		Simple::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		const Math::Vector2ui resolution = Global::GetResolution();
		const ImVec2 size(static_cast<float>(resolution.x), static_cast<float>(resolution.y));
	
		if (ImGui::Begin("Render Test"))
		{
			const ImTextureID textureID = graphicsEngine->GetWaterRefractionShaderResourceView().Get();
			ImGui::Image(textureID, size);
		}
		ImGui::End();
	}

	void RenderTool::WaterReflectionModel()
	{
		if (ImGui::Begin("Water Reflection"))
		{
			auto& waterPlane = World::GetWaterPlane()->myModel;

			std::vector<std::string> modelNames;
			modelNames.push_back(waterPlane->GetName());

			std::vector<const char*> names;
			for (const std::string& name : modelNames)
			{
				names.push_back(name.c_str());
			}

			int selectedWaterIndex = 0;
			ImGui::SetNextItemWidth(200);
			if (ImGui::Combo("Plane", &selectedWaterIndex, names.data(), static_cast<int>(names.size())))
			{
			}

			ImGui::Dummy(ImVec2(0, 10));
			ImGui::SetNextItemWidth(400);
			Math::Vector3f position = waterPlane->GetPosition();
			if (ImGui::DragFloat3("Position", &position.x, 0.1f))
			{
				waterPlane->SetPosition(position);
			}

			ImGui::SetNextItemWidth(400);
			Math::Vector3f rotation = waterPlane->GetRotation();
			if (ImGui::DragFloat3("Rotation", &rotation.x, 1.0f, 0.0f, 360.0f, "%.3f"))
			{
				waterPlane->SetRotation(rotation);
			}

			ImGui::SetNextItemWidth(400);
			Math::Vector3f scale = waterPlane->GetScale();
			if (ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.001f, 100.0f, "%0.3f"))
			{
				waterPlane->SetScale(scale);
			}
		}
		ImGui::End();
	}
}