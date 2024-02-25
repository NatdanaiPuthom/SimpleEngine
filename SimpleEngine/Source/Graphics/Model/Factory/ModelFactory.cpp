#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Model/Factory/ModelFactory.hpp"
#include "Graphics/Model/Factory/ShapeCreator3000.hpp"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"
#include <External/TheGameAssembly/FBXImporter/source/Importer.h>

namespace Simple
{
	ModelFactory::ModelFactory()
	{
	}

	ModelFactory::~ModelFactory()
	{
		TGA::FBX::Importer::UninitImporter();
	}

	void ModelFactory::Init()
	{
		TGA::FBX::Importer::InitImporter();

		MeshData cubeData = Simple::ShapeCreator3000::CreateCube();
		MeshData pyramidData = Simple::ShapeCreator3000::CreatePyramid();
		MeshData planeData = Simple::ShapeCreator3000::CreatePlane();
		MeshData skyboxData = Simple::ShapeCreator3000::CreateSkyBox(Math::Vector3f(100, 100, 100));
		MeshData terrainData = Simple::ShapeCreator3000::CreateTerrain();
		MeshData sphereData = Simple::ShapeCreator3000::CreateSphere();

		std::unique_ptr<Simple::Mesh> cubeMesh = std::make_unique<Simple::Mesh>();
		std::unique_ptr<Simple::Mesh> pyramidMesh = std::make_unique<Simple::Mesh>();
		std::unique_ptr<Simple::Mesh> planeMesh = std::make_unique<Simple::Mesh>();
		std::unique_ptr<Simple::Mesh> skyboxMesh = std::make_unique<Simple::Mesh>();
		std::unique_ptr<Simple::Mesh> directionalLightMesh = std::make_unique<Simple::Mesh>();
		std::unique_ptr<Simple::Mesh> terrainMesh = std::make_unique<Simple::Mesh>();
		std::unique_ptr<Simple::Mesh> sphereMesh = std::make_unique<Simple::Mesh>();

		if (!cubeMesh->Init(cubeData))
			assert(false && "Failed to create Cube");

		if (!pyramidMesh->Init(pyramidData))
			assert(false && "Failed to create Pyramid");

		if (!planeMesh->Init(planeData))
			assert(false && "Failed to create Plane");

		if (!skyboxMesh->Init(skyboxData))
			assert(false && "Failed to create SkyBox");

		if (!terrainMesh->Init(terrainData))
			assert(false && "Failed to create Terrain");

		if (!sphereMesh->Init(sphereData))
			assert(false && "Failed to create Sphere");

		AddMesh("Cube", std::move(cubeMesh));
		AddMesh("Pyramid", std::move(pyramidMesh));
		AddMesh("Plane", std::move(planeMesh));
		AddMesh("Skybox", std::move(skyboxMesh));
		AddMesh("DirectionalLight", std::move(directionalLightMesh));
		AddMesh("Terrain", std::move(terrainMesh));
		AddMesh("Sphere", std::move(sphereMesh));
	}

	Model ModelFactory::LoadFBX(const char* aFileName)
	{
		Simple::Model model;
		const Simple::Mesh* mesh = GetMesh(aFileName);

		if (mesh == nullptr)
		{
			TGA::FBX::Mesh tgaMesh;
			TGA::FBX::FbxImportStatus status = TGA::FBX::Importer::LoadMeshA(SimpleUtilities::GetAbsolutePath(aFileName), tgaMesh);
			assert(status && "Failed to LoadMesh from FBXImporter");

			Simple::MeshData meshData;
			Simple::Skeleton skeletonData;

			LoadMeshData(meshData, tgaMesh);
			LoadSkeletonData(skeletonData, tgaMesh);

			std::unique_ptr<Simple::Mesh> newMesh = std::make_unique<Simple::Mesh>();
			newMesh->Init(meshData);
			newMesh->mySkeleton = skeletonData; //TO-DO: Re-structure where it should be. If the Mesh class should hold it or Model class

			AddMesh(aFileName, std::move(newMesh));
			mesh = GetMesh(aFileName);

			if (mesh == nullptr)
				assert(false && "Failed to GetMesh from bank");
		}

		model.Init(mesh);
		return model;
	}

