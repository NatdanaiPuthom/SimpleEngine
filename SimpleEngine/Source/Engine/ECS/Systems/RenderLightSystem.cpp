#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Systems/RenderLightSystem.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/ECS/Components/Core/SkyBoxComponent.hpp"
#include "Engine/ECS/Components/Core/DirectionalLightComponent.hpp"
#include "Editor/Editor.hpp"
#include "External/imgui.h"

namespace ECS
{
	RenderLightSystem::RenderLightSystem(EntityComponentSystem& aEntityComponentSystem) : System(aEntityComponentSystem)
		, myEntityWithSkyBoxID(static_cast<size_t>(-1))
		, myEntityWithDirectionalLightID(static_cast<size_t>(-1))
	{
	}

	RenderLightSystem::~RenderLightSystem()
	{
	}

	void RenderLightSystem::Init()
	{
		FindAndSetSkyBox();
		FindAndSetDirectionalLight();
	}

	void RenderLightSystem::Update()
	{
		if (FindAndSetSkyBox())
		{
			const Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
			const ECS::Entity skyboxEntity = myEntityComponentSystem.GetEntity(myEntityWithSkyBoxID);
			SkyBoxComponent* skyBoxComponent = skyboxEntity->GetComponent<SkyBoxComponent>();
			skyBoxComponent->transform.SetPosition(graphicsEngine->GetCurrentCamera()->GetPosition());
		}

		if (FindAndSetDirectionalLight())
		{
			Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
			const ECS::Entity directionalLight = myEntityComponentSystem.GetEntity(myEntityWithDirectionalLightID);
			const DirectionalLightComponent* directionalLightComponent = directionalLight->GetComponent<DirectionalLightComponent>();
			const Math::Vector3f forward = directionalLightComponent->transform.GetMatrix().GetForward();
			graphicsEngine->SetDirectionalLightDirection(forward.GetNormalized() * -1.0f);
		}

		{
			Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

			PointLightData pointLight1;
			pointLight1.color = { 1.0f, 0.0f,0.0f, 5.0f };
			pointLight1.position = { 2.75f, 1.7f, 1.25f };
			pointLight1.radius = 3.0f;
			graphicsEngine->AddPointLight(pointLight1);
		}

		/*if (ImGui::Begin("DirectionalLight"))
		{
			Math::Transform& transform = directionalLightComponent->transform;
			Math::Vector3f position = transform.GetPosition();
			Math::Vector3f rotation = transform.GetRotation();

			if (ImGui::DragFloat3("Position", &position.x))
			{
				transform.SetPosition(position);
			}

			if (ImGui::DragFloat3("Rotation", &rotation.x))
			{
				transform.SetRotation(rotation);
			}
		}
		ImGui::End();*/

		/*
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		static Math::Vector4f ambientLight = graphicsEngine->GetAmbientLightColorAndIntensity();

		if (ImGui::Begin("Light"))
		{
			if (ImGui::DragFloat4("Ambient light", &ambientLight.x, 0.1f))
			{
				graphicsEngine->SetAmbientLightColorAndIntensity(ambientLight);
			}
		}
		ImGui::End();
		*/
	}

	void RenderLightSystem::Render()
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
		ID3D11DeviceContext* context = graphicsEngine->GetContext().Get(); context;
		const Drawer::Renderer* renderer = Global::GetRenderer();

		std::vector<Graphics::RenderTarget>& gBuffers = graphicsEngine->GetRenderTargets(Graphics::eRenderTargetType::GBuffer);

		ID3D11ShaderResourceView* shaderResources[5] = {};

		for (size_t i = 0; i < gBuffers.size(); ++i)
		{
			shaderResources[i] = gBuffers[i].shaderResourceView.Get();
		}

		context->PSSetShaderResources(5, 5, shaderResources);

