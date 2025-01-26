#pragma once
#include "Editor/Utility/EditorUtilities.hpp"

namespace Editor
{
	class SetEntityTransformCommand final
	{
	public:
		ECS::EntityID myEntityID = GetInvalidIndex<ECS::EntityID>();
		Math::Transform myOldTransform;
		Math::Transform myNewTransform;

		void Execute() const;
		void Undo() const;

	private:
		void SetTransform(Math::Transform aTransform) const;
	};

}