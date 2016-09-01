#include "GL\glew.h"

#include "SceneZoo.h"
#include "../../General/Application.h"
//#include "../../Graphics/LoadOBJ/LoadTGA.h"
//#include "../../Graphics/Mesh/MeshBuilder.h"
#include "../../General/SharedData.h"
#include "../../GameObject/MonsterFactory.h"

#include <sstream>

SceneZoo::SceneZoo(std::string name) : Scene(name), AREA_MAX_SIZE(50)
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

    zooCamera.Init(Vector3(0, 200, -150), Vector3(0, 0, -20), Vector3(0, 1, 0));

    Application::GetCursorPos(&Application::cursorXPos, &Application::cursorYPos);
    Application::SetCursorPos(Application::GetWindowWidth() / 2.f, Application::GetWindowHeight() / 2.f);

    grassAreaPosition.Set(50.f, 0.f, 50.f);
    fireAreaPosition.Set(-50.f, 0.f, -50.f);
    rockAreaPosition.Set(-50.f, 0.f, 50.f);
    swampAreaPosition.Set(50.f, 0.f, -50.f);

    memset(&zooWorld, 0, sizeof(zooWorld));

    grassAreaSize = 10;
    fireAreaSize = 10;
    rockAreaSize = 10;
    swampAreaSize = 10;

    /*SharedData::GetInstance()->player->monsterList.push_back("Bird");
    SharedData::GetInstance()->player->monsterList.push_back("Bird");
    SharedData::GetInstance()->player->monsterList.push_back("Bird");
    SharedData::GetInstance()->player->monsterList.push_back("Bird");
    SharedData::GetInstance()->player->monsterList.push_back("Bird");
    SharedData::GetInstance()->player->monsterList.push_back("Bird");
    SharedData::GetInstance()->player->monsterList.push_back("Bird");
    SharedData::GetInstance()->player->monsterList.push_back("Bird");
    SharedData::GetInstance()->player->monsterList.push_back("Bird");
    SharedData::GetInstance()->player->monsterList.push_back("Bird");*/

    populateMonsterList();

    isFollowingMonster = false;
    cycleIter = 0;

    currentArea = AREA_OVERVIEW;
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

    for (unsigned i = 0; i < 50; ++i)
    {
        treeRands[i] = grassAreaPosition + Vector3(Math::RandFloatMinMax(-50.0f, 50.0f), 0, (Math::RandFloatMinMax(-50.0f, 50.0f)));
        volcanoRands[i] = fireAreaPosition + Vector3(Math::RandFloatMinMax(-50.0f, 50.0f), 0, (Math::RandFloatMinMax(-50.0f, 50.0f)));
        rockRands[i] = rockAreaPosition + Vector3(Math::RandFloatMinMax(-50.0f, 50.0f), 0, (Math::RandFloatMinMax(-50.0f, 50.0f)));
        swampPlantRands[i] = swampAreaPosition + Vector3(Math::RandFloatMinMax(-50.0f, 50.0f), 0, (Math::RandFloatMinMax(-50.0f, 50.0f)));
    }
}

void SceneZoo::Update(double dt)
{
    fps = (float)(1.f / dt);
    //===============================================================================================================================//
    //                                                            Updates                                                            //
    //===============================================================================================================================//

    //Movement update for Gameobjects
    UpdateGameObjects(&zooWorld, dt);

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
        if (zooWorld.position[grassZone[i]].x < grassAreaPosition.x - grassAreaSize + 5 ||
            zooWorld.position[grassZone[i]].x > grassAreaPosition.x + grassAreaSize - 5 ||
            zooWorld.position[grassZone[i]].z < grassAreaPosition.z - grassAreaSize + 5 ||
            zooWorld.position[grassZone[i]].z > grassAreaPosition.z + grassAreaSize - 5)
            zooWorld.velocity[grassZone[i]] = -zooWorld.velocity[grassZone[i]];
    }

    for (unsigned i = 0; i < fireZone.size(); ++i)
    {
        if (zooWorld.position[fireZone[i]].x < fireAreaPosition.x - fireAreaSize + 5 ||
            zooWorld.position[fireZone[i]].x > fireAreaPosition.x + fireAreaSize - 5 ||
            zooWorld.position[fireZone[i]].z < fireAreaPosition.z - fireAreaSize + 5 ||
            zooWorld.position[fireZone[i]].z > fireAreaPosition.z + fireAreaSize - 5)
            zooWorld.velocity[fireZone[i]] = -zooWorld.velocity[fireZone[i]];
    }

    for (unsigned i = 0; i < rockZone.size(); ++i)
    {
        if (zooWorld.position[rockZone[i]].x < rockAreaPosition.x - rockAreaSize + 5 ||
            zooWorld.position[rockZone[i]].x > rockAreaPosition.x + rockAreaSize - 5 ||
            zooWorld.position[rockZone[i]].z < rockAreaPosition.z - rockAreaSize + 5 ||
            zooWorld.position[rockZone[i]].z > rockAreaPosition.z + rockAreaSize - 5)
            zooWorld.velocity[rockZone[i]] = -zooWorld.velocity[rockZone[i]];
    }

    for (unsigned i = 0; i < swampZone.size(); ++i)
    {
        if (zooWorld.position[swampZone[i]].x < swampAreaPosition.x - swampAreaSize + 5 ||
            zooWorld.position[swampZone[i]].x > swampAreaPosition.x + swampAreaSize - 5 ||
            zooWorld.position[swampZone[i]].z < swampAreaPosition.z - swampAreaSize + 5 ||
            zooWorld.position[swampZone[i]].z > swampAreaPosition.z + swampAreaSize - 5)
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
        zooCamera.position = swampAreaPosition + Vector3(0, 75, -50);
        zooCamera.target = swampAreaPosition;

        currentArea = AREA_SWAMP;

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
        zooCamera.position = fireAreaPosition + Vector3(0, 75, -50);
        zooCamera.target = fireAreaPosition;

        currentArea = AREA_FIRE;

        isFollowingMonster = false;
        currentState = STATE_ENCLOSURE;
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

    //Render World
    RenderZooScene();

    //RenderGameObjects
    RenderGameObjects(&zooWorld);

    //Render Enclosures
    RenderEnclosures();

    switch (currentState)
    {
    case STATE_ZOO:

        if (currentArea == AREA_OVERVIEW)
            RenderOverviewInterface();

        break;
    case STATE_ENCLOSURE:

        if (isFollowingMonster)
            DisplayMonsterInterface(zooWorld.monster[targetedMonster]);
        else
            RenderEnclosureInterface();

        break;
    case STATE_SHOP:

        RenderShopInterface();
        RenderHUD();

        if (isInTransaction)
            RenderTransactionInterface();

        RenderShopkeeperText();

        break;
    case STATE_CHANGE_SCENE:

        RenderHuntingScenesInterface();

        break;
    case STATE_MENU:
        
        RenderMenuInterface();

        break;
    case STATE_QUEST:

        RenderQuestInterface();

        break;
    }

    std::cout << Application::cursorXPos / Application::m_width << " | " << Application::cursorYPos / Application::m_height << std::endl;
}

