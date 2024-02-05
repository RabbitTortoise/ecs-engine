#include <Game/Scripts/CollectingGame.h>


#include <Engine/Interface/IECS.h>

#include <Game/Components/TriggerComponent.h>
#include <Game/Entities/MovingTriggerEntity.h>
#include <Game/Entities/StaticSceneEntity.h>
#include <Engine/OpenGL/ECS/Entities/CameraEntity.h>
#include <Game/Systems/TriggerSystem.h>
#include <Engine/OpenGL/ECS/Systems/RenderSystem.h>
#include <Engine/OpenGL/ECS/Systems/MeshLoaderSystem.h>
#include <Engine/OpenGL/ECS/Systems/TextureLoaderSystem.h>
#include <Engine/OpenGL/ECS/Systems/ShaderLoaderSystem.h>
#include <Engine/OpenGL/ECS/Systems/CameraControllerSystem.h>


CollectingGame::CollectingGame()
{
    GameTriggerSystem = nullptr;
    WorldEntityManager = IECS::GetEntityManager();
    WorldSystemsManager = IECS::GetSystemsManager();

    MeshLoader = static_cast<MeshLoaderSystem*>(WorldSystemsManager->GetSystemByName("MeshLoaderSystem"));
    TextureLoader = static_cast<TextureLoaderSystem*>(WorldSystemsManager->GetSystemByName("TextureLoaderSystem"));
    ShaderLoader = static_cast<ShaderLoaderSystem*>(WorldSystemsManager->GetSystemByName("ShaderLoaderSystem"));
    CameraController = static_cast<CameraControllerSystem*>(WorldSystemsManager->GetSystemByName("CameraControllerSystem"));

    Points = 0;
}

CollectingGame::~CollectingGame()
{
    if (GameTriggerSystem != nullptr)
    {
        delete GameTriggerSystem;
        GameTriggerSystem = nullptr;
    }
}

void CollectingGame::Init()
{

    //Create Component Types
    WorldEntityManager->CreateComponentType<TriggerComponent>();

    //Create Entity Storages
    WorldEntityManager->CreateEntityTypeStorage<MovingTriggerEntity>();
    WorldEntityManager->CreateEntityTypeStorage<StaticSceneEntity>();

    //Create Systems
    //Create engine WorldRenderSystem
    GameTriggerSystem = new TriggerSystem(WorldEntityManager, CameraController, this);
    WorldSystemsManager->AddSystem(GameTriggerSystem);
    GameTriggerSystem->AddRequiredComponentType<TransformComponent>();
    GameTriggerSystem->AddRequiredComponentType<TriggerComponent>();
    GameTriggerSystem->AddEntityTypeIfCompatible<MovingTriggerEntity>();

    RenderSystem* Renderer = WorldSystemsManager->GetSystem<RenderSystem>();
    static_cast<IComponentTypeQuerySystem*>(Renderer)->AddEntityTypeIfCompatible<MovingTriggerEntity>();
    static_cast<IComponentTypeQuerySystem*>(Renderer)->AddEntityTypeIfCompatible<StaticSceneEntity>();

    
    //Set main camera position.
    WorldEntityManager->GetComponent<TransformComponent>(CameraController->GetMainCamera())->Position.y = 5.0f;


    //Ground spawn and setup.
    Entity* staticEntity = WorldEntityManager->CreateEntity<StaticSceneEntity>();

    TransformComponent* transformStatic = WorldEntityManager->GetComponent<TransformComponent>(staticEntity);
    MaterialComponent* materialComponentStatic = WorldEntityManager->GetComponent<MaterialComponent>(staticEntity);
    MeshComponent* meshComponentStatic = WorldEntityManager->GetComponent<MeshComponent>(staticEntity);

    //Static Ground
    if (transformStatic != nullptr && materialComponentStatic != nullptr && meshComponentStatic != nullptr)
    {
        transformStatic->Position.x = 0;
        transformStatic->Position.y = 0;
        transformStatic->Position.z = 0;

        transformStatic->EulerAngles.x = 0;
        transformStatic->EulerAngles.y = 0;
        transformStatic->EulerAngles.z = 0;

        transformStatic->Scale.x = 0.5f;
        transformStatic->Scale.y = 0.2f;
        transformStatic->Scale.z = 0.5f;

        materialComponentStatic->ShaderID =ShaderLoader->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR_Instanced.vert", "../Engine/OpenGL/Shaders/PBR.frag");
        materialComponentStatic->Texture_Albedo =TextureLoader->LoadTextureFromFile("../Game/Assets/grass1-bl/grass1-albedo3.png");
        materialComponentStatic->Texture_AO =TextureLoader->LoadTextureFromFile("../Game/Assets/grass1-bl/grass1-ao.png");
        materialComponentStatic->Texture_Depth =TextureLoader->LoadTextureFromFile("../Game/Assets/grass1-bl/grass1-depth.png");
        materialComponentStatic->Texture_Normal = TextureLoader->LoadTextureFromFile("../Game/Assets/grass1-bl/grass1-normal1-ogl.png");
        materialComponentStatic->Texture_Roughness= TextureLoader->LoadTextureFromFile("../Game/Assets/grass1-bl/grass1-rough.png");
        materialComponentStatic->Texture_Metallic = TextureLoader->LoadTextureFromFile("../Game/Assets/grass1-bl/grass1-metallic.png");
        materialComponentStatic->TextureRepeat= 20.0f;
        materialComponentStatic->ParallaxScale= 0.0f;
        materialComponentStatic->IBLEnabled= 1;

        std::vector<int> meshIDs = MeshLoader->LoadMeshesFromFile("../Game/Assets/Ground.obj");
        MeshLoaderSystem::MeshHandles* mesh = MeshLoader->GetMeshHandles(meshIDs[0]);
        meshComponentStatic->IndiceCount = mesh->IndiceCount;
        meshComponentStatic->VAO = mesh->VAO;
    }

    SpawnTrigger();  
}

