#include "GL\glew.h"

#include "SceneZoo.h"
#include "../../General/Application.h"
//#include "../../Graphics/LoadOBJ/LoadTGA.h"
//#include "../../Graphics/Mesh/MeshBuilder.h"
#include "../../General/SharedData.h"
#include "../../GameObject/MonsterFactory.h"

#include <sstream>

SceneZoo::SceneZoo() : AREA_MAX_SIZE(50)
{
}

SceneZoo::~SceneZoo()
{
}

void SceneZoo::Init()
{
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 1000 units
    Mtx44 perspective;
    perspective.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
    //perspective.SetToOrtho(-80, 80, -60, 60, -1000, 1000);
    projectionStack.LoadMatrix(perspective);

    zooCamera.Init(Vector3(0, 100, -100), Vector3(0, 0, 0), Vector3(0, 1, 0));

    Application::GetCursorPos(&Application::cursorXPos, &Application::cursorYPos);
    Application::SetCursorPos(Application::GetWindowWidth() / 2.f, Application::GetWindowHeight() / 2.f);

    grassAreaPosition.Set(50.f, 0.f, 50.f);
    fireAreaPosition.Set(-50.f, 0.f, -50.f);
    rockAreaPosition.Set(-50.f, 0.f, 50.f);
    swampAreaPosition.Set(50.f, 0.f, -50.f);

    //For now, some random monsters
    for (unsigned i = 0; i < 40; ++i)
    switch (Math::RandIntMinMax(0, 11))
    {
    case 0:
        SharedData::GetInstance()->player->monsterList.push_back("Bird");
        break;
    case 1:
        SharedData::GetInstance()->player->monsterList.push_back("Rabbit");
        break;
    case 2:
        SharedData::GetInstance()->player->monsterList.push_back("Fairy");
        break;
    case 3:
        SharedData::GetInstance()->player->monsterList.push_back("Grimejam");
        break;
    case 4:
        SharedData::GetInstance()->player->monsterList.push_back("Kof");
        break;
    case 5:
        SharedData::GetInstance()->player->monsterList.push_back("MukBoss");
        break;
    case 6:
        SharedData::GetInstance()->player->monsterList.push_back("Fossil");
        break;
    case 7:
        SharedData::GetInstance()->player->monsterList.push_back("Golem");
        break;
    case 8:
        SharedData::GetInstance()->player->monsterList.push_back("RockSnake");
        break;
    case 9:
        SharedData::GetInstance()->player->monsterList.push_back("FireBug");
        break;
    case 10:
        SharedData::GetInstance()->player->monsterList.push_back("Magma");
        break;
    case 11:
        SharedData::GetInstance()->player->monsterList.push_back("MagmaBerzeker");
        break;
    }

    memset(&zooWorld, 0, sizeof(zooWorld));

    grassAreaSize = 10;
    fireAreaSize = 10;
    rockAreaSize = 10;
    swampAreaSize = 10;

    populateMonsterList();

    isFollowingMonster = false;
    iter = 0;

    currentState = STATE_SHOP;

    //debug stuff
    updown = 0.15f;
    leftright = 0.15f;

    currentShop = SHOP_MAIN;
}
static double counter = 0;

