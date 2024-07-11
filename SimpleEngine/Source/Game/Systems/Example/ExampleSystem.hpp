#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	/* NOTE (v11.3.0)
	*
	* Go to Game/Systems/AddSystemHere/AllGameSystems.hpp
	*
	* Add:	 Game/Systems/Example/ExampleSystem.hpp
	*/

	class ExampleSystem final : public ECS::System
	{
	public:
		ExampleSystem(EntityComponentSystem* aEntityComponentSystem);
		~ExampleSystem() override;

		void Init() override;
		void Update() override;
		void Render() override;

		void EarlyUpdate() override;
		void FixedUpdate() override;
		void LateUpdate() override;

		virtual std::unique_ptr<System> Clone(EntityComponentSystem* aEntityComponentSystem) const override;
	};
}