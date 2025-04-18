#pragma once
#include "../FlyDefines.hpp"
#include "../Pin/FlyPinType.hpp"

namespace FLY_NAMESPACE
{
	class PinTypeProxy final
	{
	public:

		explicit PinTypeProxy(PinTypeID anID);

		[[nodiscard]] const std::string& GetName() const;
		[[nodiscard]] eIODirection GetIODirection() const;
		//[[nodiscard]] DataTypeID GetDataTypeID() const;
		[[nodiscard]] GenericDataTypeID GetDataTypeID() const;

		[[nodiscard]] PinTypeID GetID() const;

		void SetName(std::string_view aName);

	private:

		[[nodiscard]] PinType& GetPinType();
		[[nodiscard]] const PinType& GetPinType() const;

	private:

		PinTypeID mPinTypeID;
	};
}