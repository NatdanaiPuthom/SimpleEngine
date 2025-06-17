#pragma once
#include "FlyCommand.hpp"
#include "../Memory/FlyHeapObject.hpp"
#include <vector>

namespace FLY_NAMESPACE
{

	class CompositeCommand final
	{
	public:
		enum class eEndCode
		{
			Ended,
			InProgress,
			Ended_Empty,
		};
	public:

		CompositeCommand(std::string aName = std::string());

		void AddCommand(Command&& aCommand);

		void Do() const;
		void Undo() const;

		void Begin(std::string_view aName);
		eEndCode End();

		const std::string& GetName() const;

	private:

		HeapObject<CompositeCommand, false> mCurrentChild;
		std::vector<Command> mCommands;
		std::string mName;
	};
}