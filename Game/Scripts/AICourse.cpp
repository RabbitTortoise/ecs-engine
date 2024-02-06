#include <Game/Scripts/AICourse.h>

#include <Engine/Interface/IECS.h>
#include <Engine/Interface/WindowManager.h>
#include <MatrixCalculations.h>
//Systems
#include <Engine/OpenGL/ECS/Systems/RenderSystem.h>
#include <Engine/OpenGL/ECS/Systems/MeshLoaderSystem.h>
#include <Engine/OpenGL/ECS/Systems/TextureLoaderSystem.h>
#include <Engine/OpenGL/ECS/Systems/ShaderLoaderSystem.h>
#include <Engine/OpenGL/ECS/Systems/CameraControllerSystem.h>
#include <Game/Systems/CollectorAgentSystem.h>
#include <Game/Systems/AttackerAgentSystem.h>
//Entities
#include <Engine/OpenGL/ECS/Entities/CameraEntity.h>
#include <Game/Entities/StaticSceneEntity.h>
#include <Game/Entities/TowerEntity.h>
#include <Game/Entities/CollectorEntity.h>
#include <Game/Entities/AttackerEntity.h>
#include <Game/Entities/GoldEntity.h>
//Components
#include <Game/Components/GameStatComponent.h>
#include <Game/Components/PlayerComponent.h>
#include <Game/Components/CarryComponent.h>

AICourse::AICourse()
{
    WorldEntityManager = IECS::GetEntityManager();
    WorldSystemsManager = IECS::GetSystemsManager();

    MeshLoader = static_cast<MeshLoaderSystem*>(WorldSystemsManager->GetSystemByName("MeshLoaderSystem"));
    TextureLoader = static_cast<TextureLoaderSystem*>(WorldSystemsManager->GetSystemByName("TextureLoaderSystem"));
    ShaderLoader = static_cast<ShaderLoaderSystem*>(WorldSystemsManager->GetSystemByName("ShaderLoaderSystem"));
    CameraController = static_cast<CameraControllerSystem*>(WorldSystemsManager->GetSystemByName("CameraControllerSystem"));

    CollectorSystem = nullptr;

    CursorPos.x = 0;
    CursorPos.y = 0;
    
    CursorID = -1;
    CursorTypeID = -1;
    DeltaTime = 0;

    LastTime = std::chrono::system_clock::now();


}

AICourse::~AICourse()
{

}

