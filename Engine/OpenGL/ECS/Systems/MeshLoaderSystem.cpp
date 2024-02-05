#include <MeshLoaderSystem.h>
#include <iostream>


MeshLoaderSystem::MeshLoaderSystem(EntityManager* entityManager) : IComponentTypeQuerySystem(entityManager)
{
	
}

MeshLoaderSystem::~MeshLoaderSystem()
{
	Destroy();
}

void MeshLoaderSystem::Init()
{
	if (PrimitiveList.size() == 0)
	{
		int id = LoadMeshesFromFile("../Engine/Assets/cube.obj")[0];
		PrimitiveList.emplace(std::pair<PrimitiveShape, int>(PrimitiveShape::Cube, id));

		id = LoadMeshesFromFile("../Engine/Assets/quad.obj")[0];
		PrimitiveList.emplace(std::pair<PrimitiveShape, int>(PrimitiveShape::Quad, id));

		id = LoadMeshesFromFile("../Engine/Assets/sphere.obj")[0];
		PrimitiveList.emplace(std::pair<PrimitiveShape, int>(PrimitiveShape::Sphere, id));
	}
}

void MeshLoaderSystem::Destroy()
{
	for (auto m : Meshes)
	{
		if (m != nullptr)
		{
			delete m;
			m = nullptr;
		}
	}
	for (auto m : MeshOpenGLHandles)
	{
		if (m != nullptr)
		{
			delete m;
			m = nullptr;
		}
	}
}

void MeshLoaderSystem::PreUpdate(float deltaTime)
{
	
}

void MeshLoaderSystem::ComponentTypeQueryUpdate(float deltaTime, IEntityStorage* entityTypeStorage)
{


}

std::vector<int> MeshLoaderSystem::LoadMeshesFromFile(std::string modelFilePath)
{
	std::vector<int> meshIDs;

	auto it = MeshList.find(modelFilePath);
	if (it == MeshList.end())
	{
		std::vector<MeshLoaderSystem::Mesh*> loadedMeshes = LoadMeshes(modelFilePath);
		if (loadedMeshes.size() > 0)
		{
			for (size_t i = 0; i < loadedMeshes.size(); i++)
			{
				MeshLoaderSystem::Mesh* m = loadedMeshes[i];
				MeshLoaderSystem::MeshHandles* handles = CreateMeshBuffers(m->vertices, m->indices);
				MeshOpenGLHandles.push_back(handles);
				m->MeshHandleID = (static_cast<int>(MeshOpenGLHandles.size() - 1));
				Meshes.push_back(m);
				int meshID = static_cast<int>(Meshes.size() - 1);
				meshIDs.push_back(meshID);
				if (i == 0) { MeshList.emplace(std::pair<std::string, int>(modelFilePath, meshID)); }
				else { MeshList.emplace(std::pair<std::string, int>((modelFilePath + "::" + std::to_string(i)), meshID)); }
			}
		}
	}
	else
	{
		meshIDs.push_back(it->second);
		int i = 0;
		while (++it != MeshList.end())
		{
			if (it->first == modelFilePath + "::" + std::to_string(++i))
			{
				meshIDs.push_back(it->second);
			}
			else { break; }
		}
	}

	

	
	return meshIDs;
}

MeshLoaderSystem::MeshHandles* MeshLoaderSystem::GetMeshHandlesWithVAO(GLuint VAO)
{
	for (auto m : MeshOpenGLHandles)
	{
		if (m->VAO == VAO)
		{
			return m;
		}
	}
	return nullptr;
}

MeshLoaderSystem::Mesh* MeshLoaderSystem::GetMesh(int meshID)
{
	if (meshID < static_cast<int>(Meshes.size()))
	{
		return Meshes[meshID];
	}
	return nullptr;
}

