#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Renderer/Drawer/BoundingBoxDrawer.hpp"
#include "Graphics/ConstantBuffer/ConstantBuffer.hpp"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"
#include "Engine/Global.hpp"

namespace Drawer
{
	using namespace Simple;

	BoundingBoxDrawer::BoundingBoxDrawer()
	{
	}

	BoundingBoxDrawer::~BoundingBoxDrawer()
	{
	}

	void BoundingBoxDrawer::Init()
	{
		myObjectBuffer = std::make_unique<Simple::ConstantBuffer>();

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

	void BoundingBoxDrawer::Render(const std::shared_ptr<const Simple::Model> aModelInstance)
	{
		const Math::Vector3f minPoint = aModelInstance->myMesh->myBoundingBox.min;
		const Math::Vector3f  maxPoint = aModelInstance->myMesh->myBoundingBox.max;

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
			vertice.color = aModelInstance->GetBoundingBoxLineColor();
		}

		auto context = Global::GetGraphicsEngine()->GetContext();
		context->UpdateSubresource(myVertexBuffer.Get(), 0, nullptr, myMeshData3D.vertices.data(), 0, 0);

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModelInstance->GetMatrix();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		myShader3D->BindThisShader(context.Get());

		context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		context->DrawIndexed(static_cast<UINT>(myMeshData3D.vertices.size()), 0, 0);
	}

	void BoundingBoxDrawer::Render(const std::shared_ptr<const Simple::AnimatedModel> aModelInstance)
	{
		const Math::Vector3f minPoint = aModelInstance->myMesh->myBoundingBox.min;
		const Math::Vector3f  maxPoint = aModelInstance->myMesh->myBoundingBox.max;

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
			vertice.color = aModelInstance->GetBoundingBoxLineColor();
		}

		auto context = Global::GetGraphicsEngine()->GetContext();
		context->UpdateSubresource(myVertexBuffer.Get(), 0, nullptr, myMeshData3D.vertices.data(), 0, 0);

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModelInstance->GetMatrix();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		myShader3D->BindThisShader(context.Get());

		context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		context->DrawIndexed(static_cast<UINT>(myMeshData3D.vertices.size()), 0, 0);
	}

	void BoundingBoxDrawer::Render(const Simple::Model& aModelInstance)
	{
		const Math::Vector3f minPoint = aModelInstance.myMesh->myBoundingBox.min;
		const Math::Vector3f  maxPoint = aModelInstance.myMesh->myBoundingBox.max;

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
			vertice.color = aModelInstance.GetBoundingBoxLineColor();
		}

		auto context = Global::GetGraphicsEngine()->GetContext();
		context->UpdateSubresource(myVertexBuffer.Get(), 0, nullptr, myMeshData3D.vertices.data(), 0, 0);

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModelInstance.GetMatrix();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		myShader3D->BindThisShader(context.Get());

		context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		context->DrawIndexed(static_cast<UINT>(myMeshData3D.vertices.size()), 0, 0);
	}

	void BoundingBoxDrawer::Render(const Simple::AnimatedModel& aModelInstance)
	{
		const Math::Vector3f minPoint = aModelInstance.myMesh->myBoundingBox.min;
		const Math::Vector3f  maxPoint = aModelInstance.myMesh->myBoundingBox.max;

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
			vertice.color = aModelInstance.GetBoundingBoxLineColor();
		}

		auto context = Global::GetGraphicsEngine()->GetContext();
		context->UpdateSubresource(myVertexBuffer.Get(), 0, nullptr, myMeshData3D.vertices.data(), 0, 0);

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = aModelInstance.GetMatrix();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		myShader3D->BindThisShader(context.Get());

		context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		context->DrawIndexed(static_cast<UINT>(myMeshData3D.vertices.size()), 0, 0);
	}

	void BoundingBoxDrawer::Render(const Simple::BoundingBox2D& aBoundingBox2D)
	{
		aBoundingBox2D;
	}
}