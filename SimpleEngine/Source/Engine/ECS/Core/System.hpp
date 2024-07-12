#pragma once
#include <memory>

namespace ECS
{
	class EntityComponentSystem;

	class System
	{
	public:
		System(EntityComponentSystem* aEntityComponentSystem): myEntityComponentSystem(aEntityComponentSystem) {};

		virtual ~System() {};

		virtual void Init() {};
		virtual void Update() = 0;
		virtual void Render() {};

		virtual void EarlyUpdate() {};
		virtual void FixedUpdate() {};
		virtual void LateUpdate() {};

		virtual std::unique_ptr<System> Clone(EntityComponentSystem* aEntityComponentSystem) const = 0;

	protected:
		EntityComponentSystem* myEntityComponentSystem;
	};
}