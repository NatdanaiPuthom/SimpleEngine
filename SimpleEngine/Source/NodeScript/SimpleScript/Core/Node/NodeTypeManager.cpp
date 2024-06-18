#include "Core/Node/NodeTypeManager.h"
#include "Core/Script.h"
#include "Core/ScriptFoundation.h"
#include "Core/ScriptManager.h"
#include "Core/Utilities/ScriptUtilities.h"
#include "Core/Node/NodeTypeRegistry.h"

namespace SCR
{
	std::vector<CustomEvent> NodeTypeManager::myCustomEvents = {};
	std::vector<Function*> NodeTypeManager::myFunctions = {};
	std::unordered_map<DataTypeID, NodeTypeID> NodeTypeManager::myGetterNodeTypeIDs = {};
	std::unordered_map<DataTypeID, NodeTypeID> NodeTypeManager::mySetterNodeTypeIDs = {};
	std::unordered_multimap<NodeTypeID, CustomEventID> NodeTypeManager::myToCustomEventID = {};
	std::unordered_multimap<NodeTypeID, CustomEventID> NodeTypeManager::myToFunctionID = {};
	std::unordered_map<eNodeOperatorTrait, std::unordered_map<DataTypeID, NodeTypeID>> NodeTypeManager::myOperatorNodeTypeIDs = {};
	std::vector<NodeType> NodeTypeManager::myNodeTypes = { CreateInvalidNodeType() };

	NodeTypeID NodeTypeManager::Register(NodeType&& aNodeType)
	{
		NodeTypeID id = myNodeTypes.size();
		myNodeTypes.emplace_back(std::move(aNodeType));
		Assert();

		return id;
	}

	void NodeTypeManager::SetGetterNodeTypeID(const DataTypeID aDataTypeID, const NodeTypeID anID)
	{
		myGetterNodeTypeIDs.emplace(aDataTypeID, anID);
	}

	void NodeTypeManager::SetSetterNodeTypeID(const DataTypeID aDataTypeID, const NodeTypeID anID)
	{
		mySetterNodeTypeIDs.emplace(aDataTypeID, anID);
	}

	void NodeTypeManager::SetOperatorNodeTypeID(const DataTypeID aDataTypeID, const eNodeOperatorTrait anOperatorTrait, const NodeTypeID anID)
	{
		myOperatorNodeTypeIDs[anOperatorTrait].emplace(aDataTypeID, anID);
	}

	Node NodeTypeManager::CreateInstance_Getter(NodeGraph& aNodeGraph, const NodeID aNodeID, const DataTypeID aDataTypeID)
	{
		NodeTypeID typeID = myGetterNodeTypeIDs.at(aDataTypeID);
		return CreateInstance(aNodeGraph, aNodeID, typeID);
	}

	Node NodeTypeManager::CreateInstance_Setter(NodeGraph& aNodeGraph, const NodeID aNodeID, const DataTypeID aDataTypeID)
	{
		NodeTypeID typeID = mySetterNodeTypeIDs.at(aDataTypeID);
		return CreateInstance(aNodeGraph, aNodeID, typeID);
	}

	Node NodeTypeManager::CreateInstance_Operator(NodeGraph& aNodeGraph, const NodeID aNodeID, const eNodeOperatorTrait aOperatorTrait, const DataTypeID aDataTypeID)
	{
		const std::unordered_map<size_t, NodeTypeID>& operatorNodes = myOperatorNodeTypeIDs.at(aOperatorTrait);
		NodeTypeID typeID = operatorNodes.at(aDataTypeID);
		return CreateInstance(aNodeGraph, aNodeID, typeID);
	}

	Node NodeTypeManager::CreateInstance(NodeGraph& aNodeGraph, const NodeID aNodeID, const NodeTypeID aNodeTypeID)
	{
		return myNodeTypes.at(aNodeTypeID).nodeRecipe.createFunction(aNodeID, aNodeTypeID, aNodeGraph);
	}