void SceneZoo::Update(double dt)
{
    fps = (float)(1.f / dt);

    //===============================================================================================================================//
    //                                                            Updates                                                            //
    //===============================================================================================================================//

    //Movement update for Gameobjects
    UpdateGameObjects(&zooWorld, 5 * dt);

    //Camera Update - Follows monster if cycling through area
    switch (currentState)
    {
    case STATE_ZOO:

        zooCamera.Update(dt);

        break;

    case STATE_ENCLOSURE:

        if (!isFollowingMonster)
            zooCamera.Update(dt);
        else
        {//Follow monster
            zooCamera.position = zooWorld.position[targetedMonster] + Vector3(0, 15, -10);
            zooCamera.target = zooWorld.position[targetedMonster];
        }

        break;

    case STATE_SHOP:

        break;
    }
    

    //Inputmanager Update
    SharedData::GetInstance()->inputManager->Update();

    //===============================================================================================================================//
    //                                                            Checks                                                             //
    //===============================================================================================================================//

    //Boundry check;
    for (unsigned i = 0; i < grassZone.size(); ++i)
    {
        if (zooWorld.position[grassZone[i]].x < grassAreaPosition.x - grassAreaSize ||
            zooWorld.position[grassZone[i]].x > grassAreaPosition.x + grassAreaSize ||
            zooWorld.position[grassZone[i]].z < grassAreaPosition.z - grassAreaSize ||
            zooWorld.position[grassZone[i]].z > grassAreaPosition.z + grassAreaSize)
            zooWorld.velocity[grassZone[i]] = -zooWorld.velocity[grassZone[i]];
    }

    for (unsigned i = 0; i < fireZone.size(); ++i)
    {
        if (zooWorld.position[fireZone[i]].x < fireAreaPosition.x - fireAreaSize ||
            zooWorld.position[fireZone[i]].x > fireAreaPosition.x + fireAreaSize ||
            zooWorld.position[fireZone[i]].z < fireAreaPosition.z - fireAreaSize ||
            zooWorld.position[fireZone[i]].z > fireAreaPosition.z + fireAreaSize)
            zooWorld.velocity[fireZone[i]] = -zooWorld.velocity[fireZone[i]];
    }

    for (unsigned i = 0; i < rockZone.size(); ++i)
    {
        if (zooWorld.position[rockZone[i]].x < rockAreaPosition.x - rockAreaSize ||
            zooWorld.position[rockZone[i]].x > rockAreaPosition.x + rockAreaSize ||
            zooWorld.position[rockZone[i]].z < rockAreaPosition.z - rockAreaSize ||
            zooWorld.position[rockZone[i]].z > rockAreaPosition.z + rockAreaSize)
            zooWorld.velocity[rockZone[i]] = -zooWorld.velocity[rockZone[i]];
    }

    for (unsigned i = 0; i < swampZone.size(); ++i)
    {
        if (zooWorld.position[swampZone[i]].x < swampAreaPosition.x - swampAreaSize ||
            zooWorld.position[swampZone[i]].x > swampAreaPosition.x + swampAreaSize ||
            zooWorld.position[swampZone[i]].z < swampAreaPosition.z - swampAreaSize ||
            zooWorld.position[swampZone[i]].z > swampAreaPosition.z + swampAreaSize)
            zooWorld.velocity[swampZone[i]] = -zooWorld.velocity[swampZone[i]];
    }

    //===============================================================================================================================//
    //                                                            Key Inputs                                                         //
    //===============================================================================================================================//

    //Changing vision to be able to see zone
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_1].isPressed)
    {
        zooCamera.position = grassAreaPosition + Vector3(0, 50, -25);
        zooCamera.target = grassAreaPosition;

        if (currentArea == AREA_GRASS)
        {
            currentArea = AREA_OVERVIEW;
            zooCamera.Reset();
        }
        else
            currentArea = AREA_GRASS;

        isFollowingMonster = false;
        currentState = STATE_ENCLOSURE;
    }
    else if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_2].isPressed)
    {
        zooCamera.position = fireAreaPosition + Vector3(0, 50, -25);
        zooCamera.target = fireAreaPosition;

        if (currentArea == AREA_FIRE)
        {
            currentArea = AREA_OVERVIEW;
            zooCamera.Reset();
        }
        else
            currentArea = AREA_FIRE;

        isFollowingMonster = false;
        currentState = STATE_ENCLOSURE;
    }
    else if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_3].isPressed)
    {
        zooCamera.position = rockAreaPosition + Vector3(0, 50, -25);
        zooCamera.target = rockAreaPosition;

        if (currentArea == AREA_ROCK)
        {
            currentArea = AREA_OVERVIEW;
            zooCamera.Reset();
        }
        else
            currentArea = AREA_ROCK;

        isFollowingMonster = false;
        currentState = STATE_ENCLOSURE;
    }
    else if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_4].isPressed)
    {
        zooCamera.position = swampAreaPosition + Vector3(0, 50, -25);
        zooCamera.target = swampAreaPosition;

        if (currentArea == AREA_SWAMP)
        {
            currentArea = AREA_OVERVIEW;
            zooCamera.Reset();
        }
        else
            currentArea = AREA_SWAMP;

        isFollowingMonster = false;
        currentState = STATE_ENCLOSURE;
    }

    //Cycle thru Monsters in the area
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_TAB].isPressed)
    {
        switch (currentArea)
        {
        case AREA_GRASS:

            CycleThroughZoneArea(grassZone);

            break;

        case AREA_FIRE:

            CycleThroughZoneArea(fireZone);

            break;

        case AREA_ROCK:

            CycleThroughZoneArea(rockZone);

            break;

        case AREA_SWAMP:

            CycleThroughZoneArea(swampZone);

            break;
        }
    }

    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_ENTER].isPressed)
    {

    }


    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_G].isPressed)
    {
        currentState = STATE_SHOP;
    }

    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_UP].isHeldDown)
    {
        updown += 0.5f * dt;
    }
    else if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_DOWN].isHeldDown)
    {
        updown -= 0.5f * dt;
    }

    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_LEFT].isHeldDown)
    {
        leftright -= 0.5f * dt;
    }
    else if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_RIGHT].isHeldDown)
    {
        leftright += 0.5f * dt;
    }

}

