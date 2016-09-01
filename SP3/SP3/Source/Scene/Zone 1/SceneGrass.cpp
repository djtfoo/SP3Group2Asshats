#include "GL\glew.h"
//#include "../../shader.hpp"

#include "SceneGrass.h"
#include "../../General/Application.h"
//#include "../../Graphics/LoadOBJ/LoadTGA.h"
//#include "../../Graphics/Mesh/MeshBuilder.h"
#include "../../General/SharedData.h"
#include "../../GameObject/MonsterFactory.h"
#include "../../GameObject/AI_Strategy.h"

SceneGrass::SceneGrass(std::string name) : Scene(name)
{

}

SceneGrass::~SceneGrass()
{

}

void SceneGrass::Init()
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
    memset(&grass, 0, sizeof(grass));

    // Load map
    //SharedData::GetInstance()->sound->PlayMusic("Sound//GrassZone//GrassScene.wav");
    Scene::LoadLevelMap("GameData/GrassScene.csv");
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
                GameObject go = createGO(&grass);

                for (unsigned i = 0; i < (it->second).second.size(); ++i)
                {
                    grass.mask[go] = grass.mask[go] | (it->second).second[i];
                }

                grass.position[go].Set(cols * Scene::tileSize, 0.f, rows * Scene::tileSize);
                grass.hitbox[go].m_origin = grass.position[go] + Vector3(0, 2, 0);
                grass.hitbox[go].m_scale.Set(4.f, 8.f, 4.f);
                grass.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh((it->second).first);
                if (tile == 'G') {      // money tree
                    grass.appearance[go].scale.Set(Math::RandFloatMinMax(0.8f, 1.f), Math::RandFloatMinMax(1.f, 1.3f), Math::RandFloatMinMax(0.8f, 1.f));
                }
                else {
                    grass.appearance[go].scale.Set(Math::RandFloatMinMax(0.8f, 1.f), Math::RandFloatMinMax(0.5f, 1.f), Math::RandFloatMinMax(0.8f, 1.f));
                }
                grass.appearance[go].angle = Math::RandFloatMinMax(0.f, 360.f);
                grass.appearance[go].billboard = false;
                //grass.appearance[go].scale.Set(1, 1, 1);

				if (tile == 'D')
				{
					grass.hitbox[go].m_scale.Set(5.f, 8.f, 5.f);
				}
				if (tile == 'C')
				{
					grass.hitbox[go].m_scale.Set(5.f, 8.f, 5.f);
				}
            }
            else if (tile >= '1' && tile <= '9')
            {
                GameObject go = createGO(&grass);
                grass.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_AI | COMPONENT_OBSTACLE;
                //grass.velocity[go].Set(Math::RandFloatMinMax(0.f, 1.f), 0, Math::RandFloatMinMax(0.f, 1.f));
                //grass.velocity[go].SetZero();
                grass.position[go].Set(cols * tileSize + Math::RandFloatMinMax(0.f, 1.f), 0.f, rows * tileSize + Math::RandFloatMinMax(0.f, 1.f));
                grass.hitbox[go].m_origin = grass.position[go] + Vector3(0, 0.75f, -0.3f);
                switch (tile)
                {
                case '1':
                    grass.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_RABBIT);
                    grass.monster[go] = MonsterFactory::CreateMonster("Rabbit");
                    grass.hitbox[go].m_scale.Set(3.f, 4.f, 3.5f);
                    grass.appearance[go].scale.Set(1.25f, 1.25f, 1.25f);
                    break;
                
                case '2':
                    grass.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_BIRD);
                    grass.monster[go] = MonsterFactory::CreateMonster("Bird");
                    grass.hitbox[go].m_scale.Set(4.f, 4.f, 4.f);
                    grass.appearance[go].scale.Set(1.25f, 1.25f, 1.25f);
                    break;
                
                case '3':
                    grass.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BOSS_FAIRY);
                    grass.monster[go] = MonsterFactory::CreateMonster("Fairy");
                    grass.hitbox[go].m_scale.Set(5.5f, 17.5f, 5.5f);
                    grass.appearance[go].scale.Set(4, 4, 4);
                    break;
                }
                grass.monster[go]->m_position = grass.position[go];
                grass.appearance[go].angle = 0.f;
                grass.appearance[go].billboard = false;

                grass.monster[go]->SetIdleState();
                grass.velocity[go] = grass.monster[go]->m_velocity;

            }
        }
    }

    // disable fog
    glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_FOG_ENABLED), false);

    camera.Update();
}

