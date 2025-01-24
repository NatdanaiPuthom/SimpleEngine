#pragma once
#include "../FlyDefines.hpp"
#include "FlyVariableProxy.hpp"

namespace FLY_NAMESPACE
{

	class Struct;
	class DataTypeProxy;
	class GenericDataTypeProxy;

	class StructProxy final
	{
	public:
		StructProxy() = default;
		StructProxy(StructID aStructID);


		[[nodiscard]] std::string_view GetName() const;
		[[nodiscard]] StructID GetID() const;
		[[nodiscard]] std::vector<VariableProxy> GetMemberVariables(bool aIncludeDestroyed = false) const;

		VariableProxy CreateVariable(const GenericDataTypeProxy& aDataTypeProxy, std::string_view aName, CommandTracker* const aCommandTracker);
		void SetName(std::string_view aName, CommandTracker* aCommandTracker);

		explicit operator bool() const;

		friend bool operator==(const StructProxy& a, const StructProxy& b);

	private:


		[[nodiscard]] Struct& GetStruct() const;

	private:

		StructID mStructID = InvalidID<StructID>();
	};
}