void SceneZoo::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Mtx44 perspective;
    perspective.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
    //perspective.SetToOrtho(-80, 80, -60, 60, -1000, 1000);
    projectionStack.LoadMatrix(perspective);

    // Camera matrix
    viewStack.LoadIdentity();
    viewStack.LookAt(
        zooCamera.position.x, zooCamera.position.y, zooCamera.position.z,
        zooCamera.target.x, zooCamera.target.y, zooCamera.target.z,
        zooCamera.up.x, zooCamera.up.y, zooCamera.up.z
        );
    // Model matrix : an identity matrix (model will be at the origin)
    modelStack.LoadIdentity();

    //Lights are currently set to directional
    Light light = SharedData::GetInstance()->graphicsLoader->GetLights();

    if (light.type == Light::LIGHT_DIRECTIONAL)
    {
        Vector3 lightDir(light.position.x, light.position.y, light.position.z);
        Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
        glUniform3fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_LIGHT0_POSITION), 1, &lightDirection_cameraspace.x);
    }

    //Axes
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_AXES), false);

    //Render World
    RenderZooScene();

    //RenderGameObjects
    RenderGameObjects(&zooWorld);

    //Render Enclosures
    RenderEnclosures();
    
    //If cycling thru monsters in zone display their stats
    if (isFollowingMonster)
        DisplayMonsterStats(zooWorld.monster[targetedMonster]);
    
    //Render Shop Interface
    if (currentState == STATE_SHOP)
        RenderShopInterface();

    if (currentState == STATE_ENCLOSURE)
        RenderEnclosureInterface();
}

void SceneZoo::RenderZooScene()
{
    //Ground mesh
    modelStack.PushMatrix();
    modelStack.Translate(0, 0, 0);
    modelStack.Rotate(-90, 1, 0, 0);
    modelStack.Scale(200, 200, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCK_TERRAIN), true);
    modelStack.PopMatrix();

    //Skyplane
    modelStack.PushMatrix();
    modelStack.Translate(500, 2800, -500);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GRASS_SKYPLANE), false);
    modelStack.PopMatrix();
}

void SceneZoo::Exit()
{

}

