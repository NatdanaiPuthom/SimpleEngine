#include "MathNodes.hpp"
#include "../FlyRegistration.hpp"
#include <imgui.h>

namespace FLY_NAMESPACE
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

	static float CalculateFrequencyByType(eWaveType aWaveType, float aValue)
	{
		switch (aWaveType)
		{
		case eWaveType::Sine:
			return std::sinf(aValue);
			break;
		case eWaveType::Cos:
			return std::cosf(aValue);
			break;
		case eWaveType::Linear:
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

	bool Edit(WaveTypeData& aData)
	{
		int currentItem = static_cast<int>(aData.waveType);
		if (ImGui::Combo("##", &currentItem, GetWaveTypesAsString()))
		{
			aData.waveType = static_cast<eWaveType>(currentItem);
			return true;
		}
		return false;
	}

	void Save(const WaveTypeData& aData, nlohmann::json& aJson)
	{
		aJson["waveType"] = aData.waveType;
	}

	void Load(WaveTypeData& aData, const nlohmann::json& aJson)
	{
		aData.waveType = aJson["waveType"];
	}

	float Wave(NodeExecutionContext<ExecutionContextBase> aContext, NodeState<WaveData> aState, float aFrequency, float aAmplitude, float aEquilibrium, WaveTypeData aWaveType)
	{
		aState.mValue.t += aContext.mContext.mDeltaTime;
		return aEquilibrium + CalculateFrequencyByType(aWaveType.waveType, aState.mValue.t * aFrequency) * aAmplitude;
	}

	FLY_STRUCT(WaveTypeData, eNodeOperatorTrait::None, DefaultColor);

	FLY_FUNCTION(sinf, Directory{ "Utility/Math" }, Pure{});
	FLY_FUNCTION(cosf, Directory{ "Utility/Math" }, Pure{});
	FLY_FUNCTION(asinf, Directory{ "Utility/Math" }, Pure{});
	FLY_FUNCTION(acosf, Directory{ "Utility/Math" }, Pure{});
	FLY_FUNCTION(Wave, Directory{ "Utility/Math" }, InputNames{ "Frequency", "Amplitude", "Equilibrium", "Wave Type" }, Pure{});

}

