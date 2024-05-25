#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Model/Factory/ModelFactory.hpp"
#include "Graphics/Model/Factory/ShapeCreator3000.hpp"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include <External/TheGameAssembly/FBXImporter/source/Importer.h>

#undef LoadMesh

namespace Graphics
{
	ModelFactory::ModelFactory()
		: myIsCachingInProgress(false)
	{
	}

	ModelFactory::~ModelFactory()
	{
		TGA::FBX::Importer::UninitImporter();
	}

	void ModelFactory::Init()
	{
		TGA::FBX::Importer::InitImporter();

		MeshData cubeData = ShapeCreator3000::CreateCube();
		MeshData pyramidData = ShapeCreator3000::CreatePyramid();
		MeshData planeData = ShapeCreator3000::CreatePlane();
		MeshData skyboxData = ShapeCreator3000::CreateSkyBox(Math::Vector3f(100, 100, 100));
		MeshData terrainData = ShapeCreator3000::CreateTerrain();
		MeshData sphereData = ShapeCreator3000::CreateSphere();

		std::unique_ptr<Mesh> cubeMesh = std::make_unique<Mesh>();
		std::unique_ptr<Mesh> pyramidMesh = std::make_unique<Mesh>();
		std::unique_ptr<Mesh> planeMesh = std::make_unique<Mesh>();
		std::unique_ptr<Mesh> skyboxMesh = std::make_unique<Mesh>();
		std::unique_ptr<Mesh> directionalLightMesh = std::make_unique<Mesh>();
		std::unique_ptr<Mesh> terrainMesh = std::make_unique<Mesh>();
		std::unique_ptr<Mesh> sphereMesh = std::make_unique<Mesh>();

		if (!cubeMesh->Init(cubeData, "Primitive Cube"))
			assert(false && "Failed to create Cube");

		if (!pyramidMesh->Init(pyramidData, "Primitive Pyramid"))
			assert(false && "Failed to create Pyramid");

		if (!planeMesh->Init(planeData, "Primitive Plane"))
			assert(false && "Failed to create Plane");

		if (!skyboxMesh->Init(skyboxData, "Primitive SkyBox"))
			assert(false && "Failed to create SkyBox");

		if (!terrainMesh->Init(terrainData, "Primitive Terrain"))
			assert(false && "Failed to create Terrain");

		if (!sphereMesh->Init(sphereData, "Primitive Sphere"))
			assert(false && "Failed to create Sphere");

		AddMesh("Cube", std::move(cubeMesh));
		AddMesh("Pyramid", std::move(pyramidMesh));
		AddMesh("Plane", std::move(planeMesh));
		AddMesh("Skybox", std::move(skyboxMesh));
		AddMesh("DirectionalLight", std::move(directionalLightMesh));
		AddMesh("Terrain", std::move(terrainMesh));
		AddMesh("Sphere", std::move(sphereMesh));
	}

	const Mesh* ModelFactory::LoadMesh(const std::string& aFileName)
	{
		const std::string filePath = aFileName;
		const Mesh* mesh = GetMesh(filePath.c_str());

		if (mesh == nullptr)
		{
			LoadAndCacheMesh(filePath);
			mesh = GetMesh(filePath.c_str());

			if (mesh == nullptr)
			{
				assert(false && "Failed to Load and cache Mesh");
				return nullptr;
			}
		}

		return mesh;
	}

	const Skeleton* ModelFactory::LoadSkeleton(const std::string& aFileName)
	{
		const Skeleton* skeleton = GetSkeleton(aFileName.c_str());

		if (skeleton == nullptr)
		{
			TGA::FBX::Mesh tgaMesh;
			TGA::FBX::FbxImportStatus status = TGA::FBX::Importer::LoadMeshA(aFileName, tgaMesh);
			assert(status && "Failed to LoadMesh from FBXImporter");

			LoadAndCacheSkeleton(aFileName, tgaMesh);
			skeleton = GetSkeleton(aFileName.c_str());

			if (skeleton == nullptr)
			{
				assert(false && "Failed to load and cache skeleton");
				return nullptr;
			}
		}

		return skeleton;
	}

