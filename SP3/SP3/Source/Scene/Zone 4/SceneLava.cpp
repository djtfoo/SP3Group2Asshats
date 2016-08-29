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
	for (rows = 0; rows < Scene::m_rows; ++rows)
	{
		for (cols = 0; cols < Scene::m_cols; ++cols)
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
					lava.monster[go] = MonsterFactory::CreateMonster("Magma");
					lava.hitbox[go].m_scale.Set(5.f, 8.5f, 5.f);
					break;

				case '2':
					lava.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_FIREBUG);
					lava.monster[go] = MonsterFactory::CreateMonster("FireBug");
					lava.hitbox[go].m_scale.Set(2.f, 5.5f, 2.f);
					break;

				case '3':
					lava.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BOSS_MAGMA_BERZEKER);
					lava.monster[go] = MonsterFactory::CreateMonster("MagmaBerzeker");
					lava.hitbox[go].m_scale.Set(7.5f, 17.5f, 7.5f);
					break;
				}
				lava.monster[go]->m_position = lava.position[go];

				randCol = cols + Math::RandIntMinMax(3, 5) * Math::RandIntMinMax(-1, 1);
				randRow = rows + Math::RandIntMinMax(3, 5) * Math::RandIntMinMax(-1, 1);
				while (randCol < 0 || randCol >= 40 || randRow < 0 || randRow >= 40 || Scene::m_levelMap[randRow][randCol] != '0')
				{
					randCol = cols + Math::RandIntMinMax(3, 5) * Math::RandIntMinMax(-1, 1);
					randRow = rows + Math::RandIntMinMax(3, 5) * Math::RandIntMinMax(-1, 1);
				}

				Vector3 RNGdestination(randCol * Scene::tileSize + Scene::tileSize * 0.5f, 0, randRow * Scene::tileSize + Scene::tileSize * 0.5f);

				lava.monster[go]->m_destination = RNGdestination;
				lava.velocity[go] = (RNGdestination - lava.position[go]).Normalized() * 2.f;
				lava.monster[go]->m_velocity = lava.velocity[go];
				lava.appearance[go].scale.Set(1, 1, 1);
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

	counter = 3;
	b_Rocks = true;
	b_Nets = false;
	b_Baits = false;
	b_Collected = false;
	f_RampageTimer = 0;

	f_RotateRock = 0;
	f_RotateNet = 0;
	f_RotateBait = 0;
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
			lava.velocity[ai].x = lava.monster[ai]->m_velocity.x * 10.f;
			lava.velocity[ai].z = lava.monster[ai]->m_velocity.z * 10.f;
			//lava.monster[ai]->m_velocity = lava.velocity[ai];
		}
	}

	//Movement update for Gameobjects
	UpdateGameObjects(&lava, dt);

	//Player Update
	SharedData::GetInstance()->inputManager->Update();
	SharedData::GetInstance()->player->Update(dt);

	bool collision = false;
	for (GameObject obstacle = 0; obstacle < lava.GAMEOBJECT_COUNT; ++obstacle)
	{
		if ((lava.mask[obstacle] & COMPONENT_OBSTACLE) == COMPONENT_OBSTACLE)
		{
			Vector3 playerPos = SharedData::GetInstance()->player->GetPositionVector() + SharedData::GetInstance()->player->GetVelocityVector();
			playerPos.y = 0.f;

			if (lava.hitbox[obstacle].CheckCollision(playerPos))
			{
				collision = true;
				break;
			}
		}
	}
	if (!collision) {
		SharedData::GetInstance()->player->Move(dt);
	}

	//Camera Update
	camera.Update(dt);

	// Check to see if text_C and text_M should be destroyed
	for (GameObject text = 0; text < lava.GAMEOBJECT_COUNT; ++text)
	{
		if ((lava.mask[text] & COMPONENT_TEXT) == COMPONENT_TEXT)
		{
			if (lava.position[text].y > 9.f)
			{
				destroyGO(&lava, text);
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
			for (GameObject ai = 0; ai < lava.GAMEOBJECT_COUNT; ++ai)
			{
				if ((lava.mask[ai] & COMPONENT_AI) == COMPONENT_AI)
				{
					if (lava.hitbox[ai].CheckCollision(ItemProjectile::RockProjectileList[i]->position))
					{
						ItemProjectile::RockProjectileList[i]->deleteBullet = true;
						if (lava.monster[ai]->GetStrategyState() != AI_Strategy::STATE_CAPTURED) {
							lava.monster[ai]->TakeDamage(SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].GetEffectiveness());
							//std::cout << "HEALTH: " << lava.monster[ai]->GetHealthStat() << std::endl;
						}
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
			for (GameObject ai = 0; ai < lava.GAMEOBJECT_COUNT; ++ai)
			{
				if ((lava.mask[ai] & COMPONENT_AI) == COMPONENT_AI && lava.hitbox[ai].CheckCollision(ItemProjectile::NetProjectileList[i]->position)
					&& lava.monster[ai]->GetStrategyState() != AI_Strategy::STATE_CAPTURED)
				{
					ItemProjectile::NetProjectileList[i]->deleteBullet = true;
					net = createGO(&lava);
					lava.mask[net] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_CAPTURE;
					lava.capture[net].capturingMonster = true;
					lava.capture[net].capturedMonster = false;
					lava.capture[net].timeBeforeCapture = 3;
					lava.appearance[net].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET);
					lava.appearance[net].scale.Set(2, 2, 2);

					if (lava.capture[net].capturingMonster == true)
					{
						lava.position[net] = lava.position[ai];
						int v1 = rand() % 100;
						//std::cout << v1 << std::endl;
						if (v1 < lava.monster[ai]->GetCaptureRateStat())
						{
							lava.capture[net].capturedMonster = true;//Captured!
							text_C = createGO(&lava);
							lava.mask[text_C] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_VELOCITY | COMPONENT_TEXT;
							lava.appearance[text_C].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_CAPTURE);
							lava.position[text_C] = lava.position[net];
							lava.position[text_C].y += 1;
							lava.velocity[text_C] = (lava.position[text_C] - camera.position).Normalized();
							lava.velocity[text_C].y = 3;
							lava.appearance[text_C].scale.Set(2, 2, 2);
							lava.appearance[text_C].billboard = true;
						}
						else
						{
							//std::cout << "HAHAHAHAHHA FAILED CAPTURED!" << std::endl;
							text_M = createGO(&lava);
							lava.mask[text_M] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_VELOCITY | COMPONENT_TEXT;
							lava.appearance[text_M].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_MISS);
							lava.position[text_M] = lava.position[net];
							lava.position[text_M].y += 1;
							lava.velocity[text_M] = (lava.position[text_M] - camera.position).Normalized();
							lava.velocity[text_M].y = 3;
							lava.appearance[text_M].scale.Set(2, 2, 2);
							lava.appearance[text_M].billboard = true;
							destroyGO(&lava, net);
							lava.capture[net].capturingMonster = false;//fail capture
						}
					}
					if (lava.capture[net].capturedMonster == true)
					{
						//std::cout << "HAHAHAHAHHA CAPTURED!" << std::endl;
						lava.velocity[ai] = 0;
						lava.monster[ai]->m_velocity = lava.velocity[ai];
						lava.monster[ai]->GetCaptured();
						lava.position[net] = lava.position[ai];
						lava.capture[net].caughtMonster = ai;
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

				bait = createGO(&lava);
				lava.mask[bait] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_BAIT;
				lava.position[bait] = tempo;
				lava.appearance[bait].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BAIT);
				lava.appearance[bait].scale.Set(1, 1, 1);
				lava.bait[bait].scentRadius = 200;
				lava.bait[bait].foundRadius = 5;
				lava.bait[bait].eattingBait = false;
				lava.bait[bait].foundBait = false;
				lava.bait[bait].timeEatting = 3.0f;
			}
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
						//std::cout << lava.bait[bait].timeEatting << std::endl;
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
		if ((lava.mask[ai] & COMPONENT_AI) == COMPONENT_AI && lava.monster[ai] && lava.monster[ai]->GetStrategyState() == AI_Strategy::STATE_ATTACK)
		{
			//if ((grass.position[ai] - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() <
			//    0.25f * (SharedData::GetInstance()->player->PlayerHitBox.m_scale.x + grass.hitbox[ai].m_scale.x) * (SharedData::GetInstance()->player->PlayerHitBox.m_scale.z + grass.hitbox[ai].m_scale.z))
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

	//===============================================================================================================================//
	//                                                            Key Inputs                                                         //
	//===============================================================================================================================//

	//Place trap
	if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_G].isPressed && SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP_ONE].Use())
	{
		GameObject trap = createGO(&lava);

		lava.mask[trap] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_TRAP;

		lava.position[trap].Set(SharedData::GetInstance()->player->GetPositionVector().x + SharedData::GetInstance()->player->GetViewVector().x * 20, 0, SharedData::GetInstance()->player->GetPositionVector().z + SharedData::GetInstance()->player->GetViewVector().z * 20);
		lava.appearance[trap].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TRAP);
		lava.appearance[trap].scale.Set(3, 3, 3);

		lava.trap[trap].triggerDuration = 3.0f;
		lava.trap[trap].triggerTimer = 0.f;
		lava.trap[trap].radius = 2.5f;
		lava.trap[trap].activated = false;
		lava.trap[trap].caughtMonster = 0;
		lava.trap[trap].caughtMonsterVel.SetZero();

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
		//for (GameObject ai = 0; ai < grass.GAMEOBJECT_COUNT; ++ai)
		//{
		//	if ((grass.mask[ai] & COMPONENT_AI) == COMPONENT_AI && grass.monster[ai] && grass.monster[ai]->GetStrategyState() == static_cast<int>(AI_Strategy::STATE_CAPTURED))
		//	{
		//		if ((camera.position - grass.position[ai]).LengthSquared() < 150)
		//		{
		//			if (ViewCheckPosition(grass.position[ai], 45.f) == true)
		//			{
		//				std::cout << "CAUGHT THE MONSTER" << std::endl;
		//                SharedData::GetInstance()->player->monsterList.push_back(grass.monster[ai]->GetName());
		//                destroyGO(&grass, ai);
		//                // destroy net
		//                //destroyGO(&grass, );
		//			}
		//		}
		//	}
		//}

		for (GameObject GO = 0; GO < lava.GAMEOBJECT_COUNT; ++GO)
		{
			// check for picking up net
			if ((lava.mask[GO] & COMPONENT_CAPTURE) == COMPONENT_CAPTURE)
			{
				GameObject ai = lava.capture[GO].caughtMonster;
				if ((camera.position - lava.position[ai]).LengthSquared() < 150)
				{
					if (ViewCheckPosition(lava.position[ai], 45.f) == true)
					{
						std::cout << "CAUGHT THE MONSTER" << std::endl;
						SharedData::GetInstance()->player->monsterList.push_back(lava.monster[ai]->GetName());
						destroyGO(&lava, ai);
						// destroy net
						destroyGO(&lava, GO);
					}
				}
			}

			// check for interacting with money tree
			if ((lava.mask[GO] & COMPONENT_MONEYTREE) == COMPONENT_MONEYTREE)
			{
				if ((camera.position - lava.position[GO]).LengthSquared() < 150)
				{
					if (ViewCheckPosition(lava.position[GO], 45.f) == true)
					{
						std::cout << "MoneyTree Found" << std::endl;
						lava.appearance[GO].scale.y -= 1;
						if (lava.appearance[GO].scale.y <= 0)
						{
							lava.mask[GO] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_COIN;
							lava.appearance[GO].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_COINS);
							lava.appearance[GO].scale.Set(5, 5, 5);
						}
						break;
					}
				}
			}

			// check for interacting with coin
			if ((lava.mask[GO] & COMPONENT_COIN) == COMPONENT_COIN)
			{
				if ((camera.position - lava.position[GO]).LengthSquared() < 150)
				{
					if (ViewCheckPosition(lava.position[GO], 45.f) == true)
					{
						std::cout << "coin picked up" << std::endl;
						destroyGO(&lava, GO);
						b_Collected = true;
						//std::cout << "KILL HIM" << std::endl;
						break;
					}
				}
			}
		}

		//for (GameObject MoneyTree = 0; MoneyTree < grass.GAMEOBJECT_COUNT; ++MoneyTree)
		//{

		//}

	}

	if (b_Collected == true)
	{
		f_RampageTimer++;
		std::cout << f_RampageTimer << std::endl;
		for (GameObject GO = 0; GO < lava.GAMEOBJECT_COUNT; ++GO)
		{
			if ((lava.mask[GO] & COMPONENT_AI) == COMPONENT_AI)
			{
				lava.monster[GO]->m_strategy->SetDestination(SharedData::GetInstance()->player->GetPositionVector());
				//lava.monster[GO]->Update(dt);
				lava.velocity[GO].x = lava.monster[GO]->m_velocity.x * 50.f;
				lava.velocity[GO].z = lava.monster[GO]->m_velocity.z * 50.f;
				//lava.monster[GO]->m_velocity = lava.velocity[GO];
			}
		}
		if (f_RampageTimer >= 500)
		{
			Init();
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
	counter += dt;
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

	if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_ENTER].isPressed)
		std::cout << "asd" << std::endl;

	std::stringstream ss;
	ss << "RampageTimer: " << f_RampageTimer;
	RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3, 0, 12);
	
	ss.str("");
	ss << "FPS: " << fps;
	RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3, 0, 3);

	ss.str("");
	ss << "Noise factor:" << SharedData::GetInstance()->player->GetNoiseFactor();
	RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3, 0, 6);

	ss.str("");
	ss << "PLAYER HEALTH:" << SharedData::GetInstance()->player->GetHealth();
	RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3, 0, 9);

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

	for (GameObject GO = 0; GO < lava.GAMEOBJECT_COUNT; ++GO)
	{
		if ((lava.mask[GO] & COMPONENT_MONEYTREE) == COMPONENT_MONEYTREE)
		{
			if ((camera.position - lava.position[GO]).LengthSquared() < 150)
			{
				if (ViewCheckPosition(lava.position[GO], 45.f) == true)
				{
					RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "PRESS E TO CUT DOWN", Color(1, 1, 0), 3, 30, 30);

				}
			}
		}
		if ((lava.mask[GO] & COMPONENT_COIN) == COMPONENT_COIN)
		{
			if ((camera.position - lava.position[GO]).LengthSquared() < 150)
			{
				if (ViewCheckPosition(lava.position[GO], 45.f) == true)
				{
					RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "PRESS E TO COLLECT", Color(1, 1, 0), 3, 30, 30);

				}
			}
		}
	}

	RenderMonsterStates();

	SetHUD(true);
	RenderHUD();
	SetHUD(false);
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

	RenderGameObjects(&lava);

	//Skyplane
	modelStack.PushMatrix();
	modelStack.Translate(500, 2800, -500);
	//modelStack.Rotate(0, 0,0,0);
	RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_LAVA_SKYPLANE), false);
	modelStack.PopMatrix();
}

void SceneLava::RenderHUD()
{
	std::stringstream ss, ss2, ss3, ss4;
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

//========================
// == OBJECTS TO RENDER
//========================

void SceneLava::RenderMonsterStates()
{
	for (GameObject ai = 0; ai < lava.GAMEOBJECT_COUNT; ++ai)
	{
		if (lava.monster[ai])
		{

			switch (lava.monster[ai]->GetStrategyState())
			{
			case AI_Strategy::STATE_ALERT:
				break;

			case AI_Strategy::STATE_ATTACK:
				break;

			case AI_Strategy::STATE_RUN:
				Vector3 pos;

				pos = (lava.position[ai]);

				modelStack.PushMatrix();
				modelStack.Translate(pos.x, pos.y + 5.f, pos.z);
				modelStack.Scale(5, 5, 5);
				modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.x - pos.x, camera.position.z - pos.z)), 0, 1, 0);
				RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_EXCLAMATION_MARK), false);
				modelStack.PopMatrix();
				break;
			}

		}
	}

}