		const Graphics::Mesh* mesh = Global::GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Sphere);
		std::shared_ptr<const Graphics::Shader> shader = Global::GetGraphicsEngine()->GetShader(Graphics::eShaderType::PointLight);
		std::shared_ptr<const Graphics::Texture> texture = Global::GetGraphicsEngine()->GetTexture(Graphics::eTextureType::Default);
		Math::Transform transform;

		const Graphics::eRasterizerState previousRasterizerState = graphicsEngine->GetCurrentRasterizerState();

		graphicsEngine->SetRasterizerState(Graphics::eRasterizerState::FrontFaceCulling);
		graphicsEngine->SetDepthStencilState(Graphics::eDepthStencilState::Greater);
		graphicsEngine->SetBlendState(Graphics::eBlendState::AdditiveBlend);

		PointLightData* pointLightBuffer = graphicsEngine->GetPointLightDataArray();

		graphicsEngine->UpdateLightBuffer(static_cast<size_t>(-1)); //TO-DO(v11.0.6): Refactor somehow, maybe seperate directionalLight buffer from pointlights?

		for (size_t i = 0; i < graphicsEngine->GetPointLightCount(); ++i)
		{
			graphicsEngine->UpdateLightBuffer(i);

			transform.SetPosition(pointLightBuffer[i].position);
			transform.SetScale(pointLightBuffer[i].radius);

			renderer->RenderUnlit(transform.GetMatrix(), mesh, shader.get(), texture.get());
		}

		graphicsEngine->SetDepthStencilState(Graphics::eDepthStencilState::Less_Equal);
		graphicsEngine->SetRasterizerState(previousRasterizerState);
		graphicsEngine->SetBlendState(Graphics::eBlendState::Disabled);

		ID3D11ShaderResourceView* nullSRVs[5] = { NULL };
		context->PSSetShaderResources(5, 5, nullSRVs);

		if (Editor::EditorEngine::myStaticShouldRenderDebugLines == true)
		{
			Drawer::Sphere pointLightDebugSpheres;
			pointLightDebugSpheres.color = { 1.0f, 0.0f, 0.0f, 1.0f };

			for (size_t i = 0; i < graphicsEngine->GetPointLightCount(); ++i)
			{
				pointLightDebugSpheres.position = pointLightBuffer[i].position;
				pointLightDebugSpheres.radius = pointLightBuffer[i].radius;
				renderer->RenderSphere(pointLightDebugSpheres);

				pointLightDebugSpheres.radius = 0.1f;
				renderer->RenderSphere(pointLightDebugSpheres);
			}
		}

		if (myEntityWithDirectionalLightID != (static_cast<size_t>(-1)))
		{

			const ECS::Entity directionalLight = myEntityComponentSystem.GetEntity(myEntityWithDirectionalLightID);
			const DirectionalLightComponent* directionalLightComponent = directionalLight->GetComponent<DirectionalLightComponent>();
			const Math::Vector3f forward = directionalLightComponent->transform.GetMatrix().GetForward();

			Drawer::Line line;
			line.color = { 1.0f, 0.0f, 0.0f, 1.0f };
			line.startPosition = directionalLightComponent->transform.GetPosition();
			line.endPosition = line.startPosition + forward * 5.0f;

			Drawer::Sphere sphere;
			sphere.radius = 0.25f;
			sphere.position = line.endPosition;
			sphere.color = { 1.0f, 0.0f, 0.0f, 1.0f };

			renderer->RenderSphere(sphere);
			renderer->RenderLine(line);
		}
	}

	void RenderLightSystem::RenderSkyBoxAndDirectionalLight() const
	{
		const Drawer::Renderer* renderer = Global::GetRenderer();

		if (myEntityWithSkyBoxID != (static_cast<size_t>(-1)))
		{
			const ECS::Entity skyBox = myEntityComponentSystem.GetEntity(myEntityWithSkyBoxID);
			const SkyBoxComponent* skyBoxComponent = skyBox->GetComponent<SkyBoxComponent>();
			renderer->RenderUnlit(skyBoxComponent->transform.GetMatrix(), skyBoxComponent->mesh, skyBoxComponent->shader, skyBoxComponent->texture);
		}

		if (myEntityWithDirectionalLightID != (static_cast<size_t>(-1)))
		{
			const ECS::Entity directionalLight = myEntityComponentSystem.GetEntity(myEntityWithDirectionalLightID);
			const DirectionalLightComponent* directionalLightComponent = directionalLight->GetComponent<DirectionalLightComponent>();
			renderer->RenderUnlit(directionalLightComponent->transform.GetMatrix(), directionalLightComponent->mesh, directionalLightComponent->shader, directionalLightComponent->texture);
		}
	}

	bool RenderLightSystem::FindAndSetSkyBox()
	{
		myEntityWithSkyBoxID = static_cast<size_t>(-1);

		const std::unordered_set<EntityID>& entitiesWithSkyBoxComponent = myEntityComponentSystem.GetEntityIDsWithThisComponent<SkyBoxComponent>();

		if (entitiesWithSkyBoxComponent.empty() == false)
		{
			myEntityWithSkyBoxID = *entitiesWithSkyBoxComponent.begin();
			return true;
		}

		return false;
	}

	bool RenderLightSystem::FindAndSetDirectionalLight()
	{
		myEntityWithDirectionalLightID = static_cast<size_t>(-1);

		const std::unordered_set<EntityID>& entitiesWithDirectionalLightComponent = myEntityComponentSystem.GetEntityIDsWithThisComponent<DirectionalLightComponent>();

		if (entitiesWithDirectionalLightComponent.empty() == false)
		{
			myEntityWithDirectionalLightID = *entitiesWithDirectionalLightComponent.begin();
			return true;
		}

		return false;
	}
}