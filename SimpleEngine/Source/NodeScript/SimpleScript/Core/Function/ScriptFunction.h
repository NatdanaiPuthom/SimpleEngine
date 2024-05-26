#pragma once
#include "../ScriptDefines.h"
//#include "Script.h"

namespace SCR
{

	class Function final
	{
	public:

		Function(const std::string& aName);
		~Function();

		inline const NodeTypeID& GetCallerNodeTypeID() const
		{
			return myCallerNodeTypeID;
		}

		inline const NodeTypeID& GetInputNodeTypeID() const
		{
			return myInputNodeTypeID;
		}

		inline const NodeTypeID& GetOutputNodeTypeID() const
		{
			return myOutputNodeTypeID;
		}

		inline const NodeID& GetInputNodeID() const
		{
			return myInputNodeID;
		}

		inline const NodeID& GetOutputNodeID() const
		{
			return myOutputNodeID;
		}
	private:

		std::string myName;

		NodeTypeID myCallerNodeTypeID;
		NodeTypeID myInputNodeTypeID;
		NodeTypeID myOutputNodeTypeID;

		NodeID myInputNodeID;
		NodeID myOutputNodeID;

		//Script myScript;
	};
}