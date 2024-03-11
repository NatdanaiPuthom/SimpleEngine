#pragma once

namespace Scenes
{
	class SpotlightScene final : public Simple::Scene
	{
	public:
		SpotlightScene();
		~SpotlightScene() override = default;

		virtual void Init() override;
		virtual void Update() override;
	};
}