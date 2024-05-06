#pragma once

namespace Test
{
	class ECSTestStuff final
	{
	public:
		ECSTestStuff();
		~ECSTestStuff();

		void Init();
		void Update();
		void Render();

		ECS::EntityID myEntityID;
	};
}