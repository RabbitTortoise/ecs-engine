#include <Game/Scripts/AICourseGameManager.h>

#include <Engine/Interface/IECS.h>
#include <Engine/Interface/WindowManager.h>
#include <MatrixCalculations.h>
//Systems
#include <Engine/OpenGL/ECS/Systems/RenderSystem.h>
#include <Engine/OpenGL/ECS/Systems/MeshLoaderSystem.h>
#include <Engine/OpenGL/ECS/Systems/TextureLoaderSystem.h>
#include <Engine/OpenGL/ECS/Systems/ShaderLoaderSystem.h>
#include <Engine/OpenGL/ECS/Systems/CameraControllerSystem.h>
#include <Game/Systems/AttackerAgentSystem.h>
#include <Game/Systems/CollectorAgentSystem.h>
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


AICourseGameManager::AICourseGameManager()
{
    WorldEntityManager = IECS::GetEntityManager();
    WorldSystemsManager = IECS::GetSystemsManager();

    MeshLoader = static_cast<MeshLoaderSystem*>(WorldSystemsManager->GetSystemByName("MeshLoaderSystem"));
    TextureLoader = static_cast<TextureLoaderSystem*>(WorldSystemsManager->GetSystemByName("TextureLoaderSystem"));
    ShaderLoader = static_cast<ShaderLoaderSystem*>(WorldSystemsManager->GetSystemByName("ShaderLoaderSystem"));
    CameraController = static_cast<CameraControllerSystem*>(WorldSystemsManager->GetSystemByName("CameraControllerSystem"));

    //Default Materials for players
    HumanPlayerMaterial.ShaderID = ShaderLoader->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR_Instanced.vert", "../Engine/OpenGL/Shaders/PBR.frag");
    HumanPlayerMaterial.Texture_Albedo = TextureLoader->LoadTextureFromFile("../Game/Assets/ridged-foam1-bl/ridged-foam1_albedo.png");
    HumanPlayerMaterial.Texture_AO = TextureLoader->LoadTextureFromFile("../Game/Assets/ridged-foam1-bl/ridged-foam1_ao.png");
    HumanPlayerMaterial.Texture_Depth = TextureLoader->LoadTextureFromFile("../Game/Assets/ridged-foam1-bl/ridged-foam1_height.png");
    HumanPlayerMaterial.Texture_Metallic = TextureLoader->LoadTextureFromFile("../Game/Assets/ridged-foam1-bl/ridged-foam1_metallic.png");
    HumanPlayerMaterial.Texture_Normal = TextureLoader->LoadTextureFromFile("../Game/Assets/ridged-foam1-bl/ridged-foam1_normal-ogl.png");
    HumanPlayerMaterial.Texture_Roughness = TextureLoader->LoadTextureFromFile("../Game/Assets/ridged-foam1-bl/ridged-foam1_roughness.png");
    HumanPlayerMaterial.TextureRepeat = 1.0f;
    HumanPlayerMaterial.ParallaxScale = 0.0f;
    HumanPlayerMaterial.IBLEnabled = 1;
    
    AIPlayerMaterial.ShaderID = ShaderLoader->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR_Instanced.vert", "../Engine/OpenGL/Shaders/PBR.frag");
    AIPlayerMaterial.Texture_Albedo = TextureLoader->LoadTextureFromFile("../Game/Assets/lava-and-rock-bl/lava-and-rock_albedo.png");
    AIPlayerMaterial.Texture_AO = TextureLoader->LoadTextureFromFile("../Game/Assets/lava-and-rock-bl/lava-and-rock_ao.png");
    AIPlayerMaterial.Texture_Depth = TextureLoader->LoadTextureFromFile("../Game/Assets/lava-and-rock-bl/lava-and-rock_height.png");
    AIPlayerMaterial.Texture_Metallic = TextureLoader->LoadTextureFromFile("../Game/Assets/lava-and-rock-bl/lava-and-rock_metallic.png");
    AIPlayerMaterial.Texture_Normal = TextureLoader->LoadTextureFromFile("../Game/Assets/lava-and-rock-bl/lava-and-rock_normal-ogl.png");
    AIPlayerMaterial.Texture_Roughness = TextureLoader->LoadTextureFromFile("../Game/Assets/lava-and-rock-bl/lava-and-rock_roughness.png");
    AIPlayerMaterial.TextureRepeat = 1.0f;
    AIPlayerMaterial.ParallaxScale = 0.0f;
    AIPlayerMaterial.IBLEnabled = 1;

}

