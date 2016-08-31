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
    /*for (unsigned i = 0; i < 40; ++i)
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
    }*/

    memset(&zooWorld, 0, sizeof(zooWorld));

    grassAreaSize = 10;
    fireAreaSize = 10;
    rockAreaSize = 10;
    swampAreaSize = 10;

    populateMonsterList();

    isFollowingMonster = false;
    cycleIter = 0;

    currentState = STATE_ZOO;

    //debug stuff
    updown = 0.15f;
    leftright = 0.15f;

    currentShop = SHOP_MAIN;
    isInTransaction = false;
    shopKeeperTextChoice = TEXT_NONE;
    transactionCounter = 1;
    upgradeCost = 1000;

    f_Rotate = 0.f;
    rotateEnclosureIcon1 = 45.f;
    rotateEnclosureIcon2 = 45.f;
    changeSceneTo = AREA_OVERVIEW;
}

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

        if (isFollowingMonster)
        {
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
        zooCamera.position = grassAreaPosition + Vector3(0, 75, -50);
        zooCamera.target = grassAreaPosition;

        currentArea = AREA_GRASS;

        isFollowingMonster = false;
        currentState = STATE_ENCLOSURE;
    }
    else if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_2].isPressed)
    {
        zooCamera.position = fireAreaPosition + Vector3(0, 75, -50);
        zooCamera.target = fireAreaPosition;

        currentArea = AREA_FIRE;

        isFollowingMonster = false;
        currentState = STATE_ENCLOSURE;
    }
    else if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_3].isPressed)
    {
        zooCamera.position = rockAreaPosition + Vector3(0, 75, -50);
        zooCamera.target = rockAreaPosition;

        currentArea = AREA_ROCK;

        isFollowingMonster = false;
        currentState = STATE_ENCLOSURE;
    }
    else if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_4].isPressed)
    {
        zooCamera.position = swampAreaPosition + Vector3(0, 75, -50);
        zooCamera.target = swampAreaPosition;

        currentArea = AREA_SWAMP;

        isFollowingMonster = false;
        currentState = STATE_ENCLOSURE;
    }

    //Cycle thru Monsters in the area
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_TAB].isPressed && currentState == STATE_ENCLOSURE)
    {
        switch (currentArea)
        {
        case AREA_GRASS:

            if (grassZone.size())
                CycleThroughZoneArea(grassZone);

            break;

        case AREA_FIRE:

            if (fireZone.size())
                CycleThroughZoneArea(fireZone);

            break;

        case AREA_ROCK:

            if (rockZone.size())
                CycleThroughZoneArea(rockZone);

            break;

        case AREA_SWAMP:

            if (swampZone.size())
                CycleThroughZoneArea(swampZone);

            break;
        }
    }

    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_ENTER].isPressed)
    {
        isFollowingMonster = false;
        currentArea = AREA_OVERVIEW;
        currentState = STATE_SHOP;
    }

    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_G].isPressed)
    {
        isFollowingMonster = false;
        currentArea = AREA_OVERVIEW;
        currentState = STATE_CHANGE_SCENE;
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

    f_Rotate += 10.f * (float)dt;


    switch (changeSceneTo)
    {
    case AREA_GRASS:

        SharedData::GetInstance()->sceneManager->ChangeScene(1);

        break;

    case AREA_FIRE:

        SharedData::GetInstance()->sceneManager->ChangeScene(4);

        break;

    case AREA_ROCK:

        SharedData::GetInstance()->sceneManager->ChangeScene(3);

        break;

    case AREA_SWAMP:

        SharedData::GetInstance()->sceneManager->ChangeScene(2);

        break;
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
    {
        DisplayMonsterInterface(zooWorld.monster[targetedMonster]);
    }

    //Else if just in enclosure view, display enclosure interface
    else if (currentState == STATE_ENCLOSURE)
    {
        RenderEnclosureInterface();
    }

    //Render Shop Interface
    if (currentState == STATE_SHOP)
    {
        RenderShopInterface();
        RenderHUD();

        if (isInTransaction)
            RenderTransactionInterface();

        RenderShopkeeperText();
    }
    
    if (currentState == STATE_CHANGE_SCENE)
        RenderHuntingScenesInterface();
    
    std::cout << Application::cursorXPos / Application::m_width << " | " << Application::cursorYPos / Application::m_height << std::endl;
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
    for (unsigned GO = 0; GO < zooWorld.GAMEOBJECT_COUNT; ++GO)
    {
        destroyGO(&zooWorld, GO);
    }
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

void SceneZoo::DisplayMonsterInterface(Monster* monster)
{
    SetHUD(true);

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

    //Slaughter button
    if (Application::cursorXPos / Application::m_width >= 0.804688 &&
        Application::cursorXPos / Application::m_width <= 0.955208 &&
        Application::cursorYPos / Application::m_height >= 0.361111 &&
        Application::cursorYPos / Application::m_height <= 0.436111
        )
    {
        rotateEnclosureIcon2 += 2.f;

        if (Application::IsMousePressed(0))
        {
            SlaughterMonster();
        }
    }

    // Sell button
    if (Application::cursorXPos / Application::m_width >= 0.804688 &&
        Application::cursorXPos / Application::m_width <= 0.955208 &&
        Application::cursorYPos / Application::m_height >= 0.510185 &&
        Application::cursorYPos / Application::m_height <= 0.568519
        )
    {
        rotateEnclosureIcon1 += 2.f;

        if (Application::IsMousePressed(0) && grassZone.size())
        {
            SellMonster();
        }
    }
  
    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_COIN), 8, 75.0f, 25.f, 0, rotateEnclosureIcon1, 0, false);
    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MEAT), 2, 75.0f, 35.f, 90, 45.f, rotateEnclosureIcon2, false);

    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Sell", Color(1, 1, 0), 2, 65.0f, 25.f);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Slaughter", Color(1, 1, 0), 2, 65.0f, 35.f);

    SetHUD(false);
}

