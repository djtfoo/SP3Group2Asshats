#include "GL\glew.h"
//#include "../../shader.hpp"

#include "SceneRock.h"
#include "../../General/Application.h"
//#include "../../Graphics/LoadOBJ/LoadTGA.h"
//#include "../../Graphics/Mesh/MeshBuilder.h"
#include "../../General/SharedData.h"

#include <sstream>

SceneRock::SceneRock()
{
}

SceneRock::~SceneRock()
{
}

void SceneRock::Init()
{
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 1000 units
    Mtx44 perspective;
    perspective.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
    //perspective.SetToOrtho(-80, 80, -60, 60, -1000, 1000);
    projectionStack.LoadMatrix(perspective);

    camera.Init(Vector3(1, 1, 0), Vector3(0, 0, 0), Vector3(0, 1, 0));

    Application::GetCursorPos(&Application::cursorXPos, &Application::cursorYPos);
    Application::SetCursorPos(Application::GetWindowWidth() / 2.f, Application::GetWindowHeight() / 2.f);

    bLButtonState = false;
    //Set heap(?)
    memset(&rockWorld, 0, sizeof(rockWorld));

    // Load map
    Scene::LoadLevelMap("GameData/GrassScene.csv");
    for (int rows = 0; rows < Scene::m_rows; ++rows)
    {
        for (int cols = 0; cols < Scene::m_cols; ++cols)
        {
            if (m_levelMap[rows][cols] == '0')
                continue;

            LevelGenerationMap::iterator it = Scene::m_levelGenerationData.find(m_levelMap[rows][cols]);

            // it->first is tileCount
            // first in it->second is mesh
            // second in it->second is vector of components
            //std::cout << m_levelMap[rows][cols] << " ";

            GameObject go = createGO(&rockWorld);
            rockWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_HITBOX;
            rockWorld.position[go].Set(-100.f + cols * Scene::tileSize, 0.f, -100.f + rows * Scene::tileSize);
            rockWorld.hitbox[go].m_origin = rockWorld.position[go] + Vector3(0, 2.5, 0);
            rockWorld.hitbox[go].m_scale.Set(5.f, 5.f, 5.f);
            rockWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh((it->second).first);
            rockWorld.appearance[go].scale.Set(1, Math::RandFloatMinMax(0.75f, 1.25f), 1);
        }
    }

    //Test monster
    monster = createGO(&rockWorld);
    rockWorld.mask[monster] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_AI;
    rockWorld.position[monster].Set(0, 0, 25);//Zero();
    rockWorld.velocity[monster].SetZero();//(0, 0, 1);
    rockWorld.hitbox[monster].m_origin = rockWorld.position[monster] + Vector3(0, 0.75f, -0.3f);
    rockWorld.hitbox[monster].m_scale.Set(1.5f, 1.5f, 1.75f);
    rockWorld.appearance[monster].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_RABBIT);
    rockWorld.appearance[monster].scale.Set(1, 1, 1);

    rock = createGO(&rockWorld);
    rockWorld.mask[rock] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX;
    rockWorld.position[rock].Set(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z);
    rockWorld.appearance[rock].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE);
    rockWorld.appearance[rock].scale.Set(1, 1, 1);

    net = createGO(&rockWorld);
    rockWorld.mask[net] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE;
    rockWorld.position[net].SetZero();
    rockWorld.appearance[net].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET);
    rockWorld.appearance[net].scale.Set(2, 2, 2);
    //HITBOX.m_origin = Vector3(0, 5, 0);
    //HITBOX.m_scale = Vector3(10, 10, 10);

    b_capturing = false;
    b_captured = false;
    captureCounter = 0;
    counter = 0;

}
static double counter = 0;

