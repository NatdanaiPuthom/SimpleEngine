#include "Engine/stdafx.h"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"
#include "Engine/Graphics/Model/PlaneReflection.h"

Renderer::Renderer()
	: myObjectBuffer(std::make_unique<ConstantBuffer>())
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

void Renderer::Render(const ModelInstance* const aModelInstance) const
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

void Renderer::RenderPlane(PlaneReflection* aPlaneReflection)
{
	const auto context = SimpleGlobal::GetGraphicsEngine()->GetContext();

	ObjectBufferData objectBuffer = {};
	objectBuffer.modelToWorldMatrix = aPlaneReflection->myModelInstance->GetMatrix();

	myObjectBuffer->Bind(myObjectBuffer->GetSlot());
	myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

	aPlaneReflection->myModelInstance->myShader->SetShader(context.Get());

	aPlaneReflection->myShaderResourceView.Reset();
	aPlaneReflection->myShaderResourceView = nullptr;
	aPlaneReflection->myShaderResourceView = SimpleGlobal::GetGraphicsEngine()->GetWaterShaderResourceView();

	context->PSSetShaderResources(0, 1, aPlaneReflection->myShaderResourceView.GetAddressOf());

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, aPlaneReflection->myModelInstance->myMesh->myVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(aPlaneReflection->myModelInstance->myMesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->DrawIndexed(static_cast<UINT>(aPlaneReflection->myModelInstance->myMesh->myMeshData.myIndices.size()), 0, 0);
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