void SceneZoo::CycleThroughZoneArea(std::vector<GameObject> area)
{
    if (isFollowingMonster)
    {
        if (++cycleIter >= area.size())
            cycleIter = 0;
        else
            targetedMonster = area[cycleIter];
    }
    else
    {
        if (area.size())
            targetedMonster = area[cycleIter];

        isFollowingMonster = true;
    }
}

bool SceneZoo::UpgradeEnclosureSize(AREA area)
{
    switch (area)
    {
    case AREA_GRASS:
        if (grassAreaSize != AREA_MAX_SIZE)
        {
            grassAreaSize += 10;
            return true;
        }

        break;

    case AREA_FIRE:
        if (fireAreaSize != AREA_MAX_SIZE)
        {
            fireAreaSize += 10;
            return true;
        }

        break;

    case AREA_ROCK:
        if (rockAreaSize != AREA_MAX_SIZE)
        {
            rockAreaSize += 10;
            return true;
        }

        break;

    case AREA_SWAMP:
        if (swampAreaSize != AREA_MAX_SIZE)
        {
            swampAreaSize += 10;
            return true;
        }

        break;
    }

    return false;
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

void SceneZoo::RenderShopkeeperText()
{
    switch (shopKeeperTextChoice)
    {
    case TEXT_TIP_1:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "random tip 1", Color(1, 1, 0), 3.f, 40.f, 12.5f);
        break;

    case TEXT_TIP_2:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "random tip 2", Color(1, 1, 0), 3.f, 40.f, 12.5f);
        break;

    case TEXT_TIP_3:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "random tip 3", Color(1, 1, 0), 3.f, 40.f, 12.5f);
        break;

    case TEXT_TIP_4:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "random tip 4", Color(1, 1, 0), 3.f, 40.f, 12.5f);
        break;

    case TEXT_TIP_5:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "random tip 5", Color(1, 1, 0), 3.f, 40.f, 12.5f);
        break;

    case TEXT_THANK:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Thank you!", Color(1, 1, 0), 3.f, 40.f, 12.5f);
        break;

    case TEXT_INSUFFICIENT_COINS:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "You don't have enough Coins!", Color(1, 1, 0), 3.f, 40.f, 12.5f);
        break;

    case TEXT_AT_MAX_UPGRADE:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "I can't updgrade this Item any further!", Color(1, 1, 0), 3.f, 40.f, 12.5f);
        break;

    default:

        break;
    }
}

