#pragma once

namespace Simple
{
	class PlaneReflection;
}

namespace Scenes
{
	class ReflectionSceneTest final : public Simple::Scene
	{
	public:
		ReflectionSceneTest();
		~ReflectionSceneTest() override;

		void Init() override;
		void Update() override;
		void Render() override;
	private:
		std::unique_ptr<Simple::PlaneReflection> myPlaneReflection;
	};
}