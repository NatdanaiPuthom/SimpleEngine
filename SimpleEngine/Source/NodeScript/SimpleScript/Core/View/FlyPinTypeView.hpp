#pragma once
#include "../FlyDefines.hpp"
#include "../Pin/FlyPinType.hpp"

namespace FLY_NAMESPACE
{
	class PinTypeView final
	{
	public:

		explicit PinTypeView(PinTypeID anID);

		[[nodiscard]] const std::string& GetName() const;
		[[nodiscard]] eFlowType GetFlowType() const;
		[[nodiscard]] DataTypeID GetDataTypeID() const;

		[[nodiscard]] PinTypeID GetID() const;

	private:

		[[nodiscard]] const PinType& GetPinType() const;

	private:

		PinTypeID mPinTypeID;
	};
}