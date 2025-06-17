#include "FlyStructInstance.hpp"
#include "../DataType/FlyStruct.hpp"

namespace FLY_NAMESPACE
{
    StructInstance::StructInstance(const Struct& aStruct)
        : mStruct(&aStruct)
        , mVariableContainerInstance(aStruct.GetVariableContainer())
    {
    }

    bool operator==(const StructInstance& a, const StructInstance& b)
    {
        if (!a && !b)
        {
            return true;
        }

        return a.mVariableContainerInstance == b.mVariableContainerInstance;
    }
}