AICourseGameManager::~AICourseGameManager()
{

}

void AICourseGameManager::SetupGame()
{
    CreateBaseGrid(GridDimensions);
    GamePlayers.push_back(PlayerStats{ StartingGold, -1 });
    GamePlayers.push_back(PlayerStats{ StartingGold, -1 });
    int towerID = SpawnTower(0, 0, HumanPlayerIndex);
    GamePlayers[HumanPlayerIndex].MainTowerID = towerID;
    towerID = SpawnTower(GridDimensions-1, GridDimensions-1, AIPlayerIndex);
    GamePlayers[AIPlayerIndex].MainTowerID = towerID;

    //Gold spawning

    int goldAmount = (int)(GridDimensions * GridDimensions / 3.0f);

    for (size_t i = 0; i < goldAmount; i++)
    {
        int x = (int)(RandomNumber0_1() * (GridDimensions - 2) + 1);
        int y = (int)(RandomNumber0_1() * (GridDimensions - 2) + 1);

        SpawnGold(x, y);
    }



    //Collector spawning
    SpawnCollector(1, 1, HumanPlayerIndex);
    SpawnCollector(GridDimensions - 2, GridDimensions - 2, AIPlayerIndex);
    SetupAI();

    std::cout << std::endl << "This is the AI demo game" << std::endl;
    std::cout << "Use arrow keys to move the cursor" << std::endl;
    std::cout << "Press 1. To make a gold collector" << std::endl;
    std::cout << "Press 2. To make an attack drone" << std::endl;
    std::cout << "Press 3. To make a Tower" << std::endl << std::endl;
    std::cout << "Towers spawn at cursor if cursor is close enough to other tower" << std::endl;
    std::cout << "Collectors and drones spawn at the closest tower" << std::endl;
    std::cout << "Drones attack the closest tower" << std::endl << std::endl;;


}

void AICourseGameManager::SetupAI()
{
    AIModifiers.MaxGoldDistance = 3.0f + RandomNumber0_1() * 3.0f;
    AIModifiers.AttackerPreference = 0.5f + RandomNumber0_1() * 1.0f;
    AIModifiers.CollectorPreference = 0.5f + RandomNumber0_1() * 1.0f;
    AIModifiers.TowerPreference = 0.5f + RandomNumber0_1() * 1.0f;

    AIModifiers.AttackerMultiplier = 0.9f + RandomNumber0_1() * 0.09f;
    AIModifiers.CollectorMultiplier = 0.9f + RandomNumber0_1() * 0.09f;
    AIModifiers.TowerAmountMultiplier = 0.9f + RandomNumber0_1() * 0.09f;
}

void AICourseGameManager::UpdateGame(float deltaTime)
{
    UpdateAI(deltaTime);
    CheckForGoldSpawn(deltaTime);

    if (GamePlayers[HumanPlayerIndex].towerPositions.size() < 1)
    {
        std::cout << "GAME OVER!! YOU LOST!" << std::endl;
    }

    
    if (GamePlayers[AIPlayerIndex].towerPositions.size() < 1)
    {
        std::cout << "YOU WON THE GAME!!" << std::endl;
    }


}

void AICourseGameManager::CheckForGoldSpawn(float deltaTime)
{
    GoldSpawnTickCount += deltaTime;
    if (GoldSpawnTickCount < NextGoldSpawnTick)
    {
        return;
    }
    GoldSpawnTickCount -= GoldSpawnTickDelay;
    NextGoldSpawnTick = GoldSpawnTickDelay;
    int x = (int)(RandomNumber0_1() * (GridDimensions - 2) + 1);
    int y = (int)(RandomNumber0_1() * (GridDimensions - 2) + 1);

    SpawnGold(x, y);
}

