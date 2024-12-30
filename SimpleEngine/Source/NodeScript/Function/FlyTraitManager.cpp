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

	bool TraitManager::HasClassTrait(const ClassID aClassID, const TraitID aTraitID) const
	{
		return mTraitsByClass.at(aClassID).contains(aTraitID);
	}

	void TraitManager::ImplementTrait(const ClassID aClassID, const TraitID aTraitID)
	{
		mTraitsByClass[aClassID].emplace(aTraitID, TraitImplementation{});
	}
}
