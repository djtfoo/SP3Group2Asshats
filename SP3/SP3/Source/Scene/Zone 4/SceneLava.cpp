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

SceneLava::SceneLava()
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

				if (tile == 'G')
				{
					lava.position[go].Set(cols * Scene::tileSize, 0.f, rows * Scene::tileSize);
					lava.hitbox[go].m_origin = lava.position[go] + Vector3(0, 2, 0);
					lava.hitbox[go].m_scale.Set(8.f, 12.f, 8.f);
					lava.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_VOLCANO);
					lava.appearance[go].scale.Set(Math::RandFloatMinMax(1.5f, 1.7f), Math::RandFloatMinMax(3.5f, 4.f), Math::RandFloatMinMax(1.5f, 1.7f));
				}
				else
				{
					lava.position[go].Set(cols * Scene::tileSize, 0.f, rows * Scene::tileSize);
					lava.hitbox[go].m_origin = lava.position[go] + Vector3(0, 2, 0);
					lava.hitbox[go].m_scale.Set(4.f, 8.f, 4.f);
					lava.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh((it->second).first);
					lava.appearance[go].scale.Set(Math::RandFloatMinMax(0.8f, 1.f), Math::RandFloatMinMax(0.5f, 1.f), Math::RandFloatMinMax(0.8f, 1.f));
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

				int randCol = cols + Math::RandIntMinMax(3, 5) * Math::RandIntMinMax(-1, 1);
				int randRow = rows + Math::RandIntMinMax(3, 5) * Math::RandIntMinMax(-1, 1);
				while (randCol < 0 || randCol >= 40 || randRow < 0 || randRow >= 40 || Scene::m_levelMap[randRow][randCol] != '0')
				{
					randCol = cols + Math::RandIntMinMax(3, 5) * Math::RandIntMinMax(-1, 1);
					randRow = rows + Math::RandIntMinMax(3, 5) * Math::RandIntMinMax(-1, 1);
				}

				Vector3 RNGdestination(randCol * Scene::tileSize + Scene::tileSize * 0.5f, 0, randRow * Scene::tileSize + Scene::tileSize * 0.5f);

				lava.monster[go]->m_destination = RNGdestination;
                lava.velocity[go] = (RNGdestination - lava.position[go]).Normalized() * lava.monster[go]->GetSpeedStat();
				lava.monster[go]->m_velocity = lava.velocity[go];
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

	b_Rocks = true;
	b_Nets = false;
	b_Baits = false;
	b_Collected = false;
    f_RampageTimer = 0.f;

	f_RotateRock = 0.f;
	f_RotateNet = 0.f;
	f_RotateBait = 0.f;

    f_HighlightPos = -34.7f;
    
    camera.Update();
}

void SceneLava::Update(double dt)
{
	//Calculating aspect ratio
	//m_worldHeight = 100.f;
	//m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	fps = (float)(1.f / dt);

	//===============================================================================================================================//
	//                                                            Updates                                                            //
	//===============================================================================================================================//

	//Update Projectiles
	itemProjectile->UpdateProjectile(dt);
	rockProjectile->UpdateRockProjectile(dt);
	netProjectile->UpdateNetProjectile(dt);
	baitProjectile->UpdateBaitProjectile(dt);

	//Monster Update
	for (GameObject ai = 0; ai < lava.GAMEOBJECT_COUNT; ++ai)
	{
		if (lava.monster[ai])
		{
			lava.monster[ai]->m_position = lava.position[ai];
			lava.monster[ai]->Update(dt);
			//lava.velocity[ai].x = lava.monster[ai]->m_velocity.x * 10.f;
			//lava.velocity[ai].z = lava.monster[ai]->m_velocity.z * 10.f;

			if (lava.monster[ai]->GetStrategyState() == AI_Strategy::STATE_ALERT)
			{
				lava.monster[ai]->m_velocity.SetZero();
			}

			lava.velocity[ai] = lava.monster[ai]->m_velocity;
		}
	}

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

	//////////////////////////////////////////////
	////////PARTICLES ////////////////////////////
	//////////////////////////////////////////////
	for (GameObject GO = 0; GO < lava.GAMEOBJECT_COUNT; ++GO)
	{
		if ((lava.mask[GO] & COMPONENT_MONEYTREE) == COMPONENT_MONEYTREE)
		{
			SharedData::GetInstance()->particle->UpdateParticle(dt, lava.position[GO], ParticleObject_TYPE::P_HIDDENBONUS);
		}
	}



	for (GameObject bait = 0; bait < lava.GAMEOBJECT_COUNT; ++bait)
	{
		if ((lava.mask[bait] & COMPONENT_BAIT) == COMPONENT_BAIT)
		{

			for (GameObject ai = 0; ai < lava.GAMEOBJECT_COUNT; ++ai)
			{
				if ((lava.mask[ai] & COMPONENT_AI) == COMPONENT_AI && lava.monster[ai])
				{
					if ((lava.position[ai] - lava.position[bait]).LengthSquared() < lava.bait[bait].scentRadius && lava.bait[bait].eattingBait == false)
					{
						lava.bait[bait].foundBait = true;
						//std::cout << " FOUND BAIT " + grass.bait[bait].foundBait << std::endl;
						if (lava.bait[bait].foundBait == true)
						{
							//grass.velocity[ai] = (grass.position[bait] - grass.position[ai]).Normalized();
							//grass.velocity[ai].y = 0;
							//grass.monster[ai]->m_velocity = grass.velocity[ai];
							if (lava.monster[ai]->GetStrategyState() == AI_Strategy::STATE_ALERT || lava.monster[ai]->GetStrategyState() == AI_Strategy::STATE_IDLE || lava.monster[ai]->GetStrategyState() == AI_Strategy::STATE_BAITED)
							{
								lava.monster[ai]->GetBaited(lava.position[bait]);
								lava.bait[bait].baitedMonsters.push_back(ai);
							}
						}
					}
					if ((lava.position[ai] - lava.position[bait]).LengthSquared() < lava.bait[bait].foundRadius)
					{
						//std::cout << "EATTING BAIT " + lava.bait[bait].foundBait << std::endl;
						lava.bait[bait].foundBait = false;
						lava.bait[bait].eattingBait = true;
						lava.velocity[ai].SetZero();
						lava.monster[ai]->m_velocity.SetZero();
					}
					if (lava.bait[bait].eattingBait == true && (lava.position[ai] - lava.position[bait]).LengthSquared() < lava.bait[bait].foundRadius)
					{
						lava.bait[bait].timeEatting -= dt;
                        if (lava.bait[bait].timeEatting > 0.1f) {
                            float scale = lava.bait[bait].timeEatting / 3.f;
                            lava.appearance[bait].scale.Set(scale, scale, scale);
                        }
					}

					if (lava.bait[bait].timeEatting <= 0)
					{
						//grass.bait[bait].eattingBait = false;
						lava.bait[bait].finishedBait = true;
						//std::cout << "Done Eatting" << std::endl;
						//grass.velocity[ai] = 2;
						//grass.velocity[ai].y = 0;
					}
				}
			}

			if (lava.bait[bait].finishedBait == true) {
				//PUT WHATEVER THE RABBIT DO NORMALLY HERE :D (DONE EATTING BAIT)
				for (unsigned i = 0; i < lava.bait[bait].baitedMonsters.size(); ++i) {
					GameObject ai = lava.bait[bait].baitedMonsters[i];
					lava.monster[ai]->SetIdleState();
					lava.velocity[ai] = lava.monster[ai]->m_velocity;
				}
				destroyGO(&lava, bait);
			}
		}
	}

	//Trap check (radius)
	for (GameObject trap = 0; trap < lava.GAMEOBJECT_COUNT; ++trap)
	{
		if ((lava.mask[trap] & COMPONENT_TRAP) == COMPONENT_TRAP)
		{
			if (!lava.trap[trap].activated)
			{
				for (GameObject ai = 0; ai < lava.GAMEOBJECT_COUNT; ++ai)
				{
					if ((lava.mask[ai] & COMPONENT_AI) == COMPONENT_AI && lava.monster[ai] && lava.monster[ai]->GetStrategyState() != AI_Strategy::STATE_TRAPPED
						&& lava.monster[ai]->GetStrategyState() != AI_Strategy::STATE_CAPTURED)
					{
						if ((lava.position[trap] - lava.position[ai]).LengthSquared() < lava.trap[trap].radius)
						{
							lava.trap[trap].caughtMonsterVel = lava.velocity[ai];
							lava.trap[trap].caughtMonster = ai;
							lava.trap[trap].activated = true;
							//grass.velocity[ai].SetZero();
							if (lava.velocity[ai].LengthSquared() > Math::EPSILON) {
								lava.velocity[ai] = lava.velocity[ai].Normalized() * 0.01f;
								lava.monster[ai]->m_velocity = lava.velocity[ai];
								lava.monster[ai]->GetTrapped();
							}
						}
					}
				}
			}
			else
			{
				if (lava.trap[trap].triggerTimer < lava.trap[trap].triggerDuration)
				{
					lava.trap[trap].triggerTimer += dt;
				}
				else
				{
					lava.velocity[lava.trap[trap].caughtMonster] = lava.trap[trap].caughtMonsterVel;
					lava.monster[lava.trap[trap].caughtMonster]->m_velocity = lava.trap[trap].caughtMonsterVel;
					lava.monster[lava.trap[trap].caughtMonster]->SetIdleState();
					destroyGO(&lava, trap);
				}
			}
		}
	}

	// Monster damage to player
	for (GameObject ai = 0; ai < lava.GAMEOBJECT_COUNT; ++ai)
	{
		if ((lava.mask[ai] & COMPONENT_AI) == COMPONENT_AI && lava.monster[ai])
		{
			//if ((grass.position[ai] - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() <
			//    0.25f * (SharedData::GetInstance()->player->PlayerHitBox.m_scale.x + grass.hitbox[ai].m_scale.x) * (SharedData::GetInstance()->player->PlayerHitBox.m_scale.z + grass.hitbox[ai].m_scale.z))
			
			if (lava.monster[ai]->GetStrategyState() == AI_Strategy::STATE_ATTACK || lava.monster[ai]->GetStrategyState() == AI_Strategy::STATE_RAMPAGE)
			{
				if (lava.hitbox[ai].CheckCollision(SharedData::GetInstance()->player->PlayerHitBox))
				{
					//std::cout << "DIstance:" << (lava.hitbox[ai].m_origin - SharedData::GetInstance()->player->PlayerHitBox.m_origin).Length() << std::endl;
					//std::cout << std::endl;

					lava.monster[ai]->AttackPlayer();
					lava.monster[ai]->ResetAggression();
					lava.monster[ai]->SetIdleState();
				}
			}

		}

	}

    //===============================================================================================================================//
    //                                                            Key Inputs                                                         //
    //===============================================================================================================================//

    //Place trap
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_G].isPressed && SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP].Use())
    {
        PlaceTrap(&lava);
    }
    //Rocks
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_1].isPressed)
    {
        b_Rocks = true;
        b_Baits = false;
        b_Nets = false;
        f_HighlightPos = -34.7f;
    }
    //Nets
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_2].isPressed)
    {
        b_Nets = true;
        b_Rocks = false;
        b_Baits = false;
        f_HighlightPos = -24.8f;
    }
    //Baits
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_3].isPressed)
    {
        b_Baits = true;
        b_Rocks = false;
        b_Nets = false;

        f_HighlightPos = -14.9f;
    }

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
                    break;
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

	if (b_Collected == true)
	{
		f_RampageTimer += (float)(dt);
		for (GameObject GO = 0; GO < lava.GAMEOBJECT_COUNT; ++GO)
		{
			if ((lava.mask[GO] & COMPONENT_AI) == COMPONENT_AI)
			{
				//lava.monster[GO]->m_velocity = lava.velocity[GO];
				//lava.monster[GO]->m_strategy->SetState(AI_Strategy::STATE_ALERT);
				//lava.monster[GO]->m_strategy->SetDestination(SharedData::GetInstance()->player->GetPositionVector());
				////lava.monster[GO]->Update(dt);
				//lava.velocity[GO].x = lava.monster[GO]->m_velocity.x * 50.f;
				//lava.velocity[GO].z = lava.monster[GO]->m_velocity.z * 50.f;
				if (lava.monster[GO]->GetStrategyState() == AI_Strategy::STATE_RAMPAGE) {
					lava.monster[GO]->m_strategy->SetRampageStateDestination();
					lava.velocity[GO] = lava.monster[GO]->m_velocity;
				}
			}
		}

		if (f_RampageTimer >= 5.f)
		{
			b_Collected = false;
			for (GameObject GO = 0; GO < lava.GAMEOBJECT_COUNT; ++GO)
			{
				if ((lava.mask[GO] & COMPONENT_AI) == COMPONENT_AI)
				{
					lava.monster[GO]->SetIdleState();
				}
			}
			//Init();
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

	// for buffer time between projectile launches
	ItemProjectile::d_rockCounter += dt;
	ItemProjectile::d_netCounter += dt;
	ItemProjectile::d_baitCounter += dt;
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

	//for (vector<ItemProjectile*>::iterator it = ItemProjectile::ItemProjectileList.begin(); it != ItemProjectile::ItemProjectileList.end(); ++it){
	//	modelStack.PushMatrix();
	//	modelStack.Translate(
	//		(*it)->position.x,
	//		(*it)->position.y,
	//		(*it)->position.z
	//		);
	//	modelStack.Scale(0.5, 0.5, 0.5);
	//	RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET), false);
	//	modelStack.PopMatrix();
	//}

    glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_FOG_ENABLED), fog.enabled);

	for (vector<ItemProjectile*>::iterator it = ItemProjectile::RockProjectileList.begin(); it != ItemProjectile::RockProjectileList.end(); ++it){
		modelStack.PushMatrix();
		modelStack.Translate(
			(*it)->position.x,
			(*it)->position.y,
			(*it)->position.z
			);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCKS1), true);
		modelStack.PopMatrix();
	}

	for (vector<ItemProjectile*>::iterator it = ItemProjectile::NetProjectileList.begin(); it != ItemProjectile::NetProjectileList.end(); ++it){
		modelStack.PushMatrix();
		modelStack.Translate(
			(*it)->position.x,
			(*it)->position.y,
			(*it)->position.z
			);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET), true);
		modelStack.PopMatrix();
	}

	for (vector<ItemProjectile*>::iterator it = ItemProjectile::BaitProjectileList.begin(); it != ItemProjectile::BaitProjectileList.end(); ++it){
		modelStack.PushMatrix();
		modelStack.Translate(
			(*it)->position.x,
			(*it)->position.y,
			(*it)->position.z
			);
		modelStack.Scale(0.5, 0.5, 0.5);
		RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BAIT), true);
		modelStack.PopMatrix();
	}

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
	double x, y;
	Application::GetCursorPos(&x, &y);
	modelStack.PushMatrix();
	modelStack.Translate(SharedData::GetInstance()->player->GetPositionVector().x + SharedData::GetInstance()->player->GetViewVector().x * 20, 0.5, SharedData::GetInstance()->player->GetPositionVector().z + SharedData::GetInstance()->player->GetViewVector().z * 20);
	modelStack.Scale(1, 1, 1);
	RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TRAP), false);
	modelStack.PopMatrix();

    // HUD THINGS
    if (SharedData::GetInstance()->sceneManager->GetGameState() == SceneManager::GAMESTATE_GAMEPLAY)
    {
        // hitboxes
        for (GameObject tallGrass = 0; tallGrass < lava.GAMEOBJECT_COUNT; ++tallGrass)
        {
            if ((lava.mask[tallGrass] & COMPONENT_HITBOX) == COMPONENT_HITBOX)
            {
                modelStack.PushMatrix();
                modelStack.Translate(lava.hitbox[tallGrass].m_origin.x, lava.hitbox[tallGrass].m_origin.y, lava.hitbox[tallGrass].m_origin.z);
                modelStack.Scale(lava.hitbox[tallGrass].m_scale.x, lava.hitbox[tallGrass].m_scale.y, lava.hitbox[tallGrass].m_scale.z);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                modelStack.PopMatrix();
            }
        }

        RenderHUD(&lava);
    }

	for (std::vector<ParticleObject* >::iterator it = SharedData::GetInstance()->particle->particleList.begin(); it != SharedData::GetInstance()->particle->particleList.end(); ++it)
	{
		ParticleObject* particle = (ParticleObject*)*it;
		if (particle->active)
		{
			RenderParticle(particle);
		}
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
	modelStack.Translate(500, 2800, -500);
	//modelStack.Rotate(0, 0,0,0);
	RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_LAVA_SKYPLANE), false);
	modelStack.PopMatrix();

    RenderGameObjects(&lava);
}

bool SceneLava::ViewCheckPosition(Vector3 pos, float degree)
{
	if (pos != camera.position)
	{
		Vector3 view = (pos - camera.position).Normalized();

		float angleX = Math::RadianToDegree(acos(view.Dot(SharedData::GetInstance()->player->GetViewVector())));

		//std::cout << "angle: " << angleX << std::endl;

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

void SceneLava::Exit()
{
	for (unsigned GO = 0; GO < lava.GAMEOBJECT_COUNT; ++GO)
	{
		//if (grass.monster[GO])
		//{
		//    delete grass.monster[GO];
		//}

		// call destroyGO instead
		destroyGO(&lava, GO);
	}

}

bool SceneLava::CheckInteractMoneyTree(World *world, GameObject GO)
{
    if ((camera.position - lava.position[GO]).LengthSquared() < 250)
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
                          lava.mask[GO] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_COIN;
                          lava.appearance[GO].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_COIN);
                          lava.appearance[GO].scale.Set(5, 5, 5);
                }
                    break;
                case 1:
                {
                          lava.mask[GO] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_ROCKS;
                          lava.appearance[GO].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCKS1);
                          lava.appearance[GO].scale.Set(2, 2, 2);
                }
                    break;
                case 2:
                    break;
                    //{
                    //	//lava.mask[GO] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_AI | COMPONENT_OBSTACLE;
                    //	//lava.appearance[GO].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_MAGMA);
                    //	//lava.appearance[GO].scale.Set(2.5f, 2.5f, 2.5f);
                    //	//lava.monster[GO] = MonsterFactory::CreateMonster("Magma");
                    //	//lava.hitbox[GO].m_scale.Set(5.f, 8.5f, 5.f);
                    //}
                    //break;
                }
            }

            return true;
        }
    }

    return false;
}

void SceneLava::RenderParticle(ParticleObject* particle)
{
	glBlendFunc(GL_ONE, GL_ONE);
	switch (particle->type)
	{
	case ParticleObject_TYPE::P_HIDDENBONUS:
		modelStack.PushMatrix();
		modelStack.Translate(particle->pos.x, particle->pos.y, particle->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.x - particle->pos.x, camera.position.z - particle->pos.z)), 0, 1, 0);
		modelStack.Scale(particle->scale.x, particle->scale.y, particle->scale.z);
		RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_HIDDENBONUS_PARTICLE), false);
		modelStack.PopMatrix();
		break;

	default:
		break;
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}