	Animation ModelFactory::LoadAnimationFBX(const char* aFileName)
	{
		const std::string path = aFileName;

		TGA::FBX::Animation tgaAnimation;
		TGA::FBX::FbxImportStatus status = TGA::FBX::Importer::LoadAnimationA(path, tgaAnimation);

		Animation animation;
		animation.name = tgaAnimation.Name;
		animation.length = tgaAnimation.Length;
		animation.framesPerSecond = tgaAnimation.FramesPerSecond;
		animation.duration = static_cast<float>(tgaAnimation.Duration);
		animation.frames.resize(tgaAnimation.Frames.size());

		for (size_t i = 0; i < tgaAnimation.Frames.size(); ++i)
		{
			for (const auto& [boneName, boneTransform] : tgaAnimation.Frames[i].LocalTransforms)
			{
				Math::Matrix4x4f modelMatrix;

				modelMatrix(1, 1) = boneTransform.m11;
				modelMatrix(1, 2) = boneTransform.m12;
				modelMatrix(1, 3) = boneTransform.m13;
				modelMatrix(1, 4) = boneTransform.m14;

				modelMatrix(2, 1) = boneTransform.m21;
				modelMatrix(2, 2) = boneTransform.m22;
				modelMatrix(2, 3) = boneTransform.m23;
				modelMatrix(2, 4) = boneTransform.m24;

				modelMatrix(3, 1) = boneTransform.m31;
				modelMatrix(3, 2) = boneTransform.m32;
				modelMatrix(3, 3) = boneTransform.m33;
				modelMatrix(3, 4) = boneTransform.m34;

				modelMatrix(4, 1) = boneTransform.m41;
				modelMatrix(4, 2) = boneTransform.m42;
				modelMatrix(4, 3) = boneTransform.m43;
				modelMatrix(4, 4) = boneTransform.m44;

				modelMatrix = Math::Matrix4x4f::Transpose(modelMatrix); //Very important
				animation.frames[i].jointNameToModelSpaceMatrix.emplace(boneName, modelMatrix);
			}
		}

		return animation;
	}

	const Mesh* ModelFactory::GetPrimitiveShape(const ePrimitiveShape aShape)
	{
		switch (aShape)
		{
		case ePrimitiveShape::Cube:
			return myMeshes["Cube"].get();
		case ePrimitiveShape::Pyramid:
			return myMeshes["Pyramid"].get();
		case ePrimitiveShape::Plane:
			return myMeshes["Plane"].get();
		case ePrimitiveShape::Sphere:
			return myMeshes["Sphere"].get();
		case ePrimitiveShape::SkyBox:
			return myMeshes["Skybox"].get();
		default:
			return nullptr;
		}
	}

	void ModelFactory::AddMesh(const std::string& aName, std::unique_ptr<const Mesh> aMesh)
	{
		myIsCachingInProgress = true;
		myFBXLoaderMutex.lock();

		myMeshes.emplace(aName, std::move(aMesh));

		myFBXLoaderMutex.unlock();
		myIsCachingInProgress = false;
	}

	void ModelFactory::AddSkeleton(const std::string& aName, std::unique_ptr<const Skeleton> aSkeleton)
	{
		myIsCachingInProgress = true;
		myFBXLoaderMutex.lock();

		mySkeletons.emplace(aName, std::move(aSkeleton));

		myIsCachingInProgress = false;
		myFBXLoaderMutex.unlock();
	}

	const Mesh* ModelFactory::GetMesh(const char* aMeshName) const
	{
		while (myIsCachingInProgress == true) {}

		auto mesh = myMeshes.find(aMeshName);

		if (mesh != myMeshes.end())
			return mesh->second.get();

		return nullptr;
	}

