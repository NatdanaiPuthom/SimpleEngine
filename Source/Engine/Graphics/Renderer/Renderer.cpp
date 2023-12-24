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

void Renderer::Render(const ModelInstance& aModelInstance)
{
	const auto context = SimpleGlobal::GetGraphicsEngine()->GetContext();

	ObjectBufferData objectBuffer = {};
	objectBuffer.modelToWorldMatrix = aModelInstance.GetMatrix();

	myObjectBuffer->Bind(myObjectBuffer->GetSlot());
	myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

	aModelInstance.myShader->SetShader(context.Get());

	for (const auto& texture : aModelInstance.myTextures)
	{
		texture->Bind(context, texture->GetSlot());
	}

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer = aModelInstance.myMesh->myVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer = aModelInstance.myMesh->myIndexBuffer;

	context->IASetVertexBuffers(0, 1, aModelInstance.myMesh->myVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(aModelInstance.myMesh->myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->DrawIndexed(static_cast<UINT>(aModelInstance.myMesh->myMeshData.myIndices.size()), 0, 0);

	SimpleGlobalRendererImpl::IncreaseDrawCall();
}

std::vector<ModelInstance*> Renderer::GetAllModelInstances()
{
	std::vector<ModelInstance*> modelInstances;

	for (auto& model : myModelInstances)
	{
		modelInstances.push_back(model);
	}

	return modelInstances;
}

const bool Renderer::CreateObjectBuffer()
{
	ObjectBufferData objectBuffer;

	if (!myObjectBuffer->Init(SimpleGlobal::GetGraphicsEngine(), sizeof(ObjectBufferData), &objectBuffer))
		return false;

	myObjectBuffer->SetSlot(1);

	return true;
}