void SceneZoo::RenderShopInterface()
{
    SetHUD(true);

    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_BACKGROUND), 55.f, 40.f, 30.f, false);

    //Check selection
    if (Application::cursorXPos / Application::m_width >= .231f &&
        Application::cursorXPos / Application::m_width <= .389f)
    {
        //Talk - Net - Net - Net
        if (Application::cursorYPos / Application::m_height >= .1325f &&
            Application::cursorYPos / Application::m_height <= .188f)
        {
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 12.5f, 3.5f, 1.f, 25.f, 50.f, 0.f, 0.f, 0.f, false);

            if (currentShop == SHOP_UPGRADE)
                RenderUpgradeInterface(Item::TYPE_NET);

            if (Application::IsMousePressed(0))
                switch (currentShop)
            {
                case SHOP_MAIN:
                    shopKeeperTextChoice = static_cast<SHOPKEEPER_TEXT>(Math::RandIntMinMax(1, 5));
                    break;
                case SHOP_BUY:
                    isInTransaction = true;
                    currentItem = Item::TYPE_NET;
                    break;
                case SHOP_SELL:
                    isInTransaction = true;
                    currentItem = Item::TYPE_NET;
                    break;
                case SHOP_UPGRADE:
                    if (SharedData::GetInstance()->player->m_currency > SharedData::GetInstance()->player->inventory[Item::TYPE_NET].GetUpgradeCost())
                    {
                        if (SharedData::GetInstance()->player->inventory[Item::TYPE_NET].Upgrade())
                        {
                            shopKeeperTextChoice = TEXT_THANK;
                            SharedData::GetInstance()->player->m_currency -= SharedData::GetInstance()->player->inventory[Item::TYPE_NET].GetUpgradeCost();
                        }
                        else
                            shopKeeperTextChoice = TEXT_AT_MAX_UPGRADE;
                    }
                    else
                        shopKeeperTextChoice = TEXT_INSUFFICIENT_COINS;
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

            if (currentShop == SHOP_UPGRADE)
                RenderUpgradeInterface(Item::TYPE_BAIT);

            if (Application::IsMousePressed(0))
                switch (currentShop)
                {
                case SHOP_MAIN:
                    currentShop = SHOP_BUY;
                    break;
                case SHOP_BUY:
                    isInTransaction = true;
                    currentItem = Item::TYPE_BAIT;
                    break;
                case SHOP_SELL:
                    isInTransaction = true;
                    currentItem = Item::TYPE_BAIT;
                    break;
                case SHOP_UPGRADE:
                    if (SharedData::GetInstance()->player->m_currency > SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].GetUpgradeCost())
                    {
                        if (SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].Upgrade())
                        {
                            shopKeeperTextChoice = TEXT_THANK;
                            SharedData::GetInstance()->player->m_currency -= SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].GetUpgradeCost();
                        }
                        else
                            shopKeeperTextChoice = TEXT_AT_MAX_UPGRADE;
                    }
                    else
                        shopKeeperTextChoice = TEXT_INSUFFICIENT_COINS;
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

            if (currentShop == SHOP_UPGRADE)
                RenderUpgradeInterface(Item::TYPE_ROCK);

            if (Application::IsMousePressed(0))
                switch (currentShop)
            {
                case SHOP_MAIN:
                    isInTransaction = true;
                    currentShop = SHOP_SELL;
                    break;
                case SHOP_BUY:
                    isInTransaction = true;
                    currentItem = Item::TYPE_TRAP;
                    break;
                case SHOP_SELL:
                    isInTransaction = true;
                    currentItem = Item::TYPE_TRAP;
                    break;
                case SHOP_UPGRADE:
                    if (SharedData::GetInstance()->player->m_currency > SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP].GetUpgradeCost())
                    {
                        if (SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP].Upgrade())
                        {
                            shopKeeperTextChoice = TEXT_THANK;
                            SharedData::GetInstance()->player->m_currency -= SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP].GetUpgradeCost();
                        }
                        else
                            shopKeeperTextChoice = TEXT_AT_MAX_UPGRADE;
                    }
                    else
                        shopKeeperTextChoice = TEXT_INSUFFICIENT_COINS;
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

            if (currentShop == SHOP_UPGRADE)
                RenderUpgradeInterface(Item::TYPE_ROCK);

            if (Application::IsMousePressed(0))
                switch (currentShop)
            {
                case SHOP_MAIN:
                    currentShop = SHOP_UPGRADE;
                    break;
                case SHOP_BUY:
                    isInTransaction = true;
                    currentItem = Item::TYPE_ROCK;
                    break;
                case SHOP_SELL:
                    isInTransaction = true;
                    currentItem = Item::TYPE_ROCK;
                    break;
                case SHOP_UPGRADE:
                    if (SharedData::GetInstance()->player->m_currency > SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].GetUpgradeCost())
                    {
                        if (SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].Upgrade())
                        {
                            SharedData::GetInstance()->player->m_currency -= SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].GetUpgradeCost();
                            shopKeeperTextChoice = TEXT_THANK;
                        }
                        else
                            shopKeeperTextChoice = TEXT_AT_MAX_UPGRADE;
                    }
                    else
                        shopKeeperTextChoice = TEXT_INSUFFICIENT_COINS;
                    break;
            }
        }
        else
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 12.5f, 3.5f, 1.f, 25.f, 20.f, 0.f, 0.f, 0.f, false);

        //Exit or back one page
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
                    shopKeeperTextChoice = TEXT_NONE;
                    transactionCounter = 1;
                    isInTransaction = false;
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
    case SHOP_SELL:
    case SHOP_UPGRADE:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Net", Color(1, 1, 0), 3.f, 18.75f, 48.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Bait", Color(1, 1, 0), 3.f, 18.75f, 38.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Trap", Color(1, 1, 0), 3.f, 18.75f, 28.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Rock", Color(1, 1, 0), 3.f, 18.75f, 18.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Back", Color(1, 1, 0), 2.5f, 18.75f, 8.5f);
        break;
    }

    //Always Render Player coins
    std::stringstream ss;
    ss << "Coins: " << SharedData::GetInstance()->player->m_currency;
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3.f, 40.f, 48.5f);

    SetHUD(false);
}

