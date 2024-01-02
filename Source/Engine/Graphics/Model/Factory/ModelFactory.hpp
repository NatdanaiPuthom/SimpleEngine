#pragma once

namespace Simple
{
	class Model;
	class Mesh;
}

namespace Simple
{
	class ModelFactory final
	{
	public:
		ModelFactory();
		~ModelFactory();

		void AddMesh(const char* aName, std::unique_ptr<const Simple::Mesh> aMesh);
		const Simple::Mesh* GetMesh(const char* aMeshName);

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
		std::unordered_map<std::string, const std::unique_ptr<const Simple::Mesh>> myMeshes;
	};
}