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


		template<typename... Args>
		void DoCommand(Args&&... aArgs);

		template<typename... Args>
		void RegisterCommand(Args&&... aArgs);

		void BeginComposite(const std::string& aName);
		void EndComposite();

		void UndoCommand();
		void RedoCommand();

		size_t GetUndoSize() const;
		size_t GetRedoSize() const;

	private:

		template<typename... Args>
		void DoCommandInternal(bool aExecute, Args&&... aArgs);

	private:


		std::stack<HeapObject<Command>> mUndoStack;
		std::stack<HeapObject<Command>> mRedoStack;
		HeapObject<CompositeCommand, false> mCurrentCompositeCommand;

	};


	template<typename ...Args>
	inline void CommandTracker::DoCommand(Args && ...aArgs)
	{
		DoCommandInternal(true, std::forward<Args>(aArgs)...);
	}

	template<typename ...Args>
	inline void CommandTracker::RegisterCommand(Args && ...aArgs)
	{
		DoCommandInternal(false, std::forward<Args>(aArgs)...);
	}

	template<typename... Args>
	inline void CommandTracker::DoCommandInternal(bool aExecute, Args&&... aArgs)
	{
		if (mCurrentCompositeCommand)
		{
			mCurrentCompositeCommand->AddCommand(std::forward<Args>(aArgs)...);
			return;
		}

		HeapObject<Command> command(std::forward<Args>(aArgs)...);
		if (aExecute)
		{
			command->DoCommand();
		}

		mUndoStack.push(std::move(command));

		mRedoStack = {};
	}
}