void SceneZoo::RenderEnclosureInterface()
{
    SetHUD(true);

    //std::cout << Application::cursorXPos / Application::m_width << " " << Application::cursorYPos / Application::m_height << std::endl;

    //Upgrade button
    if (Application::cursorXPos / Application::m_width >= 0.434896 &&
        Application::cursorXPos / Application::m_width <= 0.595833 &&
        Application::cursorYPos / Application::m_height >= 0.833333 &&
        Application::cursorYPos / Application::m_height <= 0.890741
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 13.f, 3.5f, 3.5f, 41.5f, 6.5f, 0.f, 0.f, 0.f, false);

        if (Application::IsMousePressed(0))
        {
            if (SharedData::GetInstance()->player->m_currency >= upgradeCost)
            {
                SharedData::GetInstance()->player->m_currency -= upgradeCost;
                UpgradeEnclosureSize(currentArea);
            }
            else
            {
                //Need someway to tell player is at max
            }
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 13.f, 3.5f, 3.5f, 41.5f, 6.5f, 0.f, 0.f, 0.f, false);

    //Back button
    if (Application::cursorXPos / Application::m_width >= 0.853646 &&
        Application::cursorXPos / Application::m_width <= 0.958838 &&
        Application::cursorYPos / Application::m_height >= 0.833333 &&
        Application::cursorYPos / Application::m_height <= 0.890741
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 8.5f, 3.5f, 3.5f, 73.f, 6.5f, 0.f, 0.f, 0.f, false);

        if (Application::IsMousePressed(0))
        {
            currentArea = AREA_OVERVIEW;
            currentState = STATE_ZOO;
            zooCamera.Reset();
            cycleIter = 0;
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 8.5f, 3.5f, 3.5f, 73.f, 6.5f, 0.f, 0.f, 0.f, false);

    switch (currentArea)
    {
    case AREA_GRASS:
    {
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Grass Zone", Color(1, 1, 0), 3.f, 34.5f, 10.f);
        std::stringstream ss;
        ss << "Size: " << grassZone.size() << " / " << grassAreaSize;
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3.f, 5.f, 5.f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Upgrade", Color(1, 1, 0), 3.f, 36.5f, 5.f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Back", Color(1, 1, 0), 3.f, 70.f, 5.f);

        break;
    }
    case AREA_FIRE:
    {
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Fire Zone", Color(1, 1, 0), 3.f, 34.5f, 10.f);
        std::stringstream ss;
        ss << "Size: " << fireZone.size() << " / " << fireAreaSize;
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3.f, 5.f, 5.f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Upgrade", Color(1, 1, 0), 3.f, 36.5f, 5.f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Back", Color(1, 1, 0), 3.f, 70.f, 5.f);

        break;
    }
    case AREA_ROCK:
    {
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Rock Zone", Color(1, 1, 0), 3.f, 34.5f, 10.f);
        std::stringstream ss;
        ss << "Size: " << rockZone.size() << " / " << rockAreaSize;
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3.f, 5.f, 5.f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Upgrade", Color(1, 1, 0), 3.f, 36.5f, 5.f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Back", Color(1, 1, 0), 3.f, 70.f, 5.f);

        break;
    }

    case AREA_SWAMP:
    {
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Swamp Zone", Color(1, 1, 0), 3.f, 34.5f, 10.f);
        std::stringstream ss;
        ss << "Size: " << swampZone.size() << " / " << swampAreaSize;
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3.f, 5.f, 5.f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Upgrade", Color(1, 1, 0), 3.f, 36.5f, 5.f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Back", Color(1, 1, 0), 3.f, 70.f, 5.f);

        break;
    }
    }

    SetHUD(false);
}

