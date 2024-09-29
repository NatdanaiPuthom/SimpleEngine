#pragma once
#include "../FlyDefines.hpp"
#include "../Node/FlyNodeType.hpp"
#include "FlyPinTypeFacade.hpp"

namespace FLY_NAMESPACE
{

	class NodeTypeFacade final
	{
	public:

		explicit NodeTypeFacade(NodeTypeID anID);

		const std::string& GetName() const;
		std::string GetShortName() const;
		std::string GetNameDirectory() const;
		eNodeTrait GetTraits() const;
		EventID GetEventID() const;

		std::vector<PinTypeFacade> GetInputPinTypeFacades() const;
		std::vector<PinTypeFacade> GetOutputPinTypeFacades() const;

		NodeTypeID GetID() const;

		explicit operator bool() const;

		bool operator==(const NodeTypeFacade& aOther) const;
		bool operator!=(const NodeTypeFacade& aOther) const;

	private:

		const NodeType& GetNodeType() const;
		std::vector<PinTypeFacade> GetPinTypes(eFlowType aFlowType) const;

	private:

		NodeTypeID mNodeTypeID;
	};
}