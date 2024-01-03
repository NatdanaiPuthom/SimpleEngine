#pragma once
#include "Game/Navmesh/Navmesh.hpp"

namespace Scenes
{
	class NavmeshSceneTest final : public Simple::Scene
	{
	public:
		NavmeshSceneTest();
		~NavmeshSceneTest();

		void Init() override;
		void Update() override;
		void Render() override;
	private:
		Simple::Navmesh myNavmesh;
	};
}