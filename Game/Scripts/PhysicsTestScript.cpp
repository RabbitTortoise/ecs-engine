#include <Game/Scripts/PhysicsTestScript.h>
#include <Engine/Interface/IECS.h>

#include <Engine/OpenGL/ECS/Systems/RenderSystem.h>
#include <Engine/OpenGL/ECS/Systems/MeshLoaderSystem.h>
#include <Engine/OpenGL/ECS/Systems/TextureLoaderSystem.h>
#include <Engine/OpenGL/ECS/Systems/ShaderLoaderSystem.h>
#include <Engine/ECS/Systems/PhysicsSystem.h>


#include <Engine/OpenGL/ECS/Components/MaterialComponent.h>
#include <Engine/OpenGL/ECS/Components/MeshComponent.h>
#include <Engine/ECS/Components/TransformComponent.h>
#include <Engine/ECS/Components/BoxColliderComponent.h>
#include <Engine/ECS/Components/RigidbodyComponent.h>

#include <MatrixCalculations.h>

#include <Engine/ECS/Entities/RigidbodyEntity.h>
#include <../Interface/WindowManager.h>

#include <Game/Entities/DynamicSceneEntity.h>
#include <Game/Entities/CollisionParticleEntity.h>
#include <MatrixCalculations.h>
#include <limits>

PhysicsTestScript::PhysicsTestScript()
{
    PlayerShader = 0;
    WorldEntityManager = IECS::GetEntityManager();
    WorldSystemsManager = IECS::GetSystemsManager();
}

PhysicsTestScript::~PhysicsTestScript()
{
}

