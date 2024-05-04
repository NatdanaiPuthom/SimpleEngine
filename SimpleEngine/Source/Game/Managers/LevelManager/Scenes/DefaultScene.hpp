#pragma once
#include "Game/Managers/LevelManager/Template/Scene.hpp"
#include "Game/Test/IK/InverseKinematics.hpp"
#include "Game/Player/Player.hpp"

namespace Graphics
{
	class Model;
}

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
		Player myPlayer;

		Test::InverseKinematics myIKTest;
	};
}