void SceneZoo::RenderTransactionInterface()
{
    SetHUD(true);

    if (Application::cursorXPos / Application::m_width >= 0.54375 &&
        Application::cursorXPos / Application::m_width <= 0.586458 &&
        Application::cursorYPos / Application::m_height <= 0.624074 &&
        Application::cursorYPos / Application::m_height >= 0.568519)
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 3.5f, 3.5f, 3.5f, 45.5f, 23.f, 0.f, 0.f, 0.f, false);
        if (Application::IsMousePressed(0))
            transactionCounter--;

        if (transactionCounter <= 0)
            transactionCounter = 0;
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 3.5f, 3.5f, 3.5f, 45.5f, 23.f, 0.f, 0.f, 0.f, false);
        
    if (Application::cursorXPos / Application::m_width >= 0.654687 &&
        Application::cursorXPos / Application::m_width <= 0.68948 &&
        Application::cursorYPos / Application::m_height <= 0.624074 &&
        Application::cursorYPos / Application::m_height >= 0.568519)
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 3.5f, 3.5f, 3.5f, 54.5f, 23.f, 0.f, 0.f, 0.f, false);
        if (Application::IsMousePressed(0))
            transactionCounter++;
        if (currentShop == SHOP_SELL && transactionCounter >= SharedData::GetInstance()->player->inventory[currentItem].GetCount())
            transactionCounter = SharedData::GetInstance()->player->inventory[currentItem].GetCount();
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 3.5f, 3.5f, 3.5f, 54.5f, 23.f, 0.f, 0.f, 0.f, false);

    if (Application::cursorXPos / Application::m_width >= 0.54375 &&
        Application::cursorXPos / Application::m_width <= 0.68948 &&
        Application::cursorYPos / Application::m_height <= 0.698148 &&
        Application::cursorYPos / Application::m_height >= 0.639815)
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 12.5f, 3.5f, 3.5f, 50.f, 18.5f, 0.f, 0.f, 0.f, false);
        if (Application::IsMousePressed(0))
            switch (currentShop)
            {
            case SHOP_BUY:
                if (SharedData::GetInstance()->player->inventory[currentItem].Buy(transactionCounter))
                {
                    isInTransaction = false;
                    shopKeeperTextChoice = TEXT_THANK;
                    transactionCounter = 1;
                }
                else
                    shopKeeperTextChoice = TEXT_INSUFFICIENT_COINS;
               break;

            case SHOP_SELL:
                if (SharedData::GetInstance()->player->inventory[currentItem].Sell(transactionCounter))
                {
                    isInTransaction = false;
                    shopKeeperTextChoice = TEXT_THANK;
                    transactionCounter = 1;
                }
               break;
            }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 12.5f, 3.5f, 3.5f, 50.f, 18.5f, 0.f, 0.f, 0.f, false);


    std::stringstream ss1;
    ss1 << "Total: " << transactionCounter * SharedData::GetInstance()->player->inventory[currentItem].GetBuyCost();
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss1.str(), Color(1, 1, 0), 3.f, 45.f, 25.5f);

    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "<", Color(1, 1, 0), 3.f, 45.5f, 21.f);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ">", Color(1, 1, 0), 3.f, 52.f, 21.f);
    std::stringstream ss;
    ss << transactionCounter;
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3.f, 48.25f, 21.5f);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Confirm", Color(1, 1, 0), 3.f, 44.75f, 17.f);

    SetHUD(false);
}