	const Skeleton* ModelFactory::GetSkeleton(const char* aName) const
	{
		while (myIsCachingInProgress == true) {}

		auto mesh = mySkeletons.find(aName);

		if (mesh != mySkeletons.end())
			return mesh->second.get();

		return nullptr;
	}

	void ModelFactory::LoadSkeletonData(Skeleton& aSkeletonData, const TGA::FBX::Mesh& aTGAMesh) const
	{
		aSkeletonData.myName = SimpleUtilities::ConvertFilePathToPrettyName(aTGAMesh.Name);

		if (aTGAMesh.Skeleton.GetRoot())
		{
			aSkeletonData.myJoints.resize(aTGAMesh.Skeleton.Bones.size());
			aSkeletonData.myJointNames.resize(aSkeletonData.myJoints.size());

			for (size_t i = 0; i < aTGAMesh.Skeleton.Bones.size(); ++i)
			{
				Math::Matrix4x4f bindPoseInverse;

				bindPoseInverse(1, 1) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m11;
				bindPoseInverse(1, 2) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m12;
				bindPoseInverse(1, 3) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m13;
				bindPoseInverse(1, 4) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m14;

				bindPoseInverse(2, 1) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m21;
				bindPoseInverse(2, 2) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m22;
				bindPoseInverse(2, 3) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m23;
				bindPoseInverse(2, 4) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m24;

				bindPoseInverse(3, 1) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m31;
				bindPoseInverse(3, 2) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m32;
				bindPoseInverse(3, 3) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m33;
				bindPoseInverse(3, 4) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m34;

				bindPoseInverse(4, 1) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m41;
				bindPoseInverse(4, 2) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m42;
				bindPoseInverse(4, 3) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m43;
				bindPoseInverse(4, 4) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m44;

				Joint joint;

				joint.myBindPoseInverse = bindPoseInverse;
				joint.myName = aTGAMesh.Skeleton.Bones[i].Name;
				joint.myParent = aTGAMesh.Skeleton.Bones[i].ParentIdx;
				joint.myChildren = aTGAMesh.Skeleton.Bones[i].Children;

				aSkeletonData.myJointNameToIndex.insert({ joint.myName, i });
				aSkeletonData.myJointNames[i] = joint.myName;
				aSkeletonData.myJoints[i] = joint;
			}
		}
	}

	void ModelFactory::LoadAndCacheMesh(const std::string& aFileName)
	{
		const std::string absolutePath = SimpleUtilities::CheckAndReturnAsAbsolutePath(aFileName);

		TGA::FBX::Mesh tgaMesh;
		TGA::FBX::FbxImportStatus status = TGA::FBX::Importer::LoadMeshA(absolutePath, tgaMesh);
		assert(status && "Failed to LoadMesh from FBXImporter");

		MeshData meshData;

		LoadMeshData(meshData, tgaMesh);

		std::unique_ptr<Mesh> newMesh = std::make_unique<Mesh>();

		const std::string fileName = SimpleUtilities::ConvertFilePathToPrettyName(aFileName);
		newMesh->Init(meshData, fileName);

		AddMesh(aFileName, std::move(newMesh));
	}

	void ModelFactory::LoadAndCacheMesh(const std::string& aFileName, TGA::FBX::Mesh& aTGAMesh)
	{
		TGA::FBX::FbxImportStatus status = TGA::FBX::Importer::LoadMeshA(aFileName, aTGAMesh);
		assert(status && "Failed to LoadMesh from FBXImporter");

		MeshData meshData;

		LoadMeshData(meshData, aTGAMesh);

		std::unique_ptr<Mesh> newMesh = std::make_unique<Mesh>();
		const std::string fileName = SimpleUtilities::ConvertFilePathToPrettyName(aFileName);
		newMesh->Init(meshData, fileName);

		AddMesh(aFileName, std::move(newMesh));
	}