void AICourse::Init()
{

    //Create Component Types
    //WorldEntityManager->CreateComponentType<TriggerComponent>();
    WorldEntityManager->CreateComponentType<GameStatComponent>();
    WorldEntityManager->CreateComponentType<PlayerComponent>();
    WorldEntityManager->CreateComponentType<CarryComponent>();
    WorldEntityManager->CreateComponentType<AStarComponent>();

    //Create Entity Storages
    //WorldEntityManager->CreateEntityTypeStorage<MovingTriggerEntity>();
    WorldEntityManager->CreateEntityTypeStorage<StaticSceneEntity>();
    WorldEntityManager->CreateEntityTypeStorage<TowerEntity>();
    WorldEntityManager->CreateEntityTypeStorage<CollectorEntity>();
    WorldEntityManager->CreateEntityTypeStorage<AttackerEntity>();
    WorldEntityManager->CreateEntityTypeStorage<GoldEntity>();

    //Create Systems
    //Create CollectorAgentSystem
    CollectorSystem = new CollectorAgentSystem(WorldEntityManager, &GameManager);
    WorldSystemsManager->AddSystem(CollectorSystem);
    CollectorSystem->AddRequiredComponentType<GameStatComponent>();
    CollectorSystem->AddRequiredComponentType<PlayerComponent>();
    CollectorSystem->AddRequiredComponentType<CarryComponent>();
    CollectorSystem->AddEntityTypeIfCompatible<CollectorEntity>();

    //Create AttackerAgentSystem
    AttackerSystem = new AttackerAgentSystem(WorldEntityManager, &GameManager);
    WorldSystemsManager->AddSystem(AttackerSystem);
    AttackerSystem->AddRequiredComponentType<GameStatComponent>();
    AttackerSystem->AddRequiredComponentType<PlayerComponent>();
    AttackerSystem->AddRequiredComponentType<AStarComponent>();
    AttackerSystem->AddEntityTypeIfCompatible<AttackerEntity>();

    RenderSystem* Renderer = WorldSystemsManager->GetSystem<RenderSystem>();
    //static_cast<IComponentTypeQuerySystem*>(Renderer)->AddEntityTypeIfCompatible<MovingTriggerEntity>();
    static_cast<IComponentTypeQuerySystem*>(Renderer)->AddEntityTypeIfCompatible<StaticSceneEntity>();
    static_cast<IComponentTypeQuerySystem*>(Renderer)->AddEntityTypeIfCompatible<TowerEntity>();
    static_cast<IComponentTypeQuerySystem*>(Renderer)->AddEntityTypeIfCompatible<CollectorEntity>();
    static_cast<IComponentTypeQuerySystem*>(Renderer)->AddEntityTypeIfCompatible<AttackerEntity>();
    static_cast<IComponentTypeQuerySystem*>(Renderer)->AddEntityTypeIfCompatible<GoldEntity>();

    
    //Set main camera position.

    TransformComponent* cameraTransform = WorldEntityManager->GetComponent<TransformComponent>(CameraController->GetMainCamera());
    cameraTransform->Position.y = 20.0f;
    CameraController->SetPitchYaw(-89.9f,0);
    //CameraComponent* cameraComponent = WorldEntityManager->GetComponent<CameraComponent>(CameraController->GetMainCamera());
    //cameraComponent->Orientation
    //cameraTransform->EulerAngles = glm::eulerAngles(glm::quat(cameraTransform->EulerAngles) * glm::quat(glm::vec3(45, 0, 45)));



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


    //Cursor spawn and setup.
    Entity* cursorEntity = WorldEntityManager->CreateEntity<StaticSceneEntity>();
    CursorID = cursorEntity->GetID();
    CursorTypeID = cursorEntity->GetType();
    TransformComponent* transform = WorldEntityManager->GetComponent<TransformComponent>(cursorEntity);
    MaterialComponent* materialComponent = WorldEntityManager->GetComponent<MaterialComponent>(cursorEntity);
    MeshComponent* meshComponent = WorldEntityManager->GetComponent<MeshComponent>(cursorEntity);

    //Cursor
    if (transform != nullptr && materialComponent != nullptr && meshComponent != nullptr)
    {
    
        transform->Position.x = 0;
        transform->Position.y = 0;
        transform->Position.z = 0;

        transform->EulerAngles.x = 0;
        transform->EulerAngles.y = 0;
        transform->EulerAngles.z = 0;

        transform->Scale.x = GameManager.GetGridBlockDimensions() * 0.75f;
        transform->Scale.y = GameManager.GetGridBlockDimensions() * 0.75f;
        transform->Scale.z = GameManager.GetGridBlockDimensions() * 0.75f;

        materialComponent->ShaderID = ShaderLoader->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR_Instanced.vert", "../Engine/OpenGL/Shaders/PBR-Parallax.frag");
        materialComponent->Texture_Albedo = TextureLoader->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_albedo.png");
        materialComponent->Texture_AO = TextureLoader->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_ao.png");
        materialComponent->Texture_Depth = TextureLoader->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_depth.png");
        materialComponent->Texture_Normal = TextureLoader->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_normal-ogl.png");
        materialComponent->Texture_Roughness = TextureLoader->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_roughness.png");
        materialComponent->Texture_Metallic = TextureLoader->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_metallic.png");
    
        MeshComponent m = MeshLoader->GetPrimitiveShape(MeshLoaderSystem::PrimitiveShape::Cube);
        meshComponent->VAO = m.VAO;
        meshComponent->IndiceCount = m.IndiceCount;

    }
    CursorPos.x = 2;
    CursorPos.y = 2;

    SetCursorPos();

    GameManager.SetupGame();

    
}