void SceneZoo::populateMonsterList()
{
    Vector3 randOffset;

    for (unsigned i = 0; i < SharedData::GetInstance()->player->monsterList.size(); ++i)
    {
        float offset = grassAreaSize * 0.5f;
        randOffset.Set(Math::RandFloatMinMax(-offset, offset),
                       0,
                       Math::RandFloatMinMax(-offset, offset)
                       );

        //Grass Zone
        if (SharedData::GetInstance()->player->monsterList[i] == "Bird")
        {
            GameObject go = createGO(&zooWorld);
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_AI;
            zooWorld.position[go] = grassAreaPosition + randOffset;
            zooWorld.velocity[go].Set(Math::RandFloatMinMax(-1.f, 1.f), 0.f, Math::RandFloatMinMax(-1.f, 1.f));
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_BIRD);
            zooWorld.appearance[go].scale.Set(1, 1, 1);
            zooWorld.monster[go] = MonsterFactory::CreateMonster("Bird");

            grassZone.push_back(go);
        }
        else if (SharedData::GetInstance()->player->monsterList[i] == "Rabbit")
        {
            GameObject go = createGO(&zooWorld);
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_AI;
            zooWorld.position[go] = grassAreaPosition + randOffset;
            zooWorld.velocity[go].Set(Math::RandFloatMinMax(-1.f, 1.f), 0.f, Math::RandFloatMinMax(-1.f, 1.f));
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_RABBIT);
            zooWorld.appearance[go].scale.Set(1, 1, 1);
            zooWorld.monster[go] = MonsterFactory::CreateMonster("Rabbit");

            grassZone.push_back(go);
        }
        else if (SharedData::GetInstance()->player->monsterList[i] == "Fairy")
        {
            GameObject go = createGO(&zooWorld);
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_AI;
            zooWorld.position[go] = grassAreaPosition + randOffset;
            zooWorld.velocity[go].Set(Math::RandFloatMinMax(-1.f, 1.f), 0.f, Math::RandFloatMinMax(-1.f, 1.f));
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE);
            zooWorld.appearance[go].scale.Set(1, 1, 1);
            zooWorld.monster[go] = MonsterFactory::CreateMonster("Fairy");

            grassZone.push_back(go);
        }

        // Swamp Zone
        else if (SharedData::GetInstance()->player->monsterList[i] == "Grimejam")
        {
            GameObject go = createGO(&zooWorld);
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_AI;
            zooWorld.position[go] = swampAreaPosition + randOffset;
            zooWorld.velocity[go].Set(Math::RandFloatMinMax(-1.f, 1.f), 0.f, Math::RandFloatMinMax(-1.f, 1.f));
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE);
            zooWorld.appearance[go].scale.Set(1, 1, 1);
            zooWorld.monster[go] = MonsterFactory::CreateMonster("Grimejam");

            swampZone.push_back(go);
        }
        else if (SharedData::GetInstance()->player->monsterList[i] == "Kof")
        {
            GameObject go = createGO(&zooWorld);
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_AI;
            zooWorld.position[go] = swampAreaPosition + randOffset;
            zooWorld.velocity[go].Set(Math::RandFloatMinMax(-1.f, 1.f), 0.f, Math::RandFloatMinMax(-1.f, 1.f));
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE);
            zooWorld.appearance[go].scale.Set(1, 1, 1);
            zooWorld.monster[go] = MonsterFactory::CreateMonster("Kof");

            swampZone.push_back(go);
        }
        else if (SharedData::GetInstance()->player->monsterList[i] == "MukBoss")
        {
            GameObject go = createGO(&zooWorld);
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_AI;
            zooWorld.position[go] = swampAreaPosition + randOffset;
            zooWorld.velocity[go].Set(Math::RandFloatMinMax(-1.f, 1.f), 0.f, Math::RandFloatMinMax(-1.f, 1.f));
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE);
            zooWorld.appearance[go].scale.Set(1, 1, 1);
            zooWorld.monster[go] = MonsterFactory::CreateMonster("MukBoss");

            swampZone.push_back(go);
        }

        // Rock Zone
        else if (SharedData::GetInstance()->player->monsterList[i] == "Fossil")
        {
            GameObject go = createGO(&zooWorld);
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_AI;
            zooWorld.position[go] = rockAreaPosition + randOffset;
            zooWorld.velocity[go].Set(Math::RandFloatMinMax(-1.f, 1.f), 0.f, Math::RandFloatMinMax(-1.f, 1.f));
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE);
            zooWorld.appearance[go].scale.Set(1, 1, 1);
            zooWorld.monster[go] = MonsterFactory::CreateMonster("Fossil");

            rockZone.push_back(go);
        }
        else if (SharedData::GetInstance()->player->monsterList[i] == "Golem")
        {
            GameObject go = createGO(&zooWorld);
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_AI;
            zooWorld.position[go] = rockAreaPosition + randOffset;
            zooWorld.velocity[go].Set(Math::RandFloatMinMax(-1.f, 1.f), 0.f, Math::RandFloatMinMax(-1.f, 1.f));
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE);
            zooWorld.appearance[go].scale.Set(1, 1, 1);
            zooWorld.monster[go] = MonsterFactory::CreateMonster("Golem");

            rockZone.push_back(go);
        }
        else if (SharedData::GetInstance()->player->monsterList[i] == "RockSnake")
        {
            GameObject go = createGO(&zooWorld);
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_AI;
            zooWorld.position[go] = rockAreaPosition + randOffset;
            zooWorld.velocity[go].Set(Math::RandFloatMinMax(-1.f, 1.f), 0.f, Math::RandFloatMinMax(-1.f, 1.f));
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE);
            zooWorld.appearance[go].scale.Set(1, 1, 1);
            zooWorld.monster[go] = MonsterFactory::CreateMonster("RockSnake");

            rockZone.push_back(go);
        }

        // Fire Zone
        else if (SharedData::GetInstance()->player->monsterList[i] == "FireBug")
        {
            GameObject go = createGO(&zooWorld);
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_AI;
            zooWorld.position[go] = fireAreaPosition + randOffset;
            zooWorld.velocity[go].Set(Math::RandFloatMinMax(-1.f, 1.f), 0.f, Math::RandFloatMinMax(-1.f, 1.f));
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE);
            zooWorld.appearance[go].scale.Set(1, 1, 1);
            zooWorld.monster[go] = MonsterFactory::CreateMonster("FireBug");

            fireZone.push_back(go);
        }
        else if (SharedData::GetInstance()->player->monsterList[i] == "Magma")
        {
            GameObject go = createGO(&zooWorld);
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_AI;
            zooWorld.position[go] = fireAreaPosition + randOffset;
            zooWorld.velocity[go].Set(Math::RandFloatMinMax(-1.f, 1.f), 0.f, Math::RandFloatMinMax(-1.f, 1.f));
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE);
            zooWorld.appearance[go].scale.Set(1, 1, 1);
            zooWorld.monster[go] = MonsterFactory::CreateMonster("Magma");

            fireZone.push_back(go);
        }
        else if (SharedData::GetInstance()->player->monsterList[i] == "MagmaBerzeker")
        {
            GameObject go = createGO(&zooWorld);
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_AI;
            zooWorld.position[go] = fireAreaPosition + randOffset;
            zooWorld.velocity[go].Set(Math::RandFloatMinMax(-1.f, 1.f), 0.f, Math::RandFloatMinMax(-1.f, 1.f));
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE);
            zooWorld.appearance[go].scale.Set(1, 1, 1);
            zooWorld.monster[go] = MonsterFactory::CreateMonster("MagmaBerzeker");

            fireZone.push_back(go);
        }
        else
        {
            std::cout << "monster does not exist" << std::endl;
        }
    }
}