void SceneZoo::RenderZooScene()
{
    // Grass Ground mesh
    modelStack.PushMatrix();
    modelStack.Translate(50, 0, 50);
    modelStack.Rotate(-90, 1, 0, 0);
    modelStack.Scale(100, 100, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ZOO_GRASS_GROUND), true);
    modelStack.PopMatrix();

    // Swamp Ground mesh
    modelStack.PushMatrix();
    modelStack.Translate(50, 0, -50);
    modelStack.Rotate(-90, 1, 0, 0);
    modelStack.Scale(100, 100, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ZOO_SWAMP_GROUND), true);
    modelStack.PopMatrix();

    // Rock Ground mesh
    modelStack.PushMatrix();
    modelStack.Translate(-50, 0, 50);
    modelStack.Rotate(-90, 1, 0, 0);
    modelStack.Scale(100, 100, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ZOO_ROCK_GROUND), true);
    modelStack.PopMatrix();

    // Lava Ground mesh
    modelStack.PushMatrix();
    modelStack.Translate(-50, 0, -50);
    modelStack.Rotate(-90, 1, 0, 0);
    modelStack.Scale(100, 100, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ZOO_LAVA_GROUND), true);
    modelStack.PopMatrix();

    // Dirt Ground mesh
    modelStack.PushMatrix();
    modelStack.Translate(0, -1, 0);
    modelStack.Rotate(-90, 1, 0, 0);
    modelStack.Scale(400, 400, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ZOO_DIRT_GROUND), true);
    modelStack.PopMatrix();

    for (unsigned i = 0; i < 50; ++i)
    {
        modelStack.PushMatrix();
        modelStack.Translate(treeRands[i].x, 0, treeRands[i].z);
        modelStack.Scale(1, 1, 1);
        RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TREE1), true);
        modelStack.PopMatrix();

        modelStack.PushMatrix();
        modelStack.Translate(volcanoRands[i].x, 0, volcanoRands[i].z);
        modelStack.Scale(0.5, 0.5, 0.5);
        RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_VOLCANO), true);
        modelStack.PopMatrix();

        modelStack.PushMatrix();
        modelStack.Translate(rockRands[i].x, 0, rockRands[i].z);
        modelStack.Scale(1, 1, 1);
        RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCK_ROCK), true);
        modelStack.PopMatrix();

        modelStack.PushMatrix();
        modelStack.Translate(swampPlantRands[i].x, 0, swampPlantRands[i].z);
        modelStack.Scale(1, 1, 1);
        RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SWAMP_ROOT), true);
        modelStack.PopMatrix();
    }
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
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BOSS_FAIRY);
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
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_GRIMEJAM);
            zooWorld.appearance[go].scale.Set(1, 1, 1);
            zooWorld.monster[go] = MonsterFactory::CreateMonster("Grimejam");

            swampZone.push_back(go);
        }
        else if (SharedData::GetInstance()->player->monsterList[i] == "SeaMonster")
        {
            GameObject go = createGO(&zooWorld);
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_AI;
            zooWorld.position[go] = swampAreaPosition + randOffset;
            zooWorld.velocity[go].Set(Math::RandFloatMinMax(-1.f, 1.f), 0.f, Math::RandFloatMinMax(-1.f, 1.f));
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_SEAMONSTER);
            zooWorld.appearance[go].scale.Set(1, 1, 1);
            zooWorld.monster[go] = MonsterFactory::CreateMonster("SeaMonster");

            swampZone.push_back(go);
        }
        else if (SharedData::GetInstance()->player->monsterList[i] == "MukBoss")
        {
            GameObject go = createGO(&zooWorld);
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_AI;
            zooWorld.position[go] = swampAreaPosition + randOffset;
            zooWorld.velocity[go].Set(Math::RandFloatMinMax(-1.f, 1.f), 0.f, Math::RandFloatMinMax(-1.f, 1.f));
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BOSS_MUKBOSS);
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
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_FOSSIL);
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
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_GOLEM);
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
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BOSS_ROCKSNAKE);
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
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_FIREBUG);
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
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_MAGMA);
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
            zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BOSS_MAGMA_BERZEKER);
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

    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ENCLOSURE_INTERFACE), 80.f, 40.f, 30.f, false);

    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), monster->GetName(), Color(0.95, 0.95, 0), 3, 35, 55);

    std::stringstream ss;
    ss << "Health: " << monster->GetHealthStat();
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0.95, 0.95, 0), 3, 0, 45);

    std::stringstream ss1;
    ss1 << "Capture Rate: " << monster->GetCaptureRateStat();
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss1.str(), Color(0.95, 0.95, 0), 2, 0, 35);

    std::stringstream ss2;
    ss2 << "Aggresion: " << monster->GetAggressionStat();
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss2.str(), Color(0.95, 0.95, 0), 2, 0, 25);

    std::stringstream ss3;
    ss3 << "Fear: " << monster->GetFearStat();
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss3.str(), Color(0.95, 0.95, 0), 3, 0, 15);

    //Cycle previous button
    if (Application::cursorXPos / Application::m_width >= 0.240625 &&
        Application::cursorXPos / Application::m_width <= 0.308333 &&
        Application::cursorYPos / Application::m_height >= 0.425926 &&
        Application::cursorYPos / Application::m_height <= 0.515741
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 5.5f, 5.5f, 1.f, 22.f, 31., 0.f, 0.f, 0.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
        {
            switch (currentArea)
            {
            case AREA_GRASS:

                if (cycleIter <= 0)
                    cycleIter = grassZone.size() - 1;
                else
                    cycleIter--;

                targetedMonster = grassZone[cycleIter];

                break;

            case AREA_FIRE:

                if (cycleIter <= 0)
                    cycleIter = fireZone.size() - 1;
                else
                    cycleIter--;

                targetedMonster = fireZone[cycleIter];

                break;

            case AREA_ROCK:

                if (cycleIter <= 0)
                    cycleIter = rockZone.size() - 1;
                else
                    cycleIter--;

                targetedMonster = rockZone[cycleIter];

                break;

            case AREA_SWAMP:

                if (cycleIter <= 0)
                    cycleIter = swampZone.size() - 1;
                else
                    cycleIter--;

                targetedMonster = swampZone[cycleIter];

                break;
            }
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 5.5f, 5.5f, 1.f, 22.f, 31., 0.f, 0.f, 0.f, false);

    //Cycle Next button
    if (Application::cursorXPos / Application::m_width >= 0.6777083 &&
        Application::cursorXPos / Application::m_width <= 0.744792 &&
        Application::cursorYPos / Application::m_height >= 0.425926 &&
        Application::cursorYPos / Application::m_height <= 0.515741
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 5.5f, 5.5f, 1.f, 57.f, 31., 0.f, 0.f, 0.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
        {
            switch (currentArea)
            {
            case AREA_GRASS:

                if (++cycleIter >= grassZone.size())
                    cycleIter = 0;

                targetedMonster = grassZone[cycleIter];

                break;

            case AREA_FIRE:

                if (++cycleIter >= fireZone.size())
                    cycleIter = 0;

                targetedMonster = fireZone[cycleIter];

                break;

            case AREA_ROCK:

                if (++cycleIter >= rockZone.size())
                    cycleIter = 0;

                targetedMonster = rockZone[cycleIter];

                break;

            case AREA_SWAMP:

                if (++cycleIter >= swampZone.size())
                    cycleIter = 0;

                targetedMonster = swampZone[cycleIter];

                break;
            }
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 5.5f, 5.5f, 1.f, 57.f, 31., 0.f, 0.f, 0.f, false);

    //Slaughter button
    if (Application::cursorXPos / Application::m_width >= 0.804688 &&
        Application::cursorXPos / Application::m_width <= 0.955208 &&
        Application::cursorYPos / Application::m_height >= 0.361111 &&
        Application::cursorYPos / Application::m_height <= 0.436111
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 14.5f, 5.5f, 1.f, 71.f, 36.f, 0.f, 0.f, 0.f, false);

        rotateEnclosureIcon2 += 2.f;

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
        {
            SlaughterMonster();
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 14.5f, 5.5f, 1.f, 71.f, 36.f, 0.f, 0.f, 0.f, false);

    // Sell button
    if (Application::cursorXPos / Application::m_width >= 0.804688 &&
        Application::cursorXPos / Application::m_width <= 0.955208 &&
        Application::cursorYPos / Application::m_height >= 0.510185 &&
        Application::cursorYPos / Application::m_height <= 0.568519
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 14.5f, 5.5f, 1.f, 71.f, 26.f, 0.f, 0.f, 0.f, false);

        rotateEnclosureIcon1 += 2.f;

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed && grassZone.size())
        {
            SellMonster();
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 14.5f, 5.5f, 1.f, 71.f, 26.f, 0.f, 0.f, 0.f, false);


    // Back button
    if (Application::cursorXPos / Application::m_width >= 0.804688 &&
        Application::cursorXPos / Application::m_width <= 0.955208 &&
        Application::cursorYPos / Application::m_height >= 0.835185 &&
        Application::cursorYPos / Application::m_height <= 0.919444
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 14.5f, 5.5f, 1.f, 72.f, 6.f, 0.f, 0.f, 0.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
        {
            cycleIter = 0;
            targetedMonster = NULL;

            switch (currentArea)
            {
            case AREA_GRASS:

                zooCamera.position = grassAreaPosition + Vector3(0, 75, -50);
                zooCamera.target = grassAreaPosition;

                break;

            case AREA_FIRE:

                zooCamera.position = fireAreaPosition + Vector3(0, 75, -50);
                zooCamera.target = fireAreaPosition;

                break;

            case AREA_ROCK:

                zooCamera.position = rockAreaPosition + Vector3(0, 75, -50);
                zooCamera.target = rockAreaPosition;

                break;

            case AREA_SWAMP:

                zooCamera.position = swampAreaPosition + Vector3(0, 75, -50);
                zooCamera.target = swampAreaPosition;

                break;
            }

            isFollowingMonster = false;
            currentState = STATE_ENCLOSURE;
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 14.5f, 5.5f, 1.f, 72.f, 6.f, 0.f, 0.f, 0.f, false);

    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_COIN), 8, 75.0f, 25.f, 0, rotateEnclosureIcon1, 0, false);
    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MEAT), 2, 75.0f, 35.f, 90, 45.f, rotateEnclosureIcon2, false);

    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Back", Color(0.95, 0.95, 0), 2, 70.0f, 5.f);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Sell", Color(0.95, 0.95, 0), 2, 65.0f, 25.f);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Slaughter", Color(0.95, 0.95, 0), 2, 65.0f, 35.f);

    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ICON_LEFT_ARROW), 5.5f, 5.5f, 1.f, 22.f, 31.f, 0.f, 0.f, 0.f, false);
    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ICON_RIGHT_ARROW), 5.5f, 5.5f, 1.f, 57.f, 31.f, 0.f, 0.f, 0.f, false);

    SetHUD(false);
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
    //Grass
    modelStack.PushMatrix();
    modelStack.Translate(grassAreaPosition.x + grassAreaSize, 0, grassAreaPosition.z);
    modelStack.Rotate(90, 0, 1, 0);
    modelStack.Scale(grassAreaSize / 3.5, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FENCE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(grassAreaPosition.x - grassAreaSize, 0, grassAreaPosition.z);
    modelStack.Rotate(90, 0, 1, 0);
    modelStack.Scale(grassAreaSize / 3.5, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FENCE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(grassAreaPosition.x, 0, grassAreaPosition.z + grassAreaSize);
    modelStack.Scale(grassAreaSize / 3.5, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FENCE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(grassAreaPosition.x, 0, grassAreaPosition.z - grassAreaSize);
    modelStack.Scale(grassAreaSize / 3.5, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FENCE), false);
    modelStack.PopMatrix();

    //Fire
    modelStack.PushMatrix();
    modelStack.Translate(fireAreaPosition.x + fireAreaSize, 0, fireAreaPosition.z);
    modelStack.Rotate(90, 0, 1, 0);
    modelStack.Scale(fireAreaSize / 3.5, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FENCE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(fireAreaPosition.x - fireAreaSize, 0, fireAreaPosition.z);
    modelStack.Rotate(90, 0, 1, 0);
    modelStack.Scale(fireAreaSize / 3.5, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FENCE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(fireAreaPosition.x, 0, fireAreaPosition.z + fireAreaSize);
    modelStack.Scale(fireAreaSize / 3.5, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FENCE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(fireAreaPosition.x, 0, fireAreaPosition.z - fireAreaSize);
    modelStack.Scale(fireAreaSize / 3.5, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FENCE), false);
    modelStack.PopMatrix();

    //Rock
    modelStack.PushMatrix();
    modelStack.Translate(rockAreaPosition.x + rockAreaSize, 0, rockAreaPosition.z);
    modelStack.Rotate(90, 0, 1, 0);
    modelStack.Scale(rockAreaSize / 3.5, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FENCE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(rockAreaPosition.x - rockAreaSize, 0, rockAreaPosition.z);
    modelStack.Rotate(90, 0, 1, 0);
    modelStack.Scale(rockAreaSize / 3.5, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FENCE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(rockAreaPosition.x, 0, rockAreaPosition.z + rockAreaSize);
    modelStack.Scale(rockAreaSize / 3.5, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FENCE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(rockAreaPosition.x, 0, rockAreaPosition.z - rockAreaSize);
    modelStack.Scale(rockAreaSize / 3.5, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FENCE), false);
    modelStack.PopMatrix();

    //Swamp
    modelStack.PushMatrix();
    modelStack.Translate(swampAreaPosition.x + swampAreaSize, 0, swampAreaPosition.z);
    modelStack.Rotate(90, 0, 1, 0);
    modelStack.Scale(swampAreaSize / 3.5, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FENCE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(swampAreaPosition.x - swampAreaSize, 0, swampAreaPosition.z);
    modelStack.Rotate(90, 0, 1, 0);
    modelStack.Scale(swampAreaSize / 3.5, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FENCE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(swampAreaPosition.x, 0, swampAreaPosition.z + swampAreaSize);
    modelStack.Scale(swampAreaSize / 3.5, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FENCE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(swampAreaPosition.x, 0, swampAreaPosition.z - swampAreaSize);
    modelStack.Scale(swampAreaSize / 3.5, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FENCE), false);
    modelStack.PopMatrix();
}

void SceneZoo::RenderShopkeeperText()
{
    switch (shopKeeperTextChoice)
    {
    case TEXT_TIP_1:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "All monsters have", Color(0.95, 0.95, 0), 2.f, 40.f, 16.f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "different behaviours!", Color(0.95, 0.95, 0), 2.f, 40.f, 13.5f);
        break;

    case TEXT_TIP_2:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Remember to interact with", Color(0.95, 0.95, 0), 2.f, 40.f, 16.f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "the scene for extra coins!", Color(0.95, 0.95, 0), 2.f, 40.f, 13.5f);
        break;

    case TEXT_TIP_3:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Upgrading your items", Color(0.95, 0.95, 0), 2.f, 40.f, 16.f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "increase the effectiveness!", Color(0.95, 0.95, 0), 2.f, 40.f, 13.5f);
        break;

    case TEXT_TIP_4:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Bait can only be crafted", Color(0.95, 0.95, 0), 2.f, 40.f, 16.f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "with Meat!", Color(0.95, 0.95, 0), 2.f, 40.f, 13.5f);
        break;

    case TEXT_TIP_5:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Lower health means", Color(0.95, 0.95, 0), 2.f, 40.f, 16.f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "easier captures!", Color(0.95, 0.95, 0), 2.f, 40.f, 13.5f);
        break;

    case TEXT_THANK:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Thank you!", Color(0.95, 0.95, 0), 3.f, 45.f, 10.5f);
        break;

    case TEXT_INSUFFICIENT_COINS:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "You don't have", Color(0.95, 0.95, 0), 3.f, 42.5f, 12.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "enough Coins!", Color(0.95, 0.95, 0), 3.f, 42.7f, 10.f);
        break;

    case TEXT_AT_MAX_UPGRADE:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "I can't updgrade this Item", Color(0.95, 0.95, 0), 2.f, 40.5f, 12.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "any further!", Color(0.95, 0.95, 0), 2.f, 47.f, 10.5f);
        break;

    default:

        break;
    }
}

void SceneZoo::RenderShopInterface()
{
    SetHUD(true);

    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_BACKGROUND), 60.f, 40.f, 30.f, false);

    //Check selection
    if (Application::cursorXPos / Application::m_width >= .231f &&
        Application::cursorXPos / Application::m_width <= .389f)
    {
        //Talk - Net - Net - Net
        if (Application::cursorYPos / Application::m_height >= .1325f &&
            Application::cursorYPos / Application::m_height <= .188f)
        {
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 13.5f, 4.5f, 1.f, 25.f, 50.f, 0.f, 0.f, 0.f, false);

            if (currentShop == SHOP_UPGRADE)
                RenderUpgradeInterface(Item::TYPE_NET);

            if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
                switch (currentShop)
            {
                case SHOP_MAIN:
                    shopKeeperTextChoice = static_cast<SHOPKEEPER_TEXT>(Math::RandIntMinMax(1, 5));
                    break;
                case SHOP_BUY:
                    isInTransaction = true;
                    currentItem = Item::TYPE_NET;
                    transactionCounter = 1;
                    break;
                case SHOP_SELL:
                    isInTransaction = true;
                    currentItem = Item::TYPE_NET;
                    transactionCounter = 1;
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
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 13.5f, 4.5f, 1.f, 25.f, 50.f, 0.f, 0.f, 0.f, false);

        //Buy - Bait - Bait - Bait 
        if (Application::cursorYPos / Application::m_height >= .294f &&
            Application::cursorYPos / Application::m_height <= .3525f)
        {
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 13.5f, 4.5f, 1.f, 25.f, 40.f, 0.f, 0.f, 0.f, false);

            if (currentShop == SHOP_UPGRADE)
                RenderUpgradeInterface(Item::TYPE_BAIT);

            if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
                switch (currentShop)
            {
                case SHOP_MAIN:
                    currentShop = SHOP_BUY;
                    shopKeeperTextChoice = TEXT_NONE;
                    break;
                case SHOP_BUY:
                    isInTransaction = true;
                    currentItem = Item::TYPE_BAIT;
                    transactionCounter = 1;
                    break;
                case SHOP_SELL:
                    isInTransaction = true;
                    currentItem = Item::TYPE_BAIT;
                    transactionCounter = 1;
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
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 13.5f, 4.5f, 1.f, 25.f, 40.f, 0.f, 0.f, 0.f, false);

        //Sell - Trap - Trap - Trap
        if (Application::cursorYPos / Application::m_height >= .4555f &&
            Application::cursorYPos / Application::m_height <= .517f)
        {
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 13.5f, 4.5f, 1.f, 25.f, 30.f, 0.f, 0.f, 0.f, false);

            if (currentShop == SHOP_UPGRADE)
                RenderUpgradeInterface(Item::TYPE_TRAP);

            if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
                switch (currentShop)
            {
                case SHOP_MAIN:
                    currentShop = SHOP_SELL;
                    shopKeeperTextChoice = TEXT_NONE;
                    break;
                case SHOP_BUY:
                    isInTransaction = true;
                    currentItem = Item::TYPE_TRAP;
                    transactionCounter = 1;
                    break;
                case SHOP_SELL:
                    isInTransaction = true;
                    currentItem = Item::TYPE_TRAP;
                    transactionCounter = 1;
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
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 13.5f, 4.5f, 1.f, 25.f, 30.f, 0.f, 0.f, 0.f, false);

        //Upgrade - Rock - Rock - Rock
        if (Application::cursorYPos / Application::m_height >= .617f &&
            Application::cursorYPos / Application::m_height <= 0.6785f)
        {
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 13.5f, 4.5f, 1.f, 25.f, 20.f, 0.f, 0.f, 0.f, false);

            if (currentShop == SHOP_UPGRADE)
                RenderUpgradeInterface(Item::TYPE_ROCK);

            if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
                switch (currentShop)
            {
                case SHOP_MAIN:
                    currentShop = SHOP_UPGRADE;
                    shopKeeperTextChoice = TEXT_NONE;
                    break;
                case SHOP_BUY:
                    isInTransaction = true;
                    currentItem = Item::TYPE_ROCK;
                    transactionCounter = 1;
                    break;
                case SHOP_SELL:
                    isInTransaction = true;
                    currentItem = Item::TYPE_ROCK;
                    transactionCounter = 1;
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
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 13.5f, 4.5f, 1.f, 25.f, 20.f, 0.f, 0.f, 0.f, false);

        //Exit or back one page
        if (Application::cursorYPos / Application::m_height >= .7785f &&
            Application::cursorYPos / Application::m_height <= .84f)
        {
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 13.5f, 4.5f, 1.f, 25.f, 10.f, 0.f, 0.f, 0.f, false);

            if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
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
                    currentItem = Item::NUM_TYPE;
                    zooCamera.Reset();
                    break;
            }
        }
        else
            RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 13.5f, 4.5f, 1.f, 25.f, 10.f, 0.f, 0.f, 0.f, false);
    }
    else
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 13.5f, 4.5f, 1.f, 25.f, 50.f, 0.f, 0.f, 0.f, false);
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 13.5f, 4.5f, 1.f, 25.f, 40.f, 0.f, 0.f, 0.f, false);
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 13.5f, 4.5f, 1.f, 25.f, 30.f, 0.f, 0.f, 0.f, false);
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 13.5f, 4.5f, 1.f, 25.f, 20.f, 0.f, 0.f, 0.f, false);
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 13.5f, 4.5f, 1.f, 25.f, 10.f, 0.f, 0.f, 0.f, false);
    }

    //Selection text
    switch (currentShop)
    {
    case SHOP_MAIN:
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Talk", Color(0.95, 0.95, 0), 3.f, 18.75f, 48.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Buy", Color(0.95, 0.95, 0), 3.f, 18.75f, 38.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Sell", Color(0.95, 0.95, 0), 3.f, 18.75f, 28.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Upgrade", Color(0.95, 0.95, 0), 3.f, 18.75f, 18.5f);
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Back to Zoo", Color(0.95, 0.95, 0), 2.5f, 18.75f, 8.5f);
        break;
    case SHOP_BUY:
    {
        std::stringstream ss;
        ss << "Net: (" << SharedData::GetInstance()->player->inventory[Item::TYPE_NET].GetBuyCost() << ")";
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0.95, 0.95, 0), 3.f, 18.75f, 48.5f);

        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Bait: (1 Meat)", Color(0.95, 0.95, 0), 2.f, 18.75f, 38.5f);

        std::stringstream ss1;
        ss1 << "Trap: (" << SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP].GetBuyCost() << ")";
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss1.str(), Color(0.95, 0.95, 0), 3.f, 18.75f, 28.5f);

        std::stringstream ss2;
        ss2 << "Rock: (" << SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].GetBuyCost() << ")";
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss2.str(), Color(0.95, 0.95, 0), 3.f, 18.75f, 18.5f);

        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Back", Color(0.95, 0.95, 0), 2.5f, 18.75f, 8.5f);
        break;
    }
    case SHOP_SELL:
    {
        std::stringstream ss;
        ss << "Net: (" << SharedData::GetInstance()->player->inventory[Item::TYPE_NET].GetBuyCost() * 0.5 << ")";
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0.95, 0.95, 0), 3.f, 18.75f, 48.5f);

        std::stringstream ss3;
        ss3 << "Bait: (" << SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].GetBuyCost() * 0.5 << ")";
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss3.str(), Color(0.95, 0.95, 0), 3.f, 18.75f, 38.5f);

        std::stringstream ss1;
        ss1 << "Trap: (" << SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP].GetBuyCost() * 0.5 << ")";
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss1.str(), Color(0.95, 0.95, 0), 3.f, 18.75f, 28.5f);

        std::stringstream ss2;
        ss2 << "Rock: (" << SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].GetBuyCost() * 0.5 << ")";
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss2.str(), Color(0.95, 0.95, 0), 3.f, 18.75f, 18.5f);

        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Back", Color(0.95, 0.95, 0), 2.5f, 18.75f, 8.5f);
        break;
    }
    case SHOP_UPGRADE:
    {
        std::stringstream ss;
        ss << "Net: (" << SharedData::GetInstance()->player->inventory[Item::TYPE_NET].GetUpgradeCost() << ")";
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0.95, 0.95, 0), 3.f, 18.75f, 48.5f);

        std::stringstream ss3;
        ss3 << "Bait: (" << SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].GetUpgradeCost() << ")";
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss3.str(), Color(0.95, 0.95, 0), 3.f, 18.75f, 38.5f);

        std::stringstream ss1;
        ss1 << "Trap: (" << SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP].GetUpgradeCost() << ")";
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss1.str(), Color(0.95, 0.95, 0), 3.f, 18.75f, 28.5f);

        std::stringstream ss2;
        ss2 << "Rock: (" << SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].GetUpgradeCost() << ")";
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss2.str(), Color(0.95, 0.95, 0), 3.f, 18.75f, 18.5f);

        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Back", Color(0.95, 0.95, 0), 2.5f, 18.75f, 8.5f);
        break;
    }
    }

    //Always Render Player coins
    std::stringstream ss;
    ss << "Coins: " << SharedData::GetInstance()->player->m_currency;
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0.95, 0.95, 0), 3.f, 40.f, 50.f);

    SetHUD(false);
}

