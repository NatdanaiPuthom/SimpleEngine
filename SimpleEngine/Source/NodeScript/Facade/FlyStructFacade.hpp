#pragma once
#include "../FlyDefines.hpp"
#include "FlyVariableFacade.hpp"

namespace FLY_NAMESPACE
{

	class Struct;

	class StructFacade final
	{
	public:
		StructFacade() = default;
		StructFacade(StructID aStructID);


		[[nodiscard]] std::string_view GetName() const;
		[[nodiscard]] StructID GetID() const;
		[[nodiscard]] std::vector<VariableFacade> GetMemberVariables(bool aIncludeDestroyed = false) const;

		VariableFacade CreateVariable(DataTypeFacade aDataTypeFacade, std::string_view aName, CommandTracker* const aCommandTracker);
		void SetName(std::string_view aName, CommandTracker* aCommandTracker);

		[[nodiscard]] VariableContainer& GetVariableContainer() const;

		explicit operator bool() const;

		friend bool operator==(const StructFacade& a, const StructFacade& b);

	private:


		[[nodiscard]] Struct& GetStruct() const;

	private:

		StructID mStructID = InvalidID<StructID>();
	};
}