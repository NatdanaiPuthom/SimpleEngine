#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/TestForECS.hpp"

namespace Test
{
	ECSTestStuff::ECSTestStuff()
	{
	}

	ECSTestStuff::~ECSTestStuff()
	{
	}
	void ECSTestStuff::Init()
	{

		ECS::Entity entity = World::GetECS()->CreateEntity();

		entity->AddComponent<ECS::TransformComponent>();
		entity->AddComponent<ECS::MeshComponent>();
		entity->AddComponent<ECS::AnimatedComponent>();
		entity->AddComponent<ECS::AnimationPlayerComponent>();

		ECS::MeshComponent* meshComponent = entity->GetComponent<ECS::MeshComponent>();
		ECS::AnimatedComponent* animatedComponent = entity->GetComponent<ECS::AnimatedComponent>();
		ECS::AnimationPlayerComponent* animationPlayerComponent = entity->GetComponent<ECS::AnimationPlayerComponent>();

		Graphics::ModelFactory* modelFactory = Global::GetModelFactory();
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		meshComponent->mesh = modelFactory->LoadMesh("AnimatedModels/SimpleHuman3.fbx");
		meshComponent->shader = graphicsEngine->GetDefaultShader().get();
		meshComponent->texture = graphicsEngine->GetDefaultTexture().get();

		animatedComponent->shader = graphicsEngine->GetDefaultAnimatedShader().get();
		animatedComponent->skeleton = modelFactory->LoadSkeleton("AnimatedModels/SimpleHuman3.fbx");
		animatedComponent->animation = modelFactory->LoadAnimationFBX("Animations/SimpleHuman3_Idle.fbx");

		animationPlayerComponent->animationPlayer.Init(animatedComponent->animation, animatedComponent->skeleton);
		animationPlayerComponent->animationPlayer.Play(true);

		myEntityID = entity->GetID();
	}

	void ECSTestStuff::Update()
	{
		ECS::Entity entity = World::GetECS()->GetEntity(myEntityID);

		if (entity != nullptr)
		{
			ECS::AnimatedComponent* animatedComponent = entity->GetComponent<ECS::AnimatedComponent>();
			ECS::AnimationPlayerComponent* animationPlayerComponent = entity->GetComponent<ECS::AnimationPlayerComponent>();
			animationPlayerComponent->animationPlayer.UpdateTest(animatedComponent->jointMatrices, animatedComponent);
		}
	}

	void ECSTestStuff::Render()
	{
		ECS::Entity entity = World::GetECS()->GetEntity(myEntityID);

		if (entity != nullptr)
		{
			ECS::MeshComponent* meshComponent = entity->GetComponent<ECS::MeshComponent>();
			ECS::TransformComponent* transformComponent = entity->GetComponent<ECS::TransformComponent>();
			ECS::AnimatedComponent* animatedComponent = entity->GetComponent<ECS::AnimatedComponent>();

			Global::GetRenderer()->RenderAnimatedModel(transformComponent, meshComponent, animatedComponent);
		}
	}
}