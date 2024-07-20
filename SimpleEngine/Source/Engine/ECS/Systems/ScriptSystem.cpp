#include "Engine/Precomplied/EnginePch.hpp"
#include "ScriptSystem.hpp"
#include "NodeScript/SimpleScript/Core/Global/ScriptGlobal.hpp"
#include "NodeScript/SimpleScript/Core/Node/NodeExecutor.hpp"
#include "NodeScript/SimpleScript/Core/NodeTypes/ExecutionNodes.hpp"

namespace ECS
{
	ScriptSystem::ScriptSystem()
	{
	}

	void ScriptSystem::Update(EntityComponentSystem* aEntityComponentSystem)
	{
		auto& entityIDs = aEntityComponentSystem->GetEntityIDsWithThisComponent<ScriptComponent>();

		for (auto& entityID : entityIDs)
		{
			ECS::Entity& entity = aEntityComponentSystem->GetEntity(entityID);

			ScriptComponent* const scriptComponent = entity.GetComponent<ScriptComponent>();

			const SCRIPT::ExecutionContextBase executionContext
			{
				.mDeltaTime = Global::GetDeltaTime()
			};

			SCRIPT::Global::GetNodeExecutor().ExecuteEvent(SCRIPT::Tick, *scriptComponent->classInstance, &entity, executionContext, true);
		}
	}

	std::unique_ptr<System> ScriptSystem::Clone() const
	{
		return std::make_unique<ScriptSystem>();
	}
}