void AICourseGameManager::UpdateAI(float deltaTime)
{
    AITickCount += deltaTime;
    if (AITickCount < NextAITick)
    {
        return;
    }
    AITickCount -= AITickDelay;
    NextAITick = AITickDelay;

    int gold = GamePlayers[AIPlayerIndex].Gold;

    bool canAffordTower = TowerGoldCost <= gold;
    bool canAffordAttacker = TowerGoldCost <= gold;
    bool canAffordCollector = TowerGoldCost <= gold;

    int towerCount = FriendlyTowerCount(AIPlayerIndex);
    int collectorCount = FriendlyCollectorCount(AIPlayerIndex);
    int attackerCount = FriendlyAttackerCount(AIPlayerIndex) + 0.1f * towerCount + 0.1f * collectorCount;

    float towerBuyChance = 1.0f * AIModifiers.TowerPreference * (std::pow(AIModifiers.TowerAmountMultiplier, towerCount));
    float collectorBuyChance = 1.0f * AIModifiers.CollectorPreference * (std::pow(AIModifiers.CollectorMultiplier, collectorCount));
    float attackerBuyChance = 1.0f * AIModifiers.AttackerPreference * (std::pow(AIModifiers.AttackerMultiplier, attackerCount));

    towerBuyChance += RandomNumber0_1() * 0.5f;
    collectorBuyChance += RandomNumber0_1() * 0.5f;
    attackerBuyChance += RandomNumber0_1() * 0.5f;

    //Find the nearest gold
    float minDistance = 1000.0f;
    glm::ivec2 towerCoords;
    int nearestGoldID = 0;
    bool towerFound = false;

    if (GamePlayers[AIPlayerIndex].towerPositions.size() > 0)
    {
        towerFound = true;
        towerCoords = GamePlayers[AIPlayerIndex].towerPositions[0];
        for (auto position : GamePlayers[AIPlayerIndex].towerPositions)
        {
            float distance = 0;
            float id = FindNearestGold(position, distance);

            if (distance < minDistance)
            {
                towerCoords = position;
                minDistance = distance;
                nearestGoldID = id;
            }
        }
    }
   

    if (collectorCount < towerCount * 2.0f)
    {
        collectorBuyChance += RandomNumber0_1() * 0.5f;
    }


    if (minDistance > AIModifiers.MaxGoldDistance)
    {
        towerBuyChance += 10.0f;
    }
    
    
    if (towerBuyChance > collectorBuyChance && towerBuyChance > attackerBuyChance)
    {
        //Buy Tower
        if (canAffordTower && towerFound)
        {
            GoldEntity* gold = WorldEntityManager->GetEntity<GoldEntity>(nearestGoldID);
            if (gold)
            {
                glm::vec2 dir = gold->Stats.GridPosition - towerCoords;
                dir = glm::normalize(dir);
                glm::vec2 pos = glm::vec2(gold->Stats.GridPosition) + (dir * -1.0f);
                pos = glm::clamp(pos, 0.0f, (float)GridDimensions - 1);


                if (PlacementMap[pos.x][pos.y])
                {
                    SpawnTower(pos.x, pos.y, AIPlayerIndex, true);
                }
                else
                {
                    pos = glm::vec2(gold->Stats.GridPosition) + (dir * -2.0f);
                    pos = glm::clamp(pos, 0.0f, (float)GridDimensions - 1);
                    if (PlacementMap[pos.x][pos.y])
                    {
                        SpawnTower(pos.x, pos.y, AIPlayerIndex, true);
                    }
                }
            }
        }
    }
    else if (collectorBuyChance > attackerBuyChance)
    {
        //Buy Collector
        if (canAffordCollector && towerFound)
        {
            SpawnCollector(towerCoords.x, towerCoords.y, AIPlayerIndex);
        }
    }
    else
    {
        //Buy Attacker
        if (canAffordAttacker && towerFound)
        {
            SpawnAttacker(towerCoords.x, towerCoords.y, AIPlayerIndex);
        }
    }
    
}

