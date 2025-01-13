#pragma once
#include <memory>

struct PointLightBufferData;

namespace Graphics
{
	class ConstantBuffer;
	class Camera;

	struct LightBufferData;
	struct PostProcessData;
}

namespace Graphics
{
	class ConstantBufferManager final
	{
	public:
		ConstantBufferManager();
		~ConstantBufferManager();

		void Init();
		void UpdateCameraConstantBuffer(const Camera* aCamera);
		void UpdateTimeConstantBuffer();
		void UpdateLightConstantBuffer(const LightBufferData* aLightBufferData);
		void UpdatePostProcessConstantBuffer(const PostProcessData* aPostProcessData);
		void UpdatePointlights(const size_t aLightIndex, const PointLightBufferData* aPointLightBufferData);

	private:
		void CreateCameraConstantBuffer();
		void CreateTimeConstantBuffer();
		void CreateJointsConstantBuffer();
		void CreateLightConstantBuffer();
		void CreatePointLightConstantBuffer();
		void CreatePostProcessingConstantBuffer();
	private:
		std::unique_ptr<ConstantBuffer> myCameraConstantBuffer;
		std::unique_ptr<ConstantBuffer> myTimeConstantBuffer;
		std::unique_ptr<ConstantBuffer> myJointsConstantBuffer;
		std::unique_ptr<ConstantBuffer> myLightConstantBuffer;
		std::unique_ptr<ConstantBuffer> myPointLightConstantBuffer;
		std::unique_ptr<ConstantBuffer> myPostProcessConstantBuffer;
	};
}
