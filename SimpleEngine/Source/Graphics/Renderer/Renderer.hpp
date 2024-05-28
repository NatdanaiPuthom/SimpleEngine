#pragma once
#include "Engine/Math/Matrix4x4.hpp"
#include "Graphics/Renderer/Drawer/BoundingBoxDrawer.hpp"
#include "Graphics/Renderer/Drawer/LineDrawer.hpp"
#include "Graphics/Renderer/Drawer/SphereDrawer.hpp"
#include "Graphics/Renderer/Drawer/SpriteDrawer.hpp"
#include <memory>
#include <vector>

namespace ECS
{
	struct MeshComponent;
	struct TransformComponent;
	struct AnimatedComponent;
}

namespace Graphics
{
	class ConstantBuffer;
	class Mesh;
	class Shader;
	class Texture;
}

namespace Drawer
{
	class Renderer final
	{
	public:
		Renderer();
		~Renderer();

		void Init();

		void RenderAnimatedModel(ECS::TransformComponent* aTransformComponent, ECS::MeshComponent* aMeshComponent, ECS::AnimatedComponent* aSkeletonComponent);
		void RenderStaticModel(const ECS::TransformComponent* aTransformComponent, const ECS::MeshComponent* aMeshComponent) const;

		void RenderUnlit(const Math::Matrix4x4f& aTransformMatrix, const Graphics::Mesh* aMesh, const Graphics::Shader* aShader, const Graphics::Texture* aTextures) const; //To-DO(v9.37.1): For testing SkyBox will need to refactor how rendering work later

		void RenderLine(const Drawer::Line& aLine) const;
		void RenderLine(const std::vector<Drawer::Line>& aLines) const;
		void RenderSphere(const Drawer::Sphere& aSphere) const;
		void RenderSprite2D(const Drawer::Sprite2D& aSprite);

	private:
		const bool CreateObjectBuffer();
		const bool CreateBoneBuffer();
		void LoadSettingsFromJson();
		void RenderModel(const Math::Matrix4x4f& aTransformMatrix, const Graphics::Mesh* aMesh, ID3D11DeviceContext* aContext) const;
	private:
		std::unique_ptr<Drawer::LineDrawer> myLineDrawer;
		std::unique_ptr<Drawer::SphereDrawer> mySphereDrawer;
		std::unique_ptr<Drawer::SpriteDrawer> mySpriteDrawer;
		std::unique_ptr<Drawer::BoundingBoxDrawer> myBoundingBoxDrawer;

		std::unique_ptr<Graphics::ConstantBuffer> myTransformBuffer;
		std::unique_ptr<Graphics::ConstantBuffer> myJointBuffer;
	};
}