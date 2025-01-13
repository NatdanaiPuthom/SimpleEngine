#pragma once
#include "../FlyDefines.hpp"
#include "FlyTrait.hpp"
#include "FlyTraitImplementation.hpp"
#include <unordered_map>
#include <unordered_set>

namespace FLY_NAMESPACE
{

	class TraitManager final
	{
	public:

		TraitManager() = default;

		TraitID CreateTrait(std::string_view aName);
		TraitID CreateTrait(std::string_view aName, DataTypeID aDataTypeID);
		[[nodiscard]] bool HasDataTypeTrait(DataTypeID aDataTypeID, TraitID aTraitID) const;
		void CreateTraitImplementation(DataTypeID aDataTypeID, TraitID aTraitID);

		[[nodiscard]] Trait& GetTrait(TraitID aTraitID);
		[[nodiscard]] const Trait& GetTrait(TraitID aTraitID) const;
		[[nodiscard]] Trait& GetTrait(DataTypeID aDataTypeID);
		[[nodiscard]] const Trait& GetTrait(DataTypeID aDataTypeID) const;

		[[nodiscard]] TraitImplementation* GetTraitImplementation(DataTypeID aDataTypeID, TraitID aTraitID);
		[[nodiscard]] const TraitImplementation* GetTraitImplementation(DataTypeID aDataTypeID, TraitID aTraitID) const;

		const std::vector<Trait>& GetTraits() const;
		TraitID GetTraitIDByDataTypeID(DataTypeID aDataTypeID) const;

	private:

		std::vector<Trait> mTraits;

		std::unordered_map<DataTypeID, std::unordered_map<TraitID, HeapObject<TraitImplementation>>> mTraitsByDataTypeID;

		std::unordered_map<DataTypeID, TraitID> mTraitIDsByDataTypeID;

	};

	inline TraitImplementation* TraitManager::GetTraitImplementation(const DataTypeID aDataTypeID, const TraitID aTraitID)
	{
		auto it = mTraitsByDataTypeID.find(aDataTypeID);
		if (it == mTraitsByDataTypeID.end())
		{
			return nullptr;
		}
		auto it2 = it->second.find(aTraitID);
		if (it2 == it->second.end())
		{
			return nullptr;
		}
		return it2->second.Get();
	}

	inline const TraitImplementation* TraitManager::GetTraitImplementation(DataTypeID aDataTypeID, TraitID aTraitID) const
	{
		auto it = mTraitsByDataTypeID.find(aDataTypeID);
		if (it == mTraitsByDataTypeID.end())
		{
			return nullptr;
		}
		auto it2 = it->second.find(aTraitID);
		if (it2 == it->second.end())
		{
			return nullptr;
		}
		return it2->second.Get();
	}


	inline Trait& TraitManager::GetTrait(const TraitID aTraitID)
	{
		return mTraits[aTraitID];
	}

	inline const Trait& TraitManager::GetTrait(const TraitID aTraitID) const
	{
		return mTraits[aTraitID];
	}

	inline Trait& TraitManager::GetTrait(const DataTypeID aDataTypeID)
	{
		return GetTrait(GetTraitIDByDataTypeID(aDataTypeID));
	}

	inline const Trait& TraitManager::GetTrait(const DataTypeID aDataTypeID) const
	{
		return GetTrait(GetTraitIDByDataTypeID(aDataTypeID));
	}

	inline const std::vector<Trait>& TraitManager::GetTraits() const
	{
		return mTraits;
	}

	inline TraitID TraitManager::GetTraitIDByDataTypeID(const DataTypeID aDataTypeID) const
	{
		return mTraitIDsByDataTypeID.at(aDataTypeID);
	}

}