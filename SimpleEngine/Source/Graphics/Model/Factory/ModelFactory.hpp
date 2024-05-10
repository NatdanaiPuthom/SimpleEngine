#pragma once
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Model/Skeleton.hpp"
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

namespace Graphics
{
	enum class ePrimitiveShape
	{
		Cube,
		Pyramid,
		Plane,
		Sphere
	};
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
		Animation LoadAnimationFBX(const char* aFileName);

		const Mesh* GetPrimitiveShape(const ePrimitiveShape aShape);

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