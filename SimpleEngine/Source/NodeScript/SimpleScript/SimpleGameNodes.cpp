#include "SimpleScript/SimpleGameNodes.h"
#include "SimpleScript/Core/Node/NodeTypeRegistry.h"
#include "SimpleScript/Core/DataType/DataTypeRegistry.h"

template<typename T>
std::string ToString(T a)
{
	return std::to_string(a);
}

void RegisterSimpleGameNodes()
{
	SCRIPT::NodeTypeRegistry::RegisterNodeType(ToString<int>, "Utility/String/To String (Int)");
}