#include "NodeScript/Precomplied/NodeScriptPch.hpp"
#include "FlyTraitManager.hpp"

namespace FLY_NAMESPACE
{
	TraitID TraitManager::CreateTrait(std::string_view aName)
	{
		const TraitID id{ mTraits.size() };
		mTraits.emplace_back(aName);
		return id;
	}

	TraitID TraitManager::CreateTrait(std::string_view aName, const DataTypeID aDataTypeID)
	{
		const TraitID id = CreateTrait(aName);
		mTraitIDsByDataTypeID[aDataTypeID] = id;
		return id;
	}

	bool TraitManager::HasDataTypeTrait(const DataTypeID aDataTypeID, const TraitID aTraitID) const
	{
		auto it = mTraitsByDataTypeID.find(aDataTypeID);

		if (it == mTraitsByDataTypeID.end())
		{
			return false;
		}
		return it->second.contains(aTraitID);
	}

	void TraitManager::CreateTraitImplementation(const DataTypeID aDataTypeID, const TraitID aTraitID)
	{
		mTraitsByDataTypeID[aDataTypeID].emplace(aTraitID, TraitImplementation{});
	}
}
