#pragma once
#include "Engine/ECS/Core/System.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <typeindex>

namespace ECS
{
	class SystemManager final
	{
	public:
		SystemManager();
		~SystemManager();

		void Init();
		void Update();
		void Render();

		void AddSystem(std::unique_ptr<System> aSystem);

		template<typename T>
		T* GetSystem();
	private:
		const float myFixedUpdateTime;
		float myTimer;
		std::unordered_map<std::type_index, System*> mySystemPointer;
		std::vector<std::unique_ptr<System>> mySystems;
	};

	template<typename T>
	inline T* SystemManager::GetSystem()
	{
		auto it = mySystemPointer.find(typeid(T));

		if (it != mySystemPointer.end())
		{
			return reinterpret_cast<T*>(it->second);
		}

		return nullptr;
	}
}