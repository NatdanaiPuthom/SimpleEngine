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

		CompositeCommand(const std::string& aName);
		~CompositeCommand();

		template<IsBaseOf<Command> CommandType, typename... Args> requires HasArgsConstructor<CommandType, Args...>
		void AddCommand(Args&&... args);

		void AddCommand(std::unique_ptr<Command> aCommand);

		void Begin(const std::string& aName);
		eEndCode End();

	private:

		void Do() override;
		void Undo() override;

	private:

		std::unique_ptr<CompositeCommand> myCurrentChild;
		std::vector<std::unique_ptr<Command>> myCommands;
	};

	template<IsBaseOf<Command> CommandType, typename... Args> requires HasArgsConstructor<CommandType, Args...>
	inline void CompositeCommand::AddCommand(Args&&... args)
	{
		AddCommand(std::make_shared<CommandType>(std::forward<Args>(args)...));
	}


	class CompositeCommandNew final
	{
	public:
		enum class eEndCode
		{
			Ended,
			InProgress,
			Ended_Empty,
		};
	public:

		CompositeCommandNew(const std::string& aName = std::string());

		~CompositeCommandNew();

		CompositeCommandNew(const CompositeCommandNew& aOther);
		CompositeCommandNew(CompositeCommandNew&& aOther) noexcept;


		void AddCommand(CommandNew&& aCommand);

		void operator()(eCommandType aCommandType) const;

		void Begin(const std::string& aName);
		eEndCode End();

		const std::string& GetName();

	private:

		void Do() const;
		void Undo() const;

	private:

		std::unique_ptr<CompositeCommandNew> myCurrentChild;
		std::vector<CommandNew> myCommands;
		std::string myName;
	};
}