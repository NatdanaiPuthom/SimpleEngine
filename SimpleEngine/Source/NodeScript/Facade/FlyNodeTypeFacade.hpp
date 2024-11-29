#pragma once
#include "../FlyDefines.hpp"
#include "../Node/FlyNodeType.hpp"
#include "FlyPinTypeFacade.hpp"
#include "FlyDataTypeFacade.hpp"
#include <vector>

namespace FLY_NAMESPACE
{

	class NodeTypeFacade final
	{
	public:

		explicit NodeTypeFacade(NodeTypeID anID);

		[[nodiscard]] const std::string& GetName() const;
		[[nodiscard]] std::string GetShortName() const;
		[[nodiscard]] std::string GetNameDirectory() const;
		[[nodiscard]] eNodeTrait GetTraits() const;
		[[nodiscard]] EventID GetEventID() const;

		[[nodiscard]] std::vector<PinTypeFacade> GetInputPinTypeFacades() const;
		[[nodiscard]] std::vector<PinTypeFacade> GetOutputPinTypeFacades() const;

		[[nodiscard]] std::vector<DataTypeFacade> GetReplacableDataTypes() const;

		[[nodiscard]] NodeTypeID GetID() const;

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