void SceneZoo::RenderEnclosureInterface()
{
    SetHUD(true);

    //Eye button
    if (Application::cursorXPos / Application::m_width >= 0.48229 &&
        Application::cursorXPos / Application::m_width <= 0.540625 &&
        Application::cursorYPos / Application::m_height >= 0.678604 &&
        Application::cursorYPos / Application::m_height <= 0.753704
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 5.f, 5.f, 1.f, 41.f, 16.f, 0.f, 0.f, 0.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
        {
            switch (currentArea)
            {
            case AREA_GRASS:

                if (grassZone.size())
                {
                    targetedMonster = grassZone[cycleIter];
                    isFollowingMonster = true;
                }

                break;

            case AREA_FIRE:

                if (fireZone.size())
                {
                    targetedMonster = fireZone[cycleIter];
                    isFollowingMonster = true;
                }

                break;

            case AREA_ROCK:

                if (rockZone.size())
                {
                    targetedMonster = rockZone[cycleIter];
                    isFollowingMonster = true;
                }

                break;

            case AREA_SWAMP:

                if (swampZone.size())
                {
                    targetedMonster = swampZone[cycleIter];
                    isFollowingMonster = true;
                }

                break;
            }

        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 5.f, 5.f, 1.f, 41.f, 16.f, 0.f, 0.f, 0.f, false);

    //Upgrade button
    if (Application::cursorXPos / Application::m_width >= 0.411458 &&
        Application::cursorXPos / Application::m_width <= 0.608333 &&
        Application::cursorYPos / Application::m_height >= 0.823333 &&
        Application::cursorYPos / Application::m_height <= 0.913889
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 16.f, 5.5f, 1.f, 41.f, 6.5f, 0.f, 0.f, 0.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
        {
            if (SharedData::GetInstance()->player->m_currency >= upgradeCost)
            {
                SharedData::GetInstance()->player->m_currency -= upgradeCost;
                UpgradeEnclosureSize(currentArea);
            }
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 16.f, 5.5f, 1.f, 41.f, 6.5f, 0.f, 0.f, 0.f, false);

    //Back button
    if (Application::cursorXPos / Application::m_width >= 0.853646 &&
        Application::cursorXPos / Application::m_width <= 0.958838 &&
        Application::cursorYPos / Application::m_height >= 0.833333 &&
        Application::cursorYPos / Application::m_height <= 0.923333
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 8.5f, 3.5f, 3.5f, 73.f, 6.5f, 0.f, 0.f, 0.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
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
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Grass Zone", Color(0.95, 0.95, 0), 3.f, 34.5f, 10.f);
        std::stringstream ss;
        ss << "Size: " << grassZone.size() << " / " << grassAreaSize;
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0.95, 0.95, 0), 3.f, 5.f, 5.f);

        if (grassZone.size() >= grassAreaSize)
            RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "FULL!", Color(1, 0, 0), 3.f, 9.f, 8.f);

        std::stringstream ss1;
        if (grassAreaSize != AREA_MAX_SIZE)
            ss1 << "Upgrade(" << upgradeCost << ")";
        else
            ss1 << "At Max Size";

        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss1.str(), Color(0.95, 0.95, 0), 2.f, 34.5f, 5.f);

        break;
    }
    case AREA_FIRE:
    {
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Fire Zone", Color(0.95, 0.95, 0), 3.f, 34.5f, 10.f);
        std::stringstream ss;
        ss << "Size: " << fireZone.size() << " / " << fireAreaSize;
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0.95, 0.95, 0), 3.f, 5.f, 5.f);

        if (fireZone.size() >= fireAreaSize)
            RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "FULL!", Color(1, 0, 0), 3.f, 9.f, 8.f);

        std::stringstream ss1;
        if (fireAreaSize != AREA_MAX_SIZE)
            ss1 << "Upgrade(" << upgradeCost << ")";
        else
            ss1 << "At Max Size";

        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss1.str(), Color(0.95, 0.95, 0), 2.f, 34.5f, 5.f);

        break;
    }
    case AREA_ROCK:
    {
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Rock Zone", Color(0.95, 0.95, 0), 3.f, 34.5f, 10.f);
        std::stringstream ss;
        ss << "Size: " << rockZone.size() << " / " << rockAreaSize;
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0.95, 0.95, 0), 3.f, 5.f, 5.f);

        if (rockZone.size() >= rockAreaSize)
            RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "FULL!", Color(1, 0, 0), 3.f, 9.f, 8.f);

        std::stringstream ss1;
        if (rockAreaSize != AREA_MAX_SIZE)
            ss1 << "Upgrade(" << upgradeCost << ")";
        else
            ss1 << "At Max Size";

        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss1.str(), Color(0.95, 0.95, 0), 2.f, 34.5f, 5.f);

        break;
    }

    case AREA_SWAMP:
    {
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Swamp Zone", Color(0.95, 0.95, 0), 3.f, 34.5f, 10.f);
        std::stringstream ss;
        ss << "Size: " << swampZone.size() << " / " << swampAreaSize;
        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0.95, 0.95, 0), 3.f, 5.f, 5.f);

        if (swampZone.size() >= swampAreaSize)
            RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "FULL!", Color(1, 0, 0), 3.f, 9.f, 8.f);

        std::stringstream ss1;
        if (swampAreaSize != AREA_MAX_SIZE)
            ss1 << "Upgrade(" << upgradeCost << ")";
        else
            ss1 << "At Max Size";

        RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss1.str(), Color(0.95, 0.95, 0), 2.f, 34.5f, 5.f);

        break;
    }
    }

    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ICON_EYE), 5.f, 5.f, 1.f, 41.f, 16.f, 0.f, 0.f, 0.f, false);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Back", Color(0.95, 0.95, 0), 3.f, 70.f, 5.f);

    SetHUD(false);
}

