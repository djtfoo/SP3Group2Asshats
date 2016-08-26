#include "GL\glew.h"
//#include "../../shader.hpp"

#include "SceneGrass.h"
#include "../../General/Application.h"
//#include "../../Graphics/LoadOBJ/LoadTGA.h"
//#include "../../Graphics/Mesh/MeshBuilder.h"
#include "../../General/SharedData.h"
#include "../../GameObject/MonsterFactory.h"

#include "../../GameObject/AI_Strategy.h"

#include <sstream>

SceneGrass::SceneGrass()
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
                grass.hitbox[go].m_origin = grass.position[go];
                grass.hitbox[go].m_scale.Set(4.f, 4.f, 4.f);
                grass.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh((it->second).first);
                grass.appearance[go].scale.Set(Math::RandFloatMinMax(0.8f, 1.f), Math::RandFloatMinMax(0.5f, 1.f), Math::RandFloatMinMax(0.8f, 1.f));
                //grass.appearance[go].scale.Set(1, 1, 1);
            }
            else if (tile >= '1' && tile <= '9')
            {
                GameObject go = createGO(&grass);
                grass.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_AI;
                //grass.velocity[go].Set(Math::RandFloatMinMax(0.f, 1.f), 0, Math::RandFloatMinMax(0.f, 1.f));
                //grass.velocity[go].SetZero();
                grass.position[go].Set(cols * tileSize + Math::RandFloatMinMax(0.f, 1.f), 0.f, rows * tileSize + Math::RandFloatMinMax(0.f, 1.f));
                grass.hitbox[go].m_origin = grass.position[go];
                grass.hitbox[go].m_scale.Set(4.f, 4.f, 4.f);
                switch (tile)
                {
                case '1':
                    grass.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_RABBIT);
                    grass.monster[go] = MonsterFactory::CreateMonster("Rabbit");
                    break;
                
                case '2':
                    grass.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_BIRD);
                    grass.monster[go] = MonsterFactory::CreateMonster("Bird");
                    break;
                
                case '3':
                    break;
                }
                grass.monster[go]->m_position = grass.position[go];

                int randCol = cols + Math::RandIntMinMax(3, 5) * Math::RandIntMinMax(-1, 1);
                int randRow = rows + Math::RandIntMinMax(3, 5) * Math::RandIntMinMax(-1, 1);
                while (randCol < 0 || randCol >= 40 || randRow < 0 || randRow >= 40 || Scene::m_levelMap[randRow][randCol] != '0')
                {
                    randCol = cols + Math::RandIntMinMax(3, 5) * Math::RandIntMinMax(-1, 1);
                    randRow = rows + Math::RandIntMinMax(3, 5) * Math::RandIntMinMax(-1, 1);
                }

                Vector3 RNGdestination(randCol * Scene::tileSize + Scene::tileSize * 0.5f, 0, randRow * Scene::tileSize + Scene::tileSize * 0.5f);

                grass.monster[go]->m_destination = RNGdestination;
                grass.velocity[go] = (RNGdestination - grass.position[go]).Normalized() * 2.f;
                grass.monster[go]->m_velocity = grass.velocity[go];
                grass.appearance[go].scale.Set(1, 1, 1);
                std::cout << grass.monster[go]->m_position << grass.monster[go]->m_destination << std::endl;

            }
        }
    }

    monster = createGO(&grass);
    grass.mask[monster] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_AI;
    grass.position[monster].SetZero();
    grass.velocity[monster].Set(0, 0, 1);
    grass.hitbox[monster].m_origin = grass.position[monster] + Vector3(0, 0.75f, -0.3);
    grass.hitbox[monster].m_scale.Set(1.5f, 1.5f, 1.75f);
    grass.appearance[monster].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_RABBIT);
    grass.appearance[monster].scale.Set(1, 1, 1);

	rock = createGO(&grass);
	grass.mask[rock] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX;
	grass.position[rock].Set(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z);
	grass.appearance[rock].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCKS1);
	grass.appearance[rock].scale.Set(1, 1, 1);

	net = createGO(&grass);
	grass.mask[net] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE;
	grass.position[net].SetZero();
	grass.appearance[net].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET);
	grass.appearance[net].scale.Set(2, 2, 2);
	//HITBOX.m_origin = Vector3(0, 5, 0);
	//HITBOX.m_scale = Vector3(10, 10, 10);

    counter = 3;
	b_Rocks = true;
	b_Nets = false;
	b_Baits = false;

	f_RotateRock = 0;
	f_RotateNet = 0;
	f_RotateBait = 0;
}

