#pragma once
#include "Game/Managers/LevelManager/Template/Scene.hpp"

class Player
{
public:
	Simple::AnimatedModel myModel;
	Simple::AnimationPlayer myAnimationPlayer;
	Simple::Animation myAnimation;
};

namespace Scenes
{
	class DefaultScene final : public Simple::Scene
	{
	public:
		DefaultScene();
		~DefaultScene() override;

		void OnEnter() override;

		void Init() override;
		void Update() override;
		void Render() override;
	private:
		Simple::Model myFloor;
		Player myPlayer;
	};
}