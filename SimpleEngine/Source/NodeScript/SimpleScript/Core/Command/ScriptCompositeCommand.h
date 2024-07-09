#pragma once
#include "ScriptCommand.h"
#include "../Utilities/MetaScript.h"
#include <vector>

namespace SCR
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

		~CompositeCommand();

		CompositeCommand(const CompositeCommand& aOther);
		CompositeCommand(CompositeCommand&& aOther) noexcept;


		void AddCommand(Command&& aCommand);

		void operator()(eCommandType aCommandType) const;

		void Begin(const std::string& aName);
		eEndCode End();

		const std::string& GetName();

	private:

		void Do() const;
		void Undo() const;

	private:

		std::unique_ptr<CompositeCommand> myCurrentChild;
		std::vector<Command> myCommands;
		std::string myName;
	};
}