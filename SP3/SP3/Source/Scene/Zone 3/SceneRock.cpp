#include "GL\glew.h"
//#include "../../shader.hpp"

#include "SceneRock.h"
#include "../../General/Application.h"
//#include "../../Graphics/LoadOBJ/LoadTGA.h"
//#include "../../Graphics/Mesh/MeshBuilder.h"
#include "../../General/SharedData.h"
#include "../../GameObject/MonsterFactory.h"
#include "../../GameObject/AI_Strategy.h"

#include <sstream>

SceneRock::SceneRock(std::string name) : Scene(name)
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
	SharedData::GetInstance()->sound->PlayMusic("Sound//RockZone//RockScene.wav");
    Scene::LoadLevelMap("GameData/RockScene.csv");
    for (int rows = 0; rows < Scene::m_rows; ++rows)
    {
        for (int cols = 0; cols < Scene::m_cols; ++cols)
        {
            char tile = m_levelMap[rows][cols];

            if (tile == '0')
                continue;

            LevelGenerationMap::iterator it = Scene::m_levelGenerationData.find(tile);

            // it->first is tileCount
            // first in it->second is mesh
            // second in it->second is vector of components

            if (tile >= 'A' && tile <= 'Z')
            {
                GameObject go = createGO(&rockWorld);

                for (unsigned i = 0; i < (it->second).second.size(); ++i)
                {
                    rockWorld.mask[go] = rockWorld.mask[go] | (it->second).second[i];
                }

                rockWorld.position[go].Set(cols * Scene::tileSize, 0.f, rows * Scene::tileSize);
                rockWorld.hitbox[go].m_origin = rockWorld.position[go];
                rockWorld.hitbox[go].m_scale.Set(3.f, 7.f, 3.f);
                rockWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh((it->second).first);
                rockWorld.appearance[go].scale.Set(Math::RandFloatMinMax(0.8f, 1.f), Math::RandFloatMinMax(1.2f, 2.f), Math::RandFloatMinMax(0.8f, 1.f));
                rockWorld.appearance[go].angle = Math::RandFloatMinMax(0.f, 360.f);
                rockWorld.appearance[go].billboard = false;
				
				if (tile == 'M')
				{
					rockWorld.hitbox[go].m_origin = rockWorld.position[go];
					rockWorld.hitbox[go].m_scale.Set(5.f, 15.f, 5.f);
				}
                //grass.appearance[go].scale.Set(1, 1, 1);
            }
            else if (tile >= '1' && tile <= '9')
            {
                GameObject go = createGO(&rockWorld);
                rockWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_AI | COMPONENT_OBSTACLE;
                //rockWorld.velocity[go].Set(Math::RandFloatMinMax(0.f, 1.f), 0, Math::RandFloatMinMax(0.f, 1.f));
                //rockWorld.velocity[go].SetZero();
                rockWorld.position[go].Set(cols * tileSize + Math::RandFloatMinMax(0.f, 1.f), 0.f, rows * tileSize + Math::RandFloatMinMax(0.f, 1.f));
                rockWorld.hitbox[go].m_origin = rockWorld.position[go] + Vector3(0, 0.75f, -0.3f);
                switch (tile)
                {
                case '1':
                    rockWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_GOLEM);
                    rockWorld.monster[go] = MonsterFactory::CreateMonster("Golem");
                    rockWorld.hitbox[go].m_scale.Set(3.f, 5.f, 3.5f);
                    rockWorld.appearance[go].scale.Set(1.25f, 1.25f, 1.25f);
                    break;

                case '2':
                    rockWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_FOSSIL);
                    rockWorld.monster[go] = MonsterFactory::CreateMonster("Fossil");
                    rockWorld.hitbox[go].m_scale.Set(4.f, 4.5f, 4.f);
                    rockWorld.appearance[go].scale.Set(1.25f, 1.25f, 1.25f);
                    break;

                case '3':
                    rockWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BOSS_ROCKSNAKE);
                    rockWorld.monster[go] = MonsterFactory::CreateMonster("RockSnake");
                    rockWorld.hitbox[go].m_scale.Set(11.f, 15.f, 11.f);
                    rockWorld.appearance[go].scale.Set(4, 4, 4);
                    break;
                }
                rockWorld.monster[go]->m_position = rockWorld.position[go];
                rockWorld.appearance[go].angle = 0.f;
                rockWorld.appearance[go].billboard = false;

                rockWorld.monster[go]->SetIdleState();
                rockWorld.velocity[go] = rockWorld.monster[go]->m_velocity;

            }
        }
    }

    ////Test monster
    //monster = createGO(&rockWorld);
    //rockWorld.mask[monster] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_AI;
    //rockWorld.position[monster].Set(0, 0, 25);//Zero();
    //rockWorld.velocity[monster].SetZero();//(0, 0, 1);
    //rockWorld.hitbox[monster].m_origin = rockWorld.position[monster] + Vector3(0, 0.75f, -0.3f);
    //rockWorld.hitbox[monster].m_scale.Set(1.5f, 1.5f, 1.75f);
    //rockWorld.appearance[monster].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_RABBIT);
    //rockWorld.appearance[monster].scale.Set(1, 1, 1);
    //
    //rock = createGO(&rockWorld);
    //rockWorld.mask[rock] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX;
    //rockWorld.position[rock].Set(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z);
    //rockWorld.appearance[rock].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE);
    //rockWorld.appearance[rock].scale.Set(1, 1, 1);
    //
    //net = createGO(&rockWorld);
    //rockWorld.mask[net] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE;
    //rockWorld.position[net].SetZero();
    //rockWorld.appearance[net].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET);
    //rockWorld.appearance[net].scale.Set(2, 2, 2);
    //HITBOX.m_origin = Vector3(0, 5, 0);
    //HITBOX.m_scale = Vector3(10, 10, 10);

    //b_capturing = false;
    //b_captured = false;
    //captureCounter = 0;

    camera.Update();

    // for spawning rocks
    d_spawnRocksTimer = 0.0;
    d_spawnRocksCooldown = 5.0;
}

