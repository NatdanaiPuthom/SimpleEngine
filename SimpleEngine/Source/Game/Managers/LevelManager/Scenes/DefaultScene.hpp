#pragma once

namespace Simple
{
	class PlaneReflection;
}

namespace Scenes
{
	class DefaultScene final : public Simple::Scene
	{
	public:
		DefaultScene();
		~DefaultScene() override;

		void Init() override;
		void Update() override;
		void Render() override;
	private:
		std::unique_ptr<Simple::PlaneReflection> myPlaneReflection;
	};
}