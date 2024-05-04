#pragma once
#include "ScriptCommand.h"
#include "../Utilities/MetaScript.h"
#include <vector>

namespace SCR
{


	class CompositeCommand final : public Command
	{
	public:
		enum class eEndCode
		{
			Ended,
			InProgress,
			Ended_Empty,
		};

	public:

		CompositeCommand(const std::string& aName = "Composite");
		~CompositeCommand();

		template<IsBaseOf<Command> CommandType, typename... Args> requires HasArgsConstructor<CommandType, Args...>
		void AddCommand(Args&&... args);

		void AddCommand(std::shared_ptr<Command> aCommand);

		void Begin(const std::string& aName);
		eEndCode End();

	private:

		void Do(Script& aScript) override;
		void Undo(Script& aScript) override;

	private:

		std::unique_ptr<CompositeCommand> myCurrentChild;
		std::vector<std::shared_ptr<Command>> myCommands;
	};

	template<IsBaseOf<Command> CommandType, typename... Args> requires HasArgsConstructor<CommandType, Args...>
	inline void CompositeCommand::AddCommand(Args&&... args)
	{
		AddCommand(std::make_shared<CommandType>(std::forward<Args>(args)...));
	}
}