void SceneZoo::RenderTransactionInterface()
{
    SetHUD(true);

    // Counter left button
    if (Application::cursorXPos / Application::m_width >= 0.54375 &&
        Application::cursorXPos / Application::m_width <= 0.586458 &&
        Application::cursorYPos / Application::m_height <= 0.624074 &&
        Application::cursorYPos / Application::m_height >= 0.568519)
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 3.5f, 3.5f, 3.5f, 45.5f, 23.f, 0.f, 0.f, 0.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
            transactionCounter--;

        if (transactionCounter <= 0)
        {
            switch (currentShop)
            {
            case SHOP_BUY:
                if (currentItem == Item::TYPE_BAIT)
                    transactionCounter = SharedData::GetInstance()->player->inventory[Item::TYPE_MEAT].GetCount();
                else
                    transactionCounter = SharedData::GetInstance()->player->m_currency / SharedData::GetInstance()->player->inventory[currentItem].GetBuyCost();
                break;

            case SHOP_SELL:
                transactionCounter = SharedData::GetInstance()->player->inventory[currentItem].GetCount();
                break;
            }
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 3.5f, 3.5f, 3.5f, 45.5f, 23.f, 0.f, 0.f, 0.f, false);

    // Counter right button
    if (Application::cursorXPos / Application::m_width >= 0.654687 &&
        Application::cursorXPos / Application::m_width <= 0.68948 &&
        Application::cursorYPos / Application::m_height <= 0.624074 &&
        Application::cursorYPos / Application::m_height >= 0.568519)
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 3.5f, 3.5f, 3.5f, 54.5f, 23.f, 0.f, 0.f, 0.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
        {
            transactionCounter++;
        }

        if (currentShop == SHOP_SELL && transactionCounter > SharedData::GetInstance()->player->inventory[currentItem].GetCount())
            transactionCounter = SharedData::GetInstance()->player->inventory[currentItem].GetCount();
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 3.5f, 3.5f, 3.5f, 54.5f, 23.f, 0.f, 0.f, 0.f, false);

    // Confirm button
    if (Application::cursorXPos / Application::m_width >= 0.54375 &&
        Application::cursorXPos / Application::m_width <= 0.68948 &&
        Application::cursorYPos / Application::m_height <= 0.698148 &&
        Application::cursorYPos / Application::m_height >= 0.639815)
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 12.5f, 3.5f, 3.5f, 50.f, 18.5f, 0.f, 0.f, 0.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
            switch (currentShop)
        {
            case SHOP_BUY:

                if (currentItem == Item::TYPE_BAIT)
                {
                    for (unsigned i = 0; i < transactionCounter; ++i)
                        SharedData::GetInstance()->player->inventory[Item::TYPE_MEAT].Use();

                    SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].Add(transactionCounter);

                    isInTransaction = false;
                    shopKeeperTextChoice = TEXT_THANK;
                    transactionCounter = 1;
                }
                else
                {
                    if (SharedData::GetInstance()->player->inventory[currentItem].Buy(transactionCounter))
                    {
                        isInTransaction = false;
                        shopKeeperTextChoice = TEXT_THANK;
                        transactionCounter = 1;
                    }
                    else
                        shopKeeperTextChoice = TEXT_INSUFFICIENT_COINS;
                }

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
    if (currentItem == Item::TYPE_BAIT)
        ss1 << "Total: " << transactionCounter;
    else if (currentShop == SHOP_BUY)
        ss1 << "Total: " << transactionCounter * SharedData::GetInstance()->player->inventory[currentItem].GetBuyCost();
    else if (currentShop == SHOP_SELL)
        ss1 << "Total: " << transactionCounter * SharedData::GetInstance()->player->inventory[currentItem].GetBuyCost() * 0.5;

    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss1.str(), Color(0.95, 0.95, 0), 3.f, 45.f, 25.5f);

    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ICON_LEFT_ARROW), 3.5f, 3.5f, 3.5f, 45.5f, 23.f, 0.f, 0.f, 0.f, false);
    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ICON_RIGHT_ARROW), 3.5f, 3.5f, 3.5f, 54.5f, 23.f, 0.f, 0.f, 0.f, false);
    std::stringstream ss;
    ss << transactionCounter;
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0.95, 0.95, 0), 3.f, 48.25f, 21.5f);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Confirm", Color(0.95, 0.95, 0), 3.f, 44.75f, 17.f);

    SetHUD(false);
}