void SceneRock::Update(double dt)
{
    fps = (float)(1.f / dt);
    //glEnable(GL_CULL_FACE);

    //===============================================================================================================================//
    //                                                            Updates                                                            //
    //===============================================================================================================================//

    //Update Projectiles
    itemProjectile->UpdateProjectile(dt);

    //Movement update for Gameobjects
    UpdateGameObjects(&rockWorld, dt);

    //Camera Update
    camera.Update(dt);

    //Player Update
    SharedData::GetInstance()->player->Update(dt);

    //Inputmanager Update
    SharedData::GetInstance()->inputManager->Update();

    //===============================================================================================================================//
    //                                                            Checks                                                             //
    //===============================================================================================================================//

    //Projectile checks
    if (ItemProjectile::ItemProjectileList.size())
    {
        for (unsigned i = 0; i < ItemProjectile::ItemProjectileList.size(); ++i)
        {
            if (rockWorld.hitbox[monster].CheckCollision(ItemProjectile::ItemProjectileList[i]->position))
            {
                b_capturing = true;
                ItemProjectile::ItemProjectileList[i]->deleteBullet;
                rockWorld.position[net] = rockWorld.position[monster];
            }
            if (b_capturing == true)
            {
                int v1 = rand() % 100;
                std::cout << v1 << std::endl;
                if (v1 < 20)
                {
                    b_captured = true;
                }
                b_capturing = false;//fail capture
                rockWorld.position[net] = 0;//set net to false
            }
            if (b_captured == true)
            {
                rockWorld.velocity[monster] = 0;
                rockWorld.position[net] = rockWorld.position[monster];
                break;
            }
        }
    }

    if (Application::IsKeyPressed('C') && SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].Use() && counter > 0.5)
    {
        ItemProjectile::ItemProjectileList.push_back(new ItemProjectile(
            Vector3(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z),
            Vector3(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
            500,
            50,
            10
            ));
        counter = 0;
    }
    //////////////////////////////////////////////
    ////Projectile checks//////////////NET////////
    //////////////////////////////////////////////
    if (ItemProjectile::ItemProjectileList.size())
    {
        for (unsigned i = 0; i < ItemProjectile::ItemProjectileList.size(); ++i)
        {
            for (GameObject ai = 0; ai < rockWorld.GAMEOBJECT_COUNT; ++ai)
            {
                if ((rockWorld.mask[ai] & COMPONENT_AI) == COMPONENT_AI)
                {
                    if (rockWorld.hitbox[ai].CheckCollision(ItemProjectile::ItemProjectileList[i]->position))
                    {
                        ItemProjectile::ItemProjectileList[i]->deleteBullet;
                        net = createGO(&rockWorld);
                        rockWorld.mask[net] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE;
                        rockWorld.capture[net].capturingMonster = true;
                        rockWorld.capture[net].capturedMonster = false;
                        rockWorld.capture[net].timeBeforeCapture = 3;
                        rockWorld.appearance[net].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET);
                        rockWorld.appearance[net].scale.Set(2, 2, 2);
                        if (rockWorld.capture[net].capturingMonster == true)
                        {
                            rockWorld.position[net] = rockWorld.position[ai];
                            int v1 = rand() % 100;
                            std::cout << v1 << std::endl;
                            if (v1 < 20)
                            {
                                rockWorld.capture[net].capturedMonster = true;//Captured!
                            }
                            rockWorld.capture[net].capturingMonster = false;//fail capture
                        }
                        if (rockWorld.capture[net].capturedMonster == true)
                        {
                            rockWorld.velocity[ai] = 0;
                            rockWorld.position[net] = rockWorld.position[ai];
                        }
                    }
                }
            }
        }
    }
    //////////////////////////////////////////////
    ////////BAIT /////////////////////////////////
    //////////////////////////////////////////////
    //if (ItemProjectile::ItemProjectileList.size())
    //{
    //	for (unsigned i = 0; i < ItemProjectile::ItemProjectileList.size(); ++i)
    //	{
    //		if (ItemProjectile::ItemProjectileList[i]->position.y <= 1)
    //		{
    //			Vector3 tempo = ItemProjectile::ItemProjectileList[i]->position;
    //			ItemProjectile::ItemProjectileList[i]->deleteBullet;

    //			bait = createGO(&grass);
    //			grass.mask[bait] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_BAIT;
    //			grass.position[bait] = tempo;
    //			grass.appearance[bait].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE);
    //			grass.appearance[bait].scale.Set(1, 1, 1);
    //			grass.bait[bait].scentRadius = 200;
    //			grass.bait[bait].foundRadius = 5;
    //			grass.bait[bait].eattingBait = false;
    //			grass.bait[bait].foundBait = false;
    //			grass.bait[bait].timeEatting = 3.0f;
    //		}
    //	} 
    //}

    //for (GameObject ai = 0; ai < grass.GAMEOBJECT_COUNT; ++ai)
    //{
    //	if ((grass.mask[ai] & COMPONENT_AI) == COMPONENT_AI)
    //	{
    //		for (GameObject bait = 0; bait < grass.GAMEOBJECT_COUNT; ++bait)
    //		{
    //			if ((grass.mask[bait] & COMPONENT_BAIT) == COMPONENT_BAIT)
    //			{
    //				if ((grass.position[ai] - grass.position[bait]).LengthSquared() < grass.bait[bait].scentRadius && grass.bait[bait].eattingBait == false)
    //				{
    //					grass.bait[bait].foundBait = true;
    //					std::cout << " FOUND BAIT " + grass.bait[bait].foundBait << std::endl;
    //					if (grass.bait[bait].foundBait == true)
    //					{
    //						grass.velocity[ai] = (grass.position[ai] - grass.position[bait]).Normalized();
    //						grass.velocity[ai].y = 0;
    //						grass.velocity[ai] *= -1;
    //					}
    //				}
    //				if ((grass.position[ai] - grass.position[bait]).LengthSquared() < grass.bait[bait].foundRadius)
    //				{
    //					std::cout << "EATTING BAIT " + grass.bait[bait].foundBait << std::endl;
    //					grass.bait[bait].foundBait = false;
    //					grass.bait[bait].eattingBait = true;
    //					grass.velocity[ai] = 0;
    //					grass.velocity[ai].y = 0;
    //				}
    //				if (grass.bait[bait].eattingBait == true)
    //				{
    //					grass.bait[bait].timeEatting -= dt;
    //					std::cout << grass.bait[bait].timeEatting << std::endl;
    //					if (grass.bait[bait].timeEatting <= 0)
    //					{
    //						grass.bait[bait].eattingBait == false;
    //						destroyGO(&grass, bait);
    //						std::cout << "Done Eatting" << std::endl;
    //						//PUT WHATEVER THE RABBIT DO NORMALLY HERE :D (DONE EATTING BAIT)
    //						grass.velocity[ai] = 2;
    //						grass.velocity[ai].y = 0;
    //					}
    //				}
    //			}
    //		}
    //	}
    //}


    //Trap check (radius)
    for (GameObject trap = 0; trap < rockWorld.GAMEOBJECT_COUNT; ++trap)
    {
        if ((rockWorld.mask[trap] & COMPONENT_TRAP) == COMPONENT_TRAP)
        {
            if (!rockWorld.trap[trap].activated)
            {
                for (GameObject ai = 0; ai < rockWorld.GAMEOBJECT_COUNT; ++ai)
                {
                    if ((rockWorld.mask[ai] & COMPONENT_AI) == COMPONENT_AI)
                    {
                        if ((rockWorld.position[trap] - rockWorld.position[ai]).LengthSquared() < rockWorld.trap[trap].radius)
                        {
                            rockWorld.trap[trap].caughtMonsterVel = rockWorld.velocity[ai];
                            rockWorld.trap[trap].caughtMonster = ai;
                            rockWorld.trap[trap].activated = true;
                            rockWorld.velocity[ai].SetZero();
                        }
                    }
                }
            }
            else
            {
                if (rockWorld.trap[trap].triggerTimer < rockWorld.trap[trap].triggerDuration)
                {
                    rockWorld.trap[trap].triggerTimer += dt;
                }
                else
                {
                    rockWorld.velocity[rockWorld.trap[trap].caughtMonster] = rockWorld.trap[trap].caughtMonsterVel;
                    destroyGO(&rockWorld, trap);
                }
            }
        }
    }

    //
    for (GameObject ai = 0; ai < rockWorld.GAMEOBJECT_COUNT; ++ai)
    {
        if ((rockWorld.mask[ai] & COMPONENT_AI) == COMPONENT_AI)
        {
            if ((SharedData::GetInstance()->player->GetPositionVector() - rockWorld.position[ai]).LengthSquared() < 750.0f * SharedData::GetInstance()->player->m_noiseFactor)
            {
                rockWorld.velocity[ai] = (rockWorld.position[ai] - SharedData::GetInstance()->player->GetPositionVector()).Normalized() * 5;
                rockWorld.velocity[ai].y = 0;
            }
        }

    }

    //If player is hidden
    for (GameObject tallGrass = 0; tallGrass < rockWorld.GAMEOBJECT_COUNT; ++tallGrass)
    {
        if ((rockWorld.mask[tallGrass] & COMPONENT_HITBOX) == COMPONENT_HITBOX)
        {
            if (rockWorld.hitbox[tallGrass].CheckCollision(SharedData::GetInstance()->player->GetPositionVector()))
            {
                SharedData::GetInstance()->player->m_bHiding = true;
                break;
            }
            else
                SharedData::GetInstance()->player->m_bHiding = false;
        }
    }

    //===============================================================================================================================//
    //                                                            Key Inputs                                                         //
    //===============================================================================================================================//

    //Place trap
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_G].isPressed && SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP_ONE].Use())
    {
        GameObject trap = createGO(&rockWorld);

        rockWorld.mask[trap] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_TRAP;

        rockWorld.position[trap].Set(SharedData::GetInstance()->player->GetPositionVector().x + SharedData::GetInstance()->player->GetViewVector().x * 20, 0, SharedData::GetInstance()->player->GetPositionVector().z + SharedData::GetInstance()->player->GetViewVector().z * 20);
        rockWorld.appearance[trap].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TRAP);
        rockWorld.appearance[trap].scale.Set(3, 3, 3);
        
        rockWorld.trap[trap].triggerDuration = 3.0f;
        rockWorld.trap[trap].triggerTimer = 0.f;
        rockWorld.trap[trap].radius = 2.5f;
        rockWorld.trap[trap].activated = false;
        rockWorld.trap[trap].caughtMonster = 0;
        rockWorld.trap[trap].caughtMonsterVel.SetZero();

        counter = 0;
    }

    //Shoot projectile
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_X].isPressed && SharedData::GetInstance()->player->inventory[Item::TYPE_NET].Use())
    {
        ItemProjectile::ItemProjectileList.push_back(new ItemProjectile(
            Vector3(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z),
            Vector3(SharedData::GetInstance()->player->GetViewVector().x, 0.5, SharedData::GetInstance()->player->GetViewVector().z),
            500,
            15,
            10
            ));

        counter = 0;
    }

    //Spawn monster at origin
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_E].isPressed)
    {
        monster = createGO(&rockWorld);
        rockWorld.mask[monster] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_AI;
        rockWorld.position[monster].SetZero();
        rockWorld.velocity[monster].Set(0, 0, 1);
        rockWorld.hitbox[monster].m_origin = rockWorld.position[monster] + Vector3(0, 0.75f, -0.3f);
        rockWorld.hitbox[monster].m_scale.Set(1.5f, 1.5f, 1.75f);
        rockWorld.appearance[monster].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_RABBIT);
        rockWorld.appearance[monster].scale.Set(1, 1, 1);

        counter = 0;
    }

    //Shoot projectile
    /*if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_1].IsPressed() && counter > 0.2)
    {
    ItemProjectile::ItemProjectileList.push_back(new ItemProjectile(
    Vector3(camera.position.x, camera.position.y, camera.position.z),
    Vector3(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
    500,
    25,
    400
    ));

    counter = 0;
    }*/

    // for buffer time between projectile launches
    counter += dt;
}

