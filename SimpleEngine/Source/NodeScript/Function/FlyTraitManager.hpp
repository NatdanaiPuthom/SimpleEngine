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
		[[nodiscard]] bool HasClassTrait(ClassID aClassID, TraitID aTraitID) const;
		void ImplementTrait(ClassID aClassID, TraitID aTraitID);

		[[nodiscard]] Trait& GetTrait(TraitID aTraitID) 
		{
			return mTraits[aTraitID];
		}

		[[nodiscard]] const Trait& GetTrait(TraitID aTraitID) const 
		{
			return mTraits[aTraitID]; 
		}

		[[nodiscard]] TraitImplementation& GetTraitImplementation(ClassID aClassID, TraitID aTraitID)
		{
			return *mTraitsByClass.at(aClassID).at(aTraitID); 
		}

		[[nodiscard]] const TraitImplementation& GetTraitImplementation(ClassID aClassID, TraitID aTraitID) const 
		{ 
			return *mTraitsByClass.at(aClassID).at(aTraitID);
		}
	private:

		std::vector<Trait> mTraits;

		std::unordered_map<ClassID, std::unordered_map<TraitID, HeapObject<TraitImplementation>>> mTraitsByClass;

	};

}