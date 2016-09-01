#include "GL\glew.h"
//#include "../../shader.hpp"

#include "SceneLava.h"
#include "../../General/Application.h"
//#include "../../Graphics/LoadOBJ/LoadTGA.h"
//#include "../../Graphics/Mesh/MeshBuilder.h"
#include "../../General/SharedData.h"
#include "../../GameObject/MonsterFactory.h"

#include "../../GameObject/AI_Strategy.h"

#include <sstream>

SceneLava::SceneLava(std::string name) : Scene(name)
{
}

SceneLava::~SceneLava()
{
}

void SceneLava::Init()
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
	memset(&lava, 0, sizeof(lava));

	// Load map
	SharedData::GetInstance()->sound->PlayMusic("Sound//LavaZone//LavaScene.mp3");
	Scene::LoadLevelMap("GameData/LavaScene.csv");
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
				GameObject go = createGO(&lava);

				for (unsigned i = 0; i < (it->second).second.size(); ++i)
				{
					lava.mask[go] = lava.mask[go] | (it->second).second[i];
				}

                if (tile == 'J')    // "Money Tree"
                {
                    lava.position[go].Set(cols * Scene::tileSize, 0.f, rows * Scene::tileSize);
                    lava.hitbox[go].m_origin = lava.position[go] + Vector3(0, 2, 0);
                    lava.hitbox[go].m_scale.Set(8.f, 17.f, 8.f);
                    lava.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_VOLCANO);
                    lava.appearance[go].scale.Set(Math::RandFloatMinMax(1.5f, 1.7f), Math::RandFloatMinMax(3.5f, 4.f), Math::RandFloatMinMax(1.5f, 1.7f));
                    lava.appearance[go].billboard = false;
                }
				else if (tile == 'H')
				{
					lava.position[go].Set(cols * Scene::tileSize, 0.f, rows * Scene::tileSize);
					lava.hitbox[go].m_origin = lava.position[go] + Vector3(0, 2, 0);
					lava.hitbox[go].m_scale.Set(5.f, 15.f, 5.f);
					lava.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_RED_CRYSTAL);
					lava.appearance[go].scale.Set(Math::RandFloatMinMax(0.8f, 1.f), Math::RandFloatMinMax(0.5f, 1.f), Math::RandFloatMinMax(0.8f, 1.f));
					lava.appearance[go].billboard = false;
				}
                else
                {
                    lava.position[go].Set(cols * Scene::tileSize, 0.f, rows * Scene::tileSize);
                    lava.hitbox[go].m_origin = lava.position[go] + Vector3(0, 2, 0);
                    lava.hitbox[go].m_scale.Set(4.f, 8.f, 4.f);
                    lava.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh((it->second).first);
                    lava.appearance[go].scale.Set(Math::RandFloatMinMax(0.8f, 1.f), Math::RandFloatMinMax(0.5f, 1.f), Math::RandFloatMinMax(0.8f, 1.f));
                    lava.appearance[go].billboard = false;
                }
				
				//grass.appearance[go].scale.Set(1, 1, 1);
			}
			else if (tile >= '1' && tile <= '9')
			{
				GameObject go = createGO(&lava);
				lava.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_AI | COMPONENT_OBSTACLE;
				//grass.velocity[go].Set(Math::RandFloatMinMax(0.f, 1.f), 0, Math::RandFloatMinMax(0.f, 1.f));
				//grass.velocity[go].SetZero();
				lava.position[go].Set(cols * tileSize + Math::RandFloatMinMax(0.f, 1.f), 0.f, rows * tileSize + Math::RandFloatMinMax(0.f, 1.f));
				lava.hitbox[go].m_origin = lava.position[go] + Vector3(0, 0.75f, -0.3f);
				switch (tile)
				{
				case '1':
					lava.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_MAGMA);
                    lava.appearance[go].scale.Set(2.5f, 2.5f, 2.5f);
					lava.monster[go] = MonsterFactory::CreateMonster("Magma");
					lava.hitbox[go].m_scale.Set(5.f, 9.5f, 5.f);
					break;

				case '2':
					lava.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_FIREBUG);
                    lava.appearance[go].scale.Set(2.f, 2.f, 2.f);
					lava.monster[go] = MonsterFactory::CreateMonster("FireBug");
					lava.hitbox[go].m_scale.Set(2.f, 5.5f, 2.f);
					break;

				case '3':
					lava.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BOSS_MAGMA_BERZEKER);
					lava.appearance[go].scale.Set(5.f, 5.f, 5.f);
					lava.monster[go] = MonsterFactory::CreateMonster("MagmaBerzeker");
					lava.hitbox[go].m_scale.Set(9.f, 25.5f, 9.f);
					break;
				}
				lava.monster[go]->m_position = lava.position[go];
                lava.appearance[go].billboard = false;

                lava.monster[go]->SetIdleState();
                lava.velocity[go] = lava.monster[go]->m_velocity;
			}
		}
	}

	//rock = createGO(&lava);
	//lava.mask[rock] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX;
	//lava.position[rock].Set(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z);
	//lava.appearance[rock].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCKS1);
	//lava.appearance[rock].scale.Set(1, 1, 1);

	//HITBOX.m_origin = Vector3(0, 5, 0);
	//HITBOX.m_scale = Vector3(10, 10, 10);

	fog.color.Set(0.8, 0, 0);
	fog.start = 10;
	fog.end = 1000;
	fog.density = 0.0005f;
	fog.type = Fog::FOG_LINEAR;
	fog.enabled = true;
	glUniform3fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_FOG_COLOR), 1, &fog.color.r);

	glUniform1f(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_FOG_START), fog.start);
	glUniform1f(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_FOG_END), fog.end);
	glUniform1f(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_FOG_DENSITY), fog.density);
	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_FOG_TYPE), fog.type);
	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_FOG_ENABLED), fog.enabled);

    camera.Update();
}

