#include "Engine/Precomplier/stdafx.h"
#include "Engine/Graphics/Renderer/BoundingBoxDrawer.hpp"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"

namespace Drawer
{
	BoundingBoxDrawer::BoundingBoxDrawer()
		: myObjectBuffer(std::make_unique<Simple::ConstantBuffer>())
	{
		myShader = SimpleGlobal::GetGraphicsEngine()->GetShader("LinePS.cso", "DefaultVS.cso");

		myMeshData.vertices.reserve(24);
		myMeshData.vertices.resize(24, Vertex{});

		for (unsigned int i = 0; i < myMeshData.vertices.size(); ++i)
		{
			myMeshData.indices.push_back(i);
		}

		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * static_cast<int>(myMeshData.vertices.size());
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexData = {};
		vertexData.pSysMem = &myMeshData.vertices[0];
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		HRESULT result = SimpleGlobal::GetGraphicsEngine()->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);

		if (FAILED(result))
			assert(false && "failed to create VertexBuffer");

		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * static_cast<int>(myMeshData.indices.size());
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = &myMeshData.indices[0];
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

	BoundingBoxDrawer::~BoundingBoxDrawer()
	{
	}

	void BoundingBoxDrawer::Render(const std::shared_ptr<const Simple::Model> aModelInstance)
	{
		const SimpleUtilities::Vector3f minPoint = aModelInstance->myMesh->myBoundingBox.min;
		const SimpleUtilities::Vector3f  maxPoint = aModelInstance->myMesh->myBoundingBox.max;

		Vertex down_SouthWest;
		down_SouthWest.position = { minPoint.x, minPoint.y, minPoint.z, 1 };
		Vertex down_SouthEast;
		down_SouthEast.position = { maxPoint.x, minPoint.y, minPoint.z, 1 };
		Vertex up_SouthWest;
		up_SouthWest.position = { minPoint.x, maxPoint.y, minPoint.z, 1 };
		Vertex up_SouthEast;
		up_SouthEast.position = { maxPoint.x, maxPoint.y, minPoint.z, 1 };
		Vertex down_NorthWest;
		down_NorthWest.position = { minPoint.x, minPoint.y, maxPoint.z, 1 };
		Vertex down_NorthEast;
		down_NorthEast.position = { maxPoint.x, minPoint.y, maxPoint.z, 1 };
		Vertex up_NorthWest;
		up_NorthWest.position = { minPoint.x, maxPoint.y, maxPoint.z, 1 };
		Vertex up_NorthEast;
		up_NorthEast.position = { maxPoint.x, maxPoint.y, maxPoint.z, 1 };

		myMeshData.vertices[0] = down_SouthWest;
		myMeshData.vertices[1] = down_SouthEast;
		myMeshData.vertices[2] = down_SouthEast;
		myMeshData.vertices[3] = up_SouthEast;
		myMeshData.vertices[4] = up_SouthEast;
		myMeshData.vertices[5] = up_SouthWest;
		myMeshData.vertices[6] = up_SouthWest;
		myMeshData.vertices[7] = down_SouthWest;

		myMeshData.vertices[8] = down_NorthWest;
		myMeshData.vertices[9] = down_NorthEast;
		myMeshData.vertices[10] = down_NorthEast;
		myMeshData.vertices[11] = up_NorthEast;
		myMeshData.vertices[12] = up_NorthEast;
		myMeshData.vertices[13] = up_NorthWest;
		myMeshData.vertices[14] = up_NorthWest;
		myMeshData.vertices[15] = down_NorthWest;

		myMeshData.vertices[16] = down_SouthWest;
		myMeshData.vertices[17] = down_NorthWest;
		myMeshData.vertices[18] = down_SouthEast;
		myMeshData.vertices[19] = down_NorthEast;
		myMeshData.vertices[20] = up_SouthWest;
		myMeshData.vertices[21] = up_NorthWest;
		myMeshData.vertices[22] = up_SouthEast;
		myMeshData.vertices[23] = up_NorthEast;

		for (auto& vertice : myMeshData.vertices)
		{
			vertice.color = aModelInstance->GetBoundingBoxLineColor();
		}

		auto context = SimpleGlobal::GetGraphicsEngine()->GetContext();
		context->UpdateSubresource(myVertexBuffer.Get(), 0, nullptr, myMeshData.vertices.data(), 0, 0);

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModelInstance->GetMatrix();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		myShader->UseThisShader(context.Get());

		context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		context->DrawIndexed(static_cast<UINT>(myMeshData.vertices.size()), 0, 0);
	}
}