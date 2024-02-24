#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Renderer/Drawer/SphereDrawer.hpp"
#include "Graphics/Model/Factory/ShapeCreator3000.hpp"
#include "Graphics/ConstantBuffer/ConstantBuffer.hpp"
#include "Engine/Global.hpp"

namespace Drawer
{
	using namespace Simple;

	SphereDrawer::SphereDrawer()
		: myObjectBuffer(std::make_unique<ConstantBuffer>())
		, myMeshData(std::make_unique<MeshData>())
	{
		myShader = Global::GetGraphicsEngine()->GetShader("LinePS.cso", "DefaultVS.cso");

		MeshData sphereData = ShapeCreator3000::CreateSphere(1.0f, 10, 10);

		myMeshData->vertices = sphereData.vertices;
		myMeshData->indices = sphereData.indices;

		for (auto& vertex : myMeshData->vertices)
		{
			vertex.color = { 1,1,0,1 };
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

		HRESULT result = Global::GetGraphicsEngine()->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);

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

		result = Global::GetGraphicsEngine()->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);

		if (FAILED(result))
			assert(false && "failed to create IndexBuffer");

		ObjectBufferData objectBuffer;

		if (!myObjectBuffer->Init(sizeof(ObjectBufferData), &objectBuffer))
			assert(false && "failed to create ObjectBuffer");

		myObjectBuffer->SetSlot(1);
	}

	SphereDrawer::~SphereDrawer()
	{
	}

	void SphereDrawer::Render(const Drawer::Sphere& aSphere)
	{
		for (auto& vertex : myMeshData->vertices)
		{
			vertex.color = aSphere.color;
		}

		auto context = Global::GetGraphicsEngine()->GetContext();
		context->UpdateSubresource(myVertexBuffer.Get(), 0, nullptr, myMeshData->vertices.data(), 0, 0);

		Math::Matrix4x4f matrix = Math::Matrix4x4f::Identity();
		matrix.SetScale({ aSphere.radius, aSphere.radius,aSphere.radius });
		matrix.SetPosition(aSphere.position);

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = matrix;

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		myShader->UseThisShader(context.Get());

		context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		context->DrawIndexed(static_cast<UINT>(myMeshData->indices.size()), 0, 0);
	}
}