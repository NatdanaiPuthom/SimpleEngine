#pragma once
#include "../ScriptDefines.h"
#include <string>

namespace SCR
{
	class Script;

	class Command
	{
		friend class CommandTracker;
		friend class CompositeCommand;
	public:
		
		Command();
		Command(const std::string& aName);
		virtual ~Command() = default;

	private:

		void DoInternal(Script& aScript);
		void UndoInternal(Script& aScript);

		virtual void Do(Script& aScript) = 0;
		virtual void Undo(Script& aScript) = 0;

	private:

		const std::string myName;

	};
}