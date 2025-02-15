#pragma once

namespace Simple
{
	class GameWorld final
	{
	public:
		GameWorld();
		~GameWorld();

		void Init();

		void EarlyRender();
		void Render();
		void LateRender();

		void EarlyUpdate();
		void Update();
		void LateUpdate();
		void FixedUpdate();

		void RenderBloomEntities();

	private:
		float myFixedUpdateTimer;
		const float myFixedUpdateFreqency;
	};
}