#include "Node/FlyNodeTypeManager.hpp"
#include <array>

namespace FLY_NAMESPACE
{

	NodeTypeManager::NodeTypeManager()
	{
	}

	NodeTypeManager::~NodeTypeManager()
	{
		
	}

	NodeTypeID NodeTypeManager::Register(NodeType&& aNodeType)
	{
		NodeTypeID id{ mNodeTypes.size() };
		mNodeTypes.emplace_back(std::move(aNodeType));
		Assert();

		return id;
	}

	void NodeTypeManager::SetGetterNodeTypeID(const DataTypeID aDataTypeID, const NodeTypeID anID)
	{
		mGetterNodeTypeIDs.emplace(aDataTypeID, anID);
	}

	void NodeTypeManager::SetSetterNodeTypeID(const DataTypeID aDataTypeID, const NodeTypeID anID)
	{
		mSetterNodeTypeIDs.emplace(aDataTypeID, anID);
	}

	void NodeTypeManager::SetOperatorNodeTypeID(const DataTypeID aDataTypeID, const eNodeOperatorTrait aOperatorTrait, const NodeTypeID aID)
	{
		mTemplateNodeTypeIDMap[aOperatorTrait].emplace(aDataTypeID, aID);
	}

	Node NodeTypeManager::CreateGetterNode(NodeGraph& aNodeGraph, const NodeID aNodeID, const DataTypeID aDataTypeID)
	{
		const NodeTypeID typeID = mGetterNodeTypeIDs.at(aDataTypeID);
		return CreateNode(aNodeGraph, aNodeID, typeID);
	}

	Node NodeTypeManager::CreateSetterNode(NodeGraph& aNodeGraph, const NodeID aNodeID, const DataTypeID aDataTypeID)
	{
		const NodeTypeID typeID = mSetterNodeTypeIDs.at(aDataTypeID);
		return CreateNode(aNodeGraph, aNodeID, typeID);
	}

	Node NodeTypeManager::CreateOperatorNode(NodeGraph& aNodeGraph, const NodeID aNodeID, const eNodeOperatorTrait aOperatorTrait, const DataTypeID aDataTypeID)
	{
		const std::unordered_map<DataTypeID, NodeTypeID>& operatorNodes = mTemplateNodeTypeIDMap.at(aOperatorTrait);
		const NodeTypeID typeID = operatorNodes.at(aDataTypeID);
		return CreateNode(aNodeGraph, aNodeID, typeID);
	}

	Node NodeTypeManager::CreateNode(NodeGraph& aNodeGraph, const NodeID aNodeID, const NodeTypeID aNodeTypeID)
	{
		return mNodeTypes.at(aNodeTypeID).mNodeRecipe.mCreateFunction(aNodeID, aNodeTypeID, aNodeGraph);
	}

	bool NodeTypeManager::CanCreateOperatorNode(const eNodeOperatorTrait aTrait, const DataTypeID aDataTypeID)
	{
		if (mTemplateNodeTypeIDMap.contains(aTrait))
		{
			return mTemplateNodeTypeIDMap.at(aTrait).contains(aDataTypeID);
		}
		return false;
	}

	NodeType& NodeTypeManager::GetNodeType(const NodeTypeID aID)
	{
		return mNodeTypes.at(aID);
	}

	const NodeType& NodeTypeManager::GetNodeType(const NodeTypeID aID) const
	{
		return mNodeTypes.at(aID);
	}

	const std::vector<NodeType>& NodeTypeManager::GetNodeTypes()
	{
		return mNodeTypes;
	}

	CustomEvent& NodeTypeManager::GetCustomEvent(const CustomEventID aID)
	{
		return mCustomEvents.at(aID);
	}

	const CustomEvent& NodeTypeManager::GetCustomEvent(const CustomEventID aID) const
	{
		return mCustomEvents.at(aID);
	}

	std::vector<CustomEvent>& NodeTypeManager::GetCustomEvents()
	{
		return mCustomEvents;
	}

