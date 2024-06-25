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

	enum class eCommandType
	{
		Do,
		Undo
	};

	class CommandNew final
	{
	public:

		using CommandFunction = std::function<void(eCommandType)>;

		template<typename CallableCommand>
		CommandNew(CallableCommand&& aCallableCommand, const std::string& aName)
			: myName(aName)
			, myCommandFunction(aCallableCommand)
		{
		}

		CommandNew(const CommandNew& aOther)
			: myName(aOther.myName)
			, myCommandFunction(aOther.myCommandFunction)
		{

		}

		CommandNew(CommandNew&& aOther)
			: myName(std::move(aOther.myName))
			, myCommandFunction(std::move(aOther.myCommandFunction))
		{

		}

		void operator()(eCommandType aCommandType) const;

	private:

		std::string myName;
		CommandFunction myCommandFunction;

	};

}