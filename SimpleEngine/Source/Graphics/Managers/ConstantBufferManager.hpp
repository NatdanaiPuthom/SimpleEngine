#pragma once
#include "Engine/Math/Vector2.hpp"
#include <memory>

namespace Graphics
{
	class ConstantBuffer;
	class Camera;

	struct LightBufferData;
	struct PostProcessData;
	struct PointLightBufferData;
}

namespace Graphics
{
	class ConstantBufferManager final
	{
	public:
		ConstantBufferManager();
		~ConstantBufferManager();

		void Init();
		void UpdateCameraConstantBuffer(const Camera* aCamera, const Math::Vector2ui& aResolution);
		void UpdateTimeConstantBuffer(const float aTotalTime, const float aDeltaTime);
		void UpdateLightConstantBuffer(const LightBufferData* aLightBufferData);
		void UpdatePostProcessConstantBuffer(const PostProcessData* aPostProcessData);
		void UpdatePointlights(const size_t aLightIndex, const PointLightBufferData* aPointLightBufferData);
		void UpdateSkyBoxConstantBuffer(const bool aShouldUseFlooring);
	private:
		void CreateCameraConstantBuffer();
		void CreateTimeConstantBuffer();
		void CreateJointsConstantBuffer();
		void CreateLightConstantBuffer();
		void CreatePointLightConstantBuffer();
		void CreatePostProcessingConstantBuffer();
		void CreateSkyBoxConstantBuffer();
	private:
		std::unique_ptr<ConstantBuffer> myCameraConstantBuffer;
		std::unique_ptr<ConstantBuffer> myTimeConstantBuffer;
		std::unique_ptr<ConstantBuffer> myJointsConstantBuffer;
		std::unique_ptr<ConstantBuffer> myLightConstantBuffer;
		std::unique_ptr<ConstantBuffer> myPointLightConstantBuffer;
		std::unique_ptr<ConstantBuffer> myPostProcessConstantBuffer;
		std::unique_ptr<ConstantBuffer> mySkyBoxConstantBuffer;
	};
}
