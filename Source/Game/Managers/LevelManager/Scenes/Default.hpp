#pragma once

namespace Simple
{
	class PlaneReflection;
}

namespace Scenes
{
	class Default final : public Simple::Scene
	{
	public:
		Default();
		~Default() override;

		void Init() override;
		void Update() override;
		void Render() override;
	private:
		std::unique_ptr<Simple::PlaneReflection> myPlaneReflection;
	};
}