void SceneRock::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Mtx44 perspective;
    perspective.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
    //perspective.SetToOrtho(-80, 80, -60, 60, -1000, 1000);
    projectionStack.LoadMatrix(perspective);

    // Camera matrix
    viewStack.LoadIdentity();
    viewStack.LookAt(
        camera.position.x, camera.position.y, camera.position.z,
        camera.target.x, camera.target.y, camera.target.z,
        camera.up.x, camera.up.y, camera.up.z
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

    //Render pojectiles
    for (vector<ItemProjectile*>::iterator it = ItemProjectile::ItemProjectileList.begin(); it != ItemProjectile::ItemProjectileList.end(); ++it)
    {
        modelStack.PushMatrix();
        modelStack.Translate(
            (*it)->position.x,
            (*it)->position.y,
            (*it)->position.z
            );
        modelStack.Scale(0.5, 0.5, 0.5);
        RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET), false);
        modelStack.PopMatrix();
    }

    RenderRockScene();

    //Trap placing
    double x, y;
    Application::GetCursorPos(&x, &y);
    modelStack.PushMatrix();
    modelStack.Translate(SharedData::GetInstance()->player->GetPositionVector().x + SharedData::GetInstance()->player->GetViewVector().x * 20, 0.5, SharedData::GetInstance()->player->GetPositionVector().z + SharedData::GetInstance()->player->GetViewVector().z * 20);
    modelStack.Scale(1, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TRAP), false);
    modelStack.PopMatrix();

    std::stringstream ss;
    ss << "FPS: " << fps;
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3, 0, 3);

    std::stringstream ss1;
    ss1 << "Noise: " << SharedData::GetInstance()->player->GetNoiseFactor();
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss1.str(), Color(1, 1, 0), 3, 0, 6);
    
    std::stringstream ss2;
    ss2 << "Distance Squared: " << (SharedData::GetInstance()->player->GetPositionVector() - rockWorld.position[monster]).LengthSquared();
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss2.str(), Color(1, 1, 0), 3, 0, 9);

    std::stringstream ss3;
    ss3 << "Distance Treshold: " << 750.0f * SharedData::GetInstance()->player->m_noiseFactor;
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss3.str(), Color(1, 1, 0), 3, 0, 12);
}