void PhysicsTestScript::Init()
{
    srand(static_cast <unsigned> (time(0)));

    RenderSystem* Renderer = WorldSystemsManager->GetSystem<RenderSystem>();
    MeshLoaderSystem* meshLoaderSystem = WorldSystemsManager->GetSystem<MeshLoaderSystem>();
    TextureLoaderSystem* textureLoaderSystem = WorldSystemsManager->GetSystem<TextureLoaderSystem>();
    ShaderLoaderSystem* shaderLoaderSystem = WorldSystemsManager->GetSystem<ShaderLoaderSystem>();

    //Create Entity Storages 
    WorldEntityManager->CreateEntityTypeStorage<DynamicSceneEntity>();
    WorldEntityManager->CreateEntityTypeStorage<CollisionParticleEntity>();


    static_cast<IComponentTypeQuerySystem*>(Renderer)->AddEntityTypeIfCompatible<CollisionParticleEntity>();

    PhysicsSystem* Physics = WorldSystemsManager->GetSystem<PhysicsSystem>();
    static_cast<IComponentTypeQuerySystem*>(Physics)->AddEntityTypeIfCompatible<CollisionParticleEntity>();



    EntityTypeID = WorldEntityManager->GetEntityTypeId<RigidbodyEntity>();
   
    Entity* newEntity = WorldEntityManager->CreateEntity<CollisionParticleEntity>();
    if (newEntity != nullptr)
    {

        TransformComponent* transform = WorldEntityManager->GetComponent<TransformComponent>(newEntity);
        MaterialComponent* materialComponent = WorldEntityManager->GetComponent<MaterialComponent>(newEntity);
        MeshComponent* meshComponent = WorldEntityManager->GetComponent<MeshComponent>(newEntity);
        RigidbodyComponent* rigidBodyComponent = WorldEntityManager->GetComponent<RigidbodyComponent>(newEntity);
        BoxColliderComponent* boxColliderComponent = WorldEntityManager->GetComponent<BoxColliderComponent>(newEntity);

        if (transform != nullptr && materialComponent != nullptr && meshComponent != nullptr && rigidBodyComponent != nullptr)
        {
            //Transform
            transform->Position.x = 0;
            transform->Position.y = 0;
            transform->Position.z = -10;

            transform->UseQuaternion = true;
            transform->Orientation.x = 0;
            transform->Orientation.y = 0;
            transform->Orientation.z = 0;
            transform->Orientation.w = 1;
            transform->Scale.x = 10.0f;
            transform->Scale.y = 0.5f;
            transform->Scale.z = 10.0f;

            //Material
            materialComponent->ShaderID = shaderLoaderSystem->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR_Instanced.vert", "../Engine/OpenGL/Shaders/PBR.frag");
            materialComponent->Texture_Albedo = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-albedo.png");
            materialComponent->Texture_AO = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-ao.png");
            materialComponent->Texture_Depth = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Depth.png");
            materialComponent->Texture_Metallic = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Metallic.png");
            materialComponent->Texture_Normal = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Normal-ogl.png");
            materialComponent->Texture_Roughness = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Roughness.png");
            materialComponent->TextureRepeat = 1.0f;
            materialComponent->ParallaxScale = 0.0f;
            materialComponent->IBLEnabled = 1;

            //Mesh
            *meshComponent = meshLoaderSystem->GetPrimitiveShape(MeshLoaderSystem::PrimitiveShape::Cube);

            rigidBodyComponent->AffectedByGravity = false;
            rigidBodyComponent->Restitution = 0.0f;


            boxColliderComponent->BoxHalfSize = glm::vec3(10.0f, 0.5f, 10.0f);

        }
    }
     
    //Rigidbody testing spawn
    newEntity = WorldEntityManager->CreateEntity<RigidbodyEntity>();
    if (newEntity != nullptr)
    {
        Collider1ID = newEntity->GetID();
        TransformComponent* transform = WorldEntityManager->GetComponent<TransformComponent>(newEntity);
        MaterialComponent* materialComponent = WorldEntityManager->GetComponent<MaterialComponent>(newEntity);
        MeshComponent* meshComponent = WorldEntityManager->GetComponent<MeshComponent>(newEntity);
        RigidbodyComponent* rigidBodyComponent = WorldEntityManager->GetComponent<RigidbodyComponent>(newEntity);
        BoxColliderComponent* boxColliderComponent = WorldEntityManager->GetComponent<BoxColliderComponent>(newEntity);

        if (transform != nullptr && materialComponent != nullptr && meshComponent != nullptr && rigidBodyComponent != nullptr)
        {
            //Transform
            transform->Position.x = 20;
            transform->Position.y = 5;
            transform->Position.z = -2;

            transform->UseQuaternion = true;
            transform->Orientation.x = 0;
            transform->Orientation.y = 0;
            transform->Orientation.z = 0;
            transform->Orientation.w = 1;

            transform->Scale.x = 0.5f;
            transform->Scale.y = 0.5f;
            transform->Scale.z = 0.5f;

            //Material
            materialComponent->ShaderID = shaderLoaderSystem->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR_Instanced.vert", "../Engine/OpenGL/Shaders/PBR.frag");
            materialComponent->Texture_Albedo = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-albedo.png");
            materialComponent->Texture_AO = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-ao.png");
            materialComponent->Texture_Depth = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Depth.png");
            materialComponent->Texture_Metallic = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Metallic.png");
            materialComponent->Texture_Normal = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Normal-ogl.png");
            materialComponent->Texture_Roughness = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Roughness.png");
            materialComponent->TextureRepeat = 1.0f;
            materialComponent->ParallaxScale = 0.0f;
            materialComponent->IBLEnabled = 1;

            //Mesh
            //Mesh
            std::vector<int> meshIDs = meshLoaderSystem->LoadMeshesFromFile("../Engine/Assets/cubexyz.obj");
            MeshLoaderSystem::MeshHandles* mesh = meshLoaderSystem->GetMeshHandles(meshIDs[0]);
            meshComponent->IndiceCount = mesh->IndiceCount;
            meshComponent->VAO = mesh->VAO;

            rigidBodyComponent->AffectedByGravity = false;
            rigidBodyComponent->Gravity = glm::vec3(0, -1.0f, 0);
            rigidBodyComponent->Restitution = 0.5f;
            rigidBodyComponent->Velocity = glm::vec3(0.0f, 0.0f, 0);


            boxColliderComponent->BoxHalfSize = glm::vec3(0.5f, 0.5f, 0.5f);
        }
    }
    
    //Rigidbody testing spawn
    newEntity = WorldEntityManager->CreateEntity<RigidbodyEntity>();
    if (newEntity != nullptr)
    {
        Collider2ID = newEntity->GetID();

        TransformComponent* transform = WorldEntityManager->GetComponent<TransformComponent>(newEntity);
        MaterialComponent* materialComponent = WorldEntityManager->GetComponent<MaterialComponent>(newEntity);
        MeshComponent* meshComponent = WorldEntityManager->GetComponent<MeshComponent>(newEntity);
        RigidbodyComponent* rigidBodyComponent = WorldEntityManager->GetComponent<RigidbodyComponent>(newEntity);
        BoxColliderComponent* boxColliderComponent = WorldEntityManager->GetComponent<BoxColliderComponent>(newEntity);

        if (transform != nullptr && materialComponent != nullptr && meshComponent != nullptr && rigidBodyComponent != nullptr)
        {
            //Transform
            transform->Position.x = 20;
            transform->Position.y = 5;
            transform->Position.z = 2;

            transform->UseQuaternion = true;
            transform->Orientation.x = 0;
            transform->Orientation.y = 0;
            transform->Orientation.z = 0;
            transform->Orientation.w = 1;

            transform->Scale.x = 0.5f;
            transform->Scale.y = 0.5f;
            transform->Scale.z = 0.5f;

            //Material
            materialComponent->ShaderID = shaderLoaderSystem->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR_Instanced.vert", "../Engine/OpenGL/Shaders/PBR.frag");
            materialComponent->Texture_Albedo = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-albedo.png");
            materialComponent->Texture_AO = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-ao.png");
            materialComponent->Texture_Depth = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Depth.png");
            materialComponent->Texture_Metallic = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Metallic.png");
            materialComponent->Texture_Normal = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Normal-ogl.png");
            materialComponent->Texture_Roughness = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Roughness.png");
            materialComponent->TextureRepeat = 1.0f;
            materialComponent->ParallaxScale = 0.0f;
            materialComponent->IBLEnabled = 1;

            //Mesh
            std::vector<int> meshIDs = meshLoaderSystem->LoadMeshesFromFile("../Engine/Assets/cubexyz.obj");
            MeshLoaderSystem::MeshHandles* mesh = meshLoaderSystem->GetMeshHandles(meshIDs[0]);
            meshComponent->IndiceCount = mesh->IndiceCount;
            meshComponent->VAO = mesh->VAO;

            rigidBodyComponent->AffectedByGravity = false;
            rigidBodyComponent->Gravity = glm::vec3(0, -1.0f, 0);
            rigidBodyComponent->Restitution = 0.5f;
            rigidBodyComponent->Velocity = glm::vec3(0.0f, 0.0f, 0);

            boxColliderComponent->BoxHalfSize = glm::vec3(0.5f, 0.5f, 0.5f);
        }
    }
}

