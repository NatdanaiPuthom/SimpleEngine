#pragma once
#include "../ScriptDefines.h"
#include "../Function/ScriptFunction.h"
#include "NodeView.h"
#include "NodeTypeView.h"

namespace SCR
{
	
	class FunctionView final
	{
	public:


		FunctionView(FunctionID aFunctionID);

		const std::string& GetName() const;
		NodeView GetInputNode() const;
		NodeView GetOutputNode() const;
		NodeTypeView GetCallerNodeType() const;
		NodeTypeView GetInputNodeType() const;
		NodeTypeView GetOutputNodeType() const;
		NodeGraph& GetNodeGraph();

		FunctionID GetID() const;


	private:

		Function& GetFunction();
		const Function& GetFunction() const;

	private:

		FunctionID myFunctionID;
	};
}