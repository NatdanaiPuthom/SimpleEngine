#pragma once
#include <memory>

namespace ECS
{
	class EntityComponentSystem;

	class System
	{
	public:
		System() {};

		virtual ~System() {};

		virtual void Init(EntityComponentSystem* /*aEntityComponentSystem*/) {};
		virtual void Update(EntityComponentSystem* /*aEntityComponentSystem*/) {};
		virtual void Render(EntityComponentSystem* /*aEntityComponentSystem*/) {};

		virtual void EarlyUpdate(EntityComponentSystem* /*aEntityComponentSystem*/) {};
		virtual void FixedUpdate(EntityComponentSystem* /*aEntityComponentSystem*/) {};
		virtual void LateUpdate(EntityComponentSystem* /*aEntityComponentSystem*/) {};

		virtual void EarlyRender(EntityComponentSystem* /*aEntityComponentSystem*/) {};
		virtual void LateRender(EntityComponentSystem* /*aEntityComponentSystem*/) {};

		virtual std::unique_ptr<System> Clone() const = 0;
	};
}