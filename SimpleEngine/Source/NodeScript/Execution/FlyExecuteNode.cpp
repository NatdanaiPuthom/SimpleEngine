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

			const Pin& inputPin = currentNodeGraph.GetPin(inputPinID);
			const PinType& inputPinType = aContext.mPinTypeManager->GetPinType(inputPin.GetTypeID());

			const GenericDataTypeID dtID = inputPinType.GetDataTypeID();
			if (const DataTypeID* dataTypeID = std::get_if<DataTypeID>(&dtID.mID))
			{
				if (*dataTypeID == Flow::mTypeID)
				{
					continue;
				}
			}

			if (inputPin.GetConnectedPinIDs().empty())
			{
				continue;
			}

			const PinID connectedOutputPinID = inputPin.GetConnectedPinIDs().front();

			const Pin& connectedOutputPin = currentNodeGraph.GetPin(connectedOutputPinID);
			const NodeID connectedNodeID = connectedOutputPin.GetNodeID();

			const Node& connectedNode = currentNodeGraph.GetNode(connectedNodeID);
			const NodeType& connectedNodeType = aContext.mNodeTypeManager->GetNodeType(connectedNode.GetTypeID());

			if (!HasFlag(connectedNodeType.GetTraits(), eNodeTrait::HasFlow))
			{
				aContext.mNodeExecutor->ExecuteNode(NodeExecutionData{ CreateContextualNodeRef(connectedNodeID, aContext.mNodeData.mNodeRef.GetNodeGraph()), eNodeTriggerReason::Read });
			}

			inputPinType.GetSetPinValueFromPinFunction().Invoke(SetPinValueFromPinData
				{
					.mWriteToPinNodeGraph = &currentNodeGraph,
					.mReadFromPinNodeGraph = &currentNodeGraph,
					.mWriteToPinID = inputPinID,
					.mReadFromPinID = connectedOutputPinID
				}, aContext);

		}
	}
}