void SceneZoo::RenderUpgradeInterface(Item::TYPE item)
{
    std::stringstream ss;
    ss << "Level: " << SharedData::GetInstance()->player->inventory[item].GetCurrentUpgradeLevel() << " / " << MAX_UPGRADE_LEVEL;
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3.f, 40.5f, 21.f);
}

void SceneZoo::RenderHUD()
{
    std::stringstream ss, ss2, ss3, ss4, ss5;
    ss << "Rocks: " << SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].GetCount();
    ss2 << "Nets: " << SharedData::GetInstance()->player->inventory[Item::TYPE_NET].GetCount();
    ss3 << "Baits: " << SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].GetCount();
    ss4 << "Traps: " << SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP].GetCount();
    ss5 << "Meat: " << SharedData::GetInstance()->player->inventory[Item::TYPE_MEAT].GetCount();

    RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_HUD), false, 80.f, 12.f, 0, -48);

    // Background
    RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BOX_TRANSLUCENT), false, 80.f, 12.f, 0, -48);

    // 1: Rock
    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCKS1), 2, 25.0f, 5.5f, f_Rotate, f_Rotate, 0, false);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 1, 23.5f, 2.5f);
    // 2: Net
    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET), 1, 32.5f, 4.5f, 0, f_Rotate, 0, false);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss2.str(), Color(1, 1, 0), 1, 31.0f, 2.5f);
    // 3: Bait
    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BAIT), 2, 40.0f, 5.5f, f_Rotate, f_Rotate, 0, false);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss3.str(), Color(1, 1, 0), 1, 38.5f, 2.5f);
    // 4: Trap
    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TRAP), 2, 47.5f, 5.5f, -25.f, f_Rotate, 0, false);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss4.str(), Color(1, 1, 0), 1, 46.0f, 2.5f);
    // 5: Meat
    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MEAT), 2, 55.0f, 5.2f, 45.f, 45.f, 0, false);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss5.str(), Color(1, 1, 0), 1, 53.5f, 2.5f);

}

