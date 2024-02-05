#pragma once

#include <Engine/ECS/System.h>			// System declaration
#include <glm/glm.hpp>		// glm transform functions.
#include <glad/gl.h>					// Include glad
#include <MeshComponent.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>



class MeshLoaderSystem : public IComponentTypeQuerySystem
{
public:
	enum class PrimitiveShape {Cube = 0, Quad, Sphere};

	struct MeshHandles
	{
		GLuint VAO = 0;
		GLuint EBO = 0;
		GLuint VBO = 0;
		size_t IndiceCount = 0;
		GLuint InstanceBuffer = 0;
	};

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoords;
		glm::vec3 Normal;
		glm::vec3 Tangent;
	};

	struct Mesh
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		int MeshHandleID;
	};

	MeshLoaderSystem(EntityManager* entityManager);
	~MeshLoaderSystem();
	
	//System class overrides
	void Init() override;
	void Destroy() override;
	void PreUpdate(float deltaTime) override;
	void ComponentTypeQueryUpdate(float deltaTime, IEntityStorage* entityTypeStorage) override;

	//Mesh Loader System functionality
	std::vector<int> LoadMeshesFromFile(std::string modelFilePath);
	MeshLoaderSystem::MeshHandles* GetMeshHandlesWithVAO(GLuint VAO);
	Mesh* GetMesh(int meshID);
	MeshHandles* GetMeshHandles(int meshID);
	
	MeshComponent GetPrimitiveShape(PrimitiveShape shape);

	void MakeInstanced(MeshComponent* mesh, int count, const glm::mat4* firstModelMatrix);

	inline static const std::string SystemName = "MeshLoaderSystem";
	inline static const System::SystemType SystemType = System::SystemType::System;
private:
	

	Mesh* ProcessAssimpMesh(aiMesh* mesh);
	void ProcessAssimpNode(std::vector<Mesh*>* meshes, aiNode* node, const aiScene* scene);

	std::vector<MeshLoaderSystem::Mesh*> LoadMeshes(const std::string& path);
	MeshLoaderSystem::MeshHandles* CreateMeshBuffers(const std::vector<MeshLoaderSystem::Vertex>& vertices, const std::vector<unsigned int>& indices);

	//Mesh Information Storage:
	std::vector<Mesh*> Meshes;
	std::vector<MeshHandles*> MeshOpenGLHandles;
	std::map<std::string, int> MeshList;

	inline static std::map<PrimitiveShape, int> PrimitiveList;

};
