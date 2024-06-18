#pragma once
#include <vector>

class MainSingleton;

namespace ECS
{
	class EntityComponentSystem;
}

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

		void Destroy();
	private:
		std::vector<ECS::EntityComponentSystem> myECSs;
	};
}