void SceneRock::Update(double dt)
{

    fps = (float)(1.f / dt);

    // for buffer time between projectile launches
    SharedData::GetInstance()->particleManager->d_timeCounter += dt;
    ItemProjectile::d_rockCounter += dt;
    ItemProjectile::d_netCounter += dt;
    ItemProjectile::d_baitCounter += dt;
    ItemProjectile::d_trapCounter += dt;
    d_spawnRocksTimer += dt;

    //===============================================================================================================================//
    //                                                             Pause                                                             //
    //===============================================================================================================================//

    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_ESCAPE].isPressed)
    {
        SharedData::GetInstance()->sceneManager->SetPauseState();
    }

    //===============================================================================================================================//
    //                                                            Updates                                                            //
    //===============================================================================================================================//

    //Monster Update
    UpdateMonsters(dt, &rockWorld);

    //Movement update for Gameobjects
    UpdateGameObjects(&rockWorld, dt);

    //Player Update
    UpdatePlayer(dt, &rockWorld);

    //Camera Update
    camera.Update();

    // Check to see if text_C and text_M should be destroyed
    DestroyText(&rockWorld);

    //===============================================================================================================================//
    //                                                            Checks                                                             //
    //===============================================================================================================================//
    ////////////////////////////////////////////////
    ////Projectile checks//////////////ROCKS////////
    ////////////////////////////////////////////////

    UpdateRockProjectiles(&rockWorld);


    //////////////////////////////////////////////
    ////Projectile checks//////////////NET////////
    //////////////////////////////////////////////

    UpdateNetProjectiles(&rockWorld);

    //////////////////////////////////////////////
    ////////BAIT /////////////////////////////////
    //////////////////////////////////////////////
    UpdateBaitProjectiles(&rockWorld);
    UpdateBait(&rockWorld, dt);

    //Trap check (radius)
    UpdateTrap(&rockWorld, dt);

    // Monster damage to player
    CheckMonsterAttack(&rockWorld);

    //////////////////////////////////////////////
    ////////PARTICLES ////////////////////////////
    //////////////////////////////////////////////
	//SpawnSceneParticles();
    UpdateParticles(&rockWorld, dt);

    //===============================================================================================================================//
    //                                                            Key Inputs                                                         //
    //===============================================================================================================================//


    // Update Player Inventory
    UpdateInventory();
	f_RotateMonster += dt * 50;
    if (b_Rocks)
    {
        //Rock projectile
        f_RotateRock += dt * 50;
        ShootRock();
    }
    if (b_Nets)
    {
        //Net projectile
        f_RotateNet += dt * 50;
        ShootNet();
    }
    if (b_Baits)
    {
        //Bait Projectile
        f_RotateBait += dt * 50;
        ShootBait();
    }
    if (b_Traps)
    {
        f_RotateTrap += dt * 50;
        PlaceTrap(&rockWorld);
    }

    // Check pick up monster
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_E].isPressed)
    {
        for (GameObject GO = 0; GO < rockWorld.GAMEOBJECT_COUNT; ++GO)
        {
            // check for picking up net
            if ((rockWorld.mask[GO] & COMPONENT_CAPTURE) == COMPONENT_CAPTURE)
            {
                if (CheckPickUpCaughtMonster(&rockWorld, GO))
                    break;
            }

            // check for interacting with money tree
            else if ((rockWorld.mask[GO] & COMPONENT_MONEYTREE) == COMPONENT_MONEYTREE)
            {
                if (CheckInteractMoneyTree(&rockWorld, GO))
                    break;
            }

            // check for interacting with coin
            if ((rockWorld.mask[GO] & COMPONENT_COIN) == COMPONENT_COIN)
            {
                if (CheckPickUpCoin(&rockWorld, GO))
                    break;
            }

            // check for interacting with coin
            else if ((rockWorld.mask[GO] & COMPONENT_ROCKS) == COMPONENT_ROCKS)
            {
                if (CheckPickUpRock(&rockWorld, GO))
                    break;
            }

            // check for interacting with meat
            else if ((rockWorld.mask[GO] & COMPONENT_MEAT) == COMPONENT_MEAT)
            {
                if (CheckPickUpMeat(&rockWorld, GO))
                    break;
            }

        }

    }

    // Environmental effect: rocks fall from the sky
    UpdateFallingRocks(dt);
    // spawn them
    if (d_spawnRocksTimer >= d_spawnRocksCooldown)
    {
        SceneEnvironmentEffect();
        d_spawnRocksTimer = 0.0;
    }

    // TEMPORARY DEBUG: check of inventory
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_C].isPressed)
    {
        std::cout << "MONSTER INVENTORY: ";
        for (unsigned i = 0; i < SharedData::GetInstance()->player->monsterList.size(); ++i)
        {
            std::cout << SharedData::GetInstance()->player->monsterList[i] << " ";
        }
        std::cout << std::endl;
    }

    //Update Projectiles vector - delete them from vector
    itemProjectile->UpdateProjectile(dt);
    rockProjectile->UpdateRockProjectile(dt);
    netProjectile->UpdateNetProjectile(dt);
    baitProjectile->UpdateBaitProjectile(dt);
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
    //RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_AXES), false);

    RenderProjectiles();

    RenderRockScene();

    //Trap placing
    if (b_Traps)
    {
        double x, y;
        Application::GetCursorPos(&x, &y);
        modelStack.PushMatrix();
        modelStack.Translate(SharedData::GetInstance()->player->GetPositionVector().x + SharedData::GetInstance()->player->GetViewVector().x * 20, 0.5, SharedData::GetInstance()->player->GetPositionVector().z + SharedData::GetInstance()->player->GetViewVector().z * 20);
        modelStack.Scale(1, 1, 1);
        RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TRAP), false);
        modelStack.PopMatrix();
    }

    // Render particles
    RenderParticles();

    for (GameObject tallGrass = 0; tallGrass < rockWorld.GAMEOBJECT_COUNT; ++tallGrass)
    {
        if ((rockWorld.mask[tallGrass] & COMPONENT_HITBOX) == COMPONENT_HITBOX)
        {
            modelStack.PushMatrix();
            modelStack.Translate(rockWorld.hitbox[tallGrass].m_origin.x, rockWorld.hitbox[tallGrass].m_origin.y, rockWorld.hitbox[tallGrass].m_origin.z);
            modelStack.Scale(rockWorld.hitbox[tallGrass].m_scale.x, rockWorld.hitbox[tallGrass].m_scale.y, rockWorld.hitbox[tallGrass].m_scale.z);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            modelStack.PopMatrix();
        }
    }
    
    // HUD THINGS
    if (SharedData::GetInstance()->sceneManager->GetGameState() == SceneManager::GAMESTATE_GAMEPLAY)
    {
        RenderHUD(&rockWorld);
    }
}

