#include "Engine/Precomplier/stdafx.h"
#include "Engine/Graphics/Renderer/LineDrawer.hpp"

namespace Simple
{
	LineDrawer::LineDrawer()
		: myObjectBuffer(std::make_unique<Simple::ConstantBuffer>())
		, myMeshData(std::make_unique<MeshData>())
	{
		myShader = SimpleGlobal::GetGraphicsEngine()->GetShader("LinePS.cso", "DefaultVS.cso");

		myMeshData->vertices.reserve(2);
		myMeshData->vertices.resize(2, Vertex{});

		myMeshData->vertices[0].position = { 0.0f,0.0f,0.0f,1.0f };

		for (unsigned int i = 0; i < myMeshData->vertices.size(); ++i)
		{
			myMeshData->indices.push_back(i);
		}

		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * static_cast<int>(myMeshData->vertices.size());
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexData = {};
		vertexData.pSysMem = &myMeshData->vertices[0];
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		HRESULT result = SimpleGlobal::GetGraphicsEngine()->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);

		if (FAILED(result))
			assert(false && "failed to create VertexBuffer");

		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * static_cast<int>(myMeshData->indices.size());
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = &myMeshData->indices[0];
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		result = SimpleGlobal::GetGraphicsEngine()->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);

		if (FAILED(result))
			assert(false && "failed to create IndexBuffer");

		ObjectBufferData objectBuffer;

		if (!myObjectBuffer->Init(sizeof(ObjectBufferData), &objectBuffer))
			assert(false && "failed to create ObjectBuffer");

		myObjectBuffer->SetSlot(1);
	}

	LineDrawer::~LineDrawer()
	{
	}

	void LineDrawer::Render(const Drawer::Line& aLine)
	{
		myMeshData->vertices[0].position.x = aLine.startPosition.x;
		myMeshData->vertices[0].position.y = aLine.startPosition.y;
		myMeshData->vertices[0].position.z = aLine.startPosition.z;
		myMeshData->vertices[0].position.w = 1.0f;

		myMeshData->vertices[1].position.x = aLine.endPosition.x;
		myMeshData->vertices[1].position.y = aLine.endPosition.y;
		myMeshData->vertices[1].position.z = aLine.endPosition.z;
		myMeshData->vertices[1].position.w = 1.0f;

		for (auto& vertex : myMeshData->vertices)
		{
			vertex.color = aLine.color;
		}

		auto context = SimpleGlobal::GetGraphicsEngine()->GetContext();
		context->UpdateSubresource(myVertexBuffer.Get(), 0, nullptr, myMeshData->vertices.data(), 0, 0);

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = SimpleUtilities::Matrix4x4f::Identity();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		myShader->UseThisShader(context.Get());

		context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		context->DrawIndexed(static_cast<UINT>(myMeshData->indices.size()), 0, 0);
	}
}