#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Renderer/Drawer/BoundingBoxDrawer.hpp"
#include "Graphics/ConstantBuffer/ConstantBuffer.hpp"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"
#include "Engine/SimpleUtilities/Bounds.hpp"
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
		myTransformBuffer = std::make_unique<ConstantBuffer>();

		InitMeshData2D();
		InitMeshData3D();
		InitObjectBuffer();
	}

	void BoundingBoxDrawer::Render(const Simple::BoundingBox3D& aBoundingBox3D, const Math::Matrix4x4f& aModelToWorldMatrix, const Simple::Color& aColor)
	{
		const Math::Vector3f minPoint = aBoundingBox3D.min;
		const Math::Vector3f  maxPoint = aBoundingBox3D.max;

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

		myMeshData3D.vertices[0] = down_SouthWest;
		myMeshData3D.vertices[1] = down_SouthEast;
		myMeshData3D.vertices[2] = down_SouthEast;
		myMeshData3D.vertices[3] = up_SouthEast;
		myMeshData3D.vertices[4] = up_SouthEast;
		myMeshData3D.vertices[5] = up_SouthWest;
		myMeshData3D.vertices[6] = up_SouthWest;
		myMeshData3D.vertices[7] = down_SouthWest;

		myMeshData3D.vertices[8] = down_NorthWest;
		myMeshData3D.vertices[9] = down_NorthEast;
		myMeshData3D.vertices[10] = down_NorthEast;
		myMeshData3D.vertices[11] = up_NorthEast;
		myMeshData3D.vertices[12] = up_NorthEast;
		myMeshData3D.vertices[13] = up_NorthWest;
		myMeshData3D.vertices[14] = up_NorthWest;
		myMeshData3D.vertices[15] = down_NorthWest;

		myMeshData3D.vertices[16] = down_SouthWest;
		myMeshData3D.vertices[17] = down_NorthWest;
		myMeshData3D.vertices[18] = down_SouthEast;
		myMeshData3D.vertices[19] = down_NorthEast;
		myMeshData3D.vertices[20] = up_SouthWest;
		myMeshData3D.vertices[21] = up_NorthWest;
		myMeshData3D.vertices[22] = up_SouthEast;
		myMeshData3D.vertices[23] = up_NorthEast;

		for (auto& vertice : myMeshData3D.vertices)
		{
			vertice.color = Math::Vector4f(aColor.r, aColor.g, aColor.b, aColor.a);
		}

		auto context = Global::GetGraphicsEngine()->GetContext();

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		context->Map(myVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, myMeshData3D.vertices.data(), sizeof(Vertex) * myMeshData3D.vertices.size());
		context->Unmap(myVertexBuffer.Get(), 0);

		//context->UpdateSubresource(myVertexBuffer.Get(), 0, nullptr, myMeshData3D.vertices.data(), 0, 0);

		TransformBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModelToWorldMatrix;

		myTransformBuffer->Bind(myTransformBuffer->GetSlot());
		myTransformBuffer->Update(sizeof(TransformBufferData), &objectBuffer);
		myShader3D->BindThisShader(context.Get());

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		context->DrawIndexed(static_cast<UINT>(myMeshData3D.vertices.size()), 0, 0);
	}

	void BoundingBoxDrawer::InitMeshData2D()
	{
		myShader2D = Global::GetGraphicsEngine()->GetShaderManager()->GetShader("Shaders\\LinePS.cso", "Shaders\\Line2DVS.cso");
	}

	void BoundingBoxDrawer::InitMeshData3D()
	{
		Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		myShader3D = graphicsEngine->GetShaderManager()->GetShader("Shaders\\LinePS.cso", "Shaders\\DefaultVS.cso");

		myMeshData3D.vertices.reserve(24);
		myMeshData3D.vertices.resize(24, Vertex{});

		for (unsigned int i = 0; i < myMeshData3D.vertices.size(); ++i)
		{
			myMeshData3D.indices.push_back(i);
		}

		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * static_cast<int>(myMeshData3D.vertices.size());
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexData = {};
		vertexData.pSysMem = &myMeshData3D.vertices[0];
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		HRESULT result = graphicsEngine->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);

		if (FAILED(result))
			assert(false && "failed to create VertexBuffer");

		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * static_cast<int>(myMeshData3D.indices.size());
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = &myMeshData3D.indices[0];
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		result = graphicsEngine->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);

		if (FAILED(result))
			assert(false && "failed to create IndexBuffer");
	}

	void BoundingBoxDrawer::InitObjectBuffer()
	{
		TransformBufferData objectBuffer;

		if (!myTransformBuffer->Init(sizeof(TransformBufferData), &objectBuffer))
			assert(false && "failed to create ObjectBuffer");

		myTransformBuffer->SetSlot(Graphics::Global_Constant_Buffer_Slot_Transform);
	}
}