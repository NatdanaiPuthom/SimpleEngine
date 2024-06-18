#pragma once
#include "Engine/ECS/ECS.hpp"
#include <vector>

class MainSingleton;

namespace Simpleton
{
	class SceneManager
	{
		friend class MainSingleton;
	public:
	
		void Init();
		void Update();
		void Render();
		void LateRender();

	private:
		static SceneManager& GetInstance()
		{
			static SceneManager instance;
			return instance;
		}

		SceneManager();
		~SceneManager();
	private:
		std::vector<ECS::EntityComponentSystem> myECSs;
	};
}