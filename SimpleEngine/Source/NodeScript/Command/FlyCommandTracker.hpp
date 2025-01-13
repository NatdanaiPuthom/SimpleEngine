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


		void DoCommand(Command aCommand);

		void RegisterCommand(Command aCommand);

		void BeginComposite(std::string_view aName);
		void EndComposite();

		void UndoCommand();
		void RedoCommand();

		size_t GetUndoSize() const;
		size_t GetRedoSize() const;

	private:

		void DoCommandInternal(bool aExecute, Command&& aCommand);

	private:


		std::stack<Command> mUndoStack;
		std::stack<Command> mRedoStack;
		HeapObject<CompositeCommand, false> mCurrentCompositeCommand;

	};
}