#pragma once
#include "EditorCommand.hpp"
#include <vector>

namespace Editor
{

	class CompositeCommand final
	{
		friend class CompositeCommandBuilder;
	public:

		CompositeCommand(std::string aName, std::vector<Command> aCommands);


		void Do() const;
		void Undo() const;

		const std::string& GetName() const
		{
			return myName;
		}
	private:

		std::vector<Command> myCommands;
		std::string myName;
	};
}