void AICourseGameManager::CreateBaseGrid(int gridDimension)
{
    BaseGrid.reserve(gridDimension);
    PathFindingMap.reserve(gridDimension);
    PlacementMap.reserve(gridDimension);
    for (size_t row = 0; row < gridDimension; row++)
    {
        std::vector<int> n;
        BaseGrid.push_back(n);
        BaseGrid[row].reserve(gridDimension);

        std::vector<bool> m;
        PathFindingMap.push_back(m);
        PathFindingMap[row].reserve(gridDimension);

        PlacementMap.push_back(m);
        PlacementMap[row].reserve(gridDimension);

        for (size_t i = 0; i < gridDimension; i++)
        {
            BaseGrid[row].push_back(0);
            PathFindingMap[row].push_back(true);
            PlacementMap[row].push_back(true);
        }
    }

    int x = 0;
    for (auto GridRow : BaseGrid)
    {
        int y = 0;
        for (auto block : GridRow)
        {
            SpawnGridBlock(x, y);
            y++;
        }
        x++;
    }
}


bool AICourseGameManager::SpawnGridBlock(int gridX, int gridY)
{
    //Ground spawn and setup.
    Entity* staticEntity = WorldEntityManager->CreateEntity<StaticSceneEntity>();

    TransformComponent* transformStatic = WorldEntityManager->GetComponent<TransformComponent>(staticEntity);
    MaterialComponent* materialComponentStatic = WorldEntityManager->GetComponent<MaterialComponent>(staticEntity);
    MeshComponent* meshComponentStatic = WorldEntityManager->GetComponent<MeshComponent>(staticEntity);

    if (!transformStatic || !materialComponentStatic || !meshComponentStatic)
    {
        return false;
    }

    transformStatic->Scale.x = GridBlockDimensions;
    transformStatic->Scale.y = GridBlockDimensions;
    transformStatic->Scale.z = GridBlockDimensions;

    transformStatic->Position.x = (GridDimensions / -2.0f + gridX) * GridBlockDimensions * 2;
    transformStatic->Position.y = 1;
    transformStatic->Position.z = (GridDimensions / -2.0f + gridY) * GridBlockDimensions * 2;

    transformStatic->UseQuaternion = true;
    transformStatic->Orientation = glm::quat(glm::vec3(glm::pi<float>() / 2.0f, 0, 0));

    materialComponentStatic->ShaderID = ShaderLoader->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR_Instanced.vert", "../Engine/OpenGL/Shaders/PBR.frag");
    materialComponentStatic->Texture_Albedo = TextureLoader->LoadTextureFromFile("../Game/Assets/rock-snow-ice1-2k-bl/rock-snow-ice1-2k_Base_Color.png");
    materialComponentStatic->Texture_AO = TextureLoader->LoadTextureFromFile("../Game/Assets/rock-snow-ice1-2k-bl/rock-snow-ice1-2k_Ambient_Occlusion.png");
    materialComponentStatic->Texture_Depth = TextureLoader->LoadTextureFromFile("../Game/Assets/rock-snow-ice1-2k-bl/rock-snow-ice1-2k_Height.png");
    materialComponentStatic->Texture_Metallic = TextureLoader->LoadTextureFromFile("../Game/Assets/rock-snow-ice1-2k-bl/rock-snow-ice1-2k_Metallic.png");
    materialComponentStatic->Texture_Normal = TextureLoader->LoadTextureFromFile("../Game/Assets/rock-snow-ice1-2k-bl/rock-snow-ice1-2k_Normal-ogl.png");
    materialComponentStatic->Texture_Roughness = TextureLoader->LoadTextureFromFile("../Game/Assets/rock-snow-ice1-2k-bl/rock-snow-ice1-2k_Roughness.png");
    materialComponentStatic->TextureRepeat = 1.0f;
    materialComponentStatic->ParallaxScale = 0.0f;
    materialComponentStatic->IBLEnabled = 1;

    MeshComponent m = MeshLoader->GetPrimitiveShape(MeshLoaderSystem::PrimitiveShape::Quad);
    meshComponentStatic->VAO = m.VAO;
    meshComponentStatic->IndiceCount = m.IndiceCount;

    return true;
}

