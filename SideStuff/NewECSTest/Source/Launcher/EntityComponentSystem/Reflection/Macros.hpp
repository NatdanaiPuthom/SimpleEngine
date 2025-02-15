#pragma once

#define ECS_DATATYPE_NAME(aName) #aName
#define ECS_CONVERT_TO_STRING(aName) ECS_DATATYPE_NAME(aName)

#define ECS_COMBINE_STRINGS_2(aName, aCounter) aName##aCounter
#define ECS_UNIQUE_NAME_2(aName, aCounter) ECS_COMBINE_STRINGS_2(aName, aCounter)