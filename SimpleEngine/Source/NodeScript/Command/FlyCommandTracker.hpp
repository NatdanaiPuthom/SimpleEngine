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

		void DoCommand(CommandNew&& aCommand);
		void RegisterCommand(CommandNew&& aCommand);

		void BeginComposite(const std::string& aName);
		void EndComposite();

		void UndoCommand();
		void RedoCommand();

	private:

		void DoCommandInternal(CommandNew&& aCommand, bool aExecute);

	private:


		std::stack<HeapObject<CommandNew>> mUndoStack;
		std::stack<HeapObject<CommandNew>> mRedoStack;
		HeapObject<CompositeCommand, false> mCurrentCompositeCommand;

	};
}