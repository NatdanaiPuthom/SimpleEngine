#include "Core/Node/NodeTypeManager.hpp"
#include "Core/Node/NodeTypeRegistry.hpp"
#include "Core/Type/ScriptTypeManager.hpp"

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
		NodeTypeID id = mNodeTypes.size();
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
		mOperatorNodeTypeIDs[aOperatorTrait].emplace(aDataTypeID, aID);
	}

	Node NodeTypeManager::CreateGetterNode(NodeGraph& aNodeGraph, const NodeID aNodeID, const DataTypeID aDataTypeID)
	{
		const NodeTypeID typeID = mGetterNodeTypeIDs.at(aDataTypeID);
		return CreateNode(aNodeGraph, aNodeID, typeID);
	}

	Node NodeTypeManager::CreateSetterNode(NodeGraph& aNodeGraph, const NodeID aNodeID, const DataTypeID aDataTypeID)
	{
		NodeTypeID typeID = mSetterNodeTypeIDs.at(aDataTypeID);
		return CreateNode(aNodeGraph, aNodeID, typeID);
	}

	Node NodeTypeManager::CreateOperatorNode(NodeGraph& aNodeGraph, const NodeID aNodeID, const eNodeOperatorTrait aOperatorTrait, const DataTypeID aDataTypeID)
	{
		const std::unordered_map<size_t, NodeTypeID>& operatorNodes = mOperatorNodeTypeIDs.at(aOperatorTrait);
		const NodeTypeID typeID = operatorNodes.at(aDataTypeID);
		return CreateNode(aNodeGraph, aNodeID, typeID);
	}

	Node NodeTypeManager::CreateNode(NodeGraph& aNodeGraph, const NodeID aNodeID, const NodeTypeID aNodeTypeID)
	{
		return mNodeTypes.at(aNodeTypeID).mNodeRecipe.mCreateFunction(aNodeID, aNodeTypeID, aNodeGraph);
	}

	bool NodeTypeManager::CanCreateOperatorNode(const eNodeOperatorTrait aTrait, const DataTypeID aDataTypeID)
	{
		if (mOperatorNodeTypeIDs.contains(aTrait))
		{
			return mOperatorNodeTypeIDs.at(aTrait).contains(aDataTypeID);
		}
		return false;
	}

	NodeType& NodeTypeManager::GetNodeType(const NodeTypeID anID)
	{
		return mNodeTypes.at(anID);
	}

	const NodeType& NodeTypeManager::GetNodeType(const NodeTypeID anID) const
	{
		return mNodeTypes.at(anID);
	}

	const std::vector<NodeType>& NodeTypeManager::GetNodeTypes()
	{
		return mNodeTypes;
	}

	CustomEvent& NodeTypeManager::GetCustomEvent(const CustomEventID anID)
	{
		return mCustomEvents.at(anID);
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

	const std::vector<std::unique_ptr<Function>>& NodeTypeManager::GetFunctions()
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
		for (NodeTypeID id = 0; id < mNodeTypes.size(); ++id)
		{
			if (GetShortName(id) == aName)
			{
				return id;
			}
		}
		return 0;
	}

	const std::string& NodeTypeManager::GetFullName(const NodeTypeID anID) const
	{
		return mNodeTypes.at(anID).mName;
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
		const CustomEventID id = mCustomEvents.size();
		const CustomEvent& customEvent = mCustomEvents.emplace_back(CustomEvent(aName));

		mToCustomEventID.emplace(customEvent.GetCallerTypeID(), id);
		mToCustomEventID.emplace(customEvent.GetExecutorTypeID(), id);

		return id;
	}

	FunctionID NodeTypeManager::CreateFunction(std::string_view aName)
	{
		const FunctionID id = mFunctions.size();
		const std::unique_ptr<Function>& function = mFunctions.emplace_back(std::make_unique<Function>(aName));

		mToFunctionID.emplace(function->mCallerNodeTypeID, id);
		mToFunctionID.emplace(function->mInputNodeTypeID, id);
		mToFunctionID.emplace(function->mOutputNodeTypeID, id);

		return id;
	}

	const std::unordered_map<DataTypeID, NodeTypeID> NodeTypeManager::GetWildcardMapByOperator(eNodeOperatorTrait aOperatorTrait) const
	{
		return mOperatorNodeTypeIDs.at(aOperatorTrait);
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