void SceneZoo::RenderUpgradeInterface(Item::TYPE item)
{
    std::stringstream ss;
    ss << "Level: " << SharedData::GetInstance()->player->inventory[item].GetCurrentUpgradeLevel() << " / " << MAX_UPGRADE_LEVEL;
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0.95, 0.95, 0), 3.f, 45.f, 15.f);
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
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(0.95, 0.95, 0), 1, 23.5f, 2.5f);
    // 2: Net
    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET), 1, 32.5f, 4.5f, 0, f_Rotate, 0, false);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss2.str(), Color(0.95, 0.95, 0), 1, 31.0f, 2.5f);
    // 3: Bait
    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BAIT), 2, 40.0f, 5.5f, f_Rotate, f_Rotate, 0, false);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss3.str(), Color(0.95, 0.95, 0), 1, 38.5f, 2.5f);
    // 4: Trap
    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TRAP), 2, 47.5f, 5.5f, -25.f, f_Rotate, 0, false);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss4.str(), Color(0.95, 0.95, 0), 1, 46.0f, 2.5f);
    // 5: Meat
    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MEAT), 2, 55.0f, 5.2f, 45.f, 45.f, 0, false);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss5.str(), Color(0.95, 0.95, 0), 1, 53.5f, 2.5f);

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

    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_HUNTING_GROUNDS_INTERFACE), 65.f, 38.f, 30.f, false);

    //Grass scene
    if (Application::cursorXPos / Application::m_width >= 0.189602 &&
        Application::cursorXPos / Application::m_width <= 0.46875 &&
        Application::cursorYPos / Application::m_height >= 0.212037 &&
        Application::cursorYPos / Application::m_height <= 0.496296)
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GRASSZONE_CAPTURE_ALT), 22.5f, 17.5f, 1.f, 26.5f, 38.f, 0, 0, 0, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
            changeSceneTo = AREA_GRASS;
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GRASSZONE_CAPTURE), 22.5f, 17.5f, 1.f, 26.5f, 38.f, 0, 0, 0, false);

    //Swamp Scene
    if (Application::cursorXPos / Application::m_width >= 0.525 &&
        Application::cursorXPos / Application::m_width <= 0.803646 &&
        Application::cursorYPos / Application::m_height >= 0.212037 &&
        Application::cursorYPos / Application::m_height <= 0.496296)
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SWAMPZONE_CAPTURE_ALT), 22.5f, 17.5f, 1.f, 53.5f, 38.f, 0, 0, 0, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
            changeSceneTo = AREA_SWAMP;
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SWAMPZONE_CAPTURE), 22.5f, 17.5f, 1.f, 53.5f, 38.f, 0, 0, 0, false);

    //Rock scene
    if (Application::cursorXPos / Application::m_width >= 0.189602 &&
        Application::cursorXPos / Application::m_width <= 0.46875 &&
        Application::cursorYPos / Application::m_height >= 0.536111 &&
        Application::cursorYPos / Application::m_height <= 0.818519)
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCKZONE_CAPTURE_ALT), 22.5f, 17.5f, 1.f, 26.5f, 18.f, 0, 0, 0, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
            changeSceneTo = AREA_ROCK;

    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCKZONE_CAPTURE), 22.5f, 17.5f, 1.f, 26.5f, 18.f, 0, 0, 0, false);

    //Fire Scene
    if (Application::cursorXPos / Application::m_width >= 0.525 &&
        Application::cursorXPos / Application::m_width <= 0.803646 &&
        Application::cursorYPos / Application::m_height >= 0.536111 &&
        Application::cursorYPos / Application::m_height <= 0.818519)
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FIREZONE_CAPTURE_ALT), 22.5f, 17.5f, 1.f, 53.5f, 18.f, 0, 0, 0, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
            changeSceneTo = AREA_FIRE;
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FIREZONE_CAPTURE), 22.5f, 17.5f, 1.f, 53.5f, 18.f, 0, 0, 0, false);

    //Back button
    if (Application::cursorXPos / Application::m_width >= 0.853646 &&
        Application::cursorXPos / Application::m_width <= 0.958838 &&
        Application::cursorYPos / Application::m_height >= 0.833333 &&
        Application::cursorYPos / Application::m_height <= 0.923333
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 8.5f, 3.5f, 3.5f, 73.f, 6.5f, 0.f, 0.f, 0.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
        {
            currentArea = AREA_OVERVIEW;
            currentState = STATE_ZOO;
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 8.5f, 3.5f, 3.5f, 73.f, 6.5f, 0.f, 0.f, 0.f, false);

    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Go Hunting!", Color(0.95, 0.95, 0), 4.f, 30.f, 48.5f);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Back", Color(0.95, 0.95, 0), 3.f, 70.f, 5.f);

    SetHUD(false);
}

