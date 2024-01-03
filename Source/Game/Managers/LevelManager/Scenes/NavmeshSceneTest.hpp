#pragma once

namespace Scenes
{
	class NavmeshSceneTest final : public Simple::Scene
	{
	public:
		NavmeshSceneTest();
		~NavmeshSceneTest();

		void Init() override;
		void Update() override;
	};
}