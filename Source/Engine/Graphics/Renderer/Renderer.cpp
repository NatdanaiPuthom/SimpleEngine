#include "Engine/Precomplier/stdafx.h"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"
#include "Engine/Graphics/Renderer/BoundingBoxDrawer.hpp"


Renderer::Renderer()
	: myObjectBuffer(std::make_unique<ConstantBuffer>())
	, myBoundingBoxDrawer(std::make_unique<BoundingBoxDrawer>())
	, myDebugMode(false)
{
	SimpleGlobalRendererImpl::SetRenderer(this);

	if (!CreateObjectBuffer())
		assert(false && "Failed to create ObjectBuffer");
}

Renderer::~Renderer()
{
}

void Renderer::SetModelBuffer(std::vector<ModelInstance*>& aModelBuffer)
{
	myModelBuffer = aModelBuffer;
}

void Renderer::SetDebugMode(const bool aSetDebugMode)
{
	myDebugMode = aSetDebugMode;
}

void Renderer::Render(const std::shared_ptr<const ModelInstance> aModelInstance) const
{
	const auto context = SimpleGlobal::GetGraphicsEngine()->GetContext();

	ObjectBufferData objectBuffer = {};
	objectBuffer.modelToWorldMatrix = aModelInstance->GetMatrix();

	myObjectBuffer->Bind(myObjectBuffer->GetSlot());
	myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

	aModelInstance->myShader->SetShader(context.Get());

	for (const auto& texture : aModelInstance->myTextures)
	{
		texture->Bind(context, texture->GetSlot());
	}

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, aModelInstance->myMesh->myVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(aModelInstance->myMesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->DrawIndexed(static_cast<UINT>(aModelInstance->myMesh->myMeshData.myIndices.size()), 0, 0);

	SimpleGlobalRendererImpl::IncreaseDrawCall();
}

void Renderer::RenderBoundingBox(const std::shared_ptr<const ModelInstance> aModelInstance) const
{
	myBoundingBoxDrawer->Render(aModelInstance);

	SimpleGlobalRendererImpl::IncreaseDrawCall();
}

void Renderer::RenderEverythingUpSideDown(const ModelInstance* const aModelInstance) const
{
	const auto context = SimpleGlobal::GetGraphicsEngine()->GetContext();
	auto camera = SimpleGlobal::GetGraphicsEngine()->GetCamera();

	SimpleUtilities::Matrix4x4f mirror = SimpleUtilities::Matrix4x4f::Identity();
	mirror(2, 2) = -1.0f;

	ObjectBufferData objectBuffer = {};
	objectBuffer.modelToWorldMatrix = mirror * aModelInstance->GetMatrix();

	myObjectBuffer->Bind(myObjectBuffer->GetSlot());
	myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

	aModelInstance->myShader->SetShader(context.Get());

	for (const auto& texture : aModelInstance->myTextures)
	{
		texture->Bind(context, texture->GetSlot());
	}

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, aModelInstance->myMesh->myVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(aModelInstance->myMesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->DrawIndexed(static_cast<UINT>(aModelInstance->myMesh->myMeshData.myIndices.size()), 0, 0);

	SimpleGlobalRendererImpl::IncreaseDrawCall();
}

void Renderer::RenderPlaneReflection(const ModelInstance* const aModelInstance) const
{
	const auto context = SimpleGlobal::GetGraphicsEngine()->GetContext();

	ObjectBufferData objectBuffer = {};
	objectBuffer.modelToWorldMatrix = aModelInstance->GetMatrix();

	myObjectBuffer->Bind(myObjectBuffer->GetSlot());
	myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

	aModelInstance->myShader->SetShader(context.Get());

	SimpleGlobal::GetGraphicsEngine()->GetContext()->PSSetShaderResources(0, 1, SimpleGlobal::GetGraphicsEngine()->GetWaterShaderResourceView().GetAddressOf());

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, aModelInstance->myMesh->myVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(aModelInstance->myMesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->DrawIndexed(static_cast<UINT>(aModelInstance->myMesh->myMeshData.myIndices.size()), 0, 0);
}

bool Renderer::IsDebugModeOn() const
{
	return myDebugMode;
}

std::vector<ModelInstance*> Renderer::GetAllModelInstances()
{
	return myModelBuffer;
}

const bool Renderer::CreateObjectBuffer()
{
	ObjectBufferData objectBuffer;

	if (!myObjectBuffer->Init(SimpleGlobal::GetGraphicsEngine(), sizeof(ObjectBufferData), &objectBuffer))
		return false;

	myObjectBuffer->SetSlot(1);

	return true;
}
