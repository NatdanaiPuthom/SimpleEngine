#include "Editor/Precomplied/EditorPch.hpp"
#include "FlyTraitWindow.hpp"
#include "FlyScriptEditorUtilities.hpp"
#include "NodeScript/Internal/FlyInternal.hpp"
#include "NodeScript/DataType/FlyDataTypeManager.hpp"
#include "NodeScript/Trait/FlyTraitManager.hpp"

namespace Editor
{
	FlyTraitWindow::FlyTraitWindow()
	{
	}

	FlyTraitWindow::~FlyTraitWindow()
	{
	}

	void FlyTraitWindow::Update()
	{
		if (ImGui::Begin("Trait Window"))
		{
			std::string traitName = "None";

			if (myCurrentTraitID != Fly::InvalidID<Fly::TraitID>())
			{
				traitName = Fly::Internal::GetTraitByID(myCurrentTraitID).mName;
			}

			if (ImGui::BeginCombo("Selected Trait", traitName.c_str()))
			{
				const auto& traits = Fly::Internal::GetTraitManager().GetTraits();

				for (Fly::TraitID traitID{ 0 }; traitID < traits.size(); traitID++)
				{
					if (ImGui::Selectable(Fly::Internal::GetTraitManager().GetTrait(traitID).mName.c_str()))
					{
						myCurrentTraitID = traitID;
					}
				}

				ImGui::EndCombo();
			}



			if (DataTypeComboNoFilter("Data Type", mySelectedDataType))
			{

			}

			const bool validTrait = myCurrentTraitID != Fly::InvalidID<Fly::TraitID>() && mySelectedDataType;
			bool traitExists = false;
			if (validTrait)
			{
				traitExists = Fly::Internal::GetTraitManager().HasDataTypeTrait(std::get<Fly::DataTypeID>(mySelectedDataType.GetID().mID), myCurrentTraitID);
			}
			ImGui::BeginDisabled(!validTrait || traitExists);

			if (ImGui::Button("Implement Trait"))
			{
				Fly::Internal::CreateTraitImplementation(std::get<Fly::DataTypeID>(mySelectedDataType.GetID().mID), myCurrentTraitID);
			}

			ImGui::EndDisabled();

			if (validTrait)
			{

				if (Fly::Internal::GetTraitManager().HasDataTypeTrait(std::get<Fly::DataTypeID>(mySelectedDataType.GetID().mID), myCurrentTraitID))
				{

					Fly::TraitImplementation* t = Fly::Internal::GetTraitManager().GetTraitImplementation(std::get<Fly::DataTypeID>(mySelectedDataType.GetID().mID), myCurrentTraitID);
					
					if (Fly::FlyTraitImplementation* t2 = std::get_if<Fly::FlyTraitImplementation>(t))
					{
						myNodeGraphContext.myNodeGraphProxy = Fly::NodeGraphProxy(t2->mNodeGraph);
						ShowNodeGraph(myNodeGraphContext);
					}
					

				}
			}
		}
		ImGui::End();
	}

}