void SceneLava::Update(double dt)
{
	fps = (float)(1.f / dt);

    // for buffer time between projectile launches
    SharedData::GetInstance()->particleManager->d_timeCounter += dt;
    ItemProjectile::d_rockCounter += dt;
    ItemProjectile::d_netCounter += dt;
    ItemProjectile::d_baitCounter += dt;
    ItemProjectile::d_trapCounter += dt;

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
    UpdateMonsters(dt, &lava);

	//Movement update for Gameobjects
	UpdateGameObjects(&lava, dt);

	//Player Update
    UpdatePlayer(dt, &lava);

	//Camera Update
	camera.Update();

	// Check to see if text_C and text_M should be destroyed
    DestroyText(&lava);

	//===============================================================================================================================//
	//                                                            Checks                                                             //
	//===============================================================================================================================//
    ////////////////////////////////////////////////
    ////Projectile checks//////////////ROCKS////////
    ////////////////////////////////////////////////

    UpdateRockProjectiles(&lava);

    //////////////////////////////////////////////
    ////Projectile checks//////////////NET////////
    //////////////////////////////////////////////

    UpdateNetProjectiles(&lava);


	//////////////////////////////////////////////
	////////BAIT /////////////////////////////////
	//////////////////////////////////////////////

    UpdateBaitProjectiles(&lava);
    UpdateBait(&lava, dt);

    //Trap check (radius)
    UpdateTrap(&lava, dt);

    // Monster damage to player
    CheckMonsterAttack(&lava);

	//////////////////////////////////////////////
	////////PARTICLES ////////////////////////////
	//////////////////////////////////////////////
	SpawnSceneParticles();
    UpdateParticles(&lava, dt);

    //===============================================================================================================================//
    //                                                            Key Inputs                                                         //
    //===============================================================================================================================//


    // Update Player Inventory
    UpdateInventory();

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
        PlaceTrap(&lava);
    }

	// Check pick up monster
	if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_E].isPressed)
	{
		for (GameObject GO = 0; GO < lava.GAMEOBJECT_COUNT; ++GO)
		{
			// check for picking up net
			if ((lava.mask[GO] & COMPONENT_CAPTURE) == COMPONENT_CAPTURE)
			{
                if (CheckPickUpCaughtMonster(&lava, GO))
                    break;
			}

			// check for interacting with money tree
			else if ((lava.mask[GO] & COMPONENT_MONEYTREE) == COMPONENT_MONEYTREE)
			{
                if (CheckInteractMoneyTree(&lava, GO))
                    break;
			}

			// check for interacting with coin
			if ((lava.mask[GO] & COMPONENT_COIN) == COMPONENT_COIN)
			{
                if (CheckPickUpCoin(&lava, GO))
                {
					b_RampageMode = true;
					f_RampageTimer = 0.f;
                    for (GameObject GO = 0; GO < lava.GAMEOBJECT_COUNT; ++GO)
                    {
                        if ((lava.mask[GO] & COMPONENT_AI) == COMPONENT_AI)
                        {
                            lava.monster[GO]->SetRampageState();
                            lava.velocity[GO] = lava.monster[GO]->m_velocity;
                        }
                    }
                    break;
                }
			}

            // check for interacting with coin
			else if ((lava.mask[GO] & COMPONENT_ROCKS) == COMPONENT_ROCKS)
            {
                if (CheckPickUpRock(&lava, GO))
                    break;
			}

            // check for interacting with meat
            else if ((lava.mask[GO] & COMPONENT_MEAT) == COMPONENT_MEAT)
            {
                if (CheckPickUpMeat(&lava, GO))
                    break;
            }

		}

	}

	//b_rampageMode
	if (b_RampageMode == true)
	{
		f_RampageTimer += (float)(dt);

		for (GameObject GO = 0; GO < lava.GAMEOBJECT_COUNT; ++GO)
		{
			if ((lava.mask[GO] & COMPONENT_AI) == COMPONENT_AI && (lava.monster[GO]->m_position - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() < (5 * Scene::tileSize) * (5 * Scene::tileSize))
			{
				if (lava.monster[GO]->GetStrategyState() == AI_Strategy::STATE_RAMPAGE) {
					lava.monster[GO]->m_strategy->SetRampageStateDestination();
					lava.velocity[GO] = lava.monster[GO]->m_velocity;
				}
			}
		}

		if (f_RampageTimer >= 4.f)
		{
			b_RampageMode = false;
			f_RampageTimer = 0.0;
			for (GameObject GO = 0; GO < lava.GAMEOBJECT_COUNT; ++GO)
			{
				if ((lava.mask[GO] & COMPONENT_AI) == COMPONENT_AI)
				{
					lava.monster[GO]->SetIdleState();
				}
			}
		}
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

    //Update Projectiles
    itemProjectile->UpdateProjectile(dt);
    rockProjectile->UpdateRockProjectile(dt);
    netProjectile->UpdateNetProjectile(dt);
    baitProjectile->UpdateBaitProjectile(dt);
}

void SceneLava::Render()
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

    glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_FOG_ENABLED), fog.enabled);

    RenderProjectiles();

	RenderLavaScene();

	//visible hitbox
	//modelStack.PushMatrix();
	//modelStack.Translate(HITBOX.m_origin.x, HITBOX.m_origin.y, HITBOX.m_origin.z);
	//modelStack.Scale(HITBOX.m_scale.x * 2, HITBOX.m_scale.y * 2, HITBOX.m_scale.z * 2);
	//RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
	//modelStack.PopMatrix();

	//modelStack.PushMatrix();
	//modelStack.Translate(SharedData::GetInstance()->player->PlayerHitBox.m_origin.x, 0.1, SharedData::GetInstance()->player->PlayerHitBox.m_origin.z);
	//modelStack.Scale(SharedData::GetInstance()->player->PlayerHitBox.m_scale.x, SharedData::GetInstance()->player->PlayerHitBox.m_scale.y, /SharedData::GetInstance/()->player->PlayerHitBox.m_scale.z);
	//RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_PLAYERBOX), false);
	//modelStack.PopMatrix();

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

    glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_FOG_ENABLED), false);

    // Render particles
    RenderParticles();

	//for (GameObject tallGrass = 0; tallGrass < lava.GAMEOBJECT_COUNT; ++tallGrass)
	//{
	//    if ((lava.mask[tallGrass] & COMPONENT_HITBOX) == COMPONENT_HITBOX)
	//    {
	//        modelStack.PushMatrix();
	//        modelStack.Translate(lava.hitbox[tallGrass].m_origin.x, lava.hitbox[tallGrass].m_origin.y, lava.hitbox[tallGrass].m_origin.z);
	//        modelStack.Scale(lava.hitbox[tallGrass].m_scale.x, lava.hitbox[tallGrass].m_scale.y, lava.hitbox[tallGrass].m_scale.z);
	//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//        RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
	//        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//        modelStack.PopMatrix();
	//    }
	//}

    // HUD THINGS
    if (SharedData::GetInstance()->sceneManager->GetGameState() == SceneManager::GAMESTATE_GAMEPLAY)
    {
        RenderHUD(&lava);
    }
}

