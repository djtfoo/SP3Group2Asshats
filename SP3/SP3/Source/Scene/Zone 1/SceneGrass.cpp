#include "GL\glew.h"
//#include "../../shader.hpp"

#include "SceneGrass.h"
#include "../../General/Application.h"
//#include "../../Graphics/LoadOBJ/LoadTGA.h"
//#include "../../Graphics/Mesh/MeshBuilder.h"
#include "../../General/SharedData.h"
#include "../../GameObject/MonsterFactory.h"
#include "../../GameObject/AI_Strategy.h"
#include "../../General/WorldValues.h"

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
                grass.hitbox[go].m_origin = grass.position[go] + Vector3(0, 2, 0);
                grass.hitbox[go].m_scale.Set(4.f, 8.f, 4.f);
                grass.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh((it->second).first);
                grass.appearance[go].scale.Set(Math::RandFloatMinMax(0.8f, 1.f), Math::RandFloatMinMax(0.5f, 1.f), Math::RandFloatMinMax(0.8f, 1.f));
                grass.appearance[go].angle = Math::RandFloatMinMax(0.f, 360.f);
                //grass.appearance[go].scale.Set(1, 1, 1);
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
                    grass.appearance[go].scale.Set(1, 1, 1);
                    break;
                
                case '2':
                    grass.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_BIRD);
                    grass.monster[go] = MonsterFactory::CreateMonster("Bird");
                    grass.hitbox[go].m_scale.Set(4.f, 4.f, 4.f);
                    grass.appearance[go].scale.Set(1, 1, 1);
                    break;
                
                case '3':
                    grass.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BOSS_FAIRY);
                    grass.monster[go] = MonsterFactory::CreateMonster("Fairy");
                    grass.hitbox[go].m_scale.Set(5.f, 5.f, 5.f);
                    grass.appearance[go].scale.Set(3, 3, 3);
                    break;
                }
                grass.monster[go]->m_position = grass.position[go];
                grass.appearance[go].angle = 0.f;

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

            }
        }
    }

    //monster = createGO(&grass);
    //grass.mask[monster] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_AI;
    //grass.position[monster].SetZero();
    //grass.velocity[monster].Set(0, 0, 1);
    //grass.hitbox[monster].m_origin = grass.position[monster] + Vector3(0, 0.75f, -0.3);
    //grass.hitbox[monster].m_scale.Set(1.5f, 1.5f, 1.75f);
    //grass.appearance[monster].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_RABBIT);
    //grass.appearance[monster].scale.Set(1, 1, 1);
    //grass.appearance[monster].angle = 0.f;
    //
	//rock = createGO(&grass);
	//grass.mask[rock] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX;
	//grass.position[rock].Set(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z);
	//grass.appearance[rock].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCKS1);
	//grass.appearance[rock].scale.Set(1, 1, 1);
    //grass.appearance[rock].angle = 0.f;
    //
	//net = createGO(&grass);
	//grass.mask[net] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE;
	//grass.position[net].SetZero();
	//grass.appearance[net].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET);
	//grass.appearance[net].scale.Set(2, 2, 2);
    //grass.appearance[net].angle = 0.f;

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

            if (grass.monster[ai]->GetStrategyState() == AI_Strategy::STATE_ALERT)
            {
                grass.monster[ai]->m_velocity.SetZero();
            }

			grass.velocity[ai] = grass.monster[ai]->m_velocity;
		}
	}

	//Movement update for Gameobjects
	UpdateGameObjects(&grass, dt);

	//Player Update
	SharedData::GetInstance()->inputManager->Update();
    SharedData::GetInstance()->player->Update(dt);
    
    bool collision = false;
    for (GameObject obstacle = 0; obstacle < grass.GAMEOBJECT_COUNT; ++obstacle)
    {
        if ((grass.mask[obstacle] & COMPONENT_OBSTACLE) == COMPONENT_OBSTACLE)
        {
            Vector3 playerPos = SharedData::GetInstance()->player->GetPositionVector() + SharedData::GetInstance()->player->GetVelocityVector();
            playerPos.y = 0.f;

            if (grass.hitbox[obstacle].CheckCollision(playerPos))
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
	UpdateParticle(dt);

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
                        if (grass.monster[ai]->GetStrategyState() != AI_Strategy::STATE_CAPTURED) {
                            grass.monster[ai]->TakeDamage(SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].GetEffectiveness());
                            std::cout << "HEALTH: " << grass.monster[ai]->GetHealthStat() << std::endl;
							//SharedData::GetInstance()->sound->SoundEffect3D->setSoundVolume(1.0f);
							SharedData::GetInstance()->sound->playSoundEffect3D("Sound//Hit.mp3",
								irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
								irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
								irrklang::vec3df(grass.position[ai].x, grass.position[ai].y, grass.position[ai].z));
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
			for (GameObject ai = 0; ai < grass.GAMEOBJECT_COUNT; ++ai)
			{
				if ((grass.mask[ai] & COMPONENT_AI) == COMPONENT_AI && grass.hitbox[ai].CheckCollision(ItemProjectile::NetProjectileList[i]->position)
					&& grass.monster[ai]->GetStrategyState() != AI_Strategy::STATE_CAPTURED)
				{
					ItemProjectile::NetProjectileList[i]->deleteBullet = true;
					net = createGO(&grass);
					grass.mask[net] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_CAPTURE;
					grass.capture[net].capturingMonster = true;
					grass.capture[net].capturedMonster = false;
					grass.capture[net].timeBeforeCapture = 3;
					grass.appearance[net].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET);
					grass.appearance[net].scale.Set(2, 2, 2);
                    grass.appearance[net].angle = 0.f;

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
                            grass.appearance[text_C].angle = 0.f;
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
							grass.appearance[text_M].angle = 0.f;
							SharedData::GetInstance()->sound->playSoundEffect3D("Sound//Escaped.wav",
								irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
								irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
								irrklang::vec3df(grass.position[ai].x, grass.position[ai].y, grass.position[ai].z));
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
						grass.capture[net].caughtMonster = ai;
						SharedData::GetInstance()->sound->playSoundEffect3D("Sound//NPC.wav",
							irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
							irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
							irrklang::vec3df(grass.position[ai].x, grass.position[ai].y, grass.position[ai].z));
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
                grass.appearance[bait].angle = 0.f;
				grass.bait[bait].scentRadius = 200;
				grass.bait[bait].foundRadius = 5;
				grass.bait[bait].eattingBait = false;
				grass.bait[bait].foundBait = false;
				grass.bait[bait].timeEatting = 3.0f;
			}
		}
	}


    for (GameObject bait = 0; bait < grass.GAMEOBJECT_COUNT; ++bait)
    {
        if ((grass.mask[bait] & COMPONENT_BAIT) == COMPONENT_BAIT)
        {

            for (GameObject ai = 0; ai < grass.GAMEOBJECT_COUNT; ++ai)
            {
                if ((grass.mask[ai] & COMPONENT_AI) == COMPONENT_AI && grass.monster[ai])
                {
                    if (grass.monster[ai]->GetStrategyState() == AI_Strategy::STATE_IDLE && (grass.position[ai] - grass.position[bait]).LengthSquared() < grass.bait[bait].scentRadius && grass.bait[bait].eattingBait == false)
                    {
                        grass.bait[bait].foundBait = true;
                        //std::cout << " FOUND BAIT " + grass.bait[bait].foundBait << std::endl;
                        if (grass.bait[bait].foundBait == true)
                        {
                            //grass.velocity[ai] = (grass.position[bait] - grass.position[ai]).Normalized();
                            //grass.velocity[ai].y = 0;
                            //grass.monster[ai]->m_velocity = grass.velocity[ai];
                            if (grass.monster[ai]->GetStrategyState() == AI_Strategy::STATE_ALERT || grass.monster[ai]->GetStrategyState() == AI_Strategy::STATE_IDLE || grass.monster[ai]->GetStrategyState() == AI_Strategy::STATE_BAITED)
                            {
                                grass.monster[ai]->GetBaited(grass.position[bait]);
                                grass.bait[bait].baitedMonsters.push_back(ai);
                            }
                        }
                    }
                    if ((grass.position[ai] - grass.position[bait]).LengthSquared() < grass.bait[bait].foundRadius)
                    {
                        std::cout << "EATING BAIT " + grass.bait[bait].foundBait << std::endl;
                        grass.bait[bait].foundBait = false;
                        grass.bait[bait].eattingBait = true;
                        grass.velocity[ai].SetZero();
                        grass.monster[ai]->m_velocity.SetZero();
                    }
                    if (grass.bait[bait].eattingBait == true && (grass.position[ai] - grass.position[bait]).LengthSquared() < grass.bait[bait].foundRadius)
                    {
                        grass.bait[bait].timeEatting -= (float)(dt);
                        if (grass.bait[bait].timeEatting > 0.1f) {
                            float scale = grass.bait[bait].timeEatting / 3.f;
                            grass.appearance[bait].scale.Set(scale, scale, scale);
                        }
                    }

                    if (grass.bait[bait].timeEatting <= 0)
                    {
                        //grass.bait[bait].eattingBait = false;
                        grass.bait[bait].finishedBait = true;
                        //grass.velocity[ai] = 2;
                        //grass.velocity[ai].y = 0;
                    }
                }
            }

            if (grass.bait[bait].finishedBait == true) {
                //PUT WHATEVER THE RABBIT DO NORMALLY HERE :D (DONE EATTING BAIT)
                for (unsigned i = 0; i < grass.bait[bait].baitedMonsters.size(); ++i) {
                    GameObject ai = grass.bait[bait].baitedMonsters[i];
                    grass.monster[ai]->SetIdleState();
                    grass.velocity[ai] = grass.monster[ai]->m_velocity;
                }
                destroyGO(&grass, bait);
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
                    if ((grass.mask[ai] & COMPONENT_AI) == COMPONENT_AI && grass.monster[ai] && grass.monster[ai]->GetStrategyState() != AI_Strategy::STATE_TRAPPED
                        && grass.monster[ai]->GetStrategyState() != AI_Strategy::STATE_CAPTURED)
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
							SharedData::GetInstance()->sound->playSoundEffect3D("Sound//TrapActive.wav",
								irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
								irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
								irrklang::vec3df(grass.position[ai].x, grass.position[ai].y, grass.position[ai].z));
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
		if ((grass.mask[ai] & COMPONENT_AI) == COMPONENT_AI && grass.monster[ai] && grass.monster[ai]->GetStrategyState() == AI_Strategy::STATE_ATTACK)
		{
			//if ((grass.position[ai] - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() <
			//    0.25f * (SharedData::GetInstance()->player->PlayerHitBox.m_scale.x + grass.hitbox[ai].m_scale.x) * (SharedData::GetInstance()->player->PlayerHitBox.m_scale.z + grass.hitbox[ai].m_scale.z))
			if (grass.hitbox[ai].CheckCollision(SharedData::GetInstance()->player->PlayerHitBox))
			{
				std::cout << "Distance:" << (grass.hitbox[ai].m_origin - SharedData::GetInstance()->player->PlayerHitBox.m_origin).Length() << std::endl;
				std::cout << std::endl;

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
	if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_G].isPressed && SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP].Use())
	{
		GameObject trap = createGO(&grass);

		grass.mask[trap] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_TRAP;

		grass.position[trap].Set(SharedData::GetInstance()->player->GetPositionVector().x + SharedData::GetInstance()->player->GetViewVector().x * 20, 0, SharedData::GetInstance()->player->GetPositionVector().z + SharedData::GetInstance()->player->GetViewVector().z * 20);
		grass.appearance[trap].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TRAP);
		grass.appearance[trap].scale.Set(3, 3, 3);
        grass.appearance[trap].angle = 0.f;

		grass.trap[trap].triggerDuration = 3.0f;
		grass.trap[trap].triggerTimer = 0.f;
		grass.trap[trap].radius = 2.5f;
		grass.trap[trap].activated = false;
		grass.trap[trap].caughtMonster = 0;
		grass.trap[trap].caughtMonsterVel.SetZero();

		SharedData::GetInstance()->sound->playSoundEffect3D("Sound//SetTrap.wav",
			irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
			irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
			irrklang::vec3df(grass.position[trap].x, grass.position[trap].y, grass.position[trap].z));

		counter = 0;
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
		if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed && SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].Use())
		{
			ItemProjectile::RockProjectileList.push_back(new ItemProjectile(
				Vector3(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z),
				Vector3(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
				500,
				50,
				10
				));
			SharedData::GetInstance()->sound->playSoundEffect3D("Sound//Throwing.wav",
				irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
				irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
				irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z));

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
				SharedData::GetInstance()->sound->playSoundEffect3D("Sound//ThrowingNet.wav",
					irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
					irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
					irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z));
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
			SharedData::GetInstance()->sound->playSoundEffect3D("Sound//Throwing.wav",
				irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
				irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
				irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z));
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

		for (GameObject GO = 0; GO < grass.GAMEOBJECT_COUNT; ++GO)
		{
			// check for picking up net
			if ((grass.mask[GO] & COMPONENT_CAPTURE) == COMPONENT_CAPTURE)
			{
				GameObject ai = grass.capture[GO].caughtMonster;
				if ((camera.position - grass.position[ai]).LengthSquared() < 150)
				{
					if (ViewCheckPosition(grass.position[ai], 45.f) == true)
					{
						std::cout << "CAUGHT THE MONSTER" << std::endl;
						SharedData::GetInstance()->player->monsterList.push_back(grass.monster[ai]->GetName());
						SharedData::GetInstance()->sound->playSoundEffect3D("Sound//Captured.mp3",
							irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
							irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
							irrklang::vec3df(grass.position[ai].x, grass.position[ai].y, grass.position[ai].z));
						destroyGO(&grass, ai);
						// destroy net
						destroyGO(&grass, GO);
					}
				}
			}

			// check for interacting with money tree
			if ((grass.mask[GO] & COMPONENT_MONEYTREE) == COMPONENT_MONEYTREE)
			{
				if ((camera.position - grass.position[GO]).LengthSquared() < 150)
				{
					if (ViewCheckPosition(grass.position[GO], 180.f) == true)
					{
						std::cout << "MoneyTree Found" << std::endl;
						grass.mask[GO] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_COIN;
						grass.appearance[GO].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_COINS);
						grass.appearance[GO].scale.Set(5, 5, 5);


						break;
					}
				}
			}

			// check for interacting with coin
			if ((grass.mask[GO] & COMPONENT_COIN) == COMPONENT_COIN)
			{
				if ((camera.position - grass.position[GO]).LengthSquared() < 150)
				{
					if (ViewCheckPosition(grass.position[GO], 180.f) == true)
					{
						std::cout << "coin picked up" << std::endl;
						SharedData::GetInstance()->sound->playSoundEffect3D("Sound//ItemFound.mp3",
							irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
							irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
							irrklang::vec3df(grass.position[GO].x, grass.position[GO].y, grass.position[GO].z));
						destroyGO(&grass, GO);

						break;
					}
				}
			}
		}

		//for (GameObject MoneyTree = 0; MoneyTree < grass.GAMEOBJECT_COUNT; ++MoneyTree)
		//{

		//}

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

	for (GameObject GO = 0; GO < grass.GAMEOBJECT_COUNT; ++GO)
	{
		if ((grass.mask[GO] & COMPONENT_MONEYTREE) == COMPONENT_MONEYTREE)
		{
			if ((camera.position - grass.position[GO]).LengthSquared() < 150)
			{
				if (ViewCheckPosition(grass.position[GO], 45.f) == true)
				{
					RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "PRESS E TO CUT DOWN", Color(1, 1, 0), 3, 30, 30);

				}
			}
		}
		if ((grass.mask[GO] & COMPONENT_COIN) == COMPONENT_COIN)
		{
			if ((camera.position - grass.position[GO]).LengthSquared() < 150)
			{
				if (ViewCheckPosition(grass.position[GO], 45.f) == true)
				{
					RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "PRESS E TO COLLECT", Color(1, 1, 0), 3, 30, 30);
				}
			}
		}
	}

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

	for (std::vector<ParticleObject*>::iterator it = particleList.begin(); it != particleList.end(); ++it)
	{
		ParticleObject* particle = (ParticleObject*)*it;
		if ((particle)->active)
		{
			RenderParticle(particle);
		}
	}
}

