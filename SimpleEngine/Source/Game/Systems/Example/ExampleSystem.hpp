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
		/*
		* System-constructor require to pass EntityComponentSystem pointer but you can also add more parameters to this class itself
		* Example: ExampleSystem(EntityComponentSystem* aEntityComponentSystem, int aValue);
		*/

		ExampleSystem(EntityComponentSystem* aEntityComponentSystem);

		/*
		* These functions below are pure-virtual which is required for every System
		*/

		void Update() override;
		virtual std::unique_ptr<System> Clone(EntityComponentSystem* aEntityComponentSystem) const override;

		/*
		* These virtual functions below doesn't have to be implemented if not needed
		*/

		~ExampleSystem() override;

		void Init() override;
		void Render() override;

		void EarlyUpdate() override;
		void FixedUpdate() override;
		void LateUpdate() override;
	};
}