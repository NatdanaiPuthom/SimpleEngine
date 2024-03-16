#pragma once
#include "Engine/Threadpool/ThreadPool.hpp"
#include "Engine/Global.hpp"
#include "Graphics/Model/AnimatedModel.hpp"
#include "Graphics/Animation/Animation.hpp"
#include <vector>

//NOTE(v9.21.0): No more thread as the new way of updating animations and setpose is faster. Will try to optimize more in future

namespace Simple
{
	class AnimationThreadTest final
	{
	public:
		AnimationThreadTest(const size_t aSize = 200)
			: myModelSize(aSize)
		{
			myAnimatedModels.resize(aSize);
		};

		~AnimationThreadTest() = default;

		inline void Init()
		{
			PROFILER_BEGIN("Load animatedmodels");
			Simple::AnimatedModel animatedModel;
			animatedModel = Global::GetModelFactory()->LoadAnimatedModelFBX("Test/SM_wizard.fbx");
			animatedModel.SetScale(0.01f);
			animatedModel.ClearTextures();
			animatedModel.AddTexture("Models/SM_Wizard_c.dds");

			for (size_t i = 0; i < myModelSize; ++i)
			{
				myAnimatedModels[i] = animatedModel;
			}
			PROFILER_END();


			PROFILER_BEGIN("Load animation");
			myAnimation = Global::GetModelFactory()->LoadAnimationFBX("Test/A_Wizard_Walk.fbx");
			myAnimationPlayer.Init(myAnimation, myAnimatedModels[0]);
			myAnimationPlayer.SetIsLooping(true);
			myAnimationPlayer.Play();
			PROFILER_END();

			constexpr float spacing = 5.0f;
			const size_t numRows = static_cast<size_t>(std::sqrt(myModelSize));
			const size_t numCols = numRows;

			for (size_t i = 0; i < myModelSize; ++i)
			{
				const float x = spacing * static_cast<float>((i % numCols));
				const float z = spacing * static_cast<float>((i / numRows));
				const Math::Vector3f pos(x, 0.0f, z);
				myAnimatedModels[i].SetPosition(pos);
			}
		}

		inline void Update()
		{
			PROFILER_BEGIN("Push model pointers");
			std::vector<Simple::AnimatedModel*> models(myModelSize, nullptr);
			for (size_t i = 0; i < myModelSize; ++i)
			{
				models[i] = &myAnimatedModels[i];
			}
			PROFILER_END();


			PROFILER_BEGIN("Animations Update");
			myAnimationPlayer.UpdateMultipleModels(models);
			PROFILER_END();
		}

		inline void Render()
		{
			for (size_t i = 0; i < myModelSize; ++i)
			{
				Global::GetRenderer()->RenderModel(myAnimatedModels[i]);
			}
		}

	private:
		Simple::AnimationPlayer myAnimationPlayer;
		Simple::Animation myAnimation;

		const size_t myModelSize;

		std::vector<Simple::AnimatedModel> myAnimatedModels;
	};
}