void PhysicsTestScript::Update(float deltaTime)
{
    float speed = 1.0f;

    GLFWwindow* window = WindowManager::GetGLFWWindow();
    RigidbodyEntity* Collider1 = WorldEntityManager->GetEntity<RigidbodyEntity>(Collider1ID, EntityTypeID);
    RigidbodyEntity* Collider2 = WorldEntityManager->GetEntity<RigidbodyEntity>(Collider2ID, EntityTypeID);

    TransformComponent* transform1 = WorldEntityManager->GetComponent<TransformComponent>(Collider1);
    TransformComponent* transform2 = WorldEntityManager->GetComponent<TransformComponent>(Collider2);

    if (glfwGetKey(window, GLFW_KEY_J)) { transform1->Position.x -= speed * deltaTime; }    //LEFT
    if (glfwGetKey(window, GLFW_KEY_L)) { transform1->Position.x += speed * deltaTime; }    //RIGHT
    if (glfwGetKey(window, GLFW_KEY_O)) { transform1->Position.y += speed * deltaTime; }    //UP
    if (glfwGetKey(window, GLFW_KEY_U)) { transform1->Position.y -= speed * deltaTime; }    //DOWN
    if (glfwGetKey(window, GLFW_KEY_I)) { transform1->Position.z -= speed * deltaTime; }    //FORWARD
    if (glfwGetKey(window, GLFW_KEY_K)) { transform1->Position.z += speed * deltaTime; }    //BACKWARD

    if (glfwGetKey(window, GLFW_KEY_Y)) { 
        float angle = speed * deltaTime;
        glm::quat rotator = glm::angleAxis(angle, glm::vec3(1, 0, 0));
        transform1->Orientation = rotator * transform1->Orientation;
    }    //RotateX

    if (glfwGetKey(window, GLFW_KEY_H)) {
        float angle = speed * deltaTime;
        glm::quat rotator = glm::angleAxis(angle, glm::vec3(0, 1, 0));
        transform1->Orientation = rotator * transform1->Orientation;
    }     //RotateY

    if (glfwGetKey(window, GLFW_KEY_N)) {
        float angle = speed * deltaTime;
        glm::quat rotator = glm::angleAxis(angle, glm::vec3(0, 0, 1));
        transform1->Orientation = rotator * transform1->Orientation;
    }    //RotateZ

    if (glfwGetKey(window, GLFW_KEY_T)) {
        float angle = speed * deltaTime;
        glm::quat rotator = glm::angleAxis(angle, glm::vec3(1, 0, 0));
        transform2->Orientation = rotator * transform2->Orientation;
    }    //RotateX

    if (glfwGetKey(window, GLFW_KEY_G)) {
        float angle = speed * deltaTime;
        glm::quat rotator = glm::angleAxis(angle, glm::vec3(0, 1, 0));
        transform2->Orientation = rotator * transform2->Orientation;
    }     //RotateY

    if (glfwGetKey(window, GLFW_KEY_B)) {
        float angle = speed * deltaTime;
        glm::quat rotator = glm::angleAxis(angle, glm::vec3(0, 0, 1));
        transform2->Orientation = rotator * transform2->Orientation;
    }    //RotateZ

    int eID = WorldEntityManager->GetEntityTypeId<CollisionParticleEntity>();

    
    //Delete too old particles
    for (auto it = Particles.begin(); it != Particles.end();)
    {
        auto it2 = it;
        it++;
        if (Timer.getRunTime() - (*it2).second > MaxLifeTime)
        {
            WorldEntityManager->MarkEntityForDeletion((*it2).first, eID);
            Particles.erase(it2);
        }
    }
    
    
    

    //Update Orientation
    for (auto& e : Particles)
    {
        Entity* entity = WorldEntityManager->GetEntity<CollisionParticleEntity>(e.first, eID);
        TransformComponent* transform = WorldEntityManager->GetComponent<TransformComponent>(entity);
        //MaterialComponent* materialComponent = WorldEntityManager->GetComponent<MaterialComponent>(entity);
        //MeshComponent* meshComponent = WorldEntityManager->GetComponent<MeshComponent>(entity);
        RigidbodyComponent* rigidBodyComponent = WorldEntityManager->GetComponent<RigidbodyComponent>(entity);
        //BoxColliderComponent* boxColliderComponent = WorldEntityManager->GetComponent<BoxColliderComponent>(entity);
        if (glm::length(rigidBodyComponent->Velocity) != 0)
        {
            transform->Orientation = MatrixCalculations::LookAtQuaternion(rigidBodyComponent->Velocity, MatrixCalculations::CalculateUpVector(transform->Orientation));
        }
    }

    /*
    if (glfwGetKey(window, GLFW_KEY_X) && !pressed) { 
        for (auto it = Particles.begin(); it != Particles.end();)
        {
            auto it2 = it;
            it++;

            WorldEntityManager->MarkEntityForDeletion((*it2).first, eID);
            Particles.erase(it2);

        }
        pressed = true; }
    else if (!glfwGetKey(window, GLFW_KEY_X) && pressed) {
        SpawnCollider();
        pressed = false;
    }*/

    
    if (Timer.getRunTime() - LastTime > SpawnRate)
    {
        
        LastTime = Timer.getRunTime();
        SpawnCollider();
    }
    
}