void SceneGrass::Update(double dt)
{
	//Calculating aspect ratio
	//m_worldHeight = 100.f;
	//m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	fps = (float)(1.f / dt);

    // for buffer time
    SharedData::GetInstance()->particleManager->d_timeCounter += dt;
    //ItemProjectile::d_rockCounter += dt;
    //ItemProjectile::d_netCounter += dt;
    //ItemProjectile::d_baitCounter += dt;
    //ItemProjectile::d_trapCounter += dt;


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
    UpdateMonsters(dt, &grass);

	//Movement update for Gameobjects
	UpdateGameObjects(&grass, dt);

	//Player Update
    UpdatePlayer(dt, &grass);

    //Camera Update
    camera.Update();

	// Check to see if text_C and text_M should be destroyed
    DestroyText(&grass);

	//===============================================================================================================================//
	//                                                            Checks                                                             //
	//===============================================================================================================================//
	////////////////////////////////////////////////
	////Projectile checks//////////////ROCKS////////
	////////////////////////////////////////////////

    UpdateRockProjectiles(&grass);


	//////////////////////////////////////////////
	////Projectile checks//////////////NET////////
	//////////////////////////////////////////////
	
    UpdateNetProjectiles(&grass);

	//////////////////////////////////////////////
	////////BAIT /////////////////////////////////
	//////////////////////////////////////////////
    UpdateBaitProjectiles(&grass);
    UpdateBait(&grass, dt);

	//Trap check (radius)
    UpdateTrap(&grass, dt);

	// Monster damage to player
    CheckMonsterAttack(&grass);

    //////////////////////////////////////////////
    ////////PARTICLES ////////////////////////////
    //////////////////////////////////////////////
	//SpawnSceneParticles();
    UpdateParticles(&grass, dt);

	//===============================================================================================================================//
	//                                                            Key Inputs                                                         //
	//===============================================================================================================================//

    // Update Player Inventory
    UpdateInventory(dt);
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
        PlaceTrap(&grass);
    }

	// Check pick up monster
	if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_E].isPressed)
	{
		for (GameObject GO = 0; GO < grass.GAMEOBJECT_COUNT; ++GO)
		{
			// check for picking up captured monster
            if ((grass.mask[GO] & COMPONENT_CAPTURE) == COMPONENT_CAPTURE)
            {
                if (CheckPickUpCaughtMonster(&grass, GO))
                    break;
            }

			// check for interacting with money tree
			else if ((grass.mask[GO] & COMPONENT_MONEYTREE) == COMPONENT_MONEYTREE)
			{
                if (CheckInteractMoneyTree(&grass, GO))
                    break;
			}

			// check for interacting with coin
			else if ((grass.mask[GO] & COMPONENT_COIN) == COMPONENT_COIN)
			{
                if (CheckPickUpCoin(&grass, GO))
                    break;
			}

            // check for interacting with meat
            else if ((grass.mask[GO] & COMPONENT_MEAT) == COMPONENT_MEAT)
            {
                if (CheckPickUpMeat(&grass, GO))
                    break;
            }
		}

	}

    //Update Projectiles vector - delete them from vector
    itemProjectile->UpdateProjectile(dt);
    rockProjectile->UpdateRockProjectile(dt);
    netProjectile->UpdateNetProjectile(dt);
    baitProjectile->UpdateBaitProjectile(dt);
}

void SceneGrass::Render()
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

	Light light = SharedData::GetInstance()->graphicsLoader->GetLights();

	if (light.type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light.position.x, light.position.y, light.position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_LIGHT0_POSITION), 1, &lightDirection_cameraspace.x);
	}

	//RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_AXES), false);

    RenderProjectiles();

	RenderGrassScene();

	//visible hitbox
	/*modelStack.PushMatrix();
	modelStack.Translate(HITBOX.m_origin.x, HITBOX.m_origin.y, HITBOX.m_origin.z);
	modelStack.Scale(HITBOX.m_scale.x * 2, HITBOX.m_scale.y * 2, HITBOX.m_scale.z * 2);
	RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
	modelStack.PopMatrix();*/

	//modelStack.PushMatrix();
	//modelStack.Translate(SharedData::GetInstance()->player->PlayerHitBox.m_origin.x, 0.1, SharedData::GetInstance()->player->PlayerHitBox.m_origin.z);
	//modelStack.Scale(SharedData::GetInstance()->player->PlayerHitBox.m_scale.x, SharedData::GetInstance()->player->PlayerHitBox.m_scale.y, /SharedData::GetInstance/()->player->PlayerHitBox.m_scale.z);
	//RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_PLAYERBOX), false);
	//modelStack.PopMatrix();

    //Trap placing
    if (b_Traps)
    {
        RenderTrap();
    }

    // Render particles
    RenderParticles();

    // for rendering hitboxes
    //for (GameObject tallGrass = 0; tallGrass < grass.GAMEOBJECT_COUNT; ++tallGrass)
    //{
    //    if ((grass.mask[tallGrass] & COMPONENT_HITBOX) == COMPONENT_HITBOX)
    //    {
    //        modelStack.PushMatrix();
    //        modelStack.Translate(grass.hitbox[tallGrass].m_origin.x, grass.hitbox[tallGrass].m_origin.y, grass.hitbox[tallGrass].m_origin.z);
    //        modelStack.Scale(grass.hitbox[tallGrass].m_scale.x, grass.hitbox[tallGrass].m_scale.y, grass.hitbox[tallGrass].m_scale.z);
    //        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //        RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    //        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //        modelStack.PopMatrix();
    //    }
    //}

    // HUD THINGS
    if (SharedData::GetInstance()->sceneManager->GetGameState() == SceneManager::GAMESTATE_GAMEPLAY)
    {
        RenderHUD(&grass);
    }
}

