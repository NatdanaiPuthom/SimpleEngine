#include "SimpleGameNodes.h"
#include "SimpleScript/Core/Node/NodeTypeRegistry.h"
#include "SimpleScript/Core/DataType/DataTypeRegistry.h"

#include "Game/Game.h"
#include "Game/GameLevel.h"
#include "GameObjects/Base/GameObject.h"

static bool Edit(SpriteTypeData& aData)
{
	int currentID = static_cast<int>(aData.id);
	if (ImGui::Combo("##", &currentID, GetSpriteNames()))
	{
		aData.id = static_cast<SpriteId>(currentID);
		return true;
	}
	return false;
}

static void Save(nlohmann::json& aJson, const SpriteTypeData& aData)
{
	aJson["type"] = aData.id;
}

static void Load(const nlohmann::json& aJson, SpriteTypeData& aData)
{
	aData.id = aJson["type"];
}


using GameObjectPredicate = std::function<bool(GameObject*)>;

static GameObjectPredicate operator&&(const GameObjectPredicate& aPred1, const GameObjectPredicate& aPred2)
{
	return [aPred1, aPred2](GameObject* aGameObject) -> bool
		{
			return (aPred1.operator bool() ? aPred1(aGameObject) : false) && (aPred2.operator bool() ? aPred2(aGameObject) : false);
		};
}

static GameObjectPredicate operator||(const GameObjectPredicate& aPred1, const GameObjectPredicate& aPred2)
{
	return [aPred1, aPred2](GameObject* aGameObject) -> bool
		{
			return (aPred1.operator bool() ? aPred1(aGameObject) : false) || (aPred2.operator bool() ? aPred2(aGameObject) : false);
		};
}

static GameObjectPredicate operator!(const GameObjectPredicate& aPred)
{
	return [aPred](GameObject* aGameObject) -> bool
		{
			return aPred.operator bool() ? !aPred(aGameObject) : false;
		};
}

void RegisterSimpleDataTypes()
{
	SCRIPT::DataTypeRegistry::RegisterNonSerializableType<GameObject*, SCRIPT::eNodeOperatorTrait::Equal | SCRIPT::eNodeOperatorTrait::NotEqual>("GameObject", SCRIPT::ScriptColor{ 0.42f, 0.72f, 0.85f });
	SCRIPT::DataTypeRegistry::RegisterNonSerializableType<GameObjectPredicate, SCRIPT::eNodeOperatorTrait::Logical>("GameObject Predicate", SCRIPT::ScriptColor{ 0.42f, 0.72f, 0.17f });
	SCRIPT::DataTypeRegistry::Register<SpriteTypeData>("Sprite Type", SCRIPT::ScriptColor{ 1.f, 0.44f, 0.37f });
}

static bool IsGameObjectActive(GameObject* aGameObject)
{
	return aGameObject && aGameObject->myIsActive;
}

static void DeleteThisGameObjectNode(SCRIPT::NodeExecutionContext<SimpleGameContext> aContext, GameObject* aGameObject)
{
	if (!IsGameObjectActive(aGameObject))
	{
		return;
	}
	aContext.context.gameLevel.gameObjectManager.RemoveGameObject(aGameObject->myID);
}

static std::tuple<SCRIPT::Flow, SCRIPT::Flow, GameObject*> GameObjectPredicateTriggerNode(SCRIPT::NodeExecutionContext<GameUpdateContext> aContext, GameObjectPredicate aPredicate)
{
	if (!aPredicate.operator bool())
	{
		return { false, false, nullptr };
	}

	for (GameObject* gameObject : aContext.context.gameLevel.gameObjectManager.GetAllGameObjects())
	{
		if (aPredicate(gameObject))
		{
			return { true, false, gameObject };
		}
	}

	return { false, true, nullptr };
}

static bool EvaluateGameObjectPredicate(GameObject* aGameObject, GameObjectPredicate aPredicate)
{
	if (!IsGameObjectActive(aGameObject) || !aPredicate.operator bool())
	{
		return false;
	}

	return aPredicate(aGameObject);
}

static std::tuple<bool, GameObject*> EvaluateGameObjectVectorPredicate(std::vector<GameObject*> aGameObject, GameObjectPredicate aPredicate)
{
	if (!aPredicate.operator bool())
	{
		return { false, nullptr };
	}

	for (GameObject* gameObject : aGameObject)
	{
		if (aPredicate(gameObject))
		{
			return { true, gameObject };
		}
	}


	return { false, nullptr };
}

