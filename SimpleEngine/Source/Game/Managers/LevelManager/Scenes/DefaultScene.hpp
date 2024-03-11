#pragma once
#include "Game/Managers/LevelManager/Template/Scene.hpp"

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
	};
}