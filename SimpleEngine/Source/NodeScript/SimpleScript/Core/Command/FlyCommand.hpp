#pragma once
#include "../FlyDefines.hpp"
#include <string>

namespace FLY_NAMESPACE
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

		Command(Command&& aOther) noexcept;

		void operator()(eCommandType aCommandType) const;

	private:

		std::string mName;
		CommandFunction mCommandFunction;

	};

}