void AICourseGameManager::SpawnGold(int gridX, int gridY)
{
    if (!PlacementMap[gridX][gridY])
    {
        return;
    }

    //Cursor spawn and setup.
    GoldEntity* goldEntity = WorldEntityManager->CreateEntity<GoldEntity>();

    //Cursor
    if(!goldEntity)
    {
        return;
    }


    goldEntity->Transform.Position.x = (GridDimensions / -2.0f + gridX) * GridBlockDimensions * 2;
    goldEntity->Transform.Position.y = 1 + GridBlockDimensions;
    goldEntity->Transform.Position.z = (GridDimensions / -2.0f + gridY) * GridBlockDimensions * 2;

    goldEntity->Stats.Health = 60;
    goldEntity->Stats.GridPosition.x = gridX;
    goldEntity->Stats.GridPosition.y = gridY;

    goldEntity->Transform.EulerAngles.x = 0;
    goldEntity->Transform.EulerAngles.y = 0;
    goldEntity->Transform.EulerAngles.z = 0;
    goldEntity->Transform.Scale.x = GridBlockDimensions;
    goldEntity->Transform.Scale.y = GridBlockDimensions;
    goldEntity->Transform.Scale.z = GridBlockDimensions;

    goldEntity->Material.ShaderID = ShaderLoader->LoadShaderFromFile("../Engine/OpenGL/Shaders/PBR_Instanced.vert", "../Engine/OpenGL/Shaders/PBR-Parallax.frag");
    goldEntity->Material.Texture_Albedo = TextureLoader->LoadTextureFromFile("../Game/Assets/gold-scuffed-bl/gold-scuffed_basecolor-boosted.png");
    goldEntity->Material.Texture_AO = TextureLoader->LoadTextureFromFile("../Game/Assets/gold-scuffed-bl/gold-scuffed_basecolor.png");
    goldEntity->Material.Texture_Depth = TextureLoader->LoadTextureFromFile("../Game/Assets/gold-scuffed-bl/gold-scuffed_roughness.png");
    goldEntity->Material.Texture_Normal = TextureLoader->LoadTextureFromFile("../Game/Assets/gold-scuffed-bl/gold-scuffed_normal.png");
    goldEntity->Material.Texture_Roughness = TextureLoader->LoadTextureFromFile("../Game/Assets/gold-scuffed-bl/gold-scuffed_roughness.png");
    goldEntity->Material.Texture_Metallic = TextureLoader->LoadTextureFromFile("../Game/Assets/gold-scuffed-bl/gold-scuffed_metallic.png");

    MeshComponent m = MeshLoader->GetPrimitiveShape(MeshLoaderSystem::PrimitiveShape::Cube);
    goldEntity->Mesh.VAO = m.VAO;
    goldEntity->Mesh.IndiceCount = m.IndiceCount;


    PathFindingMap[gridX][gridY] = false;
    PlacementMap[gridX][gridY] = false;

}



int AICourseGameManager::FindNearestOwnedTower(glm::ivec2 gridPosition, int playerID)
{
    int nearestTowerID = -1;
    float minDistance = 0;

    auto* towerStorage = WorldEntityManager->GetCastedEntityStorage<TowerEntity>();
    for (int i = 0; i < towerStorage->GetEntityCount(); i++)
    {
        TowerEntity* tower = towerStorage->GetEntity(i);
        if (!tower)
        {
            continue;
        }
        if (tower->Player.PlayerIndex != playerID)
        {
            continue;
        }
        if (tower->Stats.Health <= 0)
        {
            continue;
        }
        if (nearestTowerID == -1)
        {
            nearestTowerID = tower->GetID();
            glm::vec2 diff = tower->Stats.GridPosition - gridPosition;
            minDistance = glm::length(diff);

        }
        glm::vec2 diff = tower->Stats.GridPosition - gridPosition;
        float distance = glm::length(diff);

        if (distance < minDistance)
        {
            nearestTowerID = tower->GetID();
            minDistance = distance;
        }
    }
    return nearestTowerID;
}

