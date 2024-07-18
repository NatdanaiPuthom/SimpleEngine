#include "Engine/Precomplied/EnginePch.hpp"
#include "ScriptSystem.hpp"
#include "NodeScript/SimpleScript/Core/Global/ScriptGlobal.h"
#include "NodeScript/SimpleScript/Core/Node/NodeExecutor.h"
#include "NodeScript/SimpleScript/Core/NodeTypes/ExecutionNodes.h"

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
			ECS::IEntity& entity = aEntityComponentSystem->GetEntity(entityID);

			ScriptComponent* const scriptComponent = entity.GetComponent<ScriptComponent>();

			const SCRIPT::ExecutionContextBase executionContext
			{
				.deltaTime = Global::GetDeltaTime()
			};

			SCRIPT::Global::GetNodeExecutor().ExecuteEvent(SCRIPT::Tick, *scriptComponent->scriptInstance, &entity, executionContext, true);
		}
	}

	std::unique_ptr<System> ScriptSystem::Clone() const
	{
		return std::make_unique<ScriptSystem>();
	}
}

