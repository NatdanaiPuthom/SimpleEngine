#pragma once
#include "../ScriptDefines.h"
#include <string>

namespace SCR
{
	class Script;
	class NodeGraph;

	class Command
	{
		friend class CommandTracker;
		friend class CompositeCommand;
	public:
		
		Command();
		Command(const std::string& aName);
		virtual ~Command() = default;

	private:

		void DoInternal();
		void UndoInternal();

		virtual void Do() = 0;
		virtual void Undo() = 0;

	private:

		const std::string myName;

	};
}