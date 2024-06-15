#pragma once

namespace ECS
{
	class EntityComponentSystem;

	class System
	{
	public:
		System(EntityComponentSystem& aEntityComponentSystem): myEntityComponentSystem(aEntityComponentSystem) {};

		virtual ~System() {};

		virtual void Init() {};
		virtual void Update() = 0;
		virtual void Render() = 0;

		virtual void EarlyUpdate() {};
		virtual void FixedUpdate() {};
		virtual void LateUpdate() {};

	protected:
		EntityComponentSystem& myEntityComponentSystem;
	};
}