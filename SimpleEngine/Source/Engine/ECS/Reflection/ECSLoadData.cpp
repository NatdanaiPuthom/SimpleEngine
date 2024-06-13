#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Reflection/ECSLoadData.hpp"

namespace ECS
{
	bool LoadAndSetDataFromJSON(int& aValue, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		aValue = aJSONData[aVariableName];
		return true;
	}

	bool LoadAndSetDataFromJSON(float& aValue, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		aValue = aJSONData[aVariableName];
		return true;
	}

	bool LoadAndSetDataFromJSON(bool& aValue, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		aValue = aJSONData[aVariableName];
		return true;
	}

	bool LoadAndSetDataFromJSON(Math::Transform& aTransform, const std::string& aVariableName, const nlohmann::json& aJSONData)
	{
		Math::Vector3f position;
		position.x = aJSONData[aVariableName]["Position"]["x"];
		position.y = aJSONData[aVariableName]["Position"]["y"];
		position.z = aJSONData[aVariableName]["Position"]["z"];

		Math::Vector3f rotation;
		rotation.x = aJSONData[aVariableName]["Rotation"]["x"];
		rotation.y = aJSONData[aVariableName]["Rotation"]["y"];
		rotation.z = aJSONData[aVariableName]["Rotation"]["z"];

		Math::Vector3f scale;
		scale.x = aJSONData[aVariableName]["Scale"]["x"];
		scale.y = aJSONData[aVariableName]["Scale"]["y"];
		scale.z = aJSONData[aVariableName]["Scale"]["z"];

		aTransform.SetPosition(position);
		aTransform.SetRotation(rotation);
		aTransform.SetScale(scale);

		return true;
	}
}