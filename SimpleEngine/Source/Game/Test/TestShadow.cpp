#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/TestShadow.hpp"

#include "Engine/ECS/ECS.hpp"
#include "Engine/ECS/Components/Core/TransformComponent.hpp"
#include "Engine/ECS/Components/Core/MeshComponent.hpp"
#include "Engine/Global.hpp"
#include "Game/World.hpp"

using namespace ECS;

namespace Test
{
	TestShadow::TestShadow()
		: myEntityID(static_cast<size_t>(-1))
	{
	}

	TestShadow::~TestShadow()
	{
	}

	void TestShadow::Init()
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
		EntityComponentSystem* ecs = World::GetECS();

		{
			ECS::Entity entity = ecs->CreateEntity();
			entity->SetName("SkyBox");

			entity->AddComponent<TransformComponent>();
			entity->AddComponent<MeshComponent>();

			entity->GetComponent<MeshComponent>()->shader = graphicsEngine->GetShader(Graphics::eShaderType::SkyBox).get();
			entity->GetComponent<MeshComponent>()->textures[0] = graphicsEngine->GetTexture(Graphics::eTextureType::SkyBox_DayCloud).get();
			entity->GetComponent<MeshComponent>()->mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::SkyBox);
		}

		{
			ECS::Entity floor = ecs->CreateEntity();
			floor->SetName("Floor");

			floor->AddComponent<TransformComponent>();
			floor->AddComponent<MeshComponent>();

			floor->GetComponent<MeshComponent>()->shader = graphicsEngine->GetShader(Graphics::eShaderType::Unlit_Default).get();
			floor->GetComponent<MeshComponent>()->textures[0] = graphicsEngine->GetTexture(Graphics::eTextureType::Default).get();
			floor->GetComponent<MeshComponent>()->mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Cube);
			floor->GetComponent<TransformComponent>()->transform.SetPosition({ 11.0f, -2.0f, 10.0f });
			floor->GetComponent<TransformComponent>()->transform.SetScale({ 100.0f, 1.0f, 100.0f });
		}

		{
			ECS::Entity directionalLight = ecs->CreateEntity();
			directionalLight->SetName("Directional Light");
			directionalLight->AddComponent<TransformComponent>();
			directionalLight->AddComponent<MeshComponent>();

			directionalLight->GetComponent<MeshComponent>()->shader = graphicsEngine->GetShader(Graphics::eShaderType::Unlit_Default).get();
			directionalLight->GetComponent<MeshComponent>()->textures[0] = graphicsEngine->GetTexture("Assets\\Textures\\Sunlight.dds").get();
			directionalLight->GetComponent<MeshComponent>()->mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Cube);
			directionalLight->GetComponent<TransformComponent>()->transform.SetPosition({ 0.0f, 5.0f, 0.0f });

			myEntityID = directionalLight->GetID();
		}


		for (size_t i = 0; i < 10; ++i)
		{
			ECS::Entity e = ecs->CreateEntity();
			e->SetName("Horizontal Cube " + std::to_string(i));

			e->AddComponent<TransformComponent>();
			e->AddComponent<MeshComponent>();

			e->GetComponent<MeshComponent>()->shader = graphicsEngine->GetShader(Graphics::eShaderType::Unlit_Default).get();
			e->GetComponent<MeshComponent>()->textures[0] = graphicsEngine->GetTexture(Graphics::eTextureType::Default).get();
			e->GetComponent<MeshComponent>()->mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Cube);
			e->GetComponent<TransformComponent>()->transform.SetPosition({ 2.0f + i * 2.0f, 0.0f, 0.0f });
		}

		for (size_t i = 0; i < 10; ++i)
		{
			ECS::Entity e = ecs->CreateEntity();
			e->SetName("Vertical Cube " + std::to_string(i));

			e->AddComponent<TransformComponent>();
			e->AddComponent<MeshComponent>();

			e->GetComponent<MeshComponent>()->shader = graphicsEngine->GetShader(Graphics::eShaderType::Unlit_Default).get();
			e->GetComponent<MeshComponent>()->textures[0] = graphicsEngine->GetTexture(Graphics::eTextureType::Default).get();
			e->GetComponent<MeshComponent>()->mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Cube);
			e->GetComponent<TransformComponent>()->transform.SetPosition({ 11.0f, 0.0f, 2.0f + i * 2.0f });
		}
	}

	void TestShadow::Update()
	{
		EntityComponentSystem* ecs = World::GetECS();
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		ECS::Entity e = ecs->GetEntity(myEntityID);
		ECS::TransformComponent* t = e->GetComponent<TransformComponent>();
		const Math::Vector3f forward = t->transform.GetMatrix().GetForward();

		graphicsEngine->SetDirectionalLightDirection((forward.GetNormalized()));

		auto shadowCam = graphicsEngine->GetShadowCamera();
		shadowCam->SetRotation(t->transform.GetRotation());
		shadowCam->SetPosition(t->transform.GetPosition());
	}

	void TestShadow::Render() const
	{
		EntityComponentSystem* ecs = World::GetECS();
		ECS::Entity e = ecs->GetEntity(myEntityID);

		const ECS::TransformComponent* t = e->GetComponent<TransformComponent>();
		const Math::Vector3f forward = t->transform.GetMatrix().GetForward();

		Drawer::Line line;
		line.color = { 1.0f, 0.0f, 0.0f, 1.0f };
		line.startPosition = t->transform.GetPosition();
		line.endPosition = line.startPosition + forward * 5.0f;

		Drawer::Sphere sphere;
		sphere.radius = 0.25f;
		sphere.position = line.endPosition;
		sphere.color = { 1.0f, 0.0f, 0.0f, 1.0f };

		Global::GetRenderer()->RenderSphere(sphere);
		Global::GetRenderer()->RenderLine(line);
	}
}