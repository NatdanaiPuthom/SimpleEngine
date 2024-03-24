#pragma once
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>

namespace ECS
{
	class System
	{
	public:
		System() {};
		virtual ~System() {}

		virtual void Init() {}
		virtual void Update() = 0;
		virtual void Render() = 0;

		virtual void EarlyUpdate() {}
		virtual void FixedUpdate() {}
		virtual void LateUpdate() {}
	};

	class SystemManager final
	{
	public:
		SystemManager();
		~SystemManager();

		void Init();

		void AddSystem(std::unique_ptr<System> aSystem);
		void Update();
		void Render();

		template<typename T>
		T* GetSystem();
		
	private:
		const float myFixedUpdate;
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
			return (T*)it->second;
		}

		return nullptr;
	}
}