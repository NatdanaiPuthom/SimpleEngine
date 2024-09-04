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
		using CommandFunction = std::function<void(eCommandType)>;
	public:


		template<typename CallableCommand>
		Command(CallableCommand&& aCallableCommand, const std::string& aName)
			: mName(aName)
			, mCommandFunction(aCallableCommand)
		{
		}

		void operator()(eCommandType aCommandType) const;

	private:

		std::string mName;
		CommandFunction mCommandFunction;

	};

}