void SceneRock::RenderRockScene()
{
    //Ground mesh
    modelStack.PushMatrix();
    modelStack.Translate(100, -7, 100);
    modelStack.Scale(300.0f, 30.0f, 300.0f);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCK_TERRAIN), true);
    modelStack.PopMatrix();

    //Skyplane
    modelStack.PushMatrix();
    modelStack.Translate(500, 2500, -500);
    //modelStack.Rotate(0, 0,0,0);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCK_SKYPLANE), false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(SharedData::GetInstance()->player->PlayerHitBox.m_origin.x, SharedData::GetInstance()->player->PlayerHitBox.m_origin.y, SharedData::GetInstance()->player->PlayerHitBox.m_origin.z);
    modelStack.Scale(SharedData::GetInstance()->player->PlayerHitBox.m_scale.x, SharedData::GetInstance()->player->PlayerHitBox.m_scale.y, SharedData::GetInstance()->player->PlayerHitBox.m_scale.z);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    modelStack.PopMatrix();

    RenderGameObjects(&rockWorld);
}

void SceneRock::Exit()
{
    for (unsigned GO = 0; GO < rockWorld.GAMEOBJECT_COUNT; ++GO)
    {
        // call destroyGO instead
        destroyGO(&rockWorld, GO);
    }

    SharedData::GetInstance()->particleManager->ClearParticles();

    SharedData::GetInstance()->player->ClearCapturedMonsters();
}

