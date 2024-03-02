#pragma once
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Model/AnimatedModel.hpp"
#include "Graphics/Animation/Animation.hpp"
#include <unordered_map>
#include <string>
#include <memory>
#include <mutex>
#include <atomic>

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

		Model LoadStaticModelFBX(const char* aFileName);
		AnimatedModel LoadAnimatedModelFBX(const char* aFileName);
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
		void AddSkeleton(const char* aName, std::unique_ptr<const Simple::Skeleton> aSkeleton);
		const Simple::Mesh* GetMesh(const char* aMeshName) const;
		const Simple::Skeleton* GetSkeleton(const char* aName) const;
		void LoadMeshData(Simple::MeshData& aMeshData, const TGA::FBX::Mesh& aTGAMesh) const;
		void LoadSkeletonData(Simple::Skeleton& aSkeletonData, const TGA::FBX::Mesh& aTGAMesh) const;
		void LoadAndCacheMesh(const char* aFileName);
		void LoadAndCacheMesh(const char* aFileName, TGA::FBX::Mesh& aTGAMesh);
		void LoadAndCacheSkeleton(const char* aFileName, TGA::FBX::Mesh& aTGAMesh);
	private:
		std::mutex myFBXLoaderMutex;
		std::atomic<bool> myIsCachingInProgress;
		std::unordered_map<std::string, std::atomic<bool>> myFBXLoaderQueue;
		std::unordered_map<std::string, const std::unique_ptr<const Simple::Mesh>> myMeshes;
		std::unordered_map<std::string, const std::unique_ptr<const Simple::Skeleton>> mySkeletons;
	};
}