static GameObjectPredicate CanPushPredicateNode()
{
	return [](GameObject* aGameObject) -> bool
		{
			return IsGameObjectActive(aGameObject) ? aGameObject->myCanPush : false;
		};
}

static GameObjectPredicate GameObjectNameEqualPredicateNode(const std::string aName)
{
	return [aName](GameObject* aGameObject) -> bool
		{
			return IsGameObjectActive(aGameObject) ? aGameObject->myName == aName : false;
		};
}

static GameObjectPredicate IsGameObjectEqualPredicateNode(GameObject* aGameObject)
{
	return [aGameObject](GameObject* aGameObject2) -> bool
		{
			return aGameObject == aGameObject2;
		};
}

static std::vector<GameObject*> GetAllGameObjectsNode(SCRIPT::NodeExecutionContext<SimpleGameContext> aContext)
{
	return aContext.context.gameLevel.gameObjectManager.GetAllGameObjects();
}

static std::vector<GameObject*> GameObjectFilterNode(std::vector<GameObject*> aGameObjects, GameObjectPredicate aPredicate)
{
	std::vector<GameObject*> gameObjects;
	if (!aPredicate.operator bool())
	{
		return gameObjects;
	}
	for (GameObject* gameObject : aGameObjects)
	{
		if (gameObject->myIsActive && aPredicate(gameObject))
		{
			gameObjects.push_back(gameObject);
		}
	}

	return gameObjects;
}

static GameObject* GetGameObjectAtIndexNode(std::vector<GameObject*> aGameObjects, int anIndex)
{
	if (anIndex < 0 || anIndex >= aGameObjects.size())
	{
		return nullptr;
	}

	return aGameObjects[anIndex];
}

struct GameObjectCounterData
{
	int index = -1;
};

static GameObject* GetGameObjectFromVectorNode(SCRIPT::NodeState<GameObjectCounterData> aState, std::vector<GameObject*> aGameObjects)
{

	++aState.value.index;

	if (aState.value.index >= aGameObjects.size())
	{
		aState.value.index = -1;
		return nullptr;
	}

	return aGameObjects[aState.value.index];
}

static void CreateQuest(SCRIPT::NodeExecutionContext<SimpleGameContext> aContext, std::string aQuestName, std::string aInstruction, const int aMaxCount)
{
	Objective quest;
	quest.instruction = aInstruction;
	quest.maxCounter = aMaxCount;

	aContext.context.game.myObjectives.emplace(aQuestName, quest);
}

static void SetQuestDone(SCRIPT::NodeExecutionContext<SimpleGameContext> aContext, std::string aQuestName, const bool aShouldBeDone)
{
	auto it = aContext.context.game.myObjectives.find(aQuestName);

	if (it != aContext.context.game.myObjectives.end())
	{
		it->second.isDone = aShouldBeDone;
	}
}

static Objective* GetQuest(SCRIPT::NodeExecutionContext<SimpleGameContext> aContext, std::string aQuestName)
{
	if (aContext.context.game.myObjectives.contains(aQuestName))
	{
		return &aContext.context.game.myObjectives.at(aQuestName);
	}
	return nullptr;
}

template<typename T>
std::string ToString(T a)
{
	return std::to_string(a);
}

static void SetScriptOfScriptComponentNode(SCRIPT::NodeExecutionContext<SimpleGameContext> /*aContext*/, GameObject* aGameObject, const std::string /*aName*/)
{
	if (!IsGameObjectActive(aGameObject))
	{
		return;
	}
}

