#pragma once
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	/* NOTE (v11.3.0)
	*
	* 1. Go to Game/Systems/AddSystemHere/AllGameSystems.hpp
	* 2. Include this file:	#include "Game/Systems/Example/ExampleSystem.hpp"
	* 
	*/

	class ExampleSystem final : public ECS::System
	{
	public:
		/* NOTE (v11.3.0)
		* System-constructor require to pass EntityComponentSystem pointer but you can also add more parameters to this class itself
		* Example: ExampleSystem(EntityComponentSystem* aEntityComponentSystem, int aValue);
		*/

		 ExampleSystem();

		/* NOTE (v11.3.0)
		* These functions below are pure-virtual which is required for every System
		*/

		virtual std::unique_ptr<System> Clone() const override;

		/* NOTE (v11.3.0)
		* These virtual functions below doesn't have to be implemented if not needed
		*/

		~ExampleSystem() override;

		void Init() override;
		void Update(EntityComponentSystem* aEntityComponentSystem) override;
		void Render(EntityComponentSystem* aEntityComponentSystem) override;

		void EarlyUpdate(EntityComponentSystem* aEntityComponentSystem) override;
		void FixedUpdate(EntityComponentSystem* aEntityComponentSystem) override;
		void LateUpdate(EntityComponentSystem* aEntityComponentSystem) override;
	};
}