void SceneZoo::RenderOverviewInterface()
{
    //Menu button
    if (Application::cursorXPos / Application::m_width >= 0.0 &&
        Application::cursorXPos / Application::m_width <= 0.0515625 &&
        Application::cursorYPos / Application::m_height >= 0.355551 &&
        Application::cursorYPos / Application::m_height <= 0.611111
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 16.f, 5.5f, 1.f, 1.5f, 30.f, 0.f, 0.f, 90.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
        {
            currentState = STATE_MENU;
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 16.f, 5.5f, 1.f, 1.5f, 30.f, 0.f, 0.f, 90.f, false);

    //Quest button
    if (Application::cursorXPos / Application::m_width >= 0.940625 &&
        Application::cursorXPos / Application::m_width <= 1.0 &&
        Application::cursorYPos / Application::m_height >= 0.355551 &&
        Application::cursorYPos / Application::m_height <= 0.611111
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 16.f, 5.5f, 1.f, 78.5f, 30.f, 0.f, 0.f, 90.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
        {
            currentState = STATE_QUEST;
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 16.f, 5.5f, 1.f, 78.5f, 30.f, 0.f, 0.f, 90.f, false);

    //Shop button
    if (Application::cursorXPos / Application::m_width >= 0.411458 &&
        Application::cursorXPos / Application::m_width <= 0.608333 &&
        Application::cursorYPos / Application::m_height >= 0.0 &&
        Application::cursorYPos / Application::m_height <= 0.0880383
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 16.f, 5.5f, 1.f, 41.f, 57.5f, 0.f, 0.f, 0.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
        {
            currentState = STATE_SHOP;
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 16.f, 5.5f, 1.f, 41.f, 57.5f, 0.f, 0.f, 0.f, false);

    //Hunting gruonds button
    if (Application::cursorXPos / Application::m_width >= 0.411458 &&
        Application::cursorXPos / Application::m_width <= 0.608333 &&
        Application::cursorYPos / Application::m_height >= 0.844444 &&
        Application::cursorYPos / Application::m_height <= 0.928714
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 16.f, 5.5f, 1.f, 41.f, 5.f, 0.f, 0.f, 0.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
        {
            currentState = STATE_CHANGE_SCENE;
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 16.f, 5.5f, 1.f, 41.f, 5.f, 0.f, 0.f, 0.f, false);

    //Grass area button
    if (Application::cursorXPos / Application::m_width >= 0.405208 &&
        Application::cursorXPos / Application::m_width <= 0.466667 &&
        Application::cursorYPos / Application::m_height >= 0.280556 &&
        Application::cursorYPos / Application::m_height <= 0.362073
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ICON_GRASS_ALT), 5.5f, 5.5f, 1.f, 35.f, 40.f, 0.f, 0.f, 0.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
        {
            zooCamera.position = grassAreaPosition + Vector3(0, 75, -50);
            zooCamera.target = grassAreaPosition;

            currentState = STATE_ENCLOSURE;
            currentArea = AREA_GRASS;
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ICON_GRASS), 5.5f, 5.5f, 1.f, 35.f, 40.f, 0.f, 0.f, 0.f, false);

    //Fire area button
    if (Application::cursorXPos / Application::m_width >= 0.525521 &&
        Application::cursorXPos / Application::m_width <= 0.592187 &&
        Application::cursorYPos / Application::m_height >= 0.441667 &&
        Application::cursorYPos / Application::m_height <= 0.525
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ICON_FIRE_ALT), 5.5f, 5.5f, 1.f, 45.f, 30.f, 0.f, 0.f, 0.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
        {
            zooCamera.position = fireAreaPosition + Vector3(0, 75, -50);
            zooCamera.target = fireAreaPosition;

            currentState = STATE_ENCLOSURE;
            currentArea = AREA_FIRE;
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ICON_FIRE), 5.5f, 5.5f, 1.f, 45.f, 30.f, 0.f, 0.f, 0.f, false);

    //Rock Area button
    if (Application::cursorXPos / Application::m_width >= 0.525521 &&
        Application::cursorXPos / Application::m_width <= 0.592187 &&
        Application::cursorYPos / Application::m_height >= 0.280556 &&
        Application::cursorYPos / Application::m_height <= 0.362073
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ICON_ROCK_ALT), 5.5f, 5.5f, 1.f, 45.f, 40.f, 0.f, 0.f, 0.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
        {
            zooCamera.position = rockAreaPosition + Vector3(0, 75, -50);
            zooCamera.target = rockAreaPosition;

            currentState = STATE_ENCLOSURE;
            currentArea = AREA_ROCK;
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ICON_ROCK), 5.5f, 5.5f, 1.f, 45.f, 40.f, 0.f, 0.f, 0.f, false);

    //Swamp Area button
    if (Application::cursorXPos / Application::m_width >= 0.405208 &&
        Application::cursorXPos / Application::m_width <= 0.466667 &&
        Application::cursorYPos / Application::m_height >= 0.441667 &&
        Application::cursorYPos / Application::m_height <= 0.525
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ICON_SWAMP_ALT), 5.5f, 5.5f, 1.f, 35.f, 30.f, 0.f, 0.f, 0.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
        {
            zooCamera.position = swampAreaPosition + Vector3(0, 75, -50);
            zooCamera.target = swampAreaPosition;

            currentState = STATE_ENCLOSURE;
            currentArea = AREA_SWAMP;
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ICON_SWAMP), 5.5f, 5.5f, 1.f, 35.f, 30.f, 0.f, 0.f, 0.f, false);

    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Shop", Color(0.95, 0.95, 0), 3.f, 37.5f, 56.f);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Hunting Grounds", Color(0.95, 0.95, 0), 2.f, 34.f, 4.f);

    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "M", Color(0.95, 0.95, 0), 4.f, 0.5f, 33.f);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "e", Color(0.95, 0.95, 0), 4.f, 0.5f, 30.f);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "n", Color(0.95, 0.95, 0), 4.f, 0.5f, 27.f);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "u", Color(0.95, 0.95, 0), 4.f, 0.5f, 24.f);

    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Q", Color(0.95, 0.95, 0), 3.f, 77.5f, 33.f);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "u", Color(0.95, 0.95, 0), 3.f, 77.5f, 31.f);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "e", Color(0.95, 0.95, 0), 3.f, 77.5f, 29.f);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "s", Color(0.95, 0.95, 0), 3.f, 77.5f, 27.f);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "t", Color(0.95, 0.95, 0), 3.f, 77.5f, 25.f);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "s", Color(0.95, 0.95, 0), 3.f, 77.5f, 23.f);


}

