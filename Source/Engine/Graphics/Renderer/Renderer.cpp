#include "Engine/stdafx.h"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"

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

//void Renderer::RenderPlane(PlaneReflection* aModelInstance)
//{
//	const auto context = SimpleGlobal::GetGraphicsEngine()->GetContext();
//
//	SimpleUtilities::Matrix4x4f mirror = SimpleUtilities::Matrix4x4f::Identity();
//	mirror(2, 2) = -1;
//	mirror(4, 2) = (-2.0f * SimpleGlobal::GetGraphicsEngine()->GetCamera()->GetPosition().y * aModelInstance->myPlaneReflection->GetPosition().y * 2.0f);
//
//	ObjectBufferData objectBuffer = {};
//	objectBuffer.modelToWorldMatrix = mirror * aModelInstance->myPlaneReflection->GetMatrix();
//
//	myObjectBuffer->Bind(myObjectBuffer->GetSlot());
//	myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);
//
//	aModelInstance->myPlaneReflection->myShader->SetShader(context.Get());
//
//	aModelInstance->myShaderResourceView.Reset();
//	aModelInstance->myShaderResourceView = nullptr;
//	aModelInstance->myShaderResourceView = SimpleGlobal::GetGraphicsEngine()->GetWaterShaderResourceView();
//
//	context->PSSetShaderResources(0, 1, aModelInstance->myShaderResourceView.GetAddressOf());
//
//	UINT stride = sizeof(Vertex);
//	UINT offset = 0;
//
//	context->IASetVertexBuffers(0, 1, aModelInstance->myPlaneReflection->myMesh->myVertexBuffer.GetAddressOf(), &stride, &offset);
//	context->IASetIndexBuffer(aModelInstance->myPlaneReflection->myMesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
//	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	context->DrawIndexed(static_cast<UINT>(aModelInstance->myPlaneReflection->myMesh->myMeshData.myIndices.size()), 0, 0);
//}

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