	Animation ModelFactory::LoadAnimationFBX(const char* aFileName)
	{
		TGA::FBX::Animation tgaAnimation;
		TGA::FBX::FbxImportStatus status = TGA::FBX::Importer::LoadAnimationA(SimpleUtilities::GetAbsolutePath(aFileName), tgaAnimation);

		Simple::Animation animation;
		animation.name = tgaAnimation.Name;
		animation.length = tgaAnimation.Length;
		animation.framesPerSecond = tgaAnimation.FramesPerSecond;
		animation.duration = static_cast<float>(tgaAnimation.Duration);
		animation.frames.resize(tgaAnimation.Frames.size());

		for (size_t i = 0; i < tgaAnimation.Frames.size(); ++i)
		{
			animation.frames[i].localTransforms.reserve(tgaAnimation.Frames[i].LocalTransforms.size());

			for (const auto& [boneName, boneTransform] : tgaAnimation.Frames[i].LocalTransforms)
			{
				Math::Matrix4x4f localMatrix;

				localMatrix(1, 1) = boneTransform.m11;
				localMatrix(1, 2) = boneTransform.m12;
				localMatrix(1, 3) = boneTransform.m13;
				localMatrix(1, 4) = boneTransform.m14;

				localMatrix(2, 1) = boneTransform.m21;
				localMatrix(2, 2) = boneTransform.m22;
				localMatrix(2, 3) = boneTransform.m23;
				localMatrix(2, 4) = boneTransform.m24;

				localMatrix(3, 1) = boneTransform.m31;
				localMatrix(3, 2) = boneTransform.m32;
				localMatrix(3, 3) = boneTransform.m33;
				localMatrix(3, 4) = boneTransform.m34;

				localMatrix(4, 1) = boneTransform.m41;
				localMatrix(4, 2) = boneTransform.m42;
				localMatrix(4, 3) = boneTransform.m43;
				localMatrix(4, 4) = boneTransform.m44;


				Math::Transform transform;
				transform.SetScale(localMatrix.GetScale());
				transform.SetRotation(localMatrix.GetRotation()); //I WILL FIX DECOMPOSE MATRIX I AM TRYING
				transform.SetPosition(localMatrix.GetPosition());

				animation.frames[i].localTransforms.emplace(boneName, transform);

				localMatrix = Math::Matrix4x4f::Transpose(localMatrix); //No clue but it looks slightly better with this
				animation.frames[i].localMatrix.emplace(boneName, localMatrix);
			}
		}

		return animation;
	}

	void ModelFactory::AddMesh(const char* aName, std::unique_ptr<const Simple::Mesh> aMesh)
	{
		myMeshes.emplace(aName, std::move(aMesh));
	}

	const Simple::Mesh* ModelFactory::GetMesh(const char* aMeshName)
	{
		auto mesh = myMeshes.find(aMeshName);

		if (mesh != myMeshes.end())
			return mesh->second.get();

		return nullptr;
	}

	void ModelFactory::LoadSkeletonData(Simple::Skeleton& aSkeletonData, const TGA::FBX::Mesh& aTGAMesh) const
	{
		if (aTGAMesh.Skeleton.GetRoot())
		{
			aSkeletonData.myJoints.reserve(aTGAMesh.Skeleton.Bones.size());
			aSkeletonData.myJointNames.reserve(aSkeletonData.myJoints.size());

			for (size_t i = 0; i < aTGAMesh.Skeleton.Bones.size(); ++i)
			{
				Math::Matrix4x4f bindPoseInverseTranspose;

				bindPoseInverseTranspose(1, 1) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m11;
				bindPoseInverseTranspose(1, 2) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m12;
				bindPoseInverseTranspose(1, 3) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m13;
				bindPoseInverseTranspose(1, 4) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m14;

				bindPoseInverseTranspose(2, 1) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m21;
				bindPoseInverseTranspose(2, 2) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m22;
				bindPoseInverseTranspose(2, 3) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m23;
				bindPoseInverseTranspose(2, 4) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m24;

				bindPoseInverseTranspose(3, 1) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m31;
				bindPoseInverseTranspose(3, 2) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m32;
				bindPoseInverseTranspose(3, 3) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m33;
				bindPoseInverseTranspose(3, 4) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m34;

				bindPoseInverseTranspose(4, 1) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m41;
				bindPoseInverseTranspose(4, 2) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m42;
				bindPoseInverseTranspose(4, 3) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m43;
				bindPoseInverseTranspose(4, 4) = aTGAMesh.Skeleton.Bones[i].BindPoseInverse.m44;

				Simple::Joint joint;

				//joint.myBindPoseInverse = Math::Matrix4x4f::Transpose(bindPoseInverseTranspose); //TGA Did Tranpose but when I do it everything become weird
				joint.myBindPoseInverse = bindPoseInverseTranspose;
				joint.myName = aTGAMesh.Skeleton.Bones[i].Name;
				joint.myParent = aTGAMesh.Skeleton.Bones[i].ParentIdx;
				joint.myChildren = aTGAMesh.Skeleton.Bones[i].Children;

				aSkeletonData.myJointNameToIndex.insert({ joint.myName, i });
				aSkeletonData.myJointNames.push_back(joint.myName);
				aSkeletonData.myJoints.push_back(joint);
			}
		}
	}

