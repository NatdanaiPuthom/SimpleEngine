#pragma once
#include "../ScriptDefines.hpp"
#include "../Function/ScriptFunction.hpp"
#include "NodeView.hpp"
#include "NodeTypeView.hpp"

namespace FLY_NAMESPACE
{
	
	class FunctionView final
	{
	public:


		explicit FunctionView(FunctionID aFunctionID);

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

		FunctionID mFunctionID;
	};
}