#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/TestForECS.hpp"

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
		meshComponent->shader = graphicsEngine->GetShader(Graphics::eShaderType::Unlit_Default).get();
		meshComponent->texture = graphicsEngine->GetDefaultTexture().get();

		animatedComponent->shader = graphicsEngine->GetShader(Graphics::eShaderType::Unlit_Animated).get();
		animatedComponent->skeleton = modelFactory->LoadSkeleton("AnimatedModels/SimpleHuman3.fbx");
		animatedComponent->animation = modelFactory->LoadAnimationFBX("Animations/SimpleHuman3_Idle.fbx");

		animationPlayerComponent->animationPlayer.Init(animatedComponent->animation, animatedComponent->skeleton);
		animationPlayerComponent->animationPlayer.Play(true);

		myEntityID = entity->GetID();

		ECS::Entity e = World::GetECS()->CreateEntity();
		e->AddComponent<ECS::MeshComponent>();
		e->AddComponent<ECS::TransformComponent>();
		e->GetComponent<ECS::MeshComponent>()->shader = graphicsEngine->GetShader(Graphics::eShaderType::Unlit_Default).get();
		myTestEntityID = e->GetID();
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

	void ECSTestStuff::Render() const
	{
		ECS::Entity entity = World::GetECS()->GetEntity(myEntityID);

		if (entity != nullptr)
		{
			ECS::MeshComponent* meshComponent = entity->GetComponent<ECS::MeshComponent>();
			ECS::TransformComponent* transformComponent = entity->GetComponent<ECS::TransformComponent>();
			ECS::AnimatedComponent* animatedComponent = entity->GetComponent<ECS::AnimatedComponent>();

			Global::GetRenderer()->RenderAnimatedModel(transformComponent, meshComponent, animatedComponent);
		}

		ECS::Entity e = World::GetECS()->GetEntity(myTestEntityID);
		if (e != nullptr)
		{
			ECS::MeshComponent* meshComponent = e->GetComponent<ECS::MeshComponent>();
			ECS::TransformComponent* transformComponent = e->GetComponent<ECS::TransformComponent>();


			if (meshComponent->mesh != nullptr && meshComponent->texture != nullptr)
			{
				Global::GetRenderer()->RenderStaticModel(transformComponent, meshComponent);
			}
		}
	}
}