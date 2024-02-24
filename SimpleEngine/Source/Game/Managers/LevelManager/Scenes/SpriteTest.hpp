#pragma once

namespace Scenes
{
	class SpriteTestScene final : public Simple::Scene
	{
	public:
		SpriteTestScene();
		~SpriteTestScene() override;

		void Init() override;
		void Update() override;
		void Render() override;
	private:
		Drawer::Sprite2D mySprite;
	};
}