void AICourse::Update(float deltaTime)
{
    DeltaTime = deltaTime;
    ReadInput();
    SetCursorPos();
    GameManager.UpdateGame(deltaTime);

}





void AICourse::ReadInput()
{

    bool constantPressed = false;

    //Stuff that need constat delay between presses
    if (CanConstantPress)
    {
        constantPressed = true;
    }
    if (constantPressed)
    {
        LastTime = std::chrono::system_clock::now();
        CanConstantPress = false;
    }
    float difference =(float) std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - LastTime).count();
    if (!CanConstantPress && difference > 0.1f)
    {
        CanConstantPress = true;
    }

    //Selection Cursor update
    if (glfwGetKey(WindowManager::GetGLFWWindow(), GLFW_KEY_UP) == GLFW_PRESS)
    {
        CursorPos.x += CursorSpeed * DeltaTime;
    }
    if (glfwGetKey(WindowManager::GetGLFWWindow(), GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        CursorPos.x -= CursorSpeed * DeltaTime;
    }
    if (glfwGetKey(WindowManager::GetGLFWWindow(), GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        CursorPos.y += CursorSpeed * DeltaTime;
    }
    if (glfwGetKey(WindowManager::GetGLFWWindow(), GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        CursorPos.y -= CursorSpeed * DeltaTime;
    }
    CursorPos = glm::clamp(CursorPos, 0.0f, (float)GameManager.GetGridDimensions() - 1);
    glm::normalize(CursorPos);

    if (!PressedNum1 && glfwGetKey(WindowManager::GetGLFWWindow(), GLFW_KEY_1) == GLFW_PRESS)
    {
        PressedNum1 = true;
        GameManager.SpawnCollector((int)std::floor(CursorPos.x), (int)std::floor(CursorPos.y), HumanPlayerIndex);

    }
    if (PressedNum1 && glfwGetKey(WindowManager::GetGLFWWindow(), GLFW_KEY_1) == GLFW_RELEASE)
    {
        PressedNum1 = false;
    }
    if (!PressedNum2 && glfwGetKey(WindowManager::GetGLFWWindow(), GLFW_KEY_2) == GLFW_PRESS)
    {
        PressedNum2 = true;
        GameManager.SpawnAttacker((int)std::floor(CursorPos.x), (int)std::floor(CursorPos.y), HumanPlayerIndex);
    }
    if (PressedNum2 && glfwGetKey(WindowManager::GetGLFWWindow(), GLFW_KEY_2) == GLFW_RELEASE)
    {
        PressedNum2 = false;
    }
    if (!PressedNum3 && glfwGetKey(WindowManager::GetGLFWWindow(), GLFW_KEY_3) == GLFW_PRESS)
    {
        PressedNum3 = true;
        GameManager.SpawnTower((int)std::floor(CursorPos.x), (int)std::floor(CursorPos.y), HumanPlayerIndex);

    }
    if (PressedNum3 && glfwGetKey(WindowManager::GetGLFWWindow(), GLFW_KEY_3) == GLFW_RELEASE)
    {
        PressedNum3 = false;
    }
}



void AICourse::SetCursorPos()
{
    StaticSceneEntity* CursorEntity = WorldEntityManager->GetEntity<StaticSceneEntity>(CursorID, CursorTypeID);

    TransformComponent* transform = WorldEntityManager->GetComponent<TransformComponent>(CursorEntity);
    transform->Position.x = (GameManager.GetGridDimensions() / -2.0f + CursorPos.x) * GameManager.GetGridBlockDimensions() * 2;
    transform->Position.y = 1 + GameManager.GetGridBlockDimensions() * 2.0f;
    transform->Position.z = (GameManager.GetGridDimensions() / -2.0f + CursorPos.y) * GameManager.GetGridBlockDimensions() * 2;

}


float AICourse::RandomNumber0_1()
{
    return (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
}