	void ModelFactory::LoadMeshData(Simple::MeshData& aMeshData, const TGA::FBX::Mesh& aTGAMesh) const
	{
		aMeshData.vertices.reserve(aTGAMesh.Elements[0].Vertices.size());
		aMeshData.indices.reserve(aTGAMesh.Elements[0].Indices.size());

		for (size_t i = 0; i < aTGAMesh.Elements[0].Vertices.size(); ++i)
		{
			Simple::Vertex vertex;

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

	std::unique_ptr<Simple::Model> ModelFactory::CreateTerrainModel()
	{
		std::unique_ptr<Simple::Model> terrainModel = std::make_unique<Model>();

		terrainModel->Init(GetMesh("Terrain"));
		terrainModel->ClearTextures();
		terrainModel->SetShader("TerrainPS.cso", "DefaultVS.cso");

		terrainModel->AddTexture("TGA/Uppgift6/Grass_c.dds");
		terrainModel->AddTexture("TGA/Uppgift6/Rock_c.dds");
		terrainModel->AddTexture("TGA/Uppgift6/Snow_c.dds");
		terrainModel->AddTexture("TGA/Uppgift6/Grass_n.dds");
		terrainModel->AddTexture("TGA/Uppgift6/Rock_n.dds");
		terrainModel->AddTexture("TGA/Uppgift6/Snow_n.dds");
		terrainModel->AddTexture("TGA/Uppgift7/Grass_m.dds");
		terrainModel->AddTexture("TGA/Uppgift7/Rock_m.dds");
		terrainModel->AddTexture("TGA/Uppgift7/Snow_m.dds");

		terrainModel->SetScale({ 1,1,1 });
		terrainModel->SetPosition(Math::Vector3f(0, 0, 0));
		terrainModel->SetName("Terrain");

		return std::move(terrainModel);
	}

	std::unique_ptr<Simple::Model> ModelFactory::CreateSkyBoxModel()
	{
		std::unique_ptr<Simple::Model> skyBoxModel = std::make_unique<Simple::Model>();

		skyBoxModel->Init(GetMesh("Skybox"), "TGA/Uppgift7/Cubemap.dds");
		skyBoxModel->SetShader("SkyBoxPS.cso", "SkyBoxVS.cso");
		skyBoxModel->SetScale({ 1,1,1 });
		skyBoxModel->SetPosition({ 0,0,0 });
		skyBoxModel->SetName("SkyBox");

		return std::move(skyBoxModel);
	}

	std::unique_ptr<Simple::Model> ModelFactory::CreateDirectionalLightModel()
	{
		std::unique_ptr<Simple::Model> directionalLight = std::make_unique<Simple::Model>();

		directionalLight = std::make_unique<Simple::Model>();
		directionalLight->Init(GetMesh("DirectionalLight"));
		directionalLight->SetScale({ 1,1,1 });
		directionalLight->SetPosition(Math::Vector3f(0, 0, 0));
		directionalLight->SetName("DirectionalLight");

		return std::move(directionalLight);
	}

	std::unique_ptr<Simple::Model> ModelFactory::CreatePlaneModel()
	{
		std::unique_ptr<Simple::Model> plane = std::make_unique<Simple::Model>();

		plane->Init(GetMesh("Plane"));
		plane->SetShader("DefaultPS.cso", "DefaultVS.cso");
		plane->SetScale({ 1,1,1 });
		plane->SetPosition(Math::Vector3f(0, 0, 0));
		plane->SetName("Plane");

		return std::move(plane);
	}

	std::unique_ptr<Simple::Model> ModelFactory::CreateCubeModel()
	{
		std::unique_ptr<Simple::Model> cube = std::make_unique<Simple::Model>();

		cube->Init(GetMesh("Cube"));
		cube->SetScale({ 1,1,1 });
		cube->SetPosition(Math::Vector3f(0, 0, 0));
		cube->SetName("Cube");

		return std::move(cube);
	}

	std::unique_ptr<Simple::Model> ModelFactory::CreatePyramidModel()
	{
		std::unique_ptr<Simple::Model> pyramid = std::make_unique<Simple::Model>();

		pyramid->Init(GetMesh("Pyramid"), "Cat.dds");
		pyramid->SetScale({ 1,1,1 });
		pyramid->SetPosition(Math::Vector3f(0, 0, 0));
		pyramid->SetName("Pyramid");

		return std::move(pyramid);
	}

	std::unique_ptr<Simple::Model> ModelFactory::CreateSphereModel()
	{
		std::unique_ptr<Simple::Model> sphere = std::make_unique<Simple::Model>();

		sphere->Init(GetMesh("Sphere"));
		sphere->SetScale({ 1,1,1 });
		sphere->SetPosition(Math::Vector3f(0, 0, 0));
		sphere->SetName("Sphere");

		return std::move(sphere);
	}

	std::unique_ptr<Simple::Model> ModelFactory::CreatePlaneReflection()
	{
		std::unique_ptr<Simple::Model> plane = std::make_unique<Simple::Model>();

		plane->Init(GetMesh("Plane"));
		plane->SetShader("WaterReflectionPS.cso", "DefaultVS.cso");
		plane->SetScale({ 1,1,1 });
		plane->SetPosition(Math::Vector3f(0, 0, 0));
		plane->SetName("PlaneReflection");

		return std::move(plane);
	}
}