void SceneLava::RenderLavaScene()
{
	//Ground mesh
	modelStack.PushMatrix();
	modelStack.Translate(100, -7, 100);
	modelStack.Scale(300.0f, 30.0f, 300.0f);
	RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_LAVA_TERRAIN), true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(100, -1, 100);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(300.0f, 300.f, 1.f);
	RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_LAVA), false);
	modelStack.PopMatrix();

	//Skyplane
	modelStack.PushMatrix();
	modelStack.Translate(500, 2500, -500);
	//modelStack.Rotate(0, 0,0,0);
	RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_LAVA_SKYPLANE), false);
	modelStack.PopMatrix();

    RenderGameObjects(&lava);
}

void SceneLava::Exit()
{
	for (unsigned GO = 0; GO < lava.GAMEOBJECT_COUNT; ++GO)
	{
		// call destroyGO instead
		destroyGO(&lava, GO);
	}

    SharedData::GetInstance()->particleManager->ClearParticles();
}

bool SceneLava::CheckInteractMoneyTree(World *world, GameObject GO)
{
    if ((camera.position - lava.position[GO]).LengthSquared() < 150)
    {
        if (ViewCheckPosition(lava.position[GO], 45.f) == true)
        {
            //std::cout << "MoneyTree Found" << std::endl;
            lava.appearance[GO].scale.y -= 1;
            if (lava.appearance[GO].scale.y <= 0)
            {
                int loot = Math::RandIntMinMax(0, 2);
                switch (loot)
                {
                case 0:
                {
                          lava.mask[GO] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_OBSTACLE | COMPONENT_COIN;
                          lava.appearance[GO].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_COIN);
                          lava.appearance[GO].scale.Set(5, 5, 5);
                          break;
                }
                case 1:
                {
                          lava.mask[GO] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_OBSTACLE | COMPONENT_ROCKS;
                          lava.appearance[GO].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCKS1);
                          lava.appearance[GO].scale.Set(2, 2, 2);
                          break;
                }
                case 2:
                    // Nothing
                    destroyGO(&lava, GO);
                    break;
                }
            }

            return true;
        }
    }

    return false;
}

void SceneLava::SpawnSceneParticles()
{
		for (GameObject GO = 0; GO < lava.GAMEOBJECT_COUNT; ++GO)
		{
			if ((lava.mask[GO] & COMPONENT_MONEYTREE) == COMPONENT_MONEYTREE)
			{
				SharedData::GetInstance()->particleManager->SpawnParticle(lava.position[GO], ParticleObject::P_VOLCANOSPARK);
			}
		}
}

void SceneLava::SceneEnvironmentEffect()
{

}