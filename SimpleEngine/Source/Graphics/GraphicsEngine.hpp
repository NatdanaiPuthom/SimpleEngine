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
#include "Graphics/Managers/ConstantBufferManager.hpp"
#include <unordered_map>
#include <memory>
#include <array>
#include <string>
#include <vector>
#include <d3d11.h>

namespace Simple
{
	class ImGuiEngine;
}

namespace Graphics
{
	class ConstantBuffer;
}

namespace Graphics
{
	class GraphicsEngine final
	{
	public:
		Microsoft::WRL::ComPtr<ID3D11Buffer> myInstanceBuffer;

		void CreateInstanceBuffer()
		{
			D3D11_BUFFER_DESC instanceBufferDesc;
			instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			instanceBufferDesc.ByteWidth = sizeof(TransformBufferData) * 100;
			instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			instanceBufferDesc.MiscFlags = 0;
			instanceBufferDesc.StructureByteStride = 0;

			HRESULT result = myDevice->CreateBuffer(&instanceBufferDesc, nullptr, &myInstanceBuffer);
			assert(SUCCEEDED(result));
		}

		void GenerateInstanceData(std::vector<TransformBufferData>& instanceData)
		{
			instanceData.clear();

			for (int i = 0; i < 100; ++i)
			{
				TransformBufferData data;
				data.modelWorldMatrix = Math::Matrix4x4f();
				data.modelWorldMatrix.SetPosition(Math::Vector3f(i * 2.0f, 0.0f, 0.0f));
				instanceData.push_back(data);
			}
		}

		void UpdateInstanceBuffer(const std::vector<TransformBufferData>& instanceData)
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			HRESULT hr = myContext->Map(myInstanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

			if (FAILED(hr))
			{
				std::cout << "error" << std::endl;
				return;
			}

			memcpy(mappedResource.pData, instanceData.data(), sizeof(TransformBufferData) * instanceData.size());
			myContext->Unmap(myInstanceBuffer.Get(), 0);
		}

		void RenderInstances()
		{
			const Mesh* mesh = myModelFactory->GetPrimitiveShape(ePrimitiveShape::Cube);
			ID3D11Buffer* vertexBuffer = mesh->myVertexBuffer.Get();
			ID3D11Buffer* indexBuffer = mesh->myIndexBuffer.Get();
			UINT indexCount = static_cast<UINT>(mesh->myMeshData.indices.size());

			std::vector<TransformBufferData> instanceData;
			GenerateInstanceData(instanceData);

			UpdateInstanceBuffer(instanceData);

			UINT strides[2] = { sizeof(Vertex), sizeof(TransformBufferData) };
			UINT offsets[2] = { 0, 0 };
			ID3D11Buffer* buffers[2] = { vertexBuffer, myInstanceBuffer.Get() };

			myContext->IASetVertexBuffers(0, 2, buffers, strides, offsets);
			myContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			auto vertexShader = myShaderManager->GetShader(eShaderType::Unlit_Default);
			vertexShader->BindThisShader(myContext.Get());

			myContext->DrawIndexedInstanced(indexCount, static_cast<UINT>(instanceData.size()), 0, 0, 0);
		}

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
	public:
		void SetGlobalGraphicsEngineToThis();
		void SetRenderTarget(eRenderTargetType aRenderTargetType, const bool aUseDepthBuffer = false);
		void SetToDefaultCamera();
		void SetCamera(Graphics::Camera* aCamera);
		void SetWindowSize(const Math::Vector2ui& aWindowSize, const bool aSetFullScreen);
	public:
		Microsoft::WRL::ComPtr<ID3D11Device> GetDevice();
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext();
		Graphics::Camera* GetCurrentCamera();
		std::shared_ptr<Camera> GetEditorCamera();
		const Graphics::Camera* GetCurrentCamera() const;
		const std::shared_ptr<Camera> GetEditorCamera() const;

		ConstantBufferManager* GetConstantBufferManager();
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
		void CreateSwapChain(HWND& aWindowHandle, const Math::Vector2ui aSize);
	private:
		void LoadSettingsFromJson();
		void PrepareFrame();
		void PreloadShaders();
		void FilterPixelForBloom();
		void DownAndUpSampleForBloom();
		void RenderBloom();
	private:
		Microsoft::WRL::ComPtr<ID3D11Device> myDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> myContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain> mySwapChain;

		std::shared_ptr<Camera> myEditorCamera;

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
