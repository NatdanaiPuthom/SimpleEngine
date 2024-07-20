#pragma once
#include "../ScriptDefines.hpp"
#include "../Node/NodeType.hpp"
#include "PinTypeView.hpp"

namespace SCR
{

	class NodeTypeView final
	{
	public:

		NodeTypeView(NodeTypeID anID);

		const std::string& GetName() const;
		std::string GetShortName() const;
		std::string GetNameDirectory() const;
		eNodeTrait GetTraits() const;
		EventID GetEventID() const;

		std::vector<PinTypeView> GetInputPinTypes() const;
		std::vector<PinTypeView> GetOutputPinTypes() const;

		NodeTypeID GetID() const;

		bool operator==(const NodeTypeView& aOther) const;
		bool operator!=(const NodeTypeView& aOther) const;

	private:

		const NodeType& GetNodeType() const;
		std::vector<PinTypeView> GetPinTypes(eFlowType aFlowType) const;

	private:

		NodeTypeID mNodeTypeID;
	};
}