MeshLoaderSystem::MeshHandles* MeshLoaderSystem::GetMeshHandles(int meshID)
{
	if (meshID < static_cast<int>(Meshes.size()))
	{
		return MeshOpenGLHandles[Meshes[meshID]->MeshHandleID];
	}

	return nullptr;
}

MeshComponent MeshLoaderSystem::GetPrimitiveShape(PrimitiveShape shape)
{
	MeshComponent meshComponent;
	auto it = PrimitiveList.find(shape);
	MeshHandles* m = GetMeshHandles(it->second);
	meshComponent.IndiceCount = m->IndiceCount;
	meshComponent.VAO = m->VAO;
	meshComponent.VBO = m->VBO;
	meshComponent.EBO = m->EBO;
	return meshComponent;
}

void MeshLoaderSystem::MakeInstanced(MeshComponent* mesh, int count, const glm::mat4* firstModelMatrix)
{
	MeshHandles* handle = GetMeshHandlesWithVAO(mesh->VAO);
	if (handle->InstanceBuffer != 0) { glDeleteBuffers(1, &handle->InstanceBuffer); }

	glBindVertexArray(mesh->VAO);


	glGenBuffers(1, &handle->InstanceBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, handle->InstanceBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * count, firstModelMatrix, GL_STATIC_DRAW);

	GLsizei vec4Size = static_cast<GLsizei>(sizeof(glm::vec4));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	
	glBindVertexArray(0);
}



MeshLoaderSystem::Mesh* MeshLoaderSystem::ProcessAssimpMesh(aiMesh* mesh)
{
	//TODO 1: add data containers for vertices and indices
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	//std::vector<Texture> textures;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex; //temporable container for the data of each loop
		//TODO 2: load data from the Assimp mesh to our containers
		
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		if (mesh->GetNumUVChannels() > 0) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}

		if (mesh->HasTangentsAndBitangents())
		{
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
		}

		vertices.push_back(vertex);
	}
	//Now retrieve the corresponding vertex indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	//TODO 3: return with new Mesh of our own
	Mesh* m = new Mesh();
	m->vertices = vertices;
	m->indices = indices;
	return m;

}

void MeshLoaderSystem::ProcessAssimpNode(std::vector<Mesh*>* meshes, aiNode* node, const aiScene* scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		// the node object only contains indices to index the actual objects in the scene.
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes->push_back(ProcessAssimpMesh(mesh));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessAssimpNode(meshes, node->mChildren[i], scene);
	}
}

std::vector<MeshLoaderSystem::Mesh*> MeshLoaderSystem::LoadMeshes(const std::string& path)
{
	//TODO 1: create the container that will be returned by this function
	std::vector<MeshLoaderSystem::Mesh*> meshes;
	//read file with Assimp
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	//Check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		printf("Error loading model file \"%s\": \"%s\" ", path.c_str(), importer.GetErrorString());
		return meshes;
	}
	// retrieve the directory path of the filepath
	std::string dir = path.substr(0, path.find_last_of('/'));
	//TODO 2: process Assimp's root node recursively
	ProcessAssimpNode(&meshes, scene->mRootNode, scene);

	return meshes;
}

MeshLoaderSystem::MeshHandles* MeshLoaderSystem::CreateMeshBuffers(const std::vector<MeshLoaderSystem::Vertex>& vertices, const std::vector<unsigned int>& indices)
{
	MeshLoaderSystem::MeshHandles* handles = new MeshLoaderSystem::MeshHandles();
	handles->IndiceCount = indices.size();
	// create buffers/arrays
	glGenVertexArrays(1, &handles->VAO);
	glGenBuffers(1, &handles->VBO);
	glGenBuffers(1, &handles->EBO);

	glBindVertexArray(handles->VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, handles->VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handles->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex texture coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(MeshLoaderSystem::Vertex, TexCoords));
	// vertex normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(MeshLoaderSystem::Vertex, Normal));
	// vertex tangents
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(MeshLoaderSystem::Vertex, Tangent));

	glBindVertexArray(0);

	return handles;
}