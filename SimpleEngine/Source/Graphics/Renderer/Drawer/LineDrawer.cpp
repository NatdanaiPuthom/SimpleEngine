#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Renderer/Drawer/LineDrawer.hpp"
#include "Graphics/ConstantBuffer/ConstantBuffer.hpp"
#include "Engine/Global.hpp"

namespace Drawer
{
	using namespace Simple;

	LineDrawer::LineDrawer()
		: myObjectBuffer(std::make_unique<ConstantBuffer>())
		, myData(std::make_unique<Data>())
		, myInstanceData(std::make_unique<Data>())
		, myInstanceSizeLimit(32768)
	{
		myShader = Global::GetGraphicsEngine()->GetShader("LinePS.cso", "DefaultVS.cso");

		CreateBuffers();
		CreateInstanceBuffer();
		CreateObjectBuffer();
	}

	LineDrawer::~LineDrawer()
	{
	}

	void LineDrawer::Render(const Drawer::Line aLine)
	{
		myData->meshData.vertices[0].position.x = aLine.startPosition.x;
		myData->meshData.vertices[0].position.y = aLine.startPosition.y;
		myData->meshData.vertices[0].position.z = aLine.startPosition.z;
		myData->meshData.vertices[0].position.w = 1.0f;

		myData->meshData.vertices[1].position.x = aLine.endPosition.x;
		myData->meshData.vertices[1].position.y = aLine.endPosition.y;
		myData->meshData.vertices[1].position.z = aLine.endPosition.z;
		myData->meshData.vertices[1].position.w = 1.0f;

		for (auto& vertex : myData->meshData.vertices)
		{
			vertex.color = aLine.color;
		}

		auto context = Global::GetGraphicsEngine()->GetContext();
		context->UpdateSubresource(myData->vertexBuffer.Get(), 0, nullptr, myData->meshData.vertices.data(), 0, 0);

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = Math::Matrix4x4f::Identity();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		myShader->BindThisShader(context.Get());

		context->IASetVertexBuffers(0, 1, myData->vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(myData->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		context->DrawIndexed(static_cast<UINT>(myData->meshData.indices.size()), 0, 0);
	}

	void LineDrawer::RenderInstance(const std::vector<Drawer::Line>& aLines)
	{
		const size_t vertexCount = aLines.size() * 2;

		myInstanceData->meshData.vertices.resize(vertexCount, Vertex{});
		myInstanceData->meshData.indices.resize(vertexCount, 0);

		for (size_t i = 0; i < aLines.size(); ++i)
		{
			const size_t startVertex = i * 2 + 0;
			const size_t endVertex = i * 2 + 1;

			myInstanceData->meshData.vertices[startVertex].position = Math::AsVector4(aLines[i].startPosition);
			myInstanceData->meshData.vertices[startVertex].color = aLines[i].color;

			myInstanceData->meshData.vertices[endVertex].position = Math::AsVector4(aLines[i].endPosition);
			myInstanceData->meshData.vertices[endVertex].color = aLines[i].color;
		}

		for (unsigned int i = 0; i < vertexCount; ++i)
		{
			myInstanceData->meshData.indices[i] = i;
		}

		auto context = Global::GetGraphicsEngine()->GetContext();
		context->UpdateSubresource(myInstanceData->vertexBuffer.Get(), 0, nullptr, myInstanceData->meshData.vertices.data(), 0, 0);
		context->UpdateSubresource(myInstanceData->indexBuffer.Get(), 0, nullptr, myInstanceData->meshData.indices.data(), 0, 0);

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = Math::Matrix4x4f::Identity();

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		myShader->BindThisShader(context.Get());

		context->IASetVertexBuffers(0, 1, myInstanceData->vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(myInstanceData->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		context->DrawIndexed(static_cast<UINT>(myInstanceData->meshData.indices.size()), 0, 0);
	}

	size_t LineDrawer::GetInstanceSizeLimit() const
	{
		return myInstanceSizeLimit;
	}

	void LineDrawer::CreateBuffers()
	{
		myData->meshData.vertices.reserve(2);
		myData->meshData.vertices.resize(2, Vertex{});

		myData->meshData.vertices[0].position = { 0.0f,0.0f,0.0f,1.0f };

		for (unsigned int i = 0; i < myData->meshData.vertices.size(); ++i)
		{
			myData->meshData.indices.push_back(i);
		}

		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * static_cast<int>(myData->meshData.vertices.size());
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexData = {};
		vertexData.pSysMem = &myData->meshData.vertices[0];
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		HRESULT result = Global::GetGraphicsEngine()->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &myData->vertexBuffer);

		if (FAILED(result))
			assert(false && "failed to create VertexBuffer");

		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * static_cast<int>(myData->meshData.indices.size());
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = &myData->meshData.indices[0];
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		result = Global::GetGraphicsEngine()->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &myData->indexBuffer);

		if (FAILED(result))
			assert(false && "failed to create IndexBuffer");
	}

	void LineDrawer::CreateInstanceBuffer()
	{
		myInstanceData->meshData.vertices.reserve(myInstanceSizeLimit);
		myInstanceData->meshData.vertices.resize(myInstanceSizeLimit, Vertex{});

		myInstanceData->meshData.indices.reserve(myInstanceSizeLimit);
		myInstanceData->meshData.indices.resize(myInstanceSizeLimit, 0);

		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * static_cast<int>(myInstanceData->meshData.vertices.size());
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexData = {};
		vertexData.pSysMem = &myInstanceData->meshData.vertices[0];
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		HRESULT result = Global::GetGraphicsEngine()->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &myInstanceData->vertexBuffer);

		if (FAILED(result))
			assert(false && "failed to create VertexBuffer");

		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * static_cast<int>(myInstanceData->meshData.indices.size());
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = &myInstanceData->meshData.indices[0];
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		result = Global::GetGraphicsEngine()->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &myInstanceData->indexBuffer);

		if (FAILED(result))
			assert(false && "failed to create IndexBuffer");
	}

	void LineDrawer::CreateObjectBuffer()
	{
		ObjectBufferData objectBuffer;

		if (!myObjectBuffer->Init(sizeof(ObjectBufferData), &objectBuffer))
			assert(false && "failed to create ObjectBuffer");

		myObjectBuffer->SetSlot(1);
	}
}