	bool NodeTypeManager::CanCreateOperatorNode(const eNodeOperatorTrait aTrait, const DataTypeID aDataTypeID)
	{
		if (myOperatorNodeTypeIDs.contains(aTrait))
		{
			return myOperatorNodeTypeIDs.at(aTrait).contains(aDataTypeID);
		}
		return false;
	}

	NodeType& NodeTypeManager::GetNodeType(const NodeTypeID anID)
	{
		return myNodeTypes.at(anID);
	}

	const std::vector<NodeType>& NodeTypeManager::GetNodeTypes()
	{
		return myNodeTypes;
	}

	CustomEvent& NodeTypeManager::GetCustomEvent(const CustomEventID anID)
	{
		return myCustomEvents.at(anID);
	}

	const std::vector<CustomEvent>& NodeTypeManager::GetCustomEvents()
	{
		return myCustomEvents;
	}

	CustomEventID NodeTypeManager::GetCustomEventID(const NodeTypeID aNodeTypeID)
	{
		auto it = myToCustomEventID.find(aNodeTypeID);
		if (it != myToCustomEventID.end())
		{
			return it->second;
		}
		return InvalidID<CustomEventID>();
	}

	Function& NodeTypeManager::GetFunction(const FunctionID anID)
	{
		return *myFunctions.at(anID);
	}

	const std::vector<Function*>& NodeTypeManager::GetFunctions()
	{
		return myFunctions;
	}

	FunctionID NodeTypeManager::GetFunctionID(const NodeTypeID aNodeTypeID)
	{
		auto it = myToFunctionID.find(aNodeTypeID);
		if (it != myToFunctionID.end())
		{
			return it->second;
		}
		return InvalidID<FunctionID>();
	}

	NodeTypeID NodeTypeManager::GetTypeID(const std::string& aName)
	{
		for (NodeTypeID id = 0; id < myNodeTypes.size(); ++id)
		{
			if (GetShortName(id) == aName)
			{
				return id;
			}
		}
		return 0;
	}

	const std::string& NodeTypeManager::GetFullName(const NodeTypeID anID)
	{
		return myNodeTypes.at(anID).name;
	}

	std::string NodeTypeManager::GetShortName(const NodeTypeID anID)
	{
		const std::string& fullName = GetFullName(anID);
		if (fullName.find_last_of('/') != std::string::npos)
		{
			return fullName.substr(fullName.find_last_of('/') + 1, fullName.length());

		}
		else
		{
			return fullName;
		}
	}

	std::string NodeTypeManager::GetNameDirectory(const NodeTypeID anID)
	{
		const std::string& fullName = GetFullName(anID);
		if (fullName.find_last_of('/') != std::string::npos)
		{
			return fullName.substr(0, fullName.find_last_of('/') + 1);

		}
		else
		{
			return fullName;
		}
	}

	NodeType NodeTypeManager::CreateInvalidNodeType()
	{
		NodeRecipe recipe
		{
			[](const NodeID, const NodeTypeID, NodeGraph&)->Node {return Node(0, std::array<PinID, 0>(), std::array<PinID, 0>()); },
			[](const NodeExecutionData&, InternalExecutionContext&) {},
			eNodeTrait::Invalid
		};

		return { recipe, "Invalid Type" };
	}


	void NodeTypeManager::Assert()
	{
		std::unordered_set<std::string> shortNames;
		for (NodeTypeID id = 0; id < myNodeTypes.size(); ++id)
		{
			std::string shortName = GetShortName(id);

			if (!shortNames.insert(shortName).second)
			{
				throw std::runtime_error("Cannot have nodes with same name" + shortName);
			}
		};
	}

	void NodeTypeManager::Destroy()
	{
		for (Function* function : myFunctions)
		{
			delete function;
		}

	
		myNodeTypes.clear();
		myCustomEvents.clear();
		myFunctions.clear();
		myGetterNodeTypeIDs.clear();
		mySetterNodeTypeIDs.clear();
		myOperatorNodeTypeIDs.clear();
		myToCustomEventID.clear();
	}
}