#pragma once
#include "../ScriptDefines.h"
#include "DataTypeManager.h"
#include "../Node/NodeTypeRegistry.h"	
#include "../NodeTypes/OperatorNodes.h"

namespace SCR
{

	class DataTypeRegistry
	{
	public:

		template<typename T, eNodeOperatorTrait Operators = eNodeOperatorTrait::All, template<typename> typename... Templates> requires Scriptable<T, nlohmann::json> || Fundamental<T>
		static void Register(const std::string& aName, const Color& aColor = DefaultColor);

		template<typename T, eNodeOperatorTrait Operators = eNodeOperatorTrait::All>
		static void RegisterNonSerializableType(const std::string& aName, const Color& aColor = DefaultColor);

		template<template<typename> typename TemplateType>
		static void RegisterTemplateType(const std::string& aName);

		template<typename ClassType, typename PropertyType>
		static void RegisterProperty(PropertyType ClassType::* aProperty, const std::string& aName);

	private:

		template<typename T, template<typename> typename TemplateType>
		static void RegisterTemplateSpecification(const std::string& aTemplateArgumentName);

		template<typename T, template<typename> typename... TemplateTypes>
		static void RegisterTemplateTypes(const std::string& aTemplateArgumentName);

	};

	template<typename T, eNodeOperatorTrait Operators, template<typename> typename... Templates> requires Scriptable<T, nlohmann::json> || Fundamental<T>
	inline void DataTypeRegistry::Register(const std::string & aName, const Color& aColor)
	{
		DataTypeManager::Register<T>(aName, aColor);

		RegisterTemplateTypes<T, Templates...>(aName);
		RegisterGetterNodeType<T>();
		RegisterSetterNodeType<T>();
		RegisterOperatorNodeTypes<T, Operators>();
	}

	template<typename T, eNodeOperatorTrait Operators>
	inline void DataTypeRegistry::RegisterNonSerializableType(const std::string& aName, const Color& aColor)
	{
		DataTypeManager::RegisterNonSerializableType<T>(aName, aColor);

		RegisterGetterNodeType<T>();
		RegisterSetterNodeType<T>();
		RegisterOperatorNodeTypes<T, Operators>();

	}

	template<template<typename> typename TemplateType>
	inline void DataTypeRegistry::RegisterTemplateType(const std::string& aName)
	{
		DataTypeManager::RegisterTemplateType<TemplateType>(aName);
	}

	template<typename ClassType, typename PropertyType>
	inline void DataTypeRegistry::RegisterProperty(PropertyType ClassType::* aProperty, const std::string& aName)
	{
		DataTypeManager::RegisterProperty(aProperty, aName);
	}

	template<typename T, template<typename> typename TemplateType>
	inline void DataTypeRegistry::RegisterTemplateSpecification(const std::string& aTemplateArgumentName)
	{
		std::string templateTypeName;

		if (DataTypeManager::myTemplateDataTypes.contains(typeid(TemplateType).hash_code()))
		{
			templateTypeName = DataTypeManager::myTemplateDataTypes.at(typeid(TemplateType).hash_code()).name;
		}
		else
		{
			templateTypeName = typeid(TemplateType).name();
		}

		using Specification = TemplateType<T>;

		DataTypeManager::RegisterTemplateSpecification<Specification>(templateTypeName + std::string("<") + aTemplateArgumentName + ">", DefaultColor);
		RegisterGetterNodeType<Specification>();
		RegisterSetterNodeType<Specification>();
		RegisterOperatorNodeTypes<Specification, eNodeOperatorTrait::All>();
	}

	template<typename T, template<typename> typename... TemplateTypes>
	inline void DataTypeRegistry::RegisterTemplateTypes(const std::string& aTemplateArgumentName)
	{
		(RegisterTemplateSpecification<T, TemplateTypes>(aTemplateArgumentName), ...);
	}
}