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
		Sphere,
		SkyBox
	};
}

namespace Graphics
{
	class ModelFactory final
	{
		using RelativePath = std::string;
	public:
		ModelFactory();
		~ModelFactory();

		void Init();

		const Mesh* LoadMesh(const std::string& aFileName);
		const Skeleton* LoadSkeleton(const std::string& aRelativePath);
		const Animation* LoadAnimationFBX(const std::string& aRelativePath);

		const Mesh* GetPrimitiveShape(const ePrimitiveShape aShape);

	private:
		void AddMesh(const std::string& aName, std::unique_ptr<const Mesh> aMesh);
		void AddSkeleton(const std::string& aName, std::unique_ptr<const Skeleton> aSkeleton);

		const Mesh* GetMesh(const char* aMeshName) const;
		const Skeleton* GetSkeleton(const char* aName) const;
		const Animation* GetAnimation(const char* aRelativePath) const;

		void LoadMeshData(MeshData& aMeshData, const TGA::FBX::Mesh& aTGAMesh) const;
		void LoadSkeletonData(Skeleton& aSkeletonData, const TGA::FBX::Mesh& aTGAMesh) const;
		void LoadAndCacheMesh(const std::string& aFileName);
		void LoadAndCacheMesh(const std::string& aFileName, TGA::FBX::Mesh& aTGAMesh);
		void LoadAndCacheSkeleton(const std::string& aRelativePath, TGA::FBX::Mesh& aTGAMesh);
		void LoadAndCacheAnimation(const std::string& aRelativePath);
	private:
		std::mutex myFBXLoaderMutex;
		std::unordered_map<std::string, const std::unique_ptr<const Mesh>> myMeshes;
		std::unordered_map<RelativePath, const std::unique_ptr<const Skeleton>> mySkeletons;
		std::unordered_map<RelativePath, const std::unique_ptr<const Animation>> myAnimations;
		std::atomic<bool> myIsCachingInProgress;
		char myPadding[56] = "Never Give Up On Your Dreams! You Gotta Believe!!!!!!!\0";
	};
}