#include "GL\glew.h"
//#include "../../shader.hpp"

#include "SceneZoo.h"
#include "../../General/Application.h"
//#include "../../Graphics/LoadOBJ/LoadTGA.h"
//#include "../../Graphics/Mesh/MeshBuilder.h"
#include "../../General/SharedData.h"
#include "../../GameObject/MonsterFactory.h"

#include <sstream>

SceneZoo::SceneZoo()
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
    for (unsigned i = 0; i < 200; ++i)
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

    populateMonsterList();

    isFollowing = false;
    iter = 0;
}
static double counter = 0;

void SceneZoo::Update(double dt)
{
    fps = (float)(1.f / dt);

    //===============================================================================================================================//
    //                                                            Updates                                                            //
    //===============================================================================================================================//

    //Movement update for Gameobjects
    UpdateGameObjects(&zooWorld, 2 * dt);

    //Camera Update
    if (!isFollowing)
        zooCamera.Update(dt);
    else
    {
        zooCamera.position = zooWorld.position[followingGO] + Vector3(0, 10, -10);
        zooCamera.target = zooWorld.position[followingGO];
    }

    //Player Update
    //SharedData::GetInstance()->player->Update(dt);

    //Inputmanager Update
    SharedData::GetInstance()->inputManager->Update();

    //===============================================================================================================================//
    //                                                            Checks                                                             //
    //===============================================================================================================================//

    //Boundry check;
    for (unsigned i = 0; i < grassZone.size(); ++i)
    {
        if (zooWorld.position[grassZone[i]].x < grassAreaPosition.x - 50 ||
            zooWorld.position[grassZone[i]].x > grassAreaPosition.x + 50 ||
            zooWorld.position[grassZone[i]].z < grassAreaPosition.z - 50 ||
            zooWorld.position[grassZone[i]].z > grassAreaPosition.z + 50)
            zooWorld.velocity[grassZone[i]] = -zooWorld.velocity[grassZone[i]];
    }

    for (unsigned i = 0; i < fireZone.size(); ++i)
    {
        if (zooWorld.position[fireZone[i]].x < fireAreaPosition.x - 50 ||
            zooWorld.position[fireZone[i]].x > fireAreaPosition.x + 50 ||
            zooWorld.position[fireZone[i]].z < fireAreaPosition.z - 50 ||
            zooWorld.position[fireZone[i]].z > fireAreaPosition.z + 50)
            zooWorld.velocity[fireZone[i]] = -zooWorld.velocity[fireZone[i]];
    }

    for (unsigned i = 0; i < rockZone.size(); ++i)
    {
        if (zooWorld.position[rockZone[i]].x < rockAreaPosition.x - 50 ||
            zooWorld.position[rockZone[i]].x > rockAreaPosition.x + 50 ||
            zooWorld.position[rockZone[i]].z < rockAreaPosition.z - 50 ||
            zooWorld.position[rockZone[i]].z > rockAreaPosition.z + 50)
            zooWorld.velocity[rockZone[i]] = -zooWorld.velocity[rockZone[i]];
    }

    for (unsigned i = 0; i < swampZone.size(); ++i)
    {
        if (zooWorld.position[swampZone[i]].x < swampAreaPosition.x - 50 ||
            zooWorld.position[swampZone[i]].x > swampAreaPosition.x + 50 ||
            zooWorld.position[swampZone[i]].z < swampAreaPosition.z - 50 ||
            zooWorld.position[swampZone[i]].z > swampAreaPosition.z + 50)
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

        isFollowing = false;
    }
    else if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_2].isPressed)
    {
        zooCamera.position = fireAreaPosition + Vector3(0, 50, -25);
        zooCamera.target = fireAreaPosition;

        if (currentArea == AREA_GRASS)
        {
            currentArea = AREA_OVERVIEW;
            zooCamera.Reset();
        }
        else
            currentArea = AREA_FIRE;

        isFollowing = false;
    }
    else if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_3].isPressed)
    {
        zooCamera.position = rockAreaPosition + Vector3(0, 50, -25);
        zooCamera.target = rockAreaPosition;

        if (currentArea == AREA_GRASS)
        {
            currentArea = AREA_OVERVIEW;
            zooCamera.Reset();
        }
        else
            currentArea = AREA_ROCK;

        isFollowing = false;
    }
    else if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_4].isPressed)
    {
        zooCamera.position = swampAreaPosition + Vector3(0, 50, -25);
        zooCamera.target = swampAreaPosition;

        if (currentArea == AREA_GRASS)
        {
            currentArea = AREA_OVERVIEW;
            zooCamera.Reset();
        }
        else
            currentArea = AREA_SWAMP;

        isFollowing = false;
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

    //shows pressed locations
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_SPACE].isPressed)
    {
        Vector3 temp;
        temp = zooCamera.target;
        temp.y = 0;
        tempStore.push_back(temp);
    }

    //for checking stuff
    for (unsigned i = 0; i < tempStore.size(); ++i)
    {
        modelStack.PushMatrix();
        modelStack.Translate(tempStore[i].x, tempStore[i].y, tempStore[i].z);
        RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
        modelStack.PopMatrix();
    }

    //for camera.target
    modelStack.PushMatrix();
    modelStack.Translate(zooCamera.target.x, zooCamera.target.y, zooCamera.target.z);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    modelStack.PopMatrix();

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

    RenderGameObjects(&zooWorld);

    for (GameObject tallGrass = 0; tallGrass < zooWorld.GAMEOBJECT_COUNT; ++tallGrass)
    {
        if ((zooWorld.mask[tallGrass] & COMPONENT_HITBOX) == COMPONENT_HITBOX)
        {
            modelStack.PushMatrix();
            modelStack.Translate(zooWorld.hitbox[tallGrass].m_origin.x, zooWorld.hitbox[tallGrass].m_origin.y, zooWorld.hitbox[tallGrass].m_origin.z);
            modelStack.Scale(zooWorld.hitbox[tallGrass].m_scale.x, zooWorld.hitbox[tallGrass].m_scale.y, zooWorld.hitbox[tallGrass].m_scale.z);
            //modelStack.Rotate(0, 0, 0, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            modelStack.PopMatrix();
        }
    }

    if (isFollowing)
        DisplayMonsterStats(zooWorld.monster[followingGO]);
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
    //modelStack.Rotate(0, 0,0,0);
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
        randOffset.Set(Math::RandFloatMinMax(-25.f, 25.f),
                       0,
                       Math::RandFloatMinMax(-25.f, 25.f)
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
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE;
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
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE;
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
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE;
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
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE;
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
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE;
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
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE;
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
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE;
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
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE;
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
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE;
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
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE;
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
            zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE;
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
    if (isFollowing)
    {
        if (++iter >= area.size())
            iter = 0;
        else
            followingGO = area[iter];
    }
    else
    {
        if (area.size())
            followingGO = area[iter];

        isFollowing = true;
    }
}

//========================
// == OBJECTS TO RENDER
//========================





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