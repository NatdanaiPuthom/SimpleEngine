#pragma once
#include "FlyFlow.hpp"
#include <typeinfo>

namespace FLY_NAMESPACE
{
	DataTypeID Flow::mTypeID = typeid(Flow).hash_code();
}