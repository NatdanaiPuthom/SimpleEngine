#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Renderer/Drawer/BoundingBoxDrawer.hpp"
#include "Graphics/ConstantBuffer/ConstantBuffer.hpp"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"
#include "Engine/Global.hpp"

namespace Drawer
{
	using namespace Graphics;

	BoundingBoxDrawer::BoundingBoxDrawer()
	{
	}

	BoundingBoxDrawer::~BoundingBoxDrawer()
	{
	}

	void BoundingBoxDrawer::Init()
	{
		myObjectBuffer = std::make_unique<ConstantBuffer>();

		InitMeshData2D();
		InitMeshData3D();
		InitObjectBuffer();
	}

	void BoundingBoxDrawer::InitMeshData2D()
	{
		myShader2D = Global::GetGraphicsEngine()->GetShader("LinePS.cso", "Line2DVS.cso");
	}

	void BoundingBoxDrawer::InitMeshData3D()
	{
		myShader3D = Global::GetGraphicsEngine()->GetShader("LinePS.cso", "DefaultVS.cso");

		myMeshData3D.vertices.reserve(24);
		myMeshData3D.vertices.resize(24, Vertex{});

		for (unsigned int i = 0; i < myMeshData3D.vertices.size(); ++i)
		{
			myMeshData3D.indices.push_back(i);
		}

		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * static_cast<int>(myMeshData3D.vertices.size());
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexData = {};
		vertexData.pSysMem = &myMeshData3D.vertices[0];
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		HRESULT result = Global::GetGraphicsEngine()->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);

		if (FAILED(result))
			assert(false && "failed to create VertexBuffer");

		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * static_cast<int>(myMeshData3D.indices.size());
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = &myMeshData3D.indices[0];
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		result = Global::GetGraphicsEngine()->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);

		if (FAILED(result))
			assert(false && "failed to create IndexBuffer");
	}

	void BoundingBoxDrawer::InitObjectBuffer()
	{
		ObjectBufferData objectBuffer;

		if (!myObjectBuffer->Init(sizeof(ObjectBufferData), &objectBuffer))
			assert(false && "failed to create ObjectBuffer");

		myObjectBuffer->SetSlot(1);
	}
}