void SceneRock::RenderRockScene()
{
    //Ground mesh
    modelStack.PushMatrix();
    modelStack.Translate(0, 0, 0);
    modelStack.Rotate(-90, 1, 0, 0);
    modelStack.Scale(200, 200, 100);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCK_TERRAIN), true);
    modelStack.PopMatrix();

    //Gameobjects
    //if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_ENTER].isHeldDown)
        RenderGameObjects(&rockWorld);

    //Skyplane
    modelStack.PushMatrix();
    modelStack.Translate(500, 2800, -500);
    //modelStack.Rotate(0, 0,0,0);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GRASSZONESKYPLANE), false);
    modelStack.PopMatrix();

    //Render hitboxes
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_ENTER].isHeldDown)
    for (GameObject tallGrass = 0; tallGrass < rockWorld.GAMEOBJECT_COUNT; ++tallGrass)
    {
        if ((rockWorld.mask[tallGrass] & COMPONENT_HITBOX) == COMPONENT_HITBOX)
        {

            modelStack.PushMatrix();
            modelStack.Translate(rockWorld.hitbox[tallGrass].m_origin.x, rockWorld.hitbox[tallGrass].m_origin.y, rockWorld.hitbox[tallGrass].m_origin.z);
            modelStack.Scale(rockWorld.hitbox[tallGrass].m_scale.x, rockWorld.hitbox[tallGrass].m_scale.y, rockWorld.hitbox[tallGrass].m_scale.z);
            //modelStack.Rotate(0, 0, 0, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            modelStack.PopMatrix();
        }
    }
}

bool SceneRock::ViewCheckPosition(Vector3 pos, float degree)
{
    if (pos != camera.position)
    {
        Vector3 view = (pos - camera.position).Normalized();

        float angleX = Math::RadianToDegree(acos(view.Dot(camera.target)));

        if (angleX <= degree)
        {
            return true;
        }
        if (angleX > degree)
        {
            return false;
        }
    }
}

void SceneRock::Exit()
{

}

//========================
// == OBJECTS TO RENDER
//========================