int AICourseGameManager::FindNearestGold(glm::ivec2 gridPosition, float& distanceOut)
{
    int nearestGoldID = -1;
    float minDistance = 0;

    auto* goldStorage = WorldEntityManager->GetCastedEntityStorage<GoldEntity>();
    for (int i = 0; i < goldStorage->GetEntityCount(); i++)
    {
        GoldEntity* gold = goldStorage->GetEntity(i);
        if (!gold)
        {
            continue;
        }
        if (gold->Stats.Health <= 0)
        {
            continue;
        }
        if (nearestGoldID == -1)
        {
            nearestGoldID = gold->GetID();
            glm::vec2 diff = gold->Stats.GridPosition - gridPosition;
            minDistance = glm::length(diff);

        }
        glm::vec2 diff = gold->Stats.GridPosition - gridPosition;
        float distance = glm::length(diff);
        if (distance < minDistance)
        {
            nearestGoldID = gold->GetID();
            minDistance = distance;
        }
    }
    distanceOut = minDistance;
    return nearestGoldID;
}


int AICourseGameManager::SpawnTower(int gridX, int gridY, int playerIndex, bool ignoreLimit)
{
    
    if (!PlacementMap[gridX][gridY])
    {
        return -1;
    }

    if (GamePlayers[playerIndex].Gold < TowerGoldCost)
    {
        return -1;
    }

    int towerID = FindNearestOwnedTower(glm::ivec2(gridX, gridY), playerIndex);
    if (towerID > -1)
    {
        TowerEntity* tower = WorldEntityManager->GetEntity<TowerEntity>(towerID);

        glm::vec2 diff = tower->Stats.GridPosition - glm::ivec2(gridX, gridY);
        float distance = glm::length(diff);
        if (distance > TowerMaxPlacementDistance && !ignoreLimit)
        {
            std::cout << "Trying to place tower too far away! Max distance from owned tower is " << TowerMaxPlacementDistance << " tiles!" << std::endl;
            return -1;
        }
    }

    //Cursor spawn and setup.
    TowerEntity* towerEntity = WorldEntityManager->CreateEntity<TowerEntity>();


    //Cursor
    if (!towerEntity)
    {
        return -1;
    } 


    glm::vec2 worldPosition = GridToWorld(glm::vec2(gridX, gridY));
    towerEntity->Transform.Position.x = worldPosition.x;
    towerEntity->Transform.Position.y = 1 + GridBlockDimensions * 2;
    towerEntity->Transform.Position.z = worldPosition.y;

    towerEntity->Stats.GridPosition.x = gridX;
    towerEntity->Stats.GridPosition.y = gridY;

    towerEntity->Transform.EulerAngles.x = 0;
    towerEntity->Transform.EulerAngles.y = 0;
    towerEntity->Transform.EulerAngles.z = 0;

    towerEntity->Transform.Scale.x = GridBlockDimensions / 25.0f;
    towerEntity->Transform.Scale.y = GridBlockDimensions / 10.0f;
    towerEntity->Transform.Scale.z = GridBlockDimensions / 25.0f;

    std::vector<int> meshIDs = MeshLoader->LoadMeshesFromFile("../Game/Assets/torusknot.obj");
    MeshLoaderSystem::MeshHandles* mesh = MeshLoader->GetMeshHandles(meshIDs[0]);
    towerEntity->Mesh.IndiceCount = mesh->IndiceCount;
    towerEntity->Mesh.VAO = mesh->VAO;

    towerEntity->Stats.Health = (float)TowerHealth;

    if (playerIndex == HumanPlayerIndex)
    {
        towerEntity->Material = HumanPlayerMaterial;
       
        towerEntity->Player.IsAI = false;
        towerEntity->Player.PlayerIndex = 0;
    }
    else
    {
        towerEntity->Material = AIPlayerMaterial;

        towerEntity->Player.IsAI = true;
        towerEntity->Player.PlayerIndex = playerIndex;
    }


    GamePlayers[playerIndex].Gold -= TowerGoldCost;
    PlacementMap[gridX][gridY] = false;

    GamePlayers[playerIndex].towerPositions.push_back(glm::ivec2(gridX, gridY));
    return towerEntity->GetID();
}