void SceneZoo::RenderMenuInterface()
{
    //Back button
    if (Application::cursorXPos / Application::m_width >= 0.853646 &&
        Application::cursorXPos / Application::m_width <= 0.958838 &&
        Application::cursorYPos / Application::m_height >= 0.833333 &&
        Application::cursorYPos / Application::m_height <= 0.923333
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 8.5f, 3.5f, 3.5f, 73.f, 6.5f, 0.f, 0.f, 0.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
        {
            currentArea = AREA_OVERVIEW;
            currentState = STATE_ZOO;
            zooCamera.Reset();
            cycleIter = 0;
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 8.5f, 3.5f, 3.5f, 73.f, 6.5f, 0.f, 0.f, 0.f, false);

    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Back", Color(0.95, 0.95, 0), 3.f, 70.f, 5.f);

}

void SceneZoo::RenderQuestInterface()
{
    //Back button
    if (Application::cursorXPos / Application::m_width >= 0.853646 &&
        Application::cursorXPos / Application::m_width <= 0.958838 &&
        Application::cursorYPos / Application::m_height >= 0.833333 &&
        Application::cursorYPos / Application::m_height <= 0.923333
        )
    {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION_ALT), 8.5f, 3.5f, 3.5f, 73.f, 6.5f, 0.f, 0.f, 0.f, false);

        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed)
        {
            currentArea = AREA_OVERVIEW;
            currentState = STATE_ZOO;
            zooCamera.Reset();
            cycleIter = 0;
        }
    }
    else
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SHOP_SELECTION), 8.5f, 3.5f, 3.5f, 73.f, 6.5f, 0.f, 0.f, 0.f, false);

    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "Back", Color(0.95, 0.95, 0), 3.f, 70.f, 5.f);

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

void SceneZoo::SpawnSceneParticles()
{

}