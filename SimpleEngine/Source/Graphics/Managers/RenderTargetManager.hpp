#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Graphics/GraphicsDeclarations.hpp"
#include <array>
#include <vector>
#include <d3d11.h>
#include <wrl/client.h>

struct ID3D11DeviceContext;
struct ID3D11DepthStencilView;
struct ID3D11Device;
struct IDXGISwapChain;

namespace Graphics
{
	class RenderTargetManager final
	{
	public:
		RenderTargetManager();
		~RenderTargetManager();
		
		void Init(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, Microsoft::WRL::ComPtr<IDXGISwapChain> aSwapChain, const Math::Vector2ui& aResolution);

		void UnbindAllRenderTargets(Microsoft::WRL::ComPtr<ID3D11DeviceContext> aContext);
		void ClearRenderTarget(Microsoft::WRL::ComPtr<ID3D11DeviceContext> aContext, const eRenderTargetType aRenderTargetType, const std::array<float, 4>& aClearColor);
		void ClearDepthStencilView(Microsoft::WRL::ComPtr<ID3D11DeviceContext> aContext);
		void ClearAllRenderTargets(Microsoft::WRL::ComPtr<ID3D11DeviceContext> aContext, const std::array<float, 4>& aClearColor);
		void ReleaseDepthBuffer();
	public:
		std::vector<RenderTarget> CreateRenderTargets(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, const size_t aRenderTargetCount, DXGI_FORMAT* aArrayOfFormats, const Math::Vector2ui& aResolution);
		void CreateBackBuffer(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, Microsoft::WRL::ComPtr<IDXGISwapChain> aSwapChain);
		void CreateDepthBuffer(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, const Math::Vector2ui& aResolution);
		void CreateBloomRenderTarget(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, const Math::Vector2ui& aResolution);
		void CreateBloomDownAndUpSampleRenderTarget(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, const Math::Vector2ui& aResolution);
		void CreatePostProcessingRenderTarget(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, const Math::Vector2ui& aResolution);
		void CreateDeferredRenderTarget(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, const Math::Vector2ui& aResolution);
		void CreateGRenderTarget(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, const Math::Vector2ui& aResolution);
	public:
		void SetRenderTarget(Microsoft::WRL::ComPtr<ID3D11DeviceContext> aContext, eRenderTargetType aRenderTargetType, const bool aUseDepthBuffer);
	public:
		std::vector<RenderTarget>& GetRenderTargets(const eRenderTargetType aRenderTargetType);
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResourceView(const eRenderTargetType aRenderTargetType, const size_t aIndex = 0);
	private:
		std::array<std::vector<RenderTarget>, static_cast<size_t>(eRenderTargetType::Count)> myRenderTargets;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> myDepthBuffer;
	};
}