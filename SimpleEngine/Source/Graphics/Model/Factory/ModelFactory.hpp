#pragma once
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Model/AnimatedModel.hpp"
#include "Graphics/Animation/AnimationPlayer.hpp"
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

namespace Graphics
{
	class ModelFactory final
	{
	public:
		ModelFactory();
		~ModelFactory();

		void Init();

		//FilePath starts at Bin/Assets/Models/
		const Mesh* LoadMesh(const std::string& aFileName);

		//FilePath starts at Bin/Assets/Models/
		const Skeleton* LoadSkeleton(const std::string& aFileName);

		//FilePath starts at Bin/Assets/Models/
		Model LoadStaticModelFBX(const char* aFileName);

		//FilePath starts at Bin/Assets/Models/
		AnimatedModel LoadAnimatedModelFBX(const char* aFileName);

		//FilePath starts at Bin/Assets/Models/
		Animation LoadAnimationFBX(const char* aFileName);
	public:
		std::unique_ptr<Model> CreateTerrainModel();
		std::unique_ptr<Model> CreateSkyBoxModel();
		std::unique_ptr<Model> CreateDirectionalLightModel();
		std::unique_ptr<Model> CreatePlaneModel();
		std::unique_ptr<Model> CreateCubeModel();
		std::unique_ptr<Model> CreatePyramidModel();
		std::unique_ptr<Model> CreateSphereModel();
		std::unique_ptr<Model> CreatePlaneReflection();
	private:
		void AddMesh(const std::string& aName, std::unique_ptr<const Mesh> aMesh);
		void AddSkeleton(const std::string& aName, std::unique_ptr<const Skeleton> aSkeleton);
		const Mesh* GetMesh(const char* aMeshName) const;
		const Skeleton* GetSkeleton(const char* aName) const;
		void LoadMeshData(MeshData& aMeshData, const TGA::FBX::Mesh& aTGAMesh) const;
		void LoadSkeletonData(Skeleton& aSkeletonData, const TGA::FBX::Mesh& aTGAMesh) const;
		void LoadAndCacheMesh(const std::string& aFileName);
		void LoadAndCacheMesh(const std::string& aFileName, TGA::FBX::Mesh& aTGAMesh);
		void LoadAndCacheSkeleton(const std::string& aFileName, TGA::FBX::Mesh& aTGAMesh);
	private:
		std::mutex myFBXLoaderMutex;
		std::atomic<bool> myIsCachingInProgress;
		std::unordered_map<std::string, std::atomic<bool>> myFBXLoaderQueue;
		std::unordered_map<std::string, const std::unique_ptr<const Mesh>> myMeshes;
		std::unordered_map<std::string, const std::unique_ptr<const Skeleton>> mySkeletons;
	};
}