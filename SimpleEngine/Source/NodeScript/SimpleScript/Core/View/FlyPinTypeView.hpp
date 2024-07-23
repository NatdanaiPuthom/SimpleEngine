#pragma once
#include "../FlyDefines.hpp"
#include "../Pin/FlyPinType.hpp"

namespace FLY_NAMESPACE
{
	class PinTypeView final
	{
	public:

		PinTypeView(PinTypeID anID);

		const std::string& GetName() const;
		eFlowType GetFlowType() const;
		DataTypeID GetDataTypeID() const;

		PinTypeID GetID() const;

	private:

		const PinType& GetPinType() const;

	private:

		PinTypeID mPinTypeID;
	};
}