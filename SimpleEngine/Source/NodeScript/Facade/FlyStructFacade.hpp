#pragma once
#include "../FlyDefines.hpp"
#include "FlyVariableFacade.hpp"

namespace FLY_NAMESPACE
{

	class Struct;
	class DataTypeFacade;
	class GenericDataTypeFacade;

	class StructFacade final
	{
	public:
		StructFacade() = default;
		StructFacade(StructID aStructID);


		[[nodiscard]] std::string_view GetName() const;
		[[nodiscard]] StructID GetID() const;
		[[nodiscard]] std::vector<VariableFacade> GetMemberVariables(bool aIncludeDestroyed = false) const;

		VariableFacade CreateVariable(const GenericDataTypeFacade& aDataTypeFacade, std::string_view aName, CommandTracker* const aCommandTracker);
		void SetName(std::string_view aName, CommandTracker* aCommandTracker);

		explicit operator bool() const;

		friend bool operator==(const StructFacade& a, const StructFacade& b);

	private:


		[[nodiscard]] Struct& GetStruct() const;

	private:

		StructID mStructID = InvalidID<StructID>();
	};
}