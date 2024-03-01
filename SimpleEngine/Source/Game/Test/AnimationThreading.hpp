#pragma once
#include "Engine/Threadpool/ThreadPool.hpp"
#include "Engine/Global.hpp"
#include "Graphics/Model/AnimatedModel.hpp"
#include "Graphics/Animation/Animation.hpp"
#include <vector>

namespace Simple
{
	struct NatdanaiAnimationTest
	{
		Simple::AnimatedModel animatedModel;
		Simple::Animation animation;
		Simple::AnimationPlayer animationPlayer;
	};

	class AnimationThreadTest final
	{
	public:
		AnimationThreadTest(const size_t aSize = 16) 
			: myModelSize(aSize)
		{

		};

		~AnimationThreadTest() = default;

		inline void Init()
		{
			NatdanaiAnimationTest animationTest;
			animationTest.animatedModel = Global::GetModelFactory()->LoadAnimatedModel("Assets/Models/SM_wizard.fbx");
			animationTest.animation = Global::GetModelFactory()->LoadAnimationFBX("Assets/Models/A_Wizard_Falling.fbx");

			animationTest.animatedModel.SetShader("DefaultPS.cso", "AnimatedModelVS.cso");
			animationTest.animatedModel.SetScale(0.01f);
			animationTest.animatedModel.ClearTextures();
			animationTest.animatedModel.AddTexture("SM_Wizard_c.dds");

			animationTest.animationPlayer.Init(animationTest.animation, animationTest.animatedModel);
			animationTest.animationPlayer.SetIsLooping(true);
			animationTest.animationPlayer.Play();

			for (size_t i = 0; i < myModelSize; ++i)
			{
				myThreadTest.push_back(animationTest);
			}

			constexpr float spacing = 5.0f;
			const size_t numRows = static_cast<size_t>(std::sqrt(myThreadTest.size()));
			const size_t numCols = numRows;

			for (size_t i = 0; i < myThreadTest.size(); ++i)
			{
				const float x = spacing * static_cast<float>((i % numCols));
				const float z = spacing * static_cast<float>((i / numRows));
				const Math::Vector3f pos(x, 0.0f, z);
				myThreadTest[i].animatedModel.SetPosition(pos);
			}

			myFutureResults.resize(myThreadTest.size());
		}

		inline void Update()
		{
			for (size_t i = 0; i < myThreadTest.size(); ++i)
			{
				std::future<bool> result = myThreadPool.AddTask(&AnimationPlayer::UpdateThreaded, std::ref(myThreadTest[i].animationPlayer), std::ref(myThreadTest[i].animatedModel), std::ref(myThreadTest[i].animation));
				myFutureResults[i] = std::move(result);
			}

			for (auto& future : myFutureResults)
			{
				future.get();
			}
		}

		inline void Render()
		{
			for (size_t i = 0; i < myThreadTest.size(); ++i)
			{
				Global::GetRenderer()->RenderModel(myThreadTest[i].animatedModel);
			}
		}

	private:
		std::vector<NatdanaiAnimationTest> myThreadTest;
		std::vector<std::future<bool>> myFutureResults;
		ThreadPool myThreadPool;
		const size_t myModelSize;
	};
}