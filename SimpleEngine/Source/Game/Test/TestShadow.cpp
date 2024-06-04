#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/TestShadow.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/ECS/Components/Core/TransformComponent.hpp"
#include "Engine/ECS/Components/Core/MeshComponent.hpp"

using namespace ECS;

namespace Test
{
	TestShadow::TestShadow()
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
			ECS::Entity floor = ecs->CreateEntity();
			floor->SetName("Floor");

			floor->AddComponent<TransformComponent>();
			floor->AddComponent<MeshComponent>();

			floor->GetComponent<MeshComponent>()->shader = graphicsEngine->GetShader(Graphics::eShaderType::GBuffer).get();
			floor->GetComponent<MeshComponent>()->textures[Graphics::Global_Slot_Albedo] = graphicsEngine->GetTexture("Assets\\Textures\\Models\\T_DefaultMaterial_C.dds").get();
			floor->GetComponent<MeshComponent>()->textures[Graphics::Global_Slot_Normal] = graphicsEngine->GetTexture("Assets\\Textures\\Models\\T_DefaultMaterial_N.dds").get();
			floor->GetComponent<MeshComponent>()->textures[Graphics::Global_Slot_Material] = graphicsEngine->GetTexture("Assets\\Textures\\Models\\T_DefaultMaterial_M.dds").get();
			floor->GetComponent<MeshComponent>()->mesh = graphicsEngine->GetModelFactory()->GetPrimitiveShape(Graphics::ePrimitiveShape::Cube);
			floor->GetComponent<MeshComponent>()->mesh = graphicsEngine->GetModelFactory()->LoadMesh("Assets\\Models\\StaticModels\\SM_DefaultCube_1x1.fbx");
			floor->GetComponent<TransformComponent>()->transform.SetPosition({ 0.0f, -1.6f, 0.2f });
			floor->GetComponent<TransformComponent>()->transform.SetScale({ 1.0f, 1.0f, 1.0f });
		}

		{
			ECS::Entity chest = ecs->CreateEntity();
			chest->SetName("Chest");
			chest->AddComponent<TransformComponent>();
			chest->AddComponent<MeshComponent>();

			chest->GetComponent<MeshComponent>()->shader = graphicsEngine->GetShader(Graphics::eShaderType::GBuffer).get();
			chest->GetComponent<MeshComponent>()->textures[Graphics::Global_Slot_Albedo] = graphicsEngine->GetTexture("Assets\\Textures\\Models\\Particle_Chest_D.dds").get();
			chest->GetComponent<MeshComponent>()->textures[Graphics::Global_Slot_Normal] = graphicsEngine->GetTexture("Assets\\Textures\\Models\\Particle_Chest_N.dds").get();
			chest->GetComponent<MeshComponent>()->textures[Graphics::Global_Slot_Material] = graphicsEngine->GetTexture("Assets\\Textures\\Models\\Particle_Chest_M.dds").get();
			chest->GetComponent<MeshComponent>()->mesh = graphicsEngine->GetModelFactory()->LoadMesh("Assets\\Models\\StaticModels\\Particle_Chest.fbx");
			chest->GetComponent<TransformComponent>()->transform.SetPosition({ -5.0f, 0.0f, 0.0f });
			chest->GetComponent<TransformComponent>()->transform.SetScale(0.01f);
		}

		graphicsEngine->GetCurrentCamera()->SetPosition({ 0.0f,0.0f,-2.0f });
	}

	void TestShadow::Update()
	{
		//EntityComponentSystem* ecs = World::GetECS();
		//Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		/*auto shadowCam = graphicsEngine->GetShadowCamera();
		shadowCam->SetRotation(t->transform.GetRotation());
		shadowCam->SetPosition(t->transform.GetPosition());*/
	}

	void TestShadow::Render() const
	{
	}
}