void SceneZoo::SellMonster()
{
    std::vector<GameObject> temp;

    switch (currentArea)
    {
    case AREA_GRASS:

        for (unsigned i = 0; i < grassZone.size(); ++i)
        {
            if (i == cycleIter)
                destroyGO(&zooWorld, grassZone[cycleIter]);
            else
                temp.push_back(grassZone[i]);
        }

        grassZone.clear();

        if (temp.size())
        {
            grassZone = temp;
            cycleIter = 0;
            targetedMonster = grassZone[cycleIter];
        }
        else
        {
            cycleIter = 0;
            zooCamera.position = grassAreaPosition + Vector3(0, 75, -50);
            zooCamera.target = grassAreaPosition;
            currentArea = AREA_GRASS;
            targetedMonster = NULL;

            isFollowingMonster = false;
            currentState = STATE_ENCLOSURE;
        }

        break;
    case AREA_FIRE:

        for (unsigned i = 0; i < fireZone.size(); ++i)
        {
            if (i == cycleIter)
                destroyGO(&zooWorld, fireZone[cycleIter]);
            else
                temp.push_back(fireZone[i]);
        }

        fireZone.clear();

        if (temp.size())
        {
            fireZone = temp;
            cycleIter = 0;
            targetedMonster = fireZone[cycleIter];
        }
        else
        {
            cycleIter = 0;
            zooCamera.position = fireAreaPosition + Vector3(0, 75, -50);
            zooCamera.target = fireAreaPosition;
            currentArea = AREA_FIRE;
            targetedMonster = NULL;

            isFollowingMonster = false;
            currentState = STATE_ENCLOSURE;
        }

        break;
    case AREA_ROCK:

        for (unsigned i = 0; i < rockZone.size(); ++i)
        {
            if (i == cycleIter)
                destroyGO(&zooWorld, rockZone[cycleIter]);
            else
                temp.push_back(rockZone[i]);
        }

        rockZone.clear();

        if (temp.size())
        {
            rockZone = temp;
            cycleIter = 0;
            targetedMonster = rockZone[cycleIter];
        }
        else
        {
            cycleIter = 0;
            zooCamera.position = rockAreaPosition + Vector3(0, 75, -50);
            zooCamera.target = rockAreaPosition;
            currentArea = AREA_ROCK;
            targetedMonster = NULL;

            isFollowingMonster = false;
            currentState = STATE_ENCLOSURE;
        }

        break;
    case AREA_SWAMP:

        for (unsigned i = 0; i < swampZone.size(); ++i)
        {
            if (i == cycleIter)
                destroyGO(&zooWorld, swampZone[cycleIter]);
            else
                temp.push_back(swampZone[i]);
        }

        swampZone.clear();

        if (temp.size())
        {
            swampZone = temp;
            cycleIter = 0;
            targetedMonster = swampZone[cycleIter];
        }
        else
        {
            cycleIter = 0;
            zooCamera.position = swampAreaPosition + Vector3(0, 75, -50);
            zooCamera.target = swampAreaPosition;
            currentArea = AREA_SWAMP;
            targetedMonster = NULL;

            isFollowingMonster = false;
            currentState = STATE_ENCLOSURE;
        }

        break;
    }

    SharedData::GetInstance()->player->m_currency += 100; //Edit with monster's value
    
}

void SceneZoo::SlaughterMonster()
{
    std::vector<GameObject> temp;

    switch (currentArea)
    {
    case AREA_GRASS:

        for (unsigned i = 0; i < grassZone.size(); ++i)
        {
            if (i == cycleIter)
                destroyGO(&zooWorld, grassZone[cycleIter]);
            else
                temp.push_back(grassZone[i]);
        }

        grassZone.clear();

        if (temp.size())
        {
            grassZone = temp;
            cycleIter = 0;
            targetedMonster = grassZone[cycleIter];
        }
        else
        {
            cycleIter = 0;
            zooCamera.position = grassAreaPosition + Vector3(0, 75, -50);
            zooCamera.target = grassAreaPosition;
            currentArea = AREA_GRASS;
            targetedMonster = NULL;

            isFollowingMonster = false;
            currentState = STATE_ENCLOSURE;
        }

        break;
    case AREA_FIRE:

        for (unsigned i = 0; i < fireZone.size(); ++i)
        {
            if (i == cycleIter)
                destroyGO(&zooWorld, fireZone[cycleIter]);
            else
                temp.push_back(fireZone[i]);
        }

        fireZone.clear();

        if (temp.size())
        {
            fireZone = temp;
            cycleIter = 0;
            targetedMonster = fireZone[cycleIter];
        }
        else
        {
            cycleIter = 0;
            zooCamera.position = fireAreaPosition + Vector3(0, 75, -50);
            zooCamera.target = fireAreaPosition;
            currentArea = AREA_FIRE;
            targetedMonster = NULL;

            isFollowingMonster = false;
            currentState = STATE_ENCLOSURE;
        }

        break;
    case AREA_ROCK:

        for (unsigned i = 0; i < rockZone.size(); ++i)
        {
            if (i == cycleIter)
                destroyGO(&zooWorld, rockZone[cycleIter]);
            else
                temp.push_back(rockZone[i]);
        }

        rockZone.clear();

        if (temp.size())
        {
            rockZone = temp;
            cycleIter = 0;
            targetedMonster = rockZone[cycleIter];
        }
        else
        {
            cycleIter = 0;
            zooCamera.position = rockAreaPosition + Vector3(0, 75, -50);
            zooCamera.target = rockAreaPosition;
            currentArea = AREA_ROCK;
            targetedMonster = NULL;

            isFollowingMonster = false;
            currentState = STATE_ENCLOSURE;
        }

        break;
    case AREA_SWAMP:

        for (unsigned i = 0; i < swampZone.size(); ++i)
        {
            if (i == cycleIter)
                destroyGO(&zooWorld, swampZone[cycleIter]);
            else
                temp.push_back(swampZone[i]);
        }

        swampZone.clear();

        if (temp.size())
        {
            swampZone = temp;
            cycleIter = 0;
            targetedMonster = swampZone[cycleIter];
        }
        else
        {
            cycleIter = 0;
            zooCamera.position = swampAreaPosition + Vector3(0, 75, -50);
            zooCamera.target = swampAreaPosition;
            currentArea = AREA_SWAMP;
            targetedMonster = NULL;

            isFollowingMonster = false;
            currentState = STATE_ENCLOSURE;
        }

        break;
    }

    SharedData::GetInstance()->player->inventory[Item::TYPE_MEAT].Add(1);

}

