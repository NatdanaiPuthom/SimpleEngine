#pragma once
#include "../ScriptDefines.hpp"
#include "../Node/NodeType.hpp"
#include "PinTypeView.hpp"

namespace FLY_NAMESPACE
{

	class NodeTypeView final
	{
	public:

		explicit NodeTypeView(NodeTypeID anID);

		const std::string& GetName() const;
		std::string GetShortName() const;
		std::string GetNameDirectory() const;
		eNodeTrait GetTraits() const;
		EventID GetEventID() const;

		std::vector<PinTypeView> GetInputPinTypes() const;
		std::vector<PinTypeView> GetOutputPinTypes() const;

		NodeTypeID GetID() const;

		explicit operator bool() const;

		bool operator==(const NodeTypeView& aOther) const;
		bool operator!=(const NodeTypeView& aOther) const;

	private:

		const NodeType& GetNodeType() const;
		std::vector<PinTypeView> GetPinTypes(eFlowType aFlowType) const;

	private:

		NodeTypeID mNodeTypeID;
	};
}