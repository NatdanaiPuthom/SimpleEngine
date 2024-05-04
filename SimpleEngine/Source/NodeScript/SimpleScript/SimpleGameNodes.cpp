#include "SimpleGameNodes.h"
#include "SimpleScript/Core/Node/NodeTypeRegistry.h"
#include "SimpleScript/Core/DataType/DataTypeRegistry.h"
#include "Game/Game.h"

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