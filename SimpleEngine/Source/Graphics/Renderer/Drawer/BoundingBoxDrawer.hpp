#pragma once
#include "Graphics/Model/Mesh.hpp"

struct ID3D11Buffer;

namespace Simple
{
	class Shader;
	class ConstantBuffer;
	class Model;
	class AnimatedModel;
}

namespace Drawer
{
	class BoundingBoxDrawer final
	{
	public:
		BoundingBoxDrawer();
		~BoundingBoxDrawer();

		void Init();

		void Render(const std::shared_ptr<const Simple::Model> aModelInstance);
		void Render(const std::shared_ptr<const Simple::AnimatedModel> aModelInstance);
		void Render(const Simple::Model& aModelInstance);
		void Render(const Simple::AnimatedModel& aModelInstance);

		void Render(const Simple::BoundingBox2D& aBoundingBox2D);
	private:
		void InitMeshData2D();
		void InitMeshData3D();
		void InitObjectBuffer();
	private:
		Simple::MeshData myMeshData3D;
		Simple::MeshData myMeshData2D;

		ComPtr<ID3D11Buffer> myVertexBuffer;
		ComPtr<ID3D11Buffer> myIndexBuffer;

		std::unique_ptr<Simple::ConstantBuffer> myObjectBuffer;
		std::shared_ptr<const Simple::Shader> myShader3D;
		std::shared_ptr<const Simple::Shader> myShader2D;
	};
}