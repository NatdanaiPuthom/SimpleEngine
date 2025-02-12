#include "Editor/Precomplied/EditorPch.hpp"
#include "GridSnapTool.hpp"
#include "Editor/Tools/TransformEntityTool.hpp"

namespace Editor
{

	GridSnapTool::GridSnapTool(TransformEntityTool* aTransformEntityTool)
		: myTransformEntityTool(aTransformEntityTool)
	{
	}

	void GridSnapTool::Render()
	{
		bool useSnap = myTransformEntityTool->IsUsingSnap();
		ImGui::Checkbox("Use Snap", &useSnap);
		myTransformEntityTool->SetUseSnap(useSnap);
		ImGui::SetNextItemWidth(60.f);
		float snapValue = myTransformEntityTool->GetSnapValue();
		if (ImGui::DragFloat("Snap Value ", &snapValue, 1.f, 0.f, FLT_MAX / INT_MAX))
		{
			myTransformEntityTool->SetSnapValue(snapValue);
		}
	}
}