void SceneGrass::RenderHUD()
{
	std::stringstream ss, ss2, ss3, ss4;
	ss << "Rocks: " << SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].GetCount();
	ss2 << "Nets: " << SharedData::GetInstance()->player->inventory[Item::TYPE_NET].GetCount();
	ss3 << "Baits: " << SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].GetCount();
	ss4 << "Traps: " << SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP].GetCount();

	RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_HUDHIGHLIGHT), false, 11.f, 12.f, f_HighlightPos, -48);
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

void SceneGrass::Exit()
{
	while (particleList.size() > 0)
	{
		ParticleObject * particle = particleList.back();
		delete particle;
		particleList.pop_back();
	}

	for (unsigned GO = 0; GO < grass.GAMEOBJECT_COUNT; ++GO)
	{
		//if (grass.monster[GO])
		//{
		//    delete grass.monster[GO];
		//}

		// call destroyGO instead
		destroyGO(&grass, GO);
	}

}

//========================
// == OBJECTS TO RENDER
//========================

//void SceneGrass::RenderMonsterStates()
//{
//	for (GameObject ai = 0; ai < grass.GAMEOBJECT_COUNT; ++ai)
//	{
//		if (grass.monster[ai])
//		{
//			Vector3 pos;
//			float hpBar = grass.monster[ai]->GetHealthStat() / 80.f;
//			//float captureBar = grass.monster[ai]->GetCaptureRateStat() / 5;
//
//			pos = (grass.position[ai]);
//
//			modelStack.PushMatrix();
//			modelStack.Translate((pos.x /*- 0.5f * hpBar*/) /*- 0.5f*/, pos.y + 2.f, pos.z);
//			modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.x - pos.x, camera.position.z - pos.z)), 0, 1, 0);
//			//modelStack.Translate(0.5f * hpBar, 0.f, 0.f);
//			modelStack.Scale(hpBar, 0.2, 1);
//			RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_HPBAR), false);
//			modelStack.PopMatrix();
//
//			//modelStack.PushMatrix();
//			//modelStack.Translate((pos.x + 0.5 * captureBar) - 0.5, pos.y + 3.f + 0.5, pos.z);
//			//modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.x - pos.x, camera.position.z - pos.z)), 0, 1, 0);
//			//modelStack.Scale(captureBar, 0.2, 1);
//			//RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CAPTUREBAR), false);
//			//modelStack.PopMatrix();
//
//			switch (grass.monster[ai]->GetStrategyState())
//			{
//			case AI_Strategy::STATE_ALERT:
//				break;
//
//			case AI_Strategy::STATE_ATTACK:
//			case AI_Strategy::STATE_RUN:
//
//				modelStack.PushMatrix();
//				modelStack.Translate(pos.x, pos.y + 5.f, pos.z);
//				modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.x - pos.x, camera.position.z - pos.z)), 0, 1, 0);
//				modelStack.Scale(5, 5, 5);
//				RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_EXCLAMATION_MARK), false);
//				modelStack.PopMatrix();
//
//				break;
//			}
//		}
//	}
//}

