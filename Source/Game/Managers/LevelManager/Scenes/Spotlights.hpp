#pragma once

namespace Scenes
{
	class Spotlight final : public Simple::Scene
	{
	public:
		Spotlight();
		~Spotlight() override = default;

		virtual void Init() override;
		virtual void Update() override;
	};
}