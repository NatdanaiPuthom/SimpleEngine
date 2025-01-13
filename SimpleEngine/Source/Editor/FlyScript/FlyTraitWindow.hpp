#pragma once
#include "NodeScript/Fly.hpp"
#include "FlyScriptEditorUtilities.hpp"

namespace Editor
{

	class FlyTraitWindow final
	{
	public:

		FlyTraitWindow();
		~FlyTraitWindow();
		
		void Update();

	private:

		Fly::GenericDataTypeFacade mySelectedDataType;
		Fly::TraitID myCurrentTraitID;

		NodeGraphContext myNodeGraphContext;
	};

}