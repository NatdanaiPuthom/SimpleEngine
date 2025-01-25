#pragma once

namespace Editor
{
	class SetEntityTransformCommand final
	{
	public:
		ECS::EntityID myEntityID = static_cast<size_t>(-1);
		Math::Transform myOldTransform;
		Math::Transform myNewTransform;

		void Do() const;
		void Undo() const;

	private:
		void SetTransform(Math::Transform aTransform) const;
	};

}