void SceneZoo::DisplayMonsterStats(Monster* monster)
{
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), monster->GetName(), Color(1, 1, 0), 3, 35, 55);

    std::stringstream ss;
    ss << "Health: " << monster->GetHealthStat();
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3, 0, 45);

    std::stringstream ss1;
    ss1 << "Capture Rate: " << monster->GetCaptureRateStat();
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss1.str(), Color(1, 1, 0), 3, 0, 35);

    std::stringstream ss2;
    ss2 << "Aggresion: " << monster->GetAggressionStat();
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss2.str(), Color(1, 1, 0), 3, 0, 25);

    std::stringstream ss3;
    ss3 << "Fear: " << monster->GetFearStat();
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss3.str(), Color(1, 1, 0), 3, 0, 15);
}

void SceneZoo::CycleThroughZoneArea(std::vector<GameObject> area)
{
    if (isFollowingMonster)
    {
        if (++iter >= area.size())
            iter = 0;
        else
            targetedMonster = area[iter];
    }
    else
    {
        if (area.size())
            targetedMonster = area[iter];

        isFollowingMonster = true;
    }
}

void SceneZoo::UpgradeEnclosureSize(AREA area)
{
    switch (area)
    {
    case AREA_GRASS:
        if (grassAreaSize != AREA_MAX_SIZE)
            grassAreaSize += 10;
        else
            std::cout << "Currently at full size" << std::endl;
            
        break;

    case AREA_FIRE:
        if (fireAreaSize != AREA_MAX_SIZE)
            fireAreaSize += 10;
        else
            std::cout << "Currently at full size" << std::endl;

        break;

    case AREA_ROCK:
        if (rockAreaSize != AREA_MAX_SIZE)
            rockAreaSize += 10;
        else
            std::cout << "Currently at full size" << std::endl;

        break;

    case AREA_SWAMP:
        if (swampAreaSize != AREA_MAX_SIZE)
            swampAreaSize += 10;
        else
            std::cout << "Currently at full size" << std::endl;

        break;
    }
}