int AICourseGameManager::FriendlyTowerCount(int playerID)
{
    int count = 0;

    auto* towerStorage = WorldEntityManager->GetCastedEntityStorage<TowerEntity>();
    for (int i = 0; i < towerStorage->GetEntityCount(); i++)
    {
        TowerEntity* tower = towerStorage->GetEntity(i);
        if (!tower)
        {
            continue;
        }
        if (tower->Player.PlayerIndex != playerID)
        {
            continue;
        }
        if (tower->Stats.Health <= 0)
        {
            continue;
        }
        count++;
    }
    return count;
}

int AICourseGameManager::FriendlyCollectorCount(int playerID)
{
    int count = 0;

    auto* collectorStorage = WorldEntityManager->GetCastedEntityStorage<CollectorEntity>();
    for (int i = 0; i < collectorStorage->GetEntityCount(); i++)
    {
        CollectorEntity* collector = collectorStorage->GetEntity(i);
        if (!collector)
        {
            continue;
        }
        if (collector->Player.PlayerIndex != playerID)
        {
            continue;
        }
        if (collector->Stats.Health <= 0)
        {
            continue;
        }
        count++;
    }
    return count;
}

int AICourseGameManager::FriendlyAttackerCount(int playerID)
{
    int count = 0;

    auto* attackerStorage = WorldEntityManager->GetCastedEntityStorage<AttackerEntity>();
    for (int i = 0; i < attackerStorage->GetEntityCount(); i++)
    {
        AttackerEntity* attacker = attackerStorage->GetEntity(i);
        if (!attacker)
        {
            continue;
        }
        if (attacker->Player.PlayerIndex != playerID)
        {
            continue;
        }
        if (attacker->Stats.Health <= 0)
        {
            continue;
        }
        count++;
    }
    return count;
}

void AICourseGameManager::RemoveGold(glm::vec2 gridPos, int entityID)
{
    WorldEntityManager->MarkEntityForDeletion<GoldEntity>(entityID);
    PathFindingMap[(int)gridPos.x][(int)gridPos.y] = true;
    PlacementMap[(int)gridPos.x][(int)gridPos.y] = true;

}

void AICourseGameManager::RemoveTower(glm::vec2 gridPos, int entityID)
{
    TowerEntity* tower = WorldEntityManager->GetEntity<TowerEntity>(entityID);
    auto it = std::find_if(GamePlayers[tower->Player.PlayerIndex].towerPositions.begin(), GamePlayers[tower->Player.PlayerIndex].towerPositions.end(),
        [&gridPos](const glm::ivec2& pos) { return (pos.x == gridPos.x && pos.y == gridPos.y);
        });
    if (it != GamePlayers[tower->Player.PlayerIndex].towerPositions.end())
    {
        GamePlayers[tower->Player.PlayerIndex].towerPositions.erase(it);

    }

    WorldEntityManager->MarkEntityForDeletion<TowerEntity>(entityID);
    PlacementMap[(int)gridPos.x][(int)gridPos.y] = true;
}

void AICourseGameManager::RemoveCollector(int entityID)
{
    WorldEntityManager->MarkEntityForDeletion<CollectorEntity>(entityID);
}

void AICourseGameManager::RemoveAttacker(int entityID)
{
    WorldEntityManager->MarkEntityForDeletion<AttackerEntity>(entityID);
}

void AICourseGameManager::AddGoldToPlayer(int playerIndex, int amount)
{
    GamePlayers[playerIndex].Gold += amount;
}