void CollectingGame::Update(float deltaTime)
{

}

void CollectingGame::SpawnTrigger()
{
    Entity* triggerEntity = WorldEntityManager->CreateEntity<MovingTriggerEntity>();
    if (triggerEntity != nullptr)
    {
        TransformComponent* transform = WorldEntityManager->GetComponent<TransformComponent>(triggerEntity);
        MaterialComponent* materialComponent = WorldEntityManager->GetComponent<MaterialComponent>(triggerEntity);
        MeshComponent* meshComponent = WorldEntityManager->GetComponent<MeshComponent>(triggerEntity);
        TriggerComponent* trigger = WorldEntityManager->GetComponent<TriggerComponent>(triggerEntity);
        

        if (materialComponent != nullptr && meshComponent != nullptr && transform != nullptr && trigger != nullptr)
        {
            trigger->MiddlePoint = glm::vec3(RandomNumber0_1() * 5, RandomNumber0_1() * 5 + 1, RandomNumber0_1() * 5);
            trigger->CurrentAngle = RandomNumber0_1() * 360.0f;
            trigger->Radius = RandomNumber0_1() * 4.0f + 1.5f;
            trigger->Speed = RandomNumber0_1() * 50.0f + 15.0f;

            //std::cout << trigger->MiddlePoint.x << "," << trigger->MiddlePoint.y << "," << trigger->MiddlePoint.z << ": " << trigger->CurrentAngle << "," << trigger->Speed << "," << trigger->Radius << std::endl;


            float x = trigger->Radius * glm::sin(glm::radians(trigger->CurrentAngle));
            float y = trigger->Radius * glm::cos(glm::radians(trigger->CurrentAngle));

            transform->Position.x = trigger->MiddlePoint.x + x;
            transform->Position.z = trigger->MiddlePoint.z + y;
            transform->Position.y = trigger->MiddlePoint.y;

            transform->EulerAngles.x = 0;
            transform->EulerAngles.y = 0;
            transform->EulerAngles.z = 0;

                materialComponent->ShaderID =ShaderLoader->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR_Instanced.vert", "../Engine/OpenGL/Shaders/PBR.frag");
                materialComponent->Texture_Albedo =TextureLoader->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-albedo.png");
                materialComponent->Texture_AO =TextureLoader->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-ao.png");
                materialComponent->Texture_Depth =TextureLoader->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Depth.png");
                materialComponent->Texture_Metallic = TextureLoader->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Metallic.png");
                materialComponent->Texture_Normal = TextureLoader->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Normal-ogl.png");
                materialComponent->Texture_Roughness= TextureLoader->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Roughness.png");
                materialComponent->TextureRepeat= 1.0f;
                materialComponent->ParallaxScale= 0.0f;
                materialComponent->IBLEnabled= 1;

                transform->Scale.x = 0.5f;
                transform->Scale.y = 0.5f;
                transform->Scale.z = 0.5f;

                MeshComponent m = MeshLoader->GetPrimitiveShape(MeshLoaderSystem::PrimitiveShape::Sphere);
                meshComponent->VAO = m.VAO;
                meshComponent->IndiceCount = m.IndiceCount;
        }
    }
}

void CollectingGame::AddCaughtSphere()
{
    Entity* staticEntity = WorldEntityManager->CreateEntity<StaticSceneEntity>();
    if (staticEntity != nullptr)
    {

        TransformComponent* transform = WorldEntityManager->GetComponent<TransformComponent>(staticEntity);
        MaterialComponent* materialComponent = WorldEntityManager->GetComponent<MaterialComponent>(staticEntity);
        MeshComponent* meshComponent = WorldEntityManager->GetComponent<MeshComponent>(staticEntity);

        if (materialComponent != nullptr && meshComponent != nullptr && transform != nullptr)
        {

            transform->Position.x = static_cast<float>( - 5 + (Points - 1) % 10 * 1);
            transform->Position.y = 1;
            transform->Position.z = static_cast<float>(-5 + glm::floor((Points-1) / 10.0f) * 1);

            transform->EulerAngles.x = 0;
            transform->EulerAngles.y = 0;
            transform->EulerAngles.z = 0;

            materialComponent->ShaderID = ShaderLoader->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR_Instanced.vert", "../Engine/OpenGL/Shaders/PBR.frag");
            materialComponent->Texture_Albedo = TextureLoader->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-albedo.png");
            materialComponent->Texture_AO =TextureLoader->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-ao.png");
            materialComponent->Texture_Depth =TextureLoader->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Depth.png");
            materialComponent->Texture_Metallic = TextureLoader->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Metallic.png");
            materialComponent->Texture_Normal = TextureLoader->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Normal-ogl.png");
            materialComponent->Texture_Roughness= TextureLoader->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Roughness.png");
            materialComponent->TextureRepeat= 1.0f;
            materialComponent->ParallaxScale= 0.0f;
            materialComponent->IBLEnabled= 1;

            transform->Scale.x = 0.5f;
            transform->Scale.y = 0.5f;
            transform->Scale.z = 0.5f;

            MeshComponent m = MeshLoader->GetPrimitiveShape(MeshLoaderSystem::PrimitiveShape::Sphere);
            meshComponent->VAO = m.VAO;
            meshComponent->IndiceCount = m.IndiceCount;
        }
    }
}

void CollectingGame::AddPoint()
{
    Points++;
    std::cout << "Current Points: " << Points <<  std::endl;
    AddCaughtSphere();
    SpawnTrigger();
}


float CollectingGame::RandomNumber0_1()
{
    return (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
}