	void ModelFactory::LoadAndCacheSkeleton(const std::string& aFileName, TGA::FBX::Mesh& aTGAMesh)
	{
		std::unique_ptr<Skeleton> skeletonData = std::make_unique<Skeleton>();

		LoadSkeletonData(*skeletonData.get(), aTGAMesh);

		AddSkeleton(aFileName, std::move(skeletonData));
	}

	void ModelFactory::LoadMeshData(MeshData& aMeshData, const TGA::FBX::Mesh& aTGAMesh) const
	{
		aMeshData.vertices.reserve(aTGAMesh.Elements[0].Vertices.size());
		aMeshData.indices.reserve(aTGAMesh.Elements[0].Indices.size());

		for (size_t i = 0; i < aTGAMesh.Elements[0].Vertices.size(); ++i)
		{
			Vertex vertex;

			vertex.position.x = aTGAMesh.Elements[0].Vertices[i].Position[0];
			vertex.position.y = aTGAMesh.Elements[0].Vertices[i].Position[1];
			vertex.position.z = aTGAMesh.Elements[0].Vertices[i].Position[2];
			vertex.position.w = aTGAMesh.Elements[0].Vertices[i].Position[3];

			vertex.bones.x = static_cast<float>(aTGAMesh.Elements[0].Vertices[i].BoneIDs[0]);
			vertex.bones.y = static_cast<float>(aTGAMesh.Elements[0].Vertices[i].BoneIDs[1]);
			vertex.bones.z = static_cast<float>(aTGAMesh.Elements[0].Vertices[i].BoneIDs[2]);
			vertex.bones.w = static_cast<float>(aTGAMesh.Elements[0].Vertices[i].BoneIDs[3]);

			vertex.color.x = aTGAMesh.Elements[0].Vertices[i].VertexColors[0][0];
			vertex.color.y = aTGAMesh.Elements[0].Vertices[i].VertexColors[0][1];
			vertex.color.z = aTGAMesh.Elements[0].Vertices[i].VertexColors[0][2];
			vertex.color.w = aTGAMesh.Elements[0].Vertices[i].VertexColors[0][3];

			if (vertex.color.x + vertex.color.y + vertex.color.z + vertex.color.w == 0)
			{
				vertex.color.x = 1.0f;
				vertex.color.y = 1.0f;
				vertex.color.z = 1.0f;
				vertex.color.w = 1.0f;
			}

			vertex.weights.x = aTGAMesh.Elements[0].Vertices[i].BoneWeights[0];
			vertex.weights.y = aTGAMesh.Elements[0].Vertices[i].BoneWeights[1];
			vertex.weights.z = aTGAMesh.Elements[0].Vertices[i].BoneWeights[2];
			vertex.weights.w = aTGAMesh.Elements[0].Vertices[i].BoneWeights[3];

			vertex.normal.x = aTGAMesh.Elements[0].Vertices[i].Normal[0];
			vertex.normal.y = aTGAMesh.Elements[0].Vertices[i].Normal[1];
			vertex.normal.z = aTGAMesh.Elements[0].Vertices[i].Normal[2];

			vertex.tangent.x = aTGAMesh.Elements[0].Vertices[i].Tangent[0];
			vertex.tangent.y = aTGAMesh.Elements[0].Vertices[i].Tangent[1];
			vertex.tangent.z = aTGAMesh.Elements[0].Vertices[i].Tangent[2];

			vertex.bitangent.x = aTGAMesh.Elements[0].Vertices[i].BiNormal[0];
			vertex.bitangent.y = aTGAMesh.Elements[0].Vertices[i].BiNormal[1];
			vertex.bitangent.z = aTGAMesh.Elements[0].Vertices[i].BiNormal[2];

			vertex.uv.x = aTGAMesh.Elements[0].Vertices[i].UVs[0][0];
			vertex.uv.y = aTGAMesh.Elements[0].Vertices[i].UVs[0][1];

			aMeshData.vertices.push_back(vertex);
		}

		aMeshData.indices = aTGAMesh.Elements[0].Indices;
	}
}