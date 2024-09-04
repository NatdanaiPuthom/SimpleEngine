#pragma once

namespace Simple
{
	class GameWorld final
	{
	public:
		GameWorld();
		~GameWorld();

		void Init();
		void Update();
		void Render();
		void LateRender();

		void EarlyUpdate();
		void LateUpdate();
		void FixedUpdate();

		void RenderBloomEntities();

	private:
		float myFixedUpdateTimer;
		const float myFixedUpdateFreqency;
	};
}