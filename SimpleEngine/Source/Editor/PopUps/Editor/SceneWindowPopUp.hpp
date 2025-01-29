#pragma once
#include "Editor/Core/PopUp.hpp"
#include "Editor/Tools/TransformEntityTool.hpp"
#include "Editor/Tools/GridSnapTool.hpp"

namespace Editor
{
	class SceneWindowPopUp final : public PopUp
	{
	public:
		SceneWindowPopUp(const std::string& aName);

		void Render() override;
	private:
		void RenderOrientationCube();
		void RenderPlayButton();
	private:
		TransformEntityTool myTransformEntityTool;
		GridSnapTool myGridSnapTool;	
	};
}
