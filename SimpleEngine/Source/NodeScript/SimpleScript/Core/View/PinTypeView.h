#pragma once
#include "../ScriptDefines.h"
#include "../Pin/PinType.h"

namespace SCR
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

		PinTypeID myPinTypeID;
	};
}