void RegisterSimpleGameNodes()
{
	SCRIPT::NodeTypeRegistry::RegisterFlowNodeType(DeleteThisGameObjectNode, "Game/GameObject/Delete This GameObject");

	SCRIPT::NodeTypeRegistry::RegisterNodeType<SCRIPT::eNodeExecutionTrait::Tick>(GameObjectPredicateTriggerNode, "Game/GameObject/Predicate/GameObject Predicate Trigger", SCRIPT::NodeTypeDesc{ {}, { "True", "False" } });
	SCRIPT::NodeTypeRegistry::RegisterNodeType(EvaluateGameObjectPredicate, "Game/GameObject/Predicate/Evaluate GameObject Predicate", SCRIPT::NodeTypeDesc{ {  } });
	SCRIPT::NodeTypeRegistry::RegisterNodeType(EvaluateGameObjectVectorPredicate, "Game/GameObject/Predicate/Evaluate GameObject Vector Predicate", SCRIPT::NodeTypeDesc{ { "GameObject Vector"} });
	SCRIPT::NodeTypeRegistry::RegisterNodeType(CanPushPredicateNode, "Game/GameObject/Predicate/Can GameObject Push (Predicate)");
	SCRIPT::NodeTypeRegistry::RegisterNodeType(GameObjectNameEqualPredicateNode, "Game/GameObject/Predicate/GameObject Name Equal (Predicate)");
	SCRIPT::NodeTypeRegistry::RegisterNodeType(IsGameObjectEqualPredicateNode, "Game/GameObject/Predicate/Is GameObject Equal (Predicate)");
	SCRIPT::NodeTypeRegistry::RegisterNodeType(GetAllGameObjectsNode, "Game/GameObject/Get All GameObjects", SCRIPT::NodeTypeDesc{ {}, { "GameObject Vector" } });
	SCRIPT::NodeTypeRegistry::RegisterFlowNodeType(GameObjectFilterNode, "Game/GameObject/GameObject Filter", SCRIPT::NodeTypeDesc{ { "Flow", "GameObject Vector" }, { "Flow", "GameObject Vector" } });
	SCRIPT::NodeTypeRegistry::RegisterNodeType(GetGameObjectAtIndexNode, "Game/GameObject/Get GameObject At Index", SCRIPT::NodeTypeDesc{ { "GameObject Vector", "Index" } });
	SCRIPT::NodeTypeRegistry::RegisterFlowNodeType(GetGameObjectFromVectorNode, "Game/GameObject/Get GameObject From Vector", SCRIPT::NodeTypeDesc{ { "Flow", "GameObject Vector" } });
	SCRIPT::NodeTypeRegistry::RegisterFlowNodeType(SetScriptOfScriptComponentNode, "Game/GameObject/Set Script By Name", SCRIPT::NodeTypeDesc{ { "Flow", "GameObject Vector" } });

	SCRIPT::NodeTypeRegistry::RegisterMemberVariable(&GameObject::myCanPush, "Game/GameObject/Members/", "GameObject - Can Push");
	SCRIPT::NodeTypeRegistry::RegisterMemberVariable(&GameObject::myCanStandOn, "Game/GameObject/Members/", "GameObject - Can Stand On");
	SCRIPT::NodeTypeRegistry::RegisterMemberVariable(&GameObject::myName, "Game/GameObject/Members/", "GameObject - Name");
	SCRIPT::NodeTypeRegistry::RegisterMemberVariable(&GameObject::myID, "Game/GameObject/Members/", "GameObject - ID");
	SCRIPT::NodeTypeRegistry::RegisterMemberVariable(&GameObject::myPosition, "Game/GameObject/Members/", "GameObject - Position");
	SCRIPT::NodeTypeRegistry::RegisterMemberVariable(&GameObject::myKeyCount, "Game/GameObject/Members/", "GameObject - Key Count");

	SCRIPT::NodeTypeDesc questPinNames;
	questPinNames.inputPinNames.push_back("#T");
	questPinNames.inputPinNames.push_back("Name");
	questPinNames.inputPinNames.push_back("Instruction");
	questPinNames.inputPinNames.push_back("MaxCount");

	SCRIPT::NodeTypeRegistry::RegisterFlowNodeType(CreateQuest, "Game/Quest/Create Quest", questPinNames);

	SCRIPT::NodeTypeDesc questSet;
	questSet.inputPinNames.push_back("Flow");
	questSet.inputPinNames.push_back("Name");
	questSet.inputPinNames.push_back("Done");

	SCRIPT::NodeTypeRegistry::RegisterMemberVariable(&Objective::isDone, "Game/Quest/Members/", "Quest - Is Done");
	SCRIPT::NodeTypeRegistry::RegisterMemberVariable(&Objective::counter, "Game/Quest/Members/", "Quest - Counter");
	SCRIPT::NodeTypeRegistry::RegisterMemberVariable(&Objective::maxCounter, "Game/Quest/Members/", "Quest - MaxCounter");

	SCRIPT::NodeTypeRegistry::RegisterFlowNodeType(SetQuestDone, "Game/Quest/Set Quest Done", questSet);

	SCRIPT::NodeTypeDesc objectiveNames;
	objectiveNames.inputPinNames.push_back("Name");
	objectiveNames.outputPinNames.push_back("Quest");

	SCRIPT::NodeTypeRegistry::RegisterNodeType(GetQuest, "Game/Quest/Get Quest", objectiveNames);
	SCRIPT::NodeTypeRegistry::RegisterNodeType(ToString<int>, "Utility/String/To String (Int)");
}