void SceneZoo::RenderEnclosures()
{
    //Hardcoded enclosure boxes

    //Grass
    modelStack.PushMatrix();
    modelStack.Translate(grassAreaPosition.x + grassAreaSize, 0, grassAreaPosition.z);
    modelStack.Scale(1, 1, grassAreaSize);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(grassAreaPosition.x - grassAreaSize, 0, grassAreaPosition.z);
    modelStack.Scale(1, 1, grassAreaSize);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(grassAreaPosition.x, 0, grassAreaPosition.z + grassAreaSize);
    modelStack.Scale(grassAreaSize, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(grassAreaPosition.x, 0, grassAreaPosition.z - grassAreaSize);
    modelStack.Scale(grassAreaSize, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    modelStack.PopMatrix();

    //Fire
    modelStack.PushMatrix();
    modelStack.Translate(fireAreaPosition.x + fireAreaSize, 0, fireAreaPosition.z);
    modelStack.Scale(1, 1, fireAreaSize);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(fireAreaPosition.x - fireAreaSize, 0, fireAreaPosition.z);
    modelStack.Scale(1, 1, fireAreaSize);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(fireAreaPosition.x, 0, fireAreaPosition.z + fireAreaSize);
    modelStack.Scale(fireAreaSize, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(fireAreaPosition.x, 0, fireAreaPosition.z - fireAreaSize);
    modelStack.Scale(fireAreaSize, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    modelStack.PopMatrix();

    //Rock
    modelStack.PushMatrix();
    modelStack.Translate(rockAreaPosition.x + rockAreaSize, 0, rockAreaPosition.z);
    modelStack.Scale(1, 1, rockAreaSize);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(rockAreaPosition.x - rockAreaSize, 0, rockAreaPosition.z);
    modelStack.Scale(1, 1, rockAreaSize);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(rockAreaPosition.x, 0, rockAreaPosition.z + rockAreaSize);
    modelStack.Scale(rockAreaSize, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(rockAreaPosition.x, 0, rockAreaPosition.z - rockAreaSize);
    modelStack.Scale(rockAreaSize, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    modelStack.PopMatrix();

    //Swamp
    modelStack.PushMatrix();
    modelStack.Translate(swampAreaPosition.x + swampAreaSize, 0, swampAreaPosition.z);
    modelStack.Scale(1, 1, swampAreaSize);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(swampAreaPosition.x - swampAreaSize, 0, swampAreaPosition.z);
    modelStack.Scale(1, 1, swampAreaSize);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(swampAreaPosition.x, 0, swampAreaPosition.z + swampAreaSize);
    modelStack.Scale(swampAreaSize, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(swampAreaPosition.x, 0, swampAreaPosition.z - swampAreaSize);
    modelStack.Scale(swampAreaSize, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    modelStack.PopMatrix();
}

void SceneZoo::PurchaseItem(Item item)
{

}

void SceneZoo::SellItem(Item item)
{

}

void SceneZoo::RenderShopkeeperText()
{

}

void SceneZoo::RenderShopInterface()
{
    SetHUD(true);

    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_BACKGROUND), 50.f, 40.f, 30.f, false);

    //Check selection
    if (Application::cursorXPos / Application::m_width >= .231f &&
        Application::cursorXPos / Application::m_width <= .389f)
    {
        //Talk - Net - Net - Net
        if (Application::cursorYPos / Application::m_height >= .1325f &&
            Application::cursorYPos / Application::m_height <= .188f)
        {
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 12.5f, 3.5f, 1.f, 25.f, 50.f, 0.f, 0.f, 0.f, false);

            if (Application::IsMousePressed(0))
                switch (currentShop)
            {
                case SHOP_MAIN:
                    RenderShopkeeperText();
                    std::cout << "shop keeeper text" << std::endl;
                    break;
                case SHOP_BUY:
                    PurchaseItem(SharedData::GetInstance()->player->inventory[Item::TYPE_NET]);
                    break;
                case SHOP_SELL:
                    SellItem(SharedData::GetInstance()->player->inventory[Item::TYPE_NET]);
                    break;
                case SHOP_UPGRADE:
                    if (SharedData::GetInstance()->player->m_currency > 100 && SharedData::GetInstance()->player->inventory[Item::TYPE_NET].Upgrade())
                        std::cout << "upgraded" << std::endl;     //Render Sucessfully upgraded
                    else
                        std::cout << "not upgraded" << std::endl; //Might need to specify if not enough gold or already at max upgrade level
                    break;
            }
        }
        else
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 12.5f, 3.5f, 1.f, 25.f, 50.f, 0.f, 0.f, 0.f, false);

        //Buy - Bait - Bait - Bait 
        if (Application::cursorYPos / Application::m_height >= .294f &&
            Application::cursorYPos / Application::m_height <= .3525f)
        {
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 12.5f, 3.5f, 1.f, 25.f, 40.f, 0.f, 0.f, 0.f, false);

            if (Application::IsMousePressed(0))
                switch (currentShop)
            {
                case SHOP_MAIN:
                    currentShop = SHOP_BUY;
                    break;
                case SHOP_BUY:
                    PurchaseItem(SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT]);
                    break;
                case SHOP_SELL:
                    SellItem(SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT]);
                    break;
                case SHOP_UPGRADE:
                    if (SharedData::GetInstance()->player->m_currency > 100 && SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].Upgrade())
                        std::cout << "upgraded" << std::endl;     //Render Sucessfully upgraded
                    else
                        std::cout << "not upgraded" << std::endl; //Might need to specify if not enough gold or already at max upgrade level
                    break;
            }
        }
        else
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 12.5f, 3.5f, 1.f, 25.f, 40.f, 0.f, 0.f, 0.f, false);

        //Sell - Trap - Trap - Trap
        if (Application::cursorYPos / Application::m_height >= .4555f &&
            Application::cursorYPos / Application::m_height <= .517f)
        {
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 12.5f, 3.5f, 1.f, 25.f, 30.f, 0.f, 0.f, 0.f, false);

            if (Application::IsMousePressed(0))
                switch (currentShop)
            {
                case SHOP_MAIN:
                    currentShop = SHOP_SELL;
                    break;
                case SHOP_BUY:
                    PurchaseItem(SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP]);
                    break;
                case SHOP_SELL:
                    SellItem(SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP]);
                    break;
                case SHOP_UPGRADE:
                    if (SharedData::GetInstance()->player->m_currency > 100 && SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP].Upgrade())
                        std::cout << "upgraded" << std::endl;     //Render Sucessfully upgraded
                    else
                        std::cout << "not upgraded" << std::endl; //Might need to specify if not enough gold or already at max upgrade level
                    break;
            }
        }
        else
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 12.5f, 3.5f, 1.f, 25.f, 30.f, 0.f, 0.f, 0.f, false);

        //Upgrade - Rock - Rock - Rock
        if (Application::cursorYPos / Application::m_height >= .617f &&
            Application::cursorYPos / Application::m_height <= 0.6785f)
        {
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 12.5f, 3.5f, 1.f, 25.f, 20.f, 0.f, 0.f, 0.f, false);

            if (Application::IsMousePressed(0))
                switch (currentShop)
            {
                case SHOP_MAIN:
                    currentShop = SHOP_UPGRADE;
                    break;
                case SHOP_BUY:
                    PurchaseItem(SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK]);
                    break;
                case SHOP_SELL:
                    SellItem(SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK]);
                    break;
                case SHOP_UPGRADE:
                    if (SharedData::GetInstance()->player->m_currency > 100 && SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].Upgrade())
                        std::cout << "upgraded" << std::endl;//Render Sucessfully upgraded
                    else
                        std::cout << "not upgraded" << std::endl;//Might need to specify if not enough gold or already at max upgrade level
                    break;
            }
        }
        else
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 12.5f, 3.5f, 1.f, 25.f, 20.f, 0.f, 0.f, 0.f, false);

        //Exit
        if (Application::cursorYPos / Application::m_height >= .7785f &&
            Application::cursorYPos / Application::m_height <= .84f)
        {
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 12.5f, 3.5f, 1.f, 25.f, 10.f, 0.f, 0.f, 0.f, false);

            if (Application::IsMousePressed(0))
                switch (currentShop)
            {
                case SHOP_MAIN:
                    currentState = STATE_ZOO;
                    break;
                default:
                    currentShop = SHOP_MAIN;
                    break;
            }
        }
        else
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 12.5f, 3.5f, 1.f, 25.f, 10.f, 0.f, 0.f, 0.f, false);
    }
    else
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 12.5f, 3.5f, 1.f, 25.f, 50.f, 0.f, 0.f, 0.f, false);
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 12.5f, 3.5f, 1.f, 25.f, 40.f, 0.f, 0.f, 0.f, false);
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 12.5f, 3.5f, 1.f, 25.f, 30.f, 0.f, 0.f, 0.f, false);
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 12.5f, 3.5f, 1.f, 25.f, 20.f, 0.f, 0.f, 0.f, false);
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 12.5f, 3.5f, 1.f, 25.f, 10.f, 0.f, 0.f, 0.f, false);
    }

    //Selection text
    switch (currentShop)
    {
    case SHOP_MAIN:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Talk", Color(1, 1, 0), 3.f, 18.75f, 48.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Buy", Color(1, 1, 0), 3.f, 18.75f, 38.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Sell", Color(1, 1, 0), 3.f, 18.75f, 28.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Upgrade", Color(1, 1, 0), 3.f, 18.75f, 18.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Back to Zoo", Color(1, 1, 0), 2.5f, 18.75f, 8.5f);
        break;
    case SHOP_BUY:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Net", Color(1, 1, 0), 3.f, 18.75f, 48.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Bait", Color(1, 1, 0), 3.f, 18.75f, 38.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Trap", Color(1, 1, 0), 3.f, 18.75f, 28.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Rock", Color(1, 1, 0), 3.f, 18.75f, 18.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Back", Color(1, 1, 0), 2.5f, 18.75f, 8.5f);
        break;
    case SHOP_SELL:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Net", Color(1, 1, 0), 3.f, 18.75f, 48.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Bait", Color(1, 1, 0), 3.f, 18.75f, 38.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Trap", Color(1, 1, 0), 3.f, 18.75f, 28.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Rock", Color(1, 1, 0), 3.f, 18.75f, 18.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Back", Color(1, 1, 0), 2.5f, 18.75f, 8.5f);
        break;
    case SHOP_UPGRADE:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Net", Color(1, 1, 0), 3.f, 18.75f, 48.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Bait", Color(1, 1, 0), 3.f, 18.75f, 38.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Trap", Color(1, 1, 0), 3.f, 18.75f, 28.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Rock", Color(1, 1, 0), 3.f, 18.75f, 18.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Back", Color(1, 1, 0), 2.5f, 18.75f, 8.5f);
        break;
    }

    SetHUD(false);
}

