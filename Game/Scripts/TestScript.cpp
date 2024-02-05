#include <Game/Scripts/TestScript.h>

#include <Game/Systems/EntityRotateSystem.h>
#include <Engine/Interface/IECS.h>
#include <Game/Entities/DynamicSceneEntity.h>
#include <Game/Entities/StaticSceneEntity.h>
#include <Engine/OpenGL/ECS/Systems/RenderSystem.h>
#include <Engine/OpenGL/ECS/Systems/MeshLoaderSystem.h>
#include <Engine/OpenGL/ECS/Systems/TextureLoaderSystem.h>
#include <Engine/OpenGL/ECS/Systems/ShaderLoaderSystem.h>


TestScript::TestScript()
{
    PlayerShader = 0;
    WorldEntityManager = IECS::GetEntityManager();
    WorldSystemsManager = IECS::GetSystemsManager();
}

TestScript::~TestScript()
{
}

void TestScript::Init()
{
    //Create Entity Storages 
    WorldEntityManager->CreateEntityTypeStorage<DynamicSceneEntity>();
    WorldEntityManager->CreateEntityTypeStorage<StaticSceneEntity>();

    //Create Systems
    EntityRotateSystem* rotateSystem = new EntityRotateSystem(WorldEntityManager);
    WorldSystemsManager->AddSystem(rotateSystem);
    rotateSystem->AddRequiredComponentType<TransformComponent>();
    rotateSystem->AddEntityTypeIfCompatible<DynamicSceneEntity>();

    RenderSystem* Renderer = WorldSystemsManager->GetSystem<RenderSystem>();
    static_cast<IComponentTypeQuerySystem*>(Renderer)->AddEntityTypeIfCompatible<DynamicSceneEntity>();
    static_cast<IComponentTypeQuerySystem*>(Renderer)->AddEntityTypeIfCompatible<StaticSceneEntity>();

    MeshLoaderSystem* meshLoaderSystem = WorldSystemsManager->GetSystem<MeshLoaderSystem>();
    TextureLoaderSystem* textureLoaderSystem = WorldSystemsManager->GetSystem<TextureLoaderSystem>();
    ShaderLoaderSystem* shaderLoaderSystem = WorldSystemsManager->GetSystem<ShaderLoaderSystem>();

    //Rotating torus spawn and setup

    int dimX = 25;
    int dimY = 25;
    int dimZ = 2;

    /*dimX = 1;
    dimY = 1;
    dimZ = 1;*/
    for (int i = 0; i < dimX; i++)
    {
        for (int i2 = 0; i2 < dimY; i2++)
        {
            for (int i3 = 0; i3 < dimZ; i3++)
            {
                Entity* dynamicEntity = WorldEntityManager->CreateEntity<DynamicSceneEntity>();
                if (dynamicEntity != nullptr)
                {

                    TransformComponent* transform = WorldEntityManager->GetComponent<TransformComponent>(dynamicEntity);
                    MaterialComponent* materialComponent = WorldEntityManager->GetComponent<MaterialComponent>(dynamicEntity);
                    MeshComponent* meshComponent = WorldEntityManager->GetComponent<MeshComponent>(dynamicEntity);

                    if (materialComponent != nullptr && meshComponent != nullptr && transform != nullptr)
                    {
                        //Transform
                        transform->Position.x = dimX * -1.0f + i * 2.0f;
                        transform->Position.y = 26.0f + dimX * -1.0f + i2 * 2.0f;
                        transform->Position.z = dimZ * -1.0f + i3 * 2.0f + 10;

                        transform->EulerAngles.x = 0;
                        transform->EulerAngles.y = 0;
                        transform->EulerAngles.z = 0;
                        transform->Scale.x = 0.0075f;
                        transform->Scale.y = 0.0075f;
                        transform->Scale.z = 0.0075f;

                        //Material
                        materialComponent->ShaderID = shaderLoaderSystem->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR_Instanced.vert", "../Engine/OpenGL/Shaders/PBR.frag");
                        materialComponent->Texture_Albedo = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-albedo.png");
                        materialComponent->Texture_AO = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-ao.png");
                        materialComponent->Texture_Depth = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Depth.png");
                        materialComponent->Texture_Metallic = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Metallic.png");
                        materialComponent->Texture_Normal = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Normal-ogl.png");
                        materialComponent->Texture_Roughness= textureLoaderSystem->LoadTextureFromFile("../Game/Assets/worn-shiny-metal-bl/worn-shiny-metal-Roughness.png");
                        materialComponent->TextureRepeat= 1.0f;
                        materialComponent->ParallaxScale= 0.0f;
                        materialComponent->IBLEnabled= 1;

                        //Mesh
                        std::vector<int> meshIDs = meshLoaderSystem->LoadMeshesFromFile("../Game/Assets/torus.obj");
                        MeshLoaderSystem::MeshHandles* mesh = meshLoaderSystem->GetMeshHandles(meshIDs[0]);
                        meshComponent->IndiceCount = mesh->IndiceCount;
                        meshComponent->VAO = mesh->VAO;
                    }
                }
            }
        }
    }

    //Quad spawn and setup.
    Entity*  staticEntity = WorldEntityManager->CreateEntity<StaticSceneEntity>();
    TransformComponent*  transformStatic = WorldEntityManager->GetComponent<TransformComponent>(staticEntity);
    MaterialComponent*  materialComponentStatic = WorldEntityManager->GetComponent<MaterialComponent>(staticEntity);
    MeshComponent* meshComponentStatic = WorldEntityManager->GetComponent<MeshComponent>(staticEntity);

    if (transformStatic != nullptr && materialComponentStatic != nullptr && meshComponentStatic != nullptr)
    {
        transformStatic->Position.x = 0;
        transformStatic->Position.y = 2;
        transformStatic->Position.z = 5;

        transformStatic->EulerAngles.x = 0;
        transformStatic->EulerAngles.y = 0;
        transformStatic->EulerAngles.z = 0;

        transformStatic->Scale.x = 1.0f;
        transformStatic->Scale.y = 1.0f;
        transformStatic->Scale.z = 1.0f;

        materialComponentStatic->ShaderID =shaderLoaderSystem->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR_Instanced.vert", "../Engine/OpenGL/Shaders/PBR-Parallax.frag");
        materialComponentStatic->Texture_Albedo =textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_albedo.png");
        materialComponentStatic->Texture_AO =textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_ao.png");
        materialComponentStatic->Texture_Depth =textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_depth.png");
        materialComponentStatic->Texture_Normal =textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_normal-ogl.png");
        materialComponentStatic->Texture_Roughness=textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_roughness.png");
        materialComponentStatic->Texture_Metallic =textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_metallic.png");
        materialComponentStatic->TextureRepeat=2.0f;
        materialComponentStatic->ParallaxScale=0.1f;
        materialComponentStatic->IBLEnabled=1;

        MeshComponent mesh = meshLoaderSystem->GetPrimitiveShape(MeshLoaderSystem::PrimitiveShape::Quad);
        meshComponentStatic->IndiceCount = mesh.IndiceCount;
        meshComponentStatic->VAO = mesh.VAO;
    }

    //Second Quad spawn and setup.
    staticEntity = WorldEntityManager->CreateEntity<StaticSceneEntity>();
    transformStatic = WorldEntityManager->GetComponent<TransformComponent>(staticEntity);
    materialComponentStatic = WorldEntityManager->GetComponent<MaterialComponent>(staticEntity);
    meshComponentStatic = WorldEntityManager->GetComponent<MeshComponent>(staticEntity);

    if (transformStatic != nullptr && materialComponentStatic != nullptr && meshComponentStatic != nullptr)
    {
        transformStatic->Position.x = 0;
        transformStatic->Position.y = 2;
        transformStatic->Position.z = 5;

        transformStatic->EulerAngles.x = 0;
        transformStatic->EulerAngles.y = glm::pi<float>();
        transformStatic->EulerAngles.z = 0;

        transformStatic->Scale.x = 1.0f;
        transformStatic->Scale.y = 1.0f;
        transformStatic->Scale.z = 1.0f;

        materialComponentStatic->ShaderID =shaderLoaderSystem->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR_Instanced.vert", "../Engine/OpenGL/Shaders/PBR-Parallax.frag");
        materialComponentStatic->Texture_Albedo =textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_albedo.png");
        materialComponentStatic->Texture_AO =textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_ao.png");
        materialComponentStatic->Texture_Depth =textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_depth.png");
        materialComponentStatic->Texture_Normal =textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_normal-ogl.png");
        materialComponentStatic->Texture_Roughness=textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_roughness.png");
        materialComponentStatic->Texture_Metallic =textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_metallic.png");
        materialComponentStatic->TextureRepeat=1.0f;
        materialComponentStatic->ParallaxScale=0.1f;
        materialComponentStatic->IBLEnabled=1;

        MeshComponent mesh = meshLoaderSystem->GetPrimitiveShape(MeshLoaderSystem::PrimitiveShape::Quad);
        meshComponentStatic->IndiceCount = mesh.IndiceCount;
        meshComponentStatic->VAO = mesh.VAO;
    }
}

