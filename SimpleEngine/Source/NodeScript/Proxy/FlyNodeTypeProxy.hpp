#pragma once
#include "../FlyDefines.hpp"
#include "FlyPinTypeProxy.hpp"
#include "FlyDataTypeProxy.hpp"
#include "../Node/FlyNodeTrait.hpp"
#include <vector>

namespace FLY_NAMESPACE
{

	class NodeTypeProxy final
	{
	public:

		NodeTypeProxy() = default;
		explicit NodeTypeProxy(NodeTypeID aID);

		[[nodiscard]] const std::string& GetName() const;
		[[nodiscard]] std::string GetShortName() const;
		[[nodiscard]] std::string GetNameDirectory() const;
		[[nodiscard]] eNodeTrait GetTraits() const;
		[[nodiscard]] EventID GetEventID() const;

		[[nodiscard]] std::vector<PinTypeProxy> GetInputPinTypes() const;
		[[nodiscard]] std::vector<PinTypeProxy> GetOutputPinTypes() const;

		[[nodiscard]] std::vector<DataTypeProxy> GetReplacableDataTypes(bool aUseTraits = true) const;

		[[nodiscard]] NodeTypeID GetID() const;

		explicit operator bool() const;

		friend bool operator==(const NodeTypeProxy& a, const NodeTypeProxy& b);

	private:

		const NodeType& GetNodeType() const;
		std::vector<PinTypeProxy> GetPinTypes(eIODirection aIODirection) const;

	private:

		NodeTypeID mNodeTypeID = InvalidID<NodeTypeID>();
	};
}