#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/TestForECS.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Test
{
	ECSTestStuff::ECSTestStuff()
		: myEntityID(0)
		, myTestEntityID(0)
	{
	}

	ECSTestStuff::~ECSTestStuff()
	{
	}

	void ECSTestStuff::Init()
	{
		ECS::EntityComponentSystem& ecs = MainSingleton::GetSceneManager().GetCurrentECS();
		ECS::Entity entity = ecs.CreateEntity();

		entity->AddComponent<ECS::TransformComponent>();
		entity->AddComponent<ECS::MeshComponent>();
		entity->AddComponent<ECS::AnimationComponent>();
		entity->AddComponent<ECS::AnimationComponent>();

		ECS::MeshComponent* meshComponent = entity->GetComponent<ECS::MeshComponent>();
		ECS::AnimationComponent* animatedComponent = entity->GetComponent<ECS::AnimationComponent>();
		ECS::AnimationComponent* animationPlayerComponent = entity->GetComponent<ECS::AnimationComponent>();

		Graphics::ModelFactory* modelFactory = Global::GetModelFactory();
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		meshComponent->mesh = modelFactory->LoadMesh("AnimatedModels/SimpleHuman3.fbx");
		meshComponent->shader = graphicsEngine->GetShader(Graphics::eShaderType::Unlit_Default).get();
		meshComponent->textures[0] = graphicsEngine->GetTexture(Graphics::eTextureType::Default).get();

		animatedComponent->shader = graphicsEngine->GetShader(Graphics::eShaderType::Animated).get();
		animatedComponent->skeleton = modelFactory->LoadSkeleton("AnimatedModels/SimpleHuman3.fbx");
		animatedComponent->animation = modelFactory->LoadAnimationFBX("Animations/SimpleHuman3_Idle.fbx");

		animationPlayerComponent->animationPlayer.Init(animatedComponent->animation, animatedComponent->skeleton);
		animationPlayerComponent->animationPlayer.Play(true);

		myEntityID = entity->GetID();

		ECS::Entity e = ecs.CreateEntity();
		e->AddComponent<ECS::MeshComponent>();
		e->AddComponent<ECS::TransformComponent>();
		e->GetComponent<ECS::MeshComponent>()->shader = graphicsEngine->GetShader(Graphics::eShaderType::Unlit_Default).get();
		myTestEntityID = e->GetID();
	}

	void ECSTestStuff::Update() const
	{
		ECS::EntityComponentSystem& ecs = MainSingleton::GetSceneManager().GetCurrentECS();
		ECS::Entity entity = ecs.GetEntity(myEntityID);

		if (entity != nullptr)
		{
			ECS::AnimationComponent* animatedComponent = entity->GetComponent<ECS::AnimationComponent>();
			ECS::AnimationComponent* animationPlayerComponent = entity->GetComponent<ECS::AnimationComponent>();
			animationPlayerComponent->animationPlayer.UpdateTest(animatedComponent->jointMatrices, animatedComponent);
		}
	}

	void ECSTestStuff::Render() const
	{
		ECS::EntityComponentSystem& ecs = MainSingleton::GetSceneManager().GetCurrentECS();
		ECS::Entity entity = ecs.GetEntity(myEntityID);

		if (entity != nullptr)
		{
			ECS::MeshComponent* meshComponent = entity->GetComponent<ECS::MeshComponent>();
			ECS::TransformComponent* transformComponent = entity->GetComponent<ECS::TransformComponent>();
			ECS::AnimationComponent* animatedComponent = entity->GetComponent<ECS::AnimationComponent>();

			Global::GetRenderer()->RenderAnimatedModel(transformComponent, meshComponent, animatedComponent);
		}

		ECS::Entity e = ecs.GetEntity(myTestEntityID);

		if (e != nullptr)
		{
			ECS::MeshComponent* meshComponent = e->GetComponent<ECS::MeshComponent>();
			ECS::TransformComponent* transformComponent = e->GetComponent<ECS::TransformComponent>();


			if (meshComponent->mesh != nullptr && meshComponent->textures[0] != nullptr)
			{
				Global::GetRenderer()->RenderStaticModel(transformComponent, meshComponent);
			}
		}
	}
}