void SceneGrass::RenderGrassScene()
{
	//Ground mesh
	modelStack.PushMatrix();
	modelStack.Translate(100, -7, 100);
	modelStack.Scale(300.0f, 30.0f, 300.0f);
	RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GRASS_TERRAIN), true);
	modelStack.PopMatrix();

	//Skyplane
	modelStack.PushMatrix();
	modelStack.Translate(500, 2500, -500);
	//modelStack.Rotate(0, 0,0,0);
	RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GRASS_SKYPLANE), false);
	modelStack.PopMatrix();

    //modelStack.PushMatrix();
    //modelStack.Translate(SharedData::GetInstance()->player->PlayerHitBox.m_origin.x, SharedData::GetInstance()->player->PlayerHitBox.m_origin.y, SharedData::GetInstance()->player->PlayerHitBox.m_origin.z);
    //modelStack.Scale(SharedData::GetInstance()->player->PlayerHitBox.m_scale.x, SharedData::GetInstance()->player->PlayerHitBox.m_scale.y, SharedData::GetInstance()->player->PlayerHitBox.m_scale.z);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //modelStack.PopMatrix();

    RenderGameObjects(&grass);
}

void SceneGrass::Exit()
{
	for (unsigned GO = 0; GO < grass.GAMEOBJECT_COUNT; ++GO)
	{
		//if (grass.monster[GO])
		//{
		//    delete grass.monster[GO];
		//}

		// call destroyGO instead
		destroyGO(&grass, GO);
	}

    SharedData::GetInstance()->particleManager->ClearParticles();

    SharedData::GetInstance()->player->ClearCapturedMonsters();
}

void SceneGrass::SpawnSceneParticles()
{
    //if (SharedData::GetInstance()->particleManager->d_timeCounter > 0.8)
    //{
        //for (GameObject GO = 0; GO < grass.GAMEOBJECT_COUNT; ++GO)
        //{
        //    if ((grass.mask[GO] & COMPONENT_OBSTACLE) == COMPONENT_OBSTACLE)
        //    {
        //        SharedData::GetInstance()->particleManager->SpawnParticle(grass.position[GO], ParticleObject::P_FALLINGLEAF);
        //    }
        //}

        //for (int rows = 0; rows < Scene::m_rows; ++rows)
        //{
        //    for (int cols = 0; cols < Scene::m_cols; ++cols)
        //    {
        //        char tile = m_levelMap[rows][cols];
        //
        //        if (tile == 'C' || tile == 'D')
        //        {
        //            Vector3 position(cols * Scene::tileSize, 0.f, rows * Scene::tileSize);
        //            SharedData::GetInstance()->particleManager->SpawnParticle(position, ParticleObject::P_FALLINGLEAF);
        //        }
        //    }
        //}
    //}

    unsigned int spawnCount = Math::RandIntMinMax(0, 50);   // number of particles that will spawn this time round
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

            if (tile == 'C' || tile == 'D')
            {
                spawn = true;
                break;
            }
        }
        if (spawn) {
            Vector3 position(randCol * Scene::tileSize, 0.f, randRow * Scene::tileSize);
            SharedData::GetInstance()->particleManager->SpawnParticle(position, ParticleObject::P_FALLINGLEAF);
            spawn = false;
        }
    }
}

void SceneGrass::SceneEnvironmentEffect()
{

}