bool SceneRock::CheckInteractMoneyTree(World *world, GameObject GO)
{
    if ((camera.position - rockWorld.position[GO]).LengthSquared() < 250)
    {
        if (ViewCheckPosition(rockWorld.position[GO], 45.f) == true)
        {
            int loot = Math::RandIntMinMax(0, 3);
            switch (loot)
            {
            case 0:
            {
                      rockWorld.mask[GO] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_OBSTACLE | COMPONENT_COIN;
                      rockWorld.appearance[GO].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_COIN);
                      rockWorld.appearance[GO].scale.Set(5, 5, 5);
                      break;
            }
            case 1:
            {
                      rockWorld.mask[GO] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_OBSTACLE | COMPONENT_ROCKS;
                      rockWorld.appearance[GO].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCKS1);
                      rockWorld.appearance[GO].scale.Set(2, 2, 2);
                      break;
            }
            case 2:
            {
                      rockWorld.mask[GO] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_AI | COMPONENT_OBSTACLE;
                      rockWorld.appearance[GO].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_GOLEM);
                      rockWorld.appearance[GO].scale.Set(1.25f, 1.25f, 1.25f);
                      rockWorld.monster[GO] = MonsterFactory::CreateMonster("Golem");
                      rockWorld.hitbox[GO].m_origin = rockWorld.position[GO] + Vector3(0, 0.75f, -0.3f);
                      rockWorld.hitbox[GO].m_scale.Set(3.f, 4.f, 3.5f);

                      rockWorld.monster[GO]->m_position = rockWorld.position[GO];
                      rockWorld.appearance[GO].angle = 0.f;
                      rockWorld.appearance[GO].billboard = false;

                      rockWorld.monster[GO]->SetIdleState();
                      rockWorld.velocity[GO] = rockWorld.monster[GO]->m_velocity;
                      break;
            }
            case 3:
                // Nothing
                SharedData::GetInstance()->sound->PlaySoundEffect3D("Sound//DropNothing.wav",
                    irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
                    irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
                    irrklang::vec3df(world->position[GO].x, world->position[GO].y, world->position[GO].z));
                destroyGO(&rockWorld, GO);
                break;
            }

            return true;
        }
    }

    return false;
}

