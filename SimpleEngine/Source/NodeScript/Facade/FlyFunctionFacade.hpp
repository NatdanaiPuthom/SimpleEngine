#pragma once
#include "../FlyDefines.hpp"
#include "../Function/FlyFunction.hpp"
#include "FlyNodeFacade.hpp"
#include "FlyNodeTypeFacade.hpp"
#include "FlyNodeGraphFacade.hpp"

namespace FLY_NAMESPACE
{
	
	class FunctionFacade final
	{
	public:

		FunctionFacade() = default;
		explicit FunctionFacade(FunctionID aFunctionID);

		const std::string& GetName() const;
		NodeFacade GetInputNodeFacade() const;
		NodeFacade GetOutputNodeFacade() const;
		NodeTypeFacade GetCallerNodeTypeFacade() const;
		NodeTypeFacade GetInputNodeTypeFacade() const;
		NodeTypeFacade GetOutputNodeTypeFacade() const;
		NodeGraphFacade GetNodeGraphFacade();

		FunctionID GetID() const;

		void SetName(std::string_view aName, CommandTracker* aCommandTracker);
		void AddPin(DataTypeFacade aDataTypeFacade, eFlowType aFlowType, std::string_view aName, CommandTracker* aCommandTracker);
		void SetPinNameAtIndex(std::string_view aName, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker);
		void SetPinDataTypeAtIndex(DataTypeFacade aDataTypeFacade, size_t aIndex, eFlowType aFlowType, CommandTracker* aCommandTracker);

		explicit operator bool() const;

		friend bool operator==(const FunctionFacade& a, const FunctionFacade& b);

	private:

		Function& GetFunction();
		const Function& GetFunction() const;

	private:

		FunctionID mFunctionID = InvalidID<FunctionID>();
	};
}