void TestScript::Update(float deltaTime)
{

}





//WorldEntityManager->CreateEntityTypeStorage<StaticSceneEntity>("StaticSceneEntity", StaticSceneEntity::GetComponentTypeNames());
//static_cast<IComponentTypeQuerySystem*>(Renderer)->AddEntityTypeIfCompatible(WorldEntityManager->GetEntityTypeIdByName("StaticSceneEntity"), StaticSceneEntity::GetComponentTypeNames());


//ECS TESTING
   /*
   //Ground spawn and setup.
   Entity* staticEntity = WorldEntityManager->CreateEntity<StaticSceneEntity>("StaticSceneEntity");
   TransformComponent* transformStatic = staticEntity->GetComponent<TransformComponent>(WorldEntityManager->GetComponentTypeIdByName("TransformComponent"));
   MaterialComponent* materialComponentStatic = staticEntity->GetComponent<MaterialComponent>(WorldEntityManager->GetComponentTypeIdByName("MaterialComponent"));
   MeshComponent* meshComponentStatic = staticEntity->GetComponent<MeshComponent>(WorldEntityManager->GetComponentTypeIdByName("MeshComponent"));

   if (transformStatic != nullptr && materialComponentStatic != nullptr && meshComponentStatic != nullptr)
   {
       transformStatic->Position.x = 0;
       transformStatic->Position.y = -1.5;
       transformStatic->Position.z = 0;

       transformStatic->Rotation.x = 0;
       transformStatic->Rotation.y = 0;
       transformStatic->Rotation.z = 0;

       transformStatic->Scale.x = 0.5f;
       transformStatic->Scale.y = 0.2f;
       transformStatic->Scale.z = 0.5f;

       materialComponentStatic->ShaderID = shaderLoaderSystem->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR.vert", "../Engine/OpenGL/Shaders/PBR-Parallax.frag"));
       materialComponentStatic->Texture_Albedo = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/grass1-bl/grass1-albedo3.png"));
       materialComponentStatic->Texture_AO = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/grass1-bl/grass1-ao.png"));
       materialComponentStatic->Texture_Depth = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/grass1-bl/grass1-depth.png"));
       materialComponentStatic->Texture_Normal = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/grass1-bl/grass1-normal1-ogl.png"));
       materialComponentStatic->Texture_Roughness= textureLoaderSystem->LoadTextureFromFile("../Game/Assets/grass1-bl/grass1-rough.png"));
       materialComponentStatic->Texture_Metallic = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/grass1-bl/grass1-metallic.png"));
       materialComponentStatic->TextureRepeat= 20.0f);
       materialComponentStatic->ParallaxScale= 0.0f);
       materialComponentStatic->IBLEnabled= 1);

       std::vector<int> meshIDs = meshLoaderSystem->LoadMeshesFromFile("../Game/Assets/Ground.obj");
       MeshLoaderSystem::MeshHandles* mesh = meshLoaderSystem->GetMeshHandles(meshIDs[0]);
       meshComponentStatic->IndiceCount = mesh->IndiceCount;
       meshComponentStatic->VAO = mesh->VAO;

   }

   //Quad spawn and setup.
   staticEntity = WorldEntityManager->CreateEntity<StaticSceneEntity>("StaticSceneEntity");
   transformStatic = staticEntity->GetComponent<TransformComponent>(WorldEntityManager->GetComponentTypeIdByName("TransformComponent"));
   materialComponentStatic = staticEntity->GetComponent<MaterialComponent>(WorldEntityManager->GetComponentTypeIdByName("MaterialComponent"));
   meshComponentStatic = staticEntity->GetComponent<MeshComponent>(WorldEntityManager->GetComponentTypeIdByName("MeshComponent"));

   if (transformStatic != nullptr && materialComponentStatic != nullptr && meshComponentStatic != nullptr)
   {
       transformStatic->Position.x = 0;
       transformStatic->Position.y = 0;
       transformStatic->Position.z = -5;

       transformStatic->Rotation.x = 0;
       transformStatic->Rotation.y = 0;
       transformStatic->Rotation.z = 0;

       transformStatic->Scale.x = 1.0f;
       transformStatic->Scale.y = 1.0f;
       transformStatic->Scale.z = 1.0f;

       materialComponentStatic->ShaderID = shaderLoaderSystem->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR.vert", "../Engine/OpenGL/Shaders/PBR-Parallax.frag"));
       materialComponentStatic->Texture_Albedo = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_albedo.png"));
       materialComponentStatic->Texture_AO = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_ao.png"));
       materialComponentStatic->Texture_Depth = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_depth.png"));
       materialComponentStatic->Texture_Normal = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_normal-ogl.png"));
       materialComponentStatic->Texture_Roughness= textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_roughness.png"));
       materialComponentStatic->Texture_Metallic = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_metallic.png"));
       materialComponentStatic->TextureRepeat= 1.0f);
       materialComponentStatic->ParallaxScale= 0.1f);
       materialComponentStatic->IBLEnabled= 1);

       MeshComponent mesh = meshLoaderSystem->GetPrimitiveShape(MeshLoaderSystem::PrimitiveShape::Quad);
       meshComponentStatic->IndiceCount = mesh.IndiceCount;
       meshComponentStatic->VAO = mesh.VAO;
   }


   //Quad spawn and setup.
   staticEntity = WorldEntityManager->CreateEntity<StaticSceneEntity>("StaticSceneEntity");
   transformStatic = staticEntity->GetComponent<TransformComponent>(WorldEntityManager->GetComponentTypeIdByName("TransformComponent"));
   materialComponentStatic = staticEntity->GetComponent<MaterialComponent>(WorldEntityManager->GetComponentTypeIdByName("MaterialComponent"));
   meshComponentStatic = staticEntity->GetComponent<MeshComponent>(WorldEntityManager->GetComponentTypeIdByName("MeshComponent"));

   if (transformStatic != nullptr && materialComponentStatic != nullptr && meshComponentStatic != nullptr)
   {
       transformStatic->Position.x = 0;
       transformStatic->Position.y = 0;
       transformStatic->Position.z = -5;

       transformStatic->Rotation.x = 0;
       transformStatic->Rotation.y = glm::pi<float>();
       transformStatic->Rotation.z = 0;

       transformStatic->Scale.x = 1.0f;
       transformStatic->Scale.y = 1.0f;
       transformStatic->Scale.z = 1.0f;

       materialComponentStatic->ShaderID = shaderLoaderSystem->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR.vert", "../Engine/OpenGL/Shaders/PBR-Parallax.frag"));
       materialComponentStatic->Texture_Albedo = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_albedo.png"));
       materialComponentStatic->Texture_AO = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_ao.png"));
       materialComponentStatic->Texture_Depth = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_depth.png"));
       materialComponentStatic->Texture_Normal = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_normal-ogl.png"));
       materialComponentStatic->Texture_Roughness= textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_roughness.png"));
       materialComponentStatic->Texture_Metallic = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_metallic.png"));
       materialComponentStatic->TextureRepeat= 1.0f);
       materialComponentStatic->ParallaxScale= 0.1f);
       materialComponentStatic->IBLEnabled= 1);

       MeshComponent mesh = meshLoaderSystem->GetPrimitiveShape(MeshLoaderSystem::PrimitiveShape::Quad);
       meshComponentStatic->IndiceCount = mesh.IndiceCount;
       meshComponentStatic->VAO = mesh.VAO;
   }
   */