void SceneRock::SpawnSceneParticles()
{
	//SharedData::GetInstance()->particleManager->SpawnParticle(Vector3(Math::RandFloatMinMax(-100, 100), 50, Math::RandFloatMinMax(-100, 100)), ParticleObject::P_ROCK);
}

void SceneRock::SceneEnvironmentEffect()
{
    unsigned int spawnCount = Math::RandIntMinMax(0, 8);   // number of rocks that will spawn this time round
    bool spawn = false;
    int randRow;
    int randCol;

    for (unsigned i = 0; i < spawnCount; ++i)
    {
        int runCount = 0;
        for (unsigned run = 0; run < 5; ++run) {
            randRow = Math::RandIntMinMax(0, 39);
            randCol = Math::RandIntMinMax(0, 39);

            char tile = m_levelMap[randRow][randCol];

            if (tile == '0')
            {
                spawn = true;
                break;
            }
        }
        if (spawn) {
            Vector3 position(randCol * Scene::tileSize, 0.f, randRow * Scene::tileSize);
            //SharedData::GetInstance()->particleManager->SpawnParticle(position, ParticleObject::P_FALLINGLEAF);
            //createGO with a boulder component and the position damn damn high
            // when rendering game objects, if component_boulder, render a marked red circle on the ground

            GameObject go = createGO(&rockWorld);
            rockWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_BOULDER;
            rockWorld.velocity[go].Set(0, -1.f, 0);
            rockWorld.position[go].Set(randCol * tileSize + Math::RandFloatMinMax(-1.f, 1.f), 40.f, randRow * tileSize + Math::RandFloatMinMax(-1.f, 1.f));
            rockWorld.hitbox[go].m_origin = rockWorld.position[go] - Vector3(0, 3, 0);
            rockWorld.hitbox[go].m_scale.Set(2.5f, 10.f, 2.5f);
            rockWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCK_PILLAR);
            rockWorld.appearance[go].scale.Set(1.f, 2.f, 1.f);
            rockWorld.appearance[go].billboard = false;

            spawn = false;
        }
    }
}

void SceneRock::UpdateFallingRocks(double dt)
{
    for (GameObject GO = 0; GO < rockWorld.GAMEOBJECT_COUNT; ++GO)
    {
        if ((rockWorld.mask[GO] & COMPONENT_BOULDER) == COMPONENT_BOULDER)
        {
            // Check collision with player
            if (rockWorld.hitbox[GO].CheckCollision(SharedData::GetInstance()->player->PlayerHitBox))
            {
                std::cout << SharedData::GetInstance()->player->PlayerHitBox.m_origin << SharedData::GetInstance()->player->GetPositionVector() << std::endl;
                SharedData::GetInstance()->player->TakeDamage(10);
                destroyGO(&rockWorld, GO);
            }

            else if (rockWorld.position[GO].y < -5.f * rockWorld.appearance[GO].scale.y)
            {
                destroyGO(&rockWorld, GO);
            }
        }
    }
}