#include "ScriptComponent.h"
#include "Game/GameUpdateContext.h"
#include "Game/GameLevel.h"
#include "SimpleScript/Core/ScriptManager.h"
#include "SimpleScript/SimpleGameNodes.h"
#include "SimpleScript/Core/Node/NodeTypeRegistry.h"
#include "SimpleScript/Core/DataType/DataTypeRegistry.h"
#include "SimpleScript/SimpleGameNodes.h"
#include "GameObjects/Base/GameObject.h"
#include "SpriteComponent.h"
#include <imgui.h>

ScriptComponent::~ScriptComponent()
{
}

void ScriptComponent::Init(const GameInitContext& aContext)
{
	SimpleOwnerContext<ScriptComponent> context
	{
		0.f,
		aContext.gameLevel,
		*aContext.game,
		this
	};

	if (myScript)
	{
		myScript->TriggerEvent(SCRIPT::eNodeExecutionTrait::BeginPlay, context);
	}
}

void ScriptComponent::Update(const GameUpdateContext& aContext)
{
	SimpleOwnerContext<GameObject> context
	{
		aContext.deltaTime,
		aContext.gameLevel,
		*aContext.game,
		myGameObject
	};

	if (myScript)
	{
		myScript->TriggerEvent(SCRIPT::eNodeExecutionTrait::Tick, context);
	}
}

static std::tuple<SCRIPT::Flow, GameObject*> EventInteractNode(SCRIPT::NodeExecutionContext<SimpleOwnerContext<GameObject>> aContext)
{
	return { true, aContext.context.interaction };
}

InteractionResult ScriptComponent::Interact(const GameUpdateContext& aContext, GameObject* aGameObject)
{
	SimpleOwnerContext<GameObject> context
	{
		aContext.deltaTime,
		aContext.gameLevel,
		*aContext.game,
		myGameObject,
		aGameObject
	};

	myScript->TriggerEvent(SCRIPT::eNodeExecutionTrait::Interact , context);

	return myResult;
}

void ScriptComponent::SetScript(SCRIPT::Script* aScript)
{
	myScript = aScript;
}

static void Internal_SetSpriteNode(SCRIPT::NodeExecutionContext<SimpleOwnerContext<GameObject>> aContext, SpriteTypeData aSpriteType)
{
	if (SpriteComponent* spriteComponent = aContext.context.currentOwner->GetComponent<SpriteComponent>())
	{
		spriteComponent->mySpriteID = aSpriteType.id;
	}
}

static void Internal_SetIsPushable(SCRIPT::NodeExecutionContext<SimpleOwnerContext<GameObject>> aContext, bool aBool)
{
	aContext.context.currentOwner->myCanPush = aBool;
}

static void Internal_SetCanStandOn(SCRIPT::NodeExecutionContext<SimpleOwnerContext<GameObject>> aContext, bool aBool)
{
	aContext.context.currentOwner->myCanStandOn = aBool;
}

static void Internal_DestroySelf(SCRIPT::NodeExecutionContext<SimpleOwnerContext<GameObject>> aContext)
{
	aContext.context.gameLevel.gameObjectManager.RemoveGameObject(aContext.context.currentOwner->myID);
}

struct InteractionResultData
{
	InteractionResult resultType;
};

static bool Edit(InteractionResultData& aData)
{
	int currentID = static_cast<int>(aData.resultType);
	if (ImGui::Combo("##", &currentID, "NoInteraction\0MoveOntoTile\0Stay"))
	{
		aData.resultType = static_cast<InteractionResult>(currentID);
		return true;
	}
	return false;
}

static void Save(nlohmann::json& aJson, const InteractionResultData& aData)
{
	aJson["type"] = aData.resultType;
}

static void Load(const nlohmann::json& aJson, InteractionResultData& aData)
{
	aData.resultType = aJson["type"];
}

static void Internal_SetInteractionResult(SCRIPT::NodeExecutionContext<SimpleOwnerContext<GameObject>> aContext, InteractionResultData aResult)
{
	aContext.context.currentOwner->GetComponent<ScriptComponent>()->myResult = aResult.resultType;
}


void ScriptComponent::RegisterInternalScriptNodes()
{
	SCRIPT::DataTypeRegistry::Register<InteractionResultData>("Interaction Result");

	SCRIPT::NodeTypeRegistry::RegisterFlowNodeType(Internal_SetSpriteNode, "Game/GameObject/Internal/Set Sprite");
	SCRIPT::NodeTypeRegistry::RegisterFlowNodeType(Internal_SetIsPushable, "Game/GameObject/Internal/Set Pushable");
	SCRIPT::NodeTypeRegistry::RegisterFlowNodeType(Internal_SetCanStandOn, "Game/GameObject/Internal/Set Can Stand On");
	SCRIPT::NodeTypeRegistry::RegisterNodeType<SCRIPT::eNodeExecutionTrait::Interact>(EventInteractNode, "Game/GameObject/Event Interact");
	SCRIPT::NodeTypeRegistry::RegisterFlowNodeType(Internal_DestroySelf, "Game/GameObject/Internal/Delete This");
	SCRIPT::NodeTypeRegistry::RegisterFlowNodeType(Internal_SetInteractionResult, "Game/GameObject/Internal/Set Event Interaction Result");
}