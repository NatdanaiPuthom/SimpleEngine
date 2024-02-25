#pragma once
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Animation/Animation.hpp"
#include <unordered_map>
#include <string>
#include <memory>

namespace TGA
{
	namespace FBX
	{
		struct Mesh;
	}
}

namespace Simple
{
	class ModelFactory final
	{
	public:
		ModelFactory();
		~ModelFactory();

		void Init();

		Model LoadFBX(const char* aFileName);
		Animation LoadAnimationFBX(const char* aFileName);
	public:
		std::unique_ptr<Simple::Model> CreateTerrainModel();
		std::unique_ptr<Simple::Model> CreateSkyBoxModel();
		std::unique_ptr<Simple::Model> CreateDirectionalLightModel();
		std::unique_ptr<Simple::Model> CreatePlaneModel();
		std::unique_ptr<Simple::Model> CreateCubeModel();
		std::unique_ptr<Simple::Model> CreatePyramidModel();
		std::unique_ptr<Simple::Model> CreateSphereModel();
		std::unique_ptr<Simple::Model> CreatePlaneReflection();
	private:
		void AddMesh(const char* aName, std::unique_ptr<const Simple::Mesh> aMesh);
		const Simple::Mesh* GetMesh(const char* aMeshName);
		void LoadMeshData(Simple::MeshData& aMeshData, const TGA::FBX::Mesh& aTGAMesh) const;
		void LoadSkeletonData(Simple::Skeleton& aSkeletonData, const TGA::FBX::Mesh& aTGAMesh) const;
	private:
		std::unordered_map<std::string, const std::unique_ptr<const Simple::Mesh>> myMeshes;
	};
}