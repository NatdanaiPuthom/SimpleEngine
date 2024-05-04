#include "MathNodes.h"
#include "../Node/NodeTypeRegistry.h"
#include "../Contexts/ExecutionContextBase.h"
#include "../DataType/DataTypeRegistry.h"
#include <imgui.h>

namespace SCR
{
	struct WaveData
	{
		float t = 0;
	};

	enum class eWaveType
	{
		Sine,
		Cos,
		Linear,
		Triangle
	};

	static const char* GetWaveTypesAsString()
	{
		return "Sine\0Cos\0Linear\0Triangle";
	}

	float SineWave(float t, float aFrequency, float anAmplitude)
	{
		t;
		aFrequency;
		anAmplitude;
		return t;
		//return 
	}

	// Function to generate a triangle wave
	float TriangleWave(float t, float aFrequency, float anAmplitude)
	{
		// Calculate the period based on the frequency
		const float period = 1.0f / aFrequency;

		// Scale the time variable by the frequency and amplitude
		t *= aFrequency;
		float scaledAmplitude = anAmplitude / 2.0f;

		// Calculate the triangle wave value
		float y = (4.0f / period) * std::fabs(t - std::floor(period / 2.0f)) - 1.0f;

		// Scale the triangle wave value by the amplitude
		return y * scaledAmplitude;
	}

	static float Wave(eWaveType aWaveType, float aValue)
	{
		switch (aWaveType)
		{
		case SCRIPT::eWaveType::Sine:
			return std::sinf(aValue);
			break;
		case SCRIPT::eWaveType::Cos:
			return std::cosf(aValue);
			break;
		case SCRIPT::eWaveType::Linear:
			return aValue;
			break;
		case eWaveType::Triangle:
			return 0.f;
			//return TriangleWave();
			break;
		default:
			return 0.f;
			break;
		}
	}

	struct WaveTypeData
	{
		eWaveType waveType = eWaveType::Sine;
	};

	static bool Edit(WaveTypeData& aData)
	{
		int currentItem = static_cast<int>(aData.waveType);
		if (ImGui::Combo("##", &currentItem, GetWaveTypesAsString()))
		{
			aData.waveType = static_cast<eWaveType>(currentItem);
			return true;
		}
		return false;
	}

	static void Save(nlohmann::json& aJson, const WaveTypeData& aData)
	{
		aJson["waveType"] = aData.waveType;
	}

	static void Load(const nlohmann::json& aJson, WaveTypeData& aData)
	{
		aData.waveType = aJson["waveType"];
	}

	static float WaveNode(NodeExecutionContext<ExecutionContextBase> aContext, NodeState<WaveData> aData, float aFrequency, float anAmplitude, float anEquilibrium, WaveTypeData aWaveType)
	{
		aData.value.t += aContext.context.deltaTime;
		return anEquilibrium + Wave(aWaveType.waveType, aData.value.t * aFrequency) * anAmplitude;
	}


	void RegisterMathNodes()
	{
		DataTypeRegistry::Register<WaveTypeData>("Wave Type");
		NodeTypeRegistry::RegisterNodeType(WaveNode, "Utility/Math/Wave", NodeTypeDesc{ { "Frequency", "Amplitude", "Equilibrium", "Wave Function" }, { "Value" } });
	}
}