void SceneGrass::Update(double dt)
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
	for (GameObject ai = 0; ai < grass.GAMEOBJECT_COUNT; ++ai)
	{
		if (grass.monster[ai])
		{
			grass.monster[ai]->m_position = grass.position[ai];
			grass.monster[ai]->Update(dt);
			grass.velocity[ai] = grass.monster[ai]->m_velocity;
		}
	}

	//Movement update for Gameobjects
	UpdateGameObjects(&grass, dt);

	//Player Update
	SharedData::GetInstance()->player->Update(dt);
	SharedData::GetInstance()->inputManager->Update();

    //Camera Update
    camera.Update(dt);

	// Check to see if text_C and text_M should be destroyed
	for (GameObject text = 0; text < grass.GAMEOBJECT_COUNT; ++text)
	{
		if ((grass.mask[text] & COMPONENT_TEXT) == COMPONENT_TEXT)
		{
			if (grass.position[text].y > 9.f)
			{
				destroyGO(&grass, text);
			}
		}

	}

	//===============================================================================================================================//
	//                                                            Checks                                                             //
	//===============================================================================================================================//
	////////////////////////////////////////////////
	////Projectile checks//////////////ROCKS////////
	////////////////////////////////////////////////

	if (ItemProjectile::RockProjectileList.size())
	{
		for (unsigned i = 0; i < ItemProjectile::RockProjectileList.size(); ++i)
		{
			for (GameObject ai = 0; ai < grass.GAMEOBJECT_COUNT; ++ai)
			{
				if ((grass.mask[ai] & COMPONENT_AI) == COMPONENT_AI)
				{
					if (grass.hitbox[ai].CheckCollision(ItemProjectile::RockProjectileList[i]->position))
					{
                        // Get hit by rock
						//grass.velocity[ai] = (grass.position[ai] - camera.position).Normalized();
						//grass.velocity[ai].y = 0;
                        ItemProjectile::RockProjectileList[i]->deleteBullet = true;
                        grass.monster[ai]->TakeDamage(SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].GetEffectiveness());
						break;
					}
				}
			}
		}
	}


	//////////////////////////////////////////////
	////Projectile checks//////////////NET////////
	//////////////////////////////////////////////
	if (ItemProjectile::NetProjectileList.size())
	{
		for (unsigned i = 0; i < ItemProjectile::NetProjectileList.size(); ++i)
		{
			for (GameObject ai = 0; ai < grass.GAMEOBJECT_COUNT; ++ai)
			{
				if ((grass.mask[ai] & COMPONENT_AI) == COMPONENT_AI && grass.hitbox[ai].CheckCollision(ItemProjectile::NetProjectileList[i]->position)
					&& grass.monster[ai]->GetStrategyState() != static_cast<int>(AI_Strategy::STATE_CAPTURED))
				{
					ItemProjectile::NetProjectileList[i]->deleteBullet = true;
					net = createGO(&grass);
					grass.mask[net] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE;
					grass.capture[net].capturingMonster = true;
					grass.capture[net].capturedMonster = false;
					grass.capture[net].timeBeforeCapture = 3;
					grass.appearance[net].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET);
					grass.appearance[net].scale.Set(2, 2, 2);

					if (grass.capture[net].capturingMonster == true)
					{
						grass.position[net] = grass.position[ai];
						int v1 = rand() % 100;
						std::cout << v1 << std::endl;
						if (v1 < grass.monster[ai]->GetCaptureRateStat())
						{
							grass.capture[net].capturedMonster = true;//Captured!
							text_C = createGO(&grass);
							grass.mask[text_C] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_VELOCITY | COMPONENT_TEXT;
							grass.appearance[text_C].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_CAPTURE);
							grass.position[text_C] = grass.position[net];
							grass.position[text_C].y += 1;
							grass.velocity[text_C] = (grass.position[text_C] - camera.position).Normalized();
							grass.velocity[text_C].y = 3;
							grass.appearance[text_C].scale.Set(2, 2, 2);
							grass.appearance[text_C].billboard = true;
						}
						else
						{
							std::cout << "HAHAHAHAHHA FAILED CAPTURED!" << std::endl;
							text_M = createGO(&grass);
							grass.mask[text_M] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_VELOCITY | COMPONENT_TEXT;
							grass.appearance[text_M].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_MISS);
							grass.position[text_M] = grass.position[net];
							grass.position[text_M].y += 1;
							grass.velocity[text_M] = (grass.position[text_M] - camera.position).Normalized();
							grass.velocity[text_M].y = 3;
							grass.appearance[text_M].scale.Set(2, 2, 2);
							grass.appearance[text_M].billboard = true;
							destroyGO(&grass, net);
							grass.capture[net].capturingMonster = false;//fail capture
						}
					}
					if (grass.capture[net].capturedMonster == true)
					{
						std::cout << "HAHAHAHAHHA CAPTURED!" << std::endl;
						grass.velocity[ai] = 0;
						grass.monster[ai]->m_velocity = grass.velocity[ai];
						grass.monster[ai]->GetCaptured();
						grass.position[net] = grass.position[ai];
					}
					break;
				}
			}
		}
	}
	//////////////////////////////////////////////
	////////BAIT /////////////////////////////////
	//////////////////////////////////////////////
	if (ItemProjectile::BaitProjectileList.size())
	{
		for (unsigned i = 0; i < ItemProjectile::BaitProjectileList.size(); ++i)
		{
			if (ItemProjectile::BaitProjectileList[i]->position.y <= 1)
			{
				Vector3 tempo = ItemProjectile::BaitProjectileList[i]->position;
				ItemProjectile::BaitProjectileList[i]->deleteBullet = true;

				bait = createGO(&grass);
				grass.mask[bait] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_BAIT;
				grass.position[bait] = tempo;
				grass.appearance[bait].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BAIT);
				grass.appearance[bait].scale.Set(1, 1, 1);
				grass.bait[bait].scentRadius = 200;
				grass.bait[bait].foundRadius = 5;
				grass.bait[bait].eattingBait = false;
				grass.bait[bait].foundBait = false;
				grass.bait[bait].timeEatting = 3.0f;
			}
		}
	}

	for (GameObject ai = 0; ai < grass.GAMEOBJECT_COUNT; ++ai)
	{
		if ((grass.mask[ai] & COMPONENT_AI) == COMPONENT_AI)
		{
			for (GameObject bait = 0; bait < grass.GAMEOBJECT_COUNT; ++bait)
			{
				if ((grass.mask[bait] & COMPONENT_BAIT) == COMPONENT_BAIT)
				{
					if ((grass.position[ai] - grass.position[bait]).LengthSquared() < grass.bait[bait].scentRadius && grass.bait[bait].eattingBait == false)
					{
						grass.bait[bait].foundBait = true;
						std::cout << " FOUND BAIT " + grass.bait[bait].foundBait << std::endl;
						if (grass.bait[bait].foundBait == true)
						{
							grass.velocity[ai] = (grass.position[ai] - grass.position[bait]).Normalized();
							grass.velocity[ai].y = 0;
							grass.velocity[ai] *= -1;
						}
					}
					if ((grass.position[ai] - grass.position[bait]).LengthSquared() < grass.bait[bait].foundRadius)
					{
						std::cout << "EATTING BAIT " + grass.bait[bait].foundBait << std::endl;
						grass.bait[bait].foundBait = false;
						grass.bait[bait].eattingBait = true;
						grass.velocity[ai] = 0;
						grass.velocity[ai].y = 0;
					}
					if (grass.bait[bait].eattingBait == true && (grass.position[ai] - grass.position[bait]).LengthSquared() < grass.bait[bait].foundRadius)
					{
						grass.bait[bait].timeEatting -= dt;
						std::cout << grass.bait[bait].timeEatting << std::endl;
						if (grass.bait[bait].timeEatting <= 0)
						{
							grass.bait[bait].eattingBait == false;
							destroyGO(&grass, bait);
							std::cout << "Done Eatting" << std::endl;
							//PUT WHATEVER THE RABBIT DO NORMALLY HERE :D (DONE EATTING BAIT)
                            grass.velocity[ai] = grass.monster[ai]->m_velocity;
                            //grass.velocity[ai] = 2;
							//grass.velocity[ai].y = 0;
						}
					}
				}
			}
		}
	}

	//Trap check (radius)
	for (GameObject trap = 0; trap < grass.GAMEOBJECT_COUNT; ++trap)
	{
		if ((grass.mask[trap] & COMPONENT_TRAP) == COMPONENT_TRAP)
		{
			if (!grass.trap[trap].activated)
			{
				for (GameObject ai = 0; ai < grass.GAMEOBJECT_COUNT; ++ai)
				{
                    if ((grass.mask[ai] & COMPONENT_AI) == COMPONENT_AI && grass.monster[ai] && grass.monster[ai]->GetStrategyState() != static_cast<int>(AI_Strategy::STATE_TRAPPED)
                        && grass.monster[ai]->GetStrategyState() != static_cast<int>(AI_Strategy::STATE_CAPTURED))
					{
						if ((grass.position[trap] - grass.position[ai]).LengthSquared() < grass.trap[trap].radius)
						{
							grass.trap[trap].caughtMonsterVel = grass.velocity[ai];
							grass.trap[trap].caughtMonster = ai;
							grass.trap[trap].activated = true;
							//grass.velocity[ai].SetZero();
                            if (grass.velocity[ai].LengthSquared() > Math::EPSILON) {
                                grass.velocity[ai] = grass.velocity[ai].Normalized() * 0.01f;
                                grass.monster[ai]->m_velocity = grass.velocity[ai];
                                grass.monster[ai]->GetTrapped();
                            }
						}
					}
				}
			}
			else
			{
				if (grass.trap[trap].triggerTimer < grass.trap[trap].triggerDuration)
				{
					grass.trap[trap].triggerTimer += dt;
				}
				else
				{
					grass.velocity[grass.trap[trap].caughtMonster] = grass.trap[trap].caughtMonsterVel;
                    grass.monster[grass.trap[trap].caughtMonster]->m_velocity = grass.trap[trap].caughtMonsterVel;
                    grass.monster[grass.trap[trap].caughtMonster]->SetIdleState();
					destroyGO(&grass, trap);
				}
			}
		}
	}

    // Monster damage to player
    for (GameObject ai = 0; ai < grass.GAMEOBJECT_COUNT; ++ai)
    {
        if ((grass.mask[ai] & COMPONENT_AI) == COMPONENT_AI && grass.monster[ai] && grass.monster[ai]->GetStrategyState() == static_cast<int>(AI_Strategy::STATE_ATTACK))
        {
            //if ((grass.position[ai] - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() <
            //    0.25f * (SharedData::GetInstance()->player->PlayerHitBox.m_scale.x + grass.hitbox[ai].m_scale.x) * (SharedData::GetInstance()->player->PlayerHitBox.m_scale.z + grass.hitbox[ai].m_scale.z))
			if (grass.hitbox[ai].CheckCollision(SharedData::GetInstance()->player->PlayerHitBox))
			{
                grass.monster[ai]->AttackPlayer();
                grass.monster[ai]->ResetAggression();
                grass.monster[ai]->SetIdleState();
            }

        }

    }

	//===============================================================================================================================//
	//                                                            Key Inputs                                                         //
	//===============================================================================================================================//

	//Place trap
	if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_G].isPressed && SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP_ONE].Use())
	{
		GameObject trap = createGO(&grass);

		grass.mask[trap] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_TRAP;

		grass.position[trap].Set(SharedData::GetInstance()->player->GetPositionVector().x + SharedData::GetInstance()->player->GetViewVector().x * 20, 0, SharedData::GetInstance()->player->GetPositionVector().z + SharedData::GetInstance()->player->GetViewVector().z * 20);
		grass.appearance[trap].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TRAP);
		grass.appearance[trap].scale.Set(3, 3, 3);

		grass.trap[trap].triggerDuration = 3.0f;
		grass.trap[trap].triggerTimer = 0.f;
		grass.trap[trap].radius = 2.5f;
		grass.trap[trap].activated = false;
		grass.trap[trap].caughtMonster = 0;
		grass.trap[trap].caughtMonsterVel.SetZero();

		counter = 0;
	}
	//Rocks
	if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_1].isPressed)
	{
		b_Rocks = true;
		b_Baits = false;
		b_Nets = false;
	}
	//Nets
	if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_2].isPressed)
	{
		b_Nets = true;
		b_Rocks = false;
		b_Baits = false;
	}
	//Baits
	if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_3].isPressed)
	{
		b_Baits = true;
		b_Rocks = false;
		b_Nets = false;
	}

	if (b_Rocks)
	{
		//Rock projectile
		f_RotateRock += dt * 50;
		if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed && SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].Use())
		{
			ItemProjectile::RockProjectileList.push_back(new ItemProjectile(
				Vector3(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z),
				Vector3(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
				500,
				50,
				10
				));

		}
	}
	if (b_Nets)
	{
		//Net projectile
		f_RotateNet += dt * 50;
		if (counter > 0.05)
		{
			if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed && SharedData::GetInstance()->player->inventory[Item::TYPE_NET].Use())
			{
				ItemProjectile::NetProjectileList.push_back(new ItemProjectile(
					Vector3(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z),
					Vector3(SharedData::GetInstance()->player->GetViewVector().x, 0.5, SharedData::GetInstance()->player->GetViewVector().z),
					500,
					15,
					10
					));

				counter = 0;
			}
		}
	}
	if (b_Baits)
	{
		//Bait Projectile
		f_RotateBait += dt * 50;
		if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed && SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].Use())
		{
			ItemProjectile::BaitProjectileList.push_back(new ItemProjectile(
				Vector3(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z),
				Vector3(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
				500,
				50,
				10
				));

		}
	}
    // DEBUG: Spawn monster at origin
    //if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_E].isPressed)
    //{
    //    monster = createGO(&grass);
    //    grass.mask[monster] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_AI;
    //    grass.position[monster].SetZero();
    //    grass.velocity[monster].Set(0, 0, 1);
    //    grass.hitbox[monster].m_origin = grass.position[monster] + Vector3(0, 0.75f, -0.3f);
    //    grass.hitbox[monster].m_scale.Set(1.5f, 1.5f, 1.75f);
    //    grass.appearance[monster].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_RABBIT);
    //    grass.appearance[monster].scale.Set(1, 1, 1);
	//
    //    counter = 0;
    //}

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

	// Check pick up monster
	if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_E].isPressed)
	{
		//if (camera.position - )
		for (GameObject ai = 0; ai < grass.GAMEOBJECT_COUNT; ++ai)
		{
			if ((grass.mask[ai] & COMPONENT_AI) == COMPONENT_AI && grass.monster[ai] && grass.monster[ai]->GetStrategyState() == static_cast<int>(AI_Strategy::STATE_CAPTURED))
			{
				if ((camera.position - grass.position[ai]).LengthSquared() < 150)
				{
					if (ViewCheckPosition(grass.position[ai], 45.f) == true)
					{
						std::cout << "CAUGHT THE MONSTER" << std::endl;
					}
				}
			}
		}
	}


    // for buffer time between projectile launches
    counter += dt;
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

	RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_AXES), false);

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

	RenderGrassScene();

    //visible hitbox
	/*modelStack.PushMatrix();
	modelStack.Translate(HITBOX.m_origin.x, HITBOX.m_origin.y, HITBOX.m_origin.z);
	modelStack.Scale(HITBOX.m_scale.x * 2, HITBOX.m_scale.y * 2, HITBOX.m_scale.z * 2);
	RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
	modelStack.PopMatrix();*/

	modelStack.PushMatrix();
	modelStack.Translate(SharedData::GetInstance()->player->PlayerHitBox.m_origin.x, 0.1, SharedData::GetInstance()->player->PlayerHitBox.m_origin.z);
	modelStack.Scale(SharedData::GetInstance()->player->PlayerHitBox.m_scale.x, SharedData::GetInstance()->player->PlayerHitBox.m_scale.y, SharedData::GetInstance()->player->PlayerHitBox.m_scale.z);
	//modelStack.Translate(10,0,0);
	//modelStack.Scale(10,10,10);
	RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_PLAYERBOX), false);
	modelStack.PopMatrix();

    //Trap placing
    double x, y;
    Application::GetCursorPos(&x, &y);
    modelStack.PushMatrix();
    modelStack.Translate(SharedData::GetInstance()->player->GetPositionVector().x + SharedData::GetInstance()->player->GetViewVector().x * 20, 0.5, SharedData::GetInstance()->player->GetPositionVector().z + SharedData::GetInstance()->player->GetViewVector().z * 20);
    modelStack.Scale(1,1,1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TRAP), false);
    modelStack.PopMatrix();

    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_ENTER].isPressed)
        std::cout << "asd" << std::endl;

    std::stringstream ss;
    ss << "FPS: " << fps;
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3, 0, 3);

    ss.str("");
    ss << "Noise factor:" << SharedData::GetInstance()->player->GetNoiseFactor();
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3, 0, 6);

    ss.str("");
    ss << "PLAYER HEALTH:" << SharedData::GetInstance()->player->GetHealth();
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3, 0, 9);


	SetHUD(true);
	RenderHUD();
	SetHUD(false);
}