void SceneZoo::RenderEnclosureInterface()
{
    SetHUD(true);



    SetHUD(false);
}


//Mouse picking stuff//
////Step1 - ViewPort space
//float x = 1.0f - (2.0f * Application::cursorYPos) / 1080;
//float y = (2.0f * Application::cursorXPos) / 1920 - 1.0f; 
//float z = 1.0f;
//Vector3 ray_nds;
//ray_nds.Set(x, y, z);

////Step 2 - Clip space
//Mtx44 ray_clip;
//ray_clip.SetToPerspective(ray_nds.x, ray_nds.y, -1.0f, 0.0f);

////Step3 - Camera space
//Mtx44 ray_cam = ray_clip * perspective.GetInverse();

////Step4 - World space 
//Mtx44 view_matrix;
//view_matrix.SetToLookAt(zooCamera.position.x, zooCamera.position.y, zooCamera.position.z,
//    zooCamera.target.x, zooCamera.target.y, zooCamera.target.z,
//    zooCamera.up.x, zooCamera.up.y, zooCamera.up.z);

//Mtx44 ray_wor = (view_matrix.GetInverse() * ray_cam);

//Vector3 ray_world = ray_wor * Vector3(1,1,1);
//ray_world = ray_world.Normalized();

////std::cout << ray_world << std::endl;

////end of Mouse picking stuff//





////ground normal
//Vector3 ground_normal;
//ground_normal.Set(0, 1, 0);

//Vector3 ray;
//ray = zooCamera.position + ray_world;
//
//float distance = 100;

//Vector3 result;
//result = ray_world * distance;

//std::cout << result << std::endl;

//if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_SPACE].isHeldDown)
//{
//    result.y = 0;
//    tempStore.push_back(result);
//}

//for (unsigned i = 0; i < tempStore.size(); ++i)
//{
//    modelStack.PushMatrix();
//    modelStack.Translate(tempStore[i].x, tempStore[i].y, tempStore[i].z);
//    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
//    modelStack.PopMatrix();
//}