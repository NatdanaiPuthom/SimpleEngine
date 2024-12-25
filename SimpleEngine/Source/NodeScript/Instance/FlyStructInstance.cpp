#include "FlyStructInstance.hpp"
#include "../DataType/FlyStruct.hpp"

namespace FLY_NAMESPACE
{
    StructInstance::StructInstance(const Struct& aStruct)
        : mStruct(&aStruct)
        , mVariableContainerInstance(aStruct.mVariableContainer)
    {
    }
}
