#include "SimpleGameNodes.h"
#include "SimpleScript/Core/Node/NodeTypeRegistry.h"
#include "SimpleScript/Core/DataType/DataTypeRegistry.h"
#include "Game/Game.h"

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

void RegisterSimpleDataTypes()
{
	SCRIPT::DataTypeRegistry::Register<SpriteTypeData>("Sprite Type", SCRIPT::ScriptColor{ 1.f, 0.44f, 0.37f });
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

void RegisterSimpleGameNodes()
{
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