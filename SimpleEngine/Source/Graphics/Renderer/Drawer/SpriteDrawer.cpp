#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Renderer/Drawer/SpriteDrawer.hpp"
#include "Graphics/Model/Factory/ShapeCreator3000.hpp"
#include "Graphics/ConstantBuffer/ConstantBuffer.hpp"
#include "Engine/Global.hpp"
#include <cassert>

namespace Drawer
{
	using namespace Simple;

	SpriteDrawer::SpriteDrawer()
		: myObjectBuffer(std::make_unique<ConstantBuffer>())
	{
		myShader = Global::GetGraphicsEngine()->GetShader("DefaultPS.cso", "Sprite2DVS.cso");

		myMeshData = std::make_unique<const MeshData>(ShapeCreator3000::Create2DPlane());

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

	SpriteDrawer::~SpriteDrawer()
	{
	}

	void SpriteDrawer::Render(const Sprite2D& aSprite)
	{
		const auto context = Global::GetGraphicsEngine()->GetContext();

		const Math::Vector2ui resolution = Global::GetResolution();

		Math::Vector3f scale;
		scale.x = aSprite.size.x * 2.0f / static_cast<float>(resolution.x);
		scale.y = aSprite.size.y * 2.0f / static_cast<float>(resolution.y);
		scale.z = 1.0f;

		Math::Vector3f position;
		position.x = 2.0f * aSprite.position.x - 1.0f;
		position.y = 2.0f * aSprite.position.y - 1.0f;

		ObjectBufferData objectBuffer = {};
		objectBuffer.modelWorldMatrix = Math::Matrix4x4f::Identity();
		objectBuffer.modelWorldMatrix.SetScale(scale);
		objectBuffer.modelWorldMatrix.SetPosition(position);

		myObjectBuffer->Bind(myObjectBuffer->GetSlot());
		myObjectBuffer->Update(sizeof(ObjectBufferData), &objectBuffer);

		myShader->BindThisShader(context.Get());
		aSprite.texture->Bind(context, aSprite.texture->GetSlot());

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->DrawIndexed(static_cast<UINT>(myMeshData->indices.size()), 0, 0);
	}
}