ParticleObject* SceneGrass::GetParticle()
{
	for (std::vector<ParticleObject*>::iterator it = particleList.begin(); it != particleList.end(); ++it)
	{
		ParticleObject* particle = (ParticleObject*)*it;
		if (!particle->active)
		{
			particle->active = true;
			m_particleCount++;
			return particle;
		}
	}

	for (unsigned i = 0; i < 10; ++i)
	{
		ParticleObject* particle = new ParticleObject(ParticleObject_TYPE::P_WATER);
		particleList.push_back(particle);
	}

	ParticleObject* particle = particleList.back();
	particle->active = true;
	m_particleCount++;
	return particle;
}

void SceneGrass::UpdateParticle(double dt)
{
	if (m_particleCount < MAX_PARTICLE)
	{
		for (int i = 0; i < 100; i++)
		{
			ParticleObject* particle1 = GetParticle();
			particle1->type = ParticleObject_TYPE::P_HIDDENBONUS;
			particle1->scale.Set(1, 1, 1);
			particle1->vel.Set(Math::RandFloatMinMax(-30, 30), Math::RandFloatMinMax(-30, 30), Math::RandFloatMinMax(-30, 30));
			particle1->rotationSpeed = Math::RandFloatMinMax(20.0f, 40.0f);
			particle1->pos.Set(0, 1200.0f, 0);
		}
	}

	for (std::vector<ParticleObject*>::iterator it = particleList.begin(); it != particleList.end(); ++it)
	{
		ParticleObject* particle1 = (ParticleObject*)*it;
		if (particle1->active)
		{
			if (particle1->type == ParticleObject_TYPE::P_HIDDENBONUS)
			{
				particle1->vel += WV_GRAVITY *(float)dt * 5;
				particle1->pos += particle1->vel * (float)dt * 20.0f;

				particle1->rotation += particle1->rotationSpeed * (float)dt;

			}
		}
	}
}

void SceneGrass::RenderParticle(ParticleObject* particle)
{
	switch (particle->type)
	{

	case ParticleObject_TYPE::P_HIDDENBONUS:
		modelStack.PushMatrix();
		modelStack.Translate(particle->pos.x, particle->pos.y, particle->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.x - particle->pos.x, camera.position.z - particle->pos.z)), 0, 1, 0);
		modelStack.Scale(particle->scale.x, particle->scale.y, particle->scale.z);
		RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCKS1), false);
		modelStack.PopMatrix();
		break;

	default:
		break;
	}
}