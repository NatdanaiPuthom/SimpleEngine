#pragma once
#include "../ScriptDefines.hpp"
#include <string>

namespace SCR
{

	enum class eCommandType
	{
		Do,
		Undo
	};

	class Command final
	{
	public:

		using CommandFunction = std::function<void(eCommandType)>;

		template<typename CallableCommand>
		Command(CallableCommand&& aCallableCommand, const std::string& aName)
			: mName(aName)
			, mCommandFunction(aCallableCommand)
		{
		}

		Command(const Command& aOther);

		Command(Command&& aOther);

		void operator()(eCommandType aCommandType) const;

	private:

		std::string mName;
		CommandFunction mCommandFunction;

	};

}