void AICourseGameManager::SpawnCollector(int gridX, int gridY, int playerIndex)
{

    if (GamePlayers[playerIndex].Gold < CollectorGoldCost)
    {
        return;
    }

    //Cursor spawn and setup.
    CollectorEntity* collector = WorldEntityManager->CreateEntity<CollectorEntity>();


    //Cursor
    if (!collector)
    {
        return;
    }

    int towerID = FindNearestOwnedTower(glm::ivec2(gridX, gridY), playerIndex);
    if (towerID > -1)
    {
        TowerEntity* tower = WorldEntityManager->GetEntity<TowerEntity>(towerID);
        gridX = tower->Stats.GridPosition.x;
        gridY = tower->Stats.GridPosition.y;
    }

    glm::vec2 worldPosition = GridToWorld(glm::vec2 (gridX, gridY));
    collector->Transform.Position.x = worldPosition.x;
    collector->Transform.Position.y = 1 + GridBlockDimensions * 3;
    collector->Transform.Position.z = worldPosition.y;

    collector->Transform.Scale.x = GridBlockDimensions / 100.0f;
    collector->Transform.Scale.y = GridBlockDimensions / 50.0f;
    collector->Transform.Scale.z = GridBlockDimensions / 100.0f;

    collector->Transform.EulerAngles.x = 0;
    collector->Transform.EulerAngles.y = 0;
    collector->Transform.EulerAngles.z = 0;

    
    std::vector<int> meshIDs = MeshLoader->LoadMeshesFromFile("../Game/Assets/torus.obj");
    MeshLoaderSystem::MeshHandles* mesh = MeshLoader->GetMeshHandles(meshIDs[0]);
    collector->Mesh.IndiceCount = mesh->IndiceCount;
    collector->Mesh.VAO = mesh->VAO;
   

    if (playerIndex == HumanPlayerIndex)
    {
        collector->Material = HumanPlayerMaterial;

        collector->Player.IsAI = false;
        collector->Player.PlayerIndex = 0;
    }
    else
    {
        collector->Material = AIPlayerMaterial;

        collector->Player.IsAI = true;
        collector->Player.PlayerIndex = playerIndex;
    }

    collector->Stats.GridPosition.x = gridX;
    collector->Stats.GridPosition.y = gridY;
    collector->Stats.TargetEntity = -1;

    collector->Stats.Health = (float)CollectorHealth;
    collector->Carry.GoldCarried = 0;

    GamePlayers[playerIndex].Gold -= CollectorGoldCost;
}


void AICourseGameManager::SpawnAttacker(int gridX, int gridY, int playerIndex)
{
    if (GamePlayers[playerIndex].Gold < AttackerGoldCost)
    {
        return;
    }

    //Cursor spawn and setup.
    AttackerEntity* attacker = WorldEntityManager->CreateEntity<AttackerEntity>();

    //Cursor
    if (!attacker)
    {
        return;
    }

    int towerID = FindNearestOwnedTower(glm::ivec2(gridX, gridY), playerIndex);
    if (towerID > -1)
    {
        TowerEntity* tower = WorldEntityManager->GetEntity<TowerEntity>(towerID);
        gridX = tower->Stats.GridPosition.x;
        gridY = tower->Stats.GridPosition.y;
    }

    glm::vec2 worldPosition = GridToWorld(glm::vec2(gridX, gridY));
    attacker->Transform.Position.x = worldPosition.x;
    attacker->Transform.Position.y = 1 + GridBlockDimensions;
    attacker->Transform.Position.z = worldPosition.y;

    attacker->Transform.EulerAngles.x = 0;
    attacker->Transform.EulerAngles.y = 0;
    attacker->Transform.EulerAngles.z = 0;

    attacker->Stats.GridPosition.x = gridX;
    attacker->Stats.GridPosition.y = gridY;

    attacker->Transform.Scale.x = GridBlockDimensions / 2.0f;
    attacker->Transform.Scale.y = GridBlockDimensions / 2.0f;
    attacker->Transform.Scale.z = GridBlockDimensions / 2.0f;

    

    MeshComponent m = MeshLoader->GetPrimitiveShape(MeshLoaderSystem::PrimitiveShape::Sphere);
    attacker->Mesh.VAO = m.VAO;
    attacker->Mesh.IndiceCount = m.IndiceCount;

    

    attacker->Stats.Health = (float)AttackerHealth;

    if (playerIndex == HumanPlayerIndex)
    {
        attacker->Material = HumanPlayerMaterial;

        attacker->Player.IsAI = false;
        attacker->Player.PlayerIndex = 0;
    }
    else
    {
        attacker->Material = AIPlayerMaterial;

        attacker->Player.IsAI = true;
        attacker->Player.PlayerIndex = playerIndex;
    }

    GamePlayers[playerIndex].Gold -= AttackerGoldCost;
}

float AICourseGameManager::RandomNumber0_1()
{
    return (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));

}