float RandomNumber0_1()
{
    return (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
}

float RandomFloat(float LO, float HI)
{
    return LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
}


void PhysicsTestScript::SpawnCollider()
{
    RenderSystem* Renderer = WorldSystemsManager->GetSystem<RenderSystem>();
    MeshLoaderSystem* meshLoaderSystem = WorldSystemsManager->GetSystem<MeshLoaderSystem>();
    TextureLoaderSystem* textureLoaderSystem = WorldSystemsManager->GetSystem<TextureLoaderSystem>();
    ShaderLoaderSystem* shaderLoaderSystem = WorldSystemsManager->GetSystem<ShaderLoaderSystem>();

    Entity* newEntity = WorldEntityManager->CreateEntity<CollisionParticleEntity>();
    if (newEntity != nullptr)
    {

        TransformComponent* transform = WorldEntityManager->GetComponent<TransformComponent>(newEntity);
        MaterialComponent* materialComponent = WorldEntityManager->GetComponent<MaterialComponent>(newEntity);
        MeshComponent* meshComponent = WorldEntityManager->GetComponent<MeshComponent>(newEntity);
        RigidbodyComponent* rigidBodyComponent = WorldEntityManager->GetComponent<RigidbodyComponent>(newEntity);
        BoxColliderComponent* boxColliderComponent = WorldEntityManager->GetComponent<BoxColliderComponent>(newEntity);

        if (transform != nullptr && materialComponent != nullptr && meshComponent != nullptr && rigidBodyComponent != nullptr)
        {

            //RandomNumber0_1
            glm::vec3 rotation; //X=Pitch; 50_75, Y=Yaw; -90_90, Z = Roll -90_90
            
            glm::quat q1 = glm::angleAxis(glm::radians(RandomFloat(-45, -45)), glm::vec3(1, 0, 0));
            glm::quat q2 = glm::angleAxis(glm::radians(RandomFloat(0, 360)), glm::vec3(0, 1, 0));
            glm::quat q3 = glm::angleAxis(glm::radians(RandomFloat(0, 0)), glm::vec3(0, 0, 1));



            //Transform
            transform->Position.x = 0;
            transform->Position.y = 5;
            transform->Position.z = -10;

            transform->Orientation = q3 * q2 * q1;
            transform->UseQuaternion = true;

            transform->Scale.x = 0.5f;
            transform->Scale.y = 0.5f;
            transform->Scale.z = 0.5f;

            //Material
            materialComponent->ShaderID = shaderLoaderSystem->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR_Instanced.vert", "../Engine/OpenGL/Shaders/PBR.frag");
            materialComponent->Texture_Albedo = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/alien-slime1-bl/alien-slime1-albedo.png");
            materialComponent->Texture_AO = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/alien-slime1-bl/alien-slime1-ao.png");
            materialComponent->Texture_Depth = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/alien-slime1-bl/alien-slime1-height.png");//
            materialComponent->Texture_Metallic = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/alien-slime1-bl/alien-slime1-metallic.png");
            materialComponent->Texture_Normal = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/alien-slime1-bl/alien-slime1-normal-ogl.png");
            materialComponent->Texture_Roughness = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/alien-slime1-bl/alien-slime1-roughness.png");
            materialComponent->TextureRepeat = 1.0f;
            materialComponent->ParallaxScale = 0.0f;
            materialComponent->IBLEnabled = 1;

            //Mesh
            //*meshComponent = meshLoaderSystem->GetPrimitiveShape(MeshLoaderSystem::PrimitiveShape::Cube);
            std::vector<int> meshIDs = meshLoaderSystem->LoadMeshesFromFile("../Engine/Assets/cubexyz.obj");
            MeshLoaderSystem::MeshHandles* mesh = meshLoaderSystem->GetMeshHandles(meshIDs[0]);
            meshComponent->IndiceCount = mesh->IndiceCount;
            meshComponent->VAO = mesh->VAO;

            rigidBodyComponent->AffectedByGravity = true;
            rigidBodyComponent->Restitution = 0.0f;
            rigidBodyComponent->Gravity = glm::vec3(0, -9.81f, 0);
            rigidBodyComponent->Velocity = MatrixCalculations::CalculateForwardVector(transform->Orientation) * RandomFloat(5.0f, 7.0f);
            
            boxColliderComponent->BoxHalfSize = glm::vec3(0.5f, 0.5f, 0.5f);

        }
        
        Particles.insert(std::make_pair<int, float>(newEntity->GetID(), Timer.getRunTime()));


    }
}