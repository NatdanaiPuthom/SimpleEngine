#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/ConstantBuffer/ConstantBufferManager.hpp"
#include "Graphics/ConstantBuffer/ConstantBuffer.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Engine/Global.hpp"

namespace Graphics
{
	ConstantBufferManager::ConstantBufferManager()
	{
	}

	ConstantBufferManager::~ConstantBufferManager()
	{
	}

	void ConstantBufferManager::Init()
	{
		myCameraConstantBuffer = std::make_unique<ConstantBuffer>();
		myTimeConstantBuffer = std::make_unique<ConstantBuffer>();
		myJointsConstantBuffer = std::make_unique<ConstantBuffer>();
		myLightConstantBuffer = std::make_unique<ConstantBuffer>();
		//myPostProcessConstantBuffer = std::make_unique<ConstantBuffer>();
		myPointLightConstantBuffer = std::make_unique<ConstantBuffer>();

		CreateCameraConstantBuffer();
		CreateTimeConstantBuffer();
		CreateLightConstantBuffer();
		CreatePointLightConstantBuffer();
		//CreatePostProcessingConstantBuffer();
		CreateJointsConstantBuffer();

		myCameraConstantBuffer->SetSlot(Global_Constant_Buffer_Slot_Camera);
		myTimeConstantBuffer->SetSlot(Global_Constant_Buffer_Slot_Time);
		myLightConstantBuffer->SetSlot(Global_Constant_Buffer_Slot_Light);
		myJointsConstantBuffer->SetSlot(Global_Constant_Buffer_Slot_Joints);
		myPointLightConstantBuffer->SetSlot(Global_Constant_Buffer_Slot_Pointlight);
		//myPostProcessConstantBuffer->SetSlot(Global_Constant_Buffer_Slot_PostProcess);
	}

	void ConstantBufferManager::UpdateCameraConstantBuffer(const Camera* aCamera)
	{
		CameraBufferData frameBuffer = {};
		frameBuffer.worldToClipMatrix = aCamera->GetWorldToClipMatrix();
		frameBuffer.cameraPosition = aCamera->GetPosition();
		frameBuffer.resolution = Global::GetResolution();

		myCameraConstantBuffer->Bind(myCameraConstantBuffer->GetSlot());
		myCameraConstantBuffer->Update(sizeof(CameraBufferData), &frameBuffer);
	}

	void ConstantBufferManager::UpdateTimeConstantBuffer()
	{
		TimeBufferData timeBuffer = {};
		timeBuffer.totalTime = static_cast<float>(Global::GetTotalTime());
		timeBuffer.deltaTime = static_cast<float>(Global::GetDeltaTime());
		myTimeConstantBuffer->Bind(myTimeConstantBuffer->GetSlot());
		myTimeConstantBuffer->Update(sizeof(TimeBufferData), &timeBuffer);
	}

	void ConstantBufferManager::UpdateLightConstantBuffer(const LightBufferData* aLightBufferData)
	{
		LightBufferData lightBufferData;

		lightBufferData.ambientLightColorAndIntensity = aLightBufferData->ambientLightColorAndIntensity;
		lightBufferData.directionalLightColorAndIntensity = aLightBufferData->directionalLightColorAndIntensity;
		lightBufferData.directionalLightDirection = aLightBufferData->directionalLightDirection;

		myLightConstantBuffer->Bind(myLightConstantBuffer->GetSlot());
		myLightConstantBuffer->Update(sizeof(LightBufferData), &lightBufferData);
	}

	/*void BufferManager::UpdatePostProcessConstantBuffer(const PostProcessData* aPostProcessData)
	{
		myPostProcessConstantBuffer->Bind(myPostProcessConstantBuffer->GetSlot());
		myPostProcessConstantBuffer->Update(sizeof(PostProcessData), &aPostProcessData);
	}*/

	void ConstantBufferManager::UpdatePointlights(const size_t aLightIndex, const PointLightBufferData* aPointLightBufferData)
	{
		PointLightBufferData pointLightData;
		pointLightData.currentPointLightCount = aPointLightBufferData->currentPointLightCount;

		for (size_t i = 0; i < aPointLightBufferData->currentPointLightCount; i++)
		{
			pointLightData.pointLightData[i] = aPointLightBufferData->pointLightData[aLightIndex];
		}

		myPointLightConstantBuffer->Bind(myPointLightConstantBuffer->GetSlot());
		myPointLightConstantBuffer->Update(sizeof(PointLightBufferData), &pointLightData);
	}

	void ConstantBufferManager::CreateCameraConstantBuffer()
	{
		CameraBufferData cameraBuffer;

		cameraBuffer.worldToClipMatrix = Math::Matrix4x4f::Identity();
		cameraBuffer.cameraPosition = Math::Vector3f{ 0.0f,0.0f,0.0f };

		if (!myCameraConstantBuffer->Init(sizeof(CameraBufferData), &cameraBuffer))
			assert(false && "Failed to create CameraConstantBuffer");
	}

	void ConstantBufferManager::CreateTimeConstantBuffer()
	{
		TimeBufferData timeBuffer;

		timeBuffer.totalTime = 0.0f;
		timeBuffer.deltaTime = 0.0f;

		if (!myTimeConstantBuffer->Init(sizeof(TimeBufferData), &timeBuffer))
			assert(false && "Failed to create TimeConstantBuffer");
	}

	void ConstantBufferManager::CreateLightConstantBuffer()
	{
		LightBufferData lightBufferData;

		lightBufferData.directionalLightColorAndIntensity = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		lightBufferData.directionalLightDirection = Math::Vector3f(0.0f, 0.0f, 0.0f);

		if (myLightConstantBuffer->Init(sizeof(LightBufferData), &lightBufferData) == false)
		{
			assert(false && "Failed to create LightConstantBuffer");
		}
	}

	void ConstantBufferManager::CreatePointLightConstantBuffer()
	{
		PointLightBufferData pointLightBufferData;

		if (myPointLightConstantBuffer->Init(sizeof(PointLightBufferData), &pointLightBufferData) == false)
		{
			assert(false && "Failed to create PointLightConstantBuffer");
		}
	}

	/*void BufferManager::CreatePostProcessingConstantBuffer()
	{
		PostProcessData postProcessingData;

		if (myPostProcessConstantBuffer->Init(sizeof(LightBufferData), &postProcessingData) == false)
		{
			assert(false && "Failed to create LightConstantBuffer");
		}
	}*/

	void ConstantBufferManager::CreateJointsConstantBuffer()
	{
		JointsBufferData bonesBufferData;

		if (myJointsConstantBuffer->Init(sizeof(JointsBufferData), &bonesBufferData) == false)
		{
			assert(false && "Failed to create BoneConstantBuffer");
		}
	}
}
