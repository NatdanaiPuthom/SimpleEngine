#pragma once
#include "../ScriptDefines.h"
#include <string>

namespace SCR
{
	class Script;
	class NodeGraph;

	struct CommandContext
	{
		Script* script = nullptr;
		NodeGraph* nodeGraph = nullptr;
	};

	class Command
	{
		friend class CommandTracker;
		friend class CompositeCommand;
	public:
		
		Command(const CommandContext& aContext);
		Command(const CommandContext& aContext, const std::string& aName);
		virtual ~Command() = default;

	private:

		void DoInternal();
		void UndoInternal();

		virtual void Do() = 0;
		virtual void Undo() = 0;

	protected:

		const CommandContext myContext;

	private:

		const std::string myName;

	};
}