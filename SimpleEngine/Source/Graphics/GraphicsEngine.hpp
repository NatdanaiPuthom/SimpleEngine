#pragma once
#include "Graphics/BufferData.hpp"
#include "Graphics/GraphicsDeclarations.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Renderer/Renderer.hpp"
#include "Graphics/Model/Factory/ModelFactory.hpp"
#include "Graphics/Managers/GenericDataManager.hpp"
#include "Graphics/Managers/TextureManager.hpp"
#include "Graphics/Managers/LightManager.hpp"
#include "Graphics/Managers/StateManager.hpp"
#include "Graphics/Managers/ShaderManager.hpp"
#include "Graphics/Managers/RenderTargetManager.hpp"
#include <unordered_map>
#include <memory>
#include <array>
#include <string>
#include <vector>

namespace Simple
{
	class ImGuiEngine;
}

namespace Graphics
{
	class ConstantBuffer;
	class ConstantBufferManager;
}

namespace Graphics
{
	class GraphicsEngine final
	{
	public:
		GraphicsEngine();
		~GraphicsEngine();

		void Init(HWND& aWindowHandle, const Math::Vector2ui& aWindowSize);

		bool BeginFrame();
		void EndFrame();

		void ApplyAmbientAndDirectionalLightDeferred(const eRenderTargetType aRenderTargetType);
		void ApplyPostProcessing(const eRenderTargetType aRenderTargetType);
		void ApplyBloom();
		void RenderFullScreenQuad();
		void RenderFullScreenCopy(const eRenderTargetType aRenderTargetType);
		void UpdatePointlights(const size_t aLightIndex);
		void UpdateLightBuffer();
	public:
		void SetRenderTarget(eRenderTargetType aRenderTargetType, const bool aUseDepthBuffer = false);
		void SetToDefaultCamera();
		void SetGlobalGraphicsEngineToThis();
		void SetCamera(Graphics::Camera* aCamera);
		void SetWindowSize(const Math::Vector2ui& aWindowSize, const bool aSetFullScreen);
	public:
		ComPtr<ID3D11Device> GetDevice();
		ComPtr<ID3D11DeviceContext> GetContext();
		Graphics::Camera* GetCurrentCamera();
		std::shared_ptr<Camera> GetEditorCamera();
		const Graphics::Camera* GetCurrentCamera() const;
		const std::shared_ptr<Camera> GetEditorCamera() const;

		RenderTargetManager* GetRenderTargetManager();
		GenericDataManager* GetGenericDataManager();
		ShaderManager* GetShaderManager();
		StateManager* GetStateManager();
		LightManager* GetLightManager();
		TextureManager* GetTextureManager();
		ModelFactory* GetModelFactory();
		Drawer::Renderer* GetRenderer();
		const Drawer::Renderer* GetRenderer() const;
		const ModelFactory* GetModelFactory() const;
	private:
		void CreateViewport(const Math::Vector2ui aSize);
		void CreateSwapChain(HWND& aWindowHandle, const Math::Vector2ui aSize);
	private:
		void LoadSettingsFromJson();
		void PrepareFrame();
		void PreloadShaders();
		void FilterPixelForBloom();
		void DownAndUpSampleForBloom();
		void RenderBloom();
	private:
		ComPtr<ID3D11Device> myDevice;
		ComPtr<ID3D11DeviceContext> myContext;
		ComPtr<IDXGISwapChain> mySwapChain;

		std::shared_ptr<Camera> myEditorCamera;
		std::shared_ptr<const D3D11_VIEWPORT> myViewPort;

		std::unique_ptr<GenericDataManager> myGenericDataManager;
		std::unique_ptr<RenderTargetManager> myRenderTargetManager;
		std::unique_ptr<ConstantBufferManager> myConstantBufferManager;
		std::unique_ptr<ShaderManager> myShaderManager;
		std::unique_ptr<StateManager> myStateManager;
		std::unique_ptr<LightManager> myLightManager;
		std::unique_ptr<TextureManager> myTextureManager;
		std::unique_ptr<ModelFactory> myModelFactory;
		std::unique_ptr<Drawer::Renderer> myRenderer;
		std::unique_ptr<Simple::ImGuiEngine> myImGuiEngine;

		Camera* myCurrentCameraRaw;
	};
}