	const std::vector<CustomEvent>& NodeTypeManager::GetCustomEvents() const
	{
		return mCustomEvents;
	}

	CustomEventID NodeTypeManager::GetCustomEventID(const NodeTypeID aNodeTypeID) const
	{
		auto it = mToCustomEventID.find(aNodeTypeID);
		if (it != mToCustomEventID.end())
		{
			return it->second;
		}
		return InvalidID<CustomEventID>();
	}

	Function& NodeTypeManager::GetFunction(const FunctionID anID)
	{
		return *mFunctions.at(anID);
	}

	const Function& NodeTypeManager::GetFunction(const FunctionID anID) const
	{
		return *mFunctions.at(anID);
	}

	const std::vector<HeapObject<Function>>& NodeTypeManager::GetFunctions()
	{
		return mFunctions;
	}

	FunctionID NodeTypeManager::GetFunctionID(const NodeTypeID aNodeTypeID) const
	{
		auto it = mToFunctionID.find(aNodeTypeID);
		if (it != mToFunctionID.end())
		{
			return it->second;
		}
		return InvalidID<FunctionID>();
	}

	NodeTypeID NodeTypeManager::GetTypeID(std::string_view aName)
	{
		for (NodeTypeID id{ 0 }; id < mNodeTypes.size(); ++id)
		{
			if (GetShortName(id) == aName)
			{
				return id;
			}
		}
		return NodeTypeID{};
	}

	const std::string& NodeTypeManager::GetFullName(const NodeTypeID anID) const
	{
		return mNodeTypes.at(anID).mNodeRecipe.mName;
	}

	std::string NodeTypeManager::GetShortName(const NodeTypeID anID) const
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

	std::string NodeTypeManager::GetNameDirectory(const NodeTypeID anID) const
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

	CustomEventID NodeTypeManager::CreateCustomEvent(std::string_view aName)
	{
		const CustomEventID id{ mCustomEvents.size() };
		const CustomEvent& customEvent = mCustomEvents.emplace_back(CustomEvent(aName));

		mToCustomEventID.emplace(customEvent.GetCallerTypeID(), id);
		mToCustomEventID.emplace(customEvent.GetExecutorTypeID(), id);

		return id;
	}

	FunctionID NodeTypeManager::CreateFunction(std::string_view aName)
	{
		const FunctionID id{ mFunctions.size() };
		const HeapObject<Function>& function = mFunctions.emplace_back(HeapObject<Function>(aName));

		mToFunctionID.emplace(function->mCallerNodeTypeID, id);
		mToFunctionID.emplace(function->mInputNodeTypeID, id);
		mToFunctionID.emplace(function->mOutputNodeTypeID, id);

		return id;
	}

	const std::unordered_map<DataTypeID, NodeTypeID>& NodeTypeManager::GetTemplateMapByOperator(const eNodeOperatorTrait aOperatorTrait) const
	{
		return mTemplateNodeTypeIDMap.at(aOperatorTrait);
	}

	NodeType NodeTypeManager::CreateInvalidNodeType()
	{
		NodeRecipe recipe
		{
			.mCreateFunction = [](const NodeID, const NodeTypeID, NodeGraph&)->Node {return Node(NodeTypeID{ 0 }, std::array<PinID, 0>(), std::array<PinID, 0>()); },
			.mExecuteFunction = [](const NodeExecutionData&, InternalExecutionContext&) {},
			.mTraits = eNodeTrait::Invalid,
			.mName = "Invalid Node"
		};

		return NodeType{ .mNodeRecipe = recipe };
	}


	void NodeTypeManager::Assert()
	{
		/*std::unordered_set<std::string> shortNames;
		for (NodeTypeID id = 0; id < myNodeTypes.size(); ++id)
		{
			std::string shortName = GetShortName(id);

			if (!shortNames.insert(shortName).second)
			{
				throw std::runtime_error("Cannot have nodes with same name" + shortName);
			}
		};*/
	}
}