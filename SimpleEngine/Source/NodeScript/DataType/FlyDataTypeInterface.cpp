#include "FlyDataTypeInterface.hpp"

namespace FLY_NAMESPACE
{
	DataTypeInterface::DataTypeInterface(FundamentalInterface aFundamentalInterface, FunctionInterface aFunctionInterface, ExecutionInterface aExecutionInterface)
		: mFundamental(aFundamentalInterface)
		, mFunction(aFunctionInterface)
		, mExecution(aExecutionInterface)
	{
	}
}