#pragma once

namespace Test
{
	class ECSTestStuff final
	{
	public:
		ECSTestStuff();
		~ECSTestStuff();

		void Init();
		void Update() const;
		void Render() const;

		ECS::EntityID myEntityID;
		ECS::EntityID myTestEntityID;
	};
}