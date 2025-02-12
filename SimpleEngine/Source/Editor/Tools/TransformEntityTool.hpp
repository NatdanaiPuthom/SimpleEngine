#pragma once
#include "Editor/Command/Commands/SetEntityTransformCommand.hpp"

namespace Editor
{

	class TransformEntityTool final
	{
	public:

		TransformEntityTool() = default;

		void Render();

		void SetSnapValue(float aSnapValue)
		{
			mySnapValue = aSnapValue;
		}

		float GetSnapValue() const
		{
			return mySnapValue;
		}

		void SetUseSnap(bool aUseSnap)
		{
			myUseSnap = aUseSnap;
		}

		bool IsUsingSnap() const
		{
			return myUseSnap;
		}

	private:

		SetEntityTransformCommand mySetEntityTransformCommand;
		bool myIsDraggingEntity = false;
		float mySnapValue = 1.f;
		bool myUseSnap = false;
	};



}