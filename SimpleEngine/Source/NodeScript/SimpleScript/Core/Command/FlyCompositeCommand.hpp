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

		CompositeCommand(const std::string& aName = std::string());

		void AddCommand(Command&& aCommand);

		void operator()(eCommandType aCommandType) const;

		void Begin(const std::string& aName);
		eEndCode End();

		const std::string& GetName();

	private:

		void Do() const;
		void Undo() const;

	private:

		HeapObject<CompositeCommand> myCurrentChild;
		std::vector<Command> myCommands;
		std::string myName;
	};
}