void SceneZoo::RenderHuntingScenesInterface()
{
    SetHUD(true);

    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_BACKGROUND), 55.f, 40.f, 30.f, false);

    //Grass scene
    if (Application::cursorXPos / Application::m_width >= 0.189602 &&
        Application::cursorXPos / Application::m_width <= 0.46875 &&
        Application::cursorYPos / Application::m_height >= 0.212037 &&
        Application::cursorYPos / Application::m_height <= 0.496296)
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 22.5f, 17.5f, 1.f, 26.5f, 38.f, 0, 0, 0, false);

        if (Application::IsMousePressed(0))
            changeSceneTo = AREA_GRASS;
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 22.5f, 17.5f, 1.f, 26.5f, 38.f, 0, 0, 0, false);

    //Fire Scene
    if (Application::cursorXPos / Application::m_width >= 0.525 &&
        Application::cursorXPos / Application::m_width <= 0.803646 &&
        Application::cursorYPos / Application::m_height >= 0.212037 &&
        Application::cursorYPos / Application::m_height <= 0.496296)
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 22.5f, 17.5f, 1.f, 53.5f, 38.f, 0, 0, 0, false);

        if (Application::IsMousePressed(0))
            changeSceneTo = AREA_FIRE;
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 22.5f, 17.5f, 1.f, 53.5f, 38.f, 0, 0, 0, false);

    //Rock scene
    if (Application::cursorXPos / Application::m_width >= 0.189602 &&
        Application::cursorXPos / Application::m_width <= 0.46875 &&
        Application::cursorYPos / Application::m_height >= 0.536111 &&
        Application::cursorYPos / Application::m_height <= 0.818519)
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 22.5f, 17.5f, 1.f, 26.5f, 18.f, 0, 0, 0, false);

        if (Application::IsMousePressed(0))
            changeSceneTo = AREA_ROCK;

    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 22.5f, 17.5f, 1.f, 26.5f, 18.f, 0, 0, 0, false);

    //Swamp Scene
    if (Application::cursorXPos / Application::m_width >= 0.525 &&
        Application::cursorXPos / Application::m_width <= 0.803646 &&
        Application::cursorYPos / Application::m_height >= 0.536111 &&
        Application::cursorYPos / Application::m_height <= 0.818519)
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 22.5f, 17.5f, 1.f, 53.5f, 18.f, 0, 0, 0, false);
        
        if (Application::IsMousePressed(0))
            changeSceneTo = AREA_SWAMP;
    }
    else 
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 22.5f, 17.5f, 1.f, 53.5f, 18.f, 0, 0, 0, false);

    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Go Hunting!", Color(1, 1, 0), 4.f, 30.f, 48.5f);

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