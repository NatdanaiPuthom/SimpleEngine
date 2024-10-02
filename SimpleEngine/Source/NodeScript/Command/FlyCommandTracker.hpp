#pragma once
#include "../FlyDefines.hpp"
#include "FlyCommand.hpp"
#include "FlyCompositeCommand.hpp"
#include "../Memory/FlyHeapObject.hpp"
#include <stack>

namespace FLY_NAMESPACE
{

	class CommandTracker final
	{
	public:

		CommandTracker();
		~CommandTracker();

		void Clear();

		size_t GetUndoSize() const;
		size_t GetRedoSize() const;

	public:

		void DoCommand(Command&& aCommand);
		void RegisterCommand(Command&& aCommand);

		void BeginComposite(const std::string& aName);
		void EndComposite();

		void UndoCommand();
		void RedoCommand();

	private:

		void DoCommandInternal(Command&& aCommand, bool aExecute);

	private:


		std::stack<HeapObject<Command, false>> myUndoStack;
		std::stack<HeapObject<Command, false>> myRedoStack;
		HeapObject<CompositeCommand, true> myCurrentCompositeCommand;

	};
}