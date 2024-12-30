#pragma once
#include "../FlyDefines.hpp"

namespace FLY_NAMESPACE
{

    class Trait final
    {
    public:

        Trait(std::string_view aName);


    private:

        std::string mName;
        
    };
}