/*
                       else if (i == 1)
                       {
                           materialComponent->ShaderID = shaderLoaderSystem->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR.vert", "../Engine/OpenGL/Shaders/PBR-Parallax.frag"));
                           materialComponent->Texture_Albedo = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/dark-rough-rock1-bl/dark_rough_rock1_albedo.png"));
                           materialComponent->Texture_AO = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/dark-rough-rock1-bl/dark_rough_rock1_ao.png"));
                           materialComponent->Texture_Depth = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/dark-rough-rock1-bl/dark_rough_rock1_Depth.png"));
                           materialComponent->Texture_Metallic = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/dark-rough-rock1-bl/dark_rough_rock1_Metallic.png"));
                           materialComponent->Texture_Normal = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/dark-rough-rock1-bl/dark_rough_rock1_Normal-ogl.png"));
                           materialComponent->Texture_Roughness= textureLoaderSystem->LoadTextureFromFile("../Game/Assets/dark-rough-rock1-bl/dark_rough_rock1_Roughness.png"));
                           materialComponent->TextureRepeat= 1.0f);
                           materialComponent->ParallaxScale= 0.0f);
                           materialComponent->IBLEnabled= 1);


                       }
                       else
                       {
                           materialComponent->ShaderID = shaderLoaderSystem->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR.vert", "../Engine/OpenGL/Shaders/PBR-Parallax.frag"));
                           materialComponent->Texture_Albedo = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_albedo.png"));
                           materialComponent->Texture_AO = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_ao.png"));
                           materialComponent->Texture_Depth = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_depth.png"));
                           materialComponent->Texture_Normal = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_normal-ogl.png"));
                           materialComponent->Texture_Roughness= textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_roughness.png"));
                           materialComponent->Texture_Metallic = textureLoaderSystem->LoadTextureFromFile("../Game/Assets/fancy-brass-pattern1-bl/fancy-brass-pattern1_metallic.png"));
                           materialComponent->TextureRepeat= 1.0f);
                           materialComponent->ParallaxScale= 0.0f);
                           materialComponent->IBLEnabled= 1);
                       }
                       */
                       /*
                       if (i2 == 2)
                       {
                           transform->Scale.x = 0.5f;
                           transform->Scale.y = 0.5f;
                           transform->Scale.z = 0.5f;

                           MeshComponent m = meshLoaderSystem->GetPrimitiveShape(MeshLoaderSystem::PrimitiveShape::Sphere);
                           meshComponent->VAO = m.VAO;
                           meshComponent->IndiceCount = m.IndiceCount;
                       }
                       else if (i2 == 1)
                       {
                           transform->Scale.x = 0.02f;
                           transform->Scale.y = 0.02f;
                           transform->Scale.z = 0.02f;

                           std::vector<int> meshIDs = meshLoaderSystem->LoadMeshesFromFile("../Game/Assets/torusknot.obj");
                           MeshLoaderSystem::MeshHandles* mesh = meshLoaderSystem->GetMeshHandles(meshIDs[0]);
                           meshComponent->IndiceCount = mesh->IndiceCount;
                           meshComponent->VAO = mesh->VAO;
                       }

                       (i2 == 0)
                       
if (true)
{

}
else
{
    transform->Scale.x = 0.0075f;
    transform->Scale.y = 0.0075f;
    transform->Scale.z = 0.0075f;

    std::vector<int> meshIDs = meshLoaderSystem->LoadMeshesFromFile("../Game/Assets/torus.obj");
    MeshLoaderSystem::MeshHandles* mesh = meshLoaderSystem->GetMeshHandles(meshIDs[0]);
    meshComponent->IndiceCount = mesh->IndiceCount;
    meshComponent->VAO = mesh->VAO;
}
*/