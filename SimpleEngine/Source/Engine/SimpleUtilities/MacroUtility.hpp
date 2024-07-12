#pragma once

#define DATATYPE_NAME(aName) #aName
#define CONVERT_TO_STRING(aName) DATATYPE_NAME(aName)

#define PROPERTY_DETAIL(aName, aNumberCounter) aName##aNumberCounter
#define COMBINE_FOR_UNIQUE_NAME(aName, aNumberCounter) PROPERTY_DETAIL(aName, aNumberCounter)