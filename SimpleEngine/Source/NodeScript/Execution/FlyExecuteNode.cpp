#include "FlyExecuteNode.hpp"
#include "../Pin/FlyPin.hpp"
#include "../Pin/FlyPinTypeManager.hpp"
#include "FlyNodeExecutor.hpp"
#include "../Utilities/FlyUtilities.hpp"
#include "SystemTypes/FlyFlow.hpp"

namespace FLY_NAMESPACE
{

	void EvaluateInputValues(const std::vector<PinID>& aInputPinIDs, const InternalExecutionContext& aContext, const size_t aStartIndex)
	{
		NodeGraph& currentNodeGraph = aContext.mNodeData.mNodeRef.GetNodeGraph();
		for (size_t i = aStartIndex; i < aInputPinIDs.size(); ++i)
		{

			const PinID inputPinID = aInputPinIDs[i];

			const Pin& inputPin = currentNodeGraph.mPins[inputPinID];
			const PinType& inputPinType = aContext.mPinTypeManager->GetPinType(inputPin.mTypeID);

			if (const DataTypeID* dataTypeID = std::get_if<DataTypeID>(&inputPinType.mGenericDataTypeID.mID))
			{
				if (*dataTypeID == Flow::mTypeID)
				{
					continue;
				}
			}

			if (inputPin.mConnectedPinIDs.empty())
			{
				continue;
			}

			const PinID connectedOutputPinID = inputPin.mConnectedPinIDs.front();

			const Pin& connectedOutputPin = currentNodeGraph.mPins[connectedOutputPinID];
			const NodeID connectedNodeID = connectedOutputPin.mNodeID;

			const Node& connectedNode = currentNodeGraph.mNodes[connectedNodeID];
			const NodeType& connectedNodeType = aContext.mNodeTypeManager->GetNodeType(connectedNode.mTypeID);

			if (!HasFlag(connectedNodeType.mNodeRecipe.mTraits, eNodeTrait::HasFlow))
			{
				aContext.mNodeExecutor->ExecuteNode(NodeExecutionData{ CreateContextualNodeRef(connectedNodeID, aContext.mNodeData.mNodeRef.GetNodeGraph()), eNodeTriggerReason::Read });
			}

			inputPinType.mSetPinValueFromPinFunction(SetPinValueFromPinData
				{
					.mWriteToPinNodeGraph = &currentNodeGraph,
					.mReadFromPinNodeGraph = &currentNodeGraph,
					.mWriteToPinID = inputPinID,
					.mReadFromPinID = connectedOutputPinID
				}, aContext);

		}
	}
}