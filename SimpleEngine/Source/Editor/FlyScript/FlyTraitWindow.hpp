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

		Fly::GenericDataTypeProxy mySelectedDataType;
		Fly::TraitID myCurrentTraitID;

		NodeGraphContext myNodeGraphContext;
	};

}