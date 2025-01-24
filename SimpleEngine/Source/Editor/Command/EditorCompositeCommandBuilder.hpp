#pragma once
#include <string>
#include <memory>
#include "Editor/Command/EditorCommand.hpp"
#include "Editor/Command/EditorCompositeCommand.hpp"

namespace Editor
{

	class CompositeCommandBuilder final
	{
	public:
		enum class eEndCode
		{
			Ended,
			InProgress,
			Ended_Empty,
		};
	public:

		CompositeCommandBuilder() = default;

		void AddCommand(Command&& aCommand);

		void Begin(std::string_view aName);
		[[nodiscard]] std::optional<CompositeCommand> End();

		[[nodiscard]] bool IsActive() const
		{
			return myCurrentCompositeCommand.operator bool();
		}

	private:


	private:

		class CompositeCommandInternal final
		{
			friend class CompositeCommandBuilder;
		public:

			CompositeCommandInternal(std::string aName = std::string())
				: myName(aName)
			{

			}

			void AddCommand(Command&& aCommand);

			void Begin(std::string_view aName);
			[[nodiscard]] eEndCode End();

			[[nodiscard]] CompositeCommand Build() const;

		private:

			std::unique_ptr<CompositeCommandInternal> myCurrentChild;
			std::vector<Command> myCommands;
			std::string myName;
		};

		std::unique_ptr<CompositeCommandInternal> myCurrentCompositeCommand;
	};
}