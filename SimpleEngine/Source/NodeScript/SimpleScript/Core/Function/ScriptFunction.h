#pragma once
#include "../ScriptDefines.h"
#include "../ScriptNodeGraph.h"

namespace SCR
{
	class Script;

	class Function final
	{
	public:

		Function(const std::string& aName);
		~Function();

		const NodeTypeID& GetCallerNodeTypeID() const
		{
			return myCallerNodeTypeID;
		}

		const NodeTypeID& GetInputNodeTypeID() const
		{
			return myInputNodeTypeID;
		}

		const NodeTypeID& GetOutputNodeTypeID() const
		{
			return myOutputNodeTypeID;
		}

		const NodeID& GetInputNodeID() const
		{
			return myInputNodeID;
		}

		const NodeID& GetOutputNodeID() const
		{
			return myOutputNodeID;
		}

		struct FunctionCaller
		{
			NodeID nodeID;
			NodeGraph* nodeGraph;
		};

		const FunctionCaller& GetCaller() const
		{
			return myCaller;
		}

		void SetCaller(const FunctionCaller& aCaller)
		{
			myCaller = aCaller;
		}

	private:

		std::string myName;

		NodeTypeID myCallerNodeTypeID;
		NodeTypeID myInputNodeTypeID;
		NodeTypeID myOutputNodeTypeID;

		NodeID myInputNodeID;
		NodeID myOutputNodeID;

		// Temp
		FunctionCaller myCaller;
		
		NodeGraph myNodeGraph;
	};
}