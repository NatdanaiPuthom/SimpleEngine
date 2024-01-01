#pragma once

class ConstantBuffer;
class Shader;

struct ID3D11Buffer;
struct MeshData;

namespace Drawer
{
	struct Line final
	{
		SimpleUtilities::Vector4f color = { 0.0f, 1.0f, 0.0f, 1.0f };
		SimpleUtilities::Vector3f startPosition;
		SimpleUtilities::Vector3f endPosition;
	};
}

class LineDrawer final
{
public:
	LineDrawer();
	~LineDrawer();

	void Render(const Drawer::Line& aLine);
private:
	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11Buffer> myIndexBuffer;

	std::unique_ptr<MeshData> myMeshData;
	std::unique_ptr<ConstantBuffer> myObjectBuffer;
	std::shared_ptr<const Shader> myShader;
};