void SceneGrass::RenderGrassScene()
{
    //Ground mesh
	modelStack.PushMatrix();
    modelStack.Translate(100, -7, 100);
	modelStack.Scale(300.0f, 30.0f, 300.0f);
	RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GRASS_TERRAIN), true);
	modelStack.PopMatrix();

    RenderGameObjects(&grass);

    //Skyplane
	modelStack.PushMatrix();
	modelStack.Translate(500, 2800, -500);
	//modelStack.Rotate(0, 0,0,0);
	RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GRASS_SKYPLANE), false);
	modelStack.PopMatrix();
}

void SceneGrass::RenderHUD()
{
	std::stringstream ss,ss2,ss3,ss4;
	ss << "Rocks: " << SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].GetCount();
	ss2 << "Nets: " << SharedData::GetInstance()->player->inventory[Item::TYPE_NET].GetCount();
	ss3 << "Baits: " << SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].GetCount();
	ss4 << "Traps: " << SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP_ONE].GetCount();

	RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_HUD), false, 80.f, 12.f, 0, -48);

	RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCKS1), 2, 22.5f, 6.5f, f_RotateRock, f_RotateRock, 0, false);
	RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 1, 20.5f, 3.5f);
	RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET), 2, 27.5f, 5.5f, 0, f_RotateNet, 0, false);
	RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss2.str(), Color(1, 1, 0), 1, 25.5f, 3.5f);
	RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BAIT), 2, 32.5f, 6.5f, f_RotateBait, f_RotateBait, 0, false);
	RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss3.str(), Color(1, 1, 0), 1, 30.5f, 3.5f);
	RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TRAP), 2, 37.5f, 6.5f, 0, 0, 0, false);
	RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss4.str(), Color(1, 1, 0), 1, 35.5f, 3.5f);
	//RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BAIT), false, 30.f, 10, -30, false);
	//RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_HUDHIGHLIGHT), true, 10.0f, 40, -48);
}

bool SceneGrass::ViewCheckPosition(Vector3 pos, float degree)
{
	if (pos != camera.position)
	{
		Vector3 view = (pos - camera.position).Normalized();

		float angleX = Math::RadianToDegree(acos(view.Dot(camera.target) / camera.target.Length() ));

		//Math::RadianToDegree(atan2(world->velocity[GO].x, world->velocity[GO].z)), 0, 1, 0

		std::cout << "angle: " << angleX << std::endl;

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

void SceneGrass::Exit()
{
    for (unsigned GO = 0; GO < grass.GAMEOBJECT_COUNT; ++GO)
    {
        if (grass.monster[GO])
        {
            delete grass.monster[GO];
        }
    }
}

//========================
// == OBJECTS TO RENDER
//========================
