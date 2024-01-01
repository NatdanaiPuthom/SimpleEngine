#pragma once

namespace Scenes
{
	class Default final : public Simple::Scene
	{
	public:
		Default();
		~Default() override = default;

		void Init() override;
		void Update() override;
	};
}