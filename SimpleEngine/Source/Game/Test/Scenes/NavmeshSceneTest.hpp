#pragma once
#include "Game/Navmesh/Pathfinder.hpp"

namespace Scenes
{
	class NavmeshSceneTest final : public Simple::Scene, public Simple::Observer<Simple::eEvent>
	{
	public:
		NavmeshSceneTest();
		~NavmeshSceneTest();

		void OnEnter() override;

		void Init() override;
		void Update() override;
		void Render() override;
	private:
		void ReceiveMessage(const Simple::Message<Simple::eEvent>& aMessage) override;
	private:
		Simple::Pathfinder myPathFinder;
		Math::Vector3f myStartPosition;
		Math::Vector3f myEndPosition;
	};
}