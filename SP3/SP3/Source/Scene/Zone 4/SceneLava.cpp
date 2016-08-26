#include "GL\glew.h"
//#include "../../shader.hpp"

#include "SceneLava.h"
#include "../../General/Application.h"
//#include "../../Graphics/LoadOBJ/LoadTGA.h"
//#include "../../Graphics/Mesh/MeshBuilder.h"
#include "../../General/SharedData.h"
#include "../../GameObject/MonsterFactory.h"

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

				lava.position[go].Set(cols * Scene::tileSize, 0.f, rows * Scene::tileSize);
				lava.hitbox[go].m_origin = lava.position[go];
				lava.hitbox[go].m_scale.Set(4.f, 4.f, 4.f);
				lava.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh((it->second).first);
				lava.appearance[go].scale.Set(Math::RandFloatMinMax(0.8f, 1.f), Math::RandFloatMinMax(0.5f, 1.f), Math::RandFloatMinMax(0.8f, 1.f));
				//lava.appearance[go].scale.Set(1, 1, 1);
			}
			else if (tile >= '1' && tile <= '9')
			{
				GameObject go = createGO(&lava);
				lava.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_AI;
				//lava.velocity[go].Set(Math::RandFloatMinMax(0.f, 1.f), 0, Math::RandFloatMinMax(0.f, 1.f));
				//lava.velocity[go].SetZero();
				lava.position[go].Set(cols * tileSize + Math::RandFloatMinMax(0.f, 1.f), 0.f, rows * tileSize + Math::RandFloatMinMax(0.f, 1.f));
				lava.hitbox[go].m_origin = lava.position[go];
				lava.hitbox[go].m_scale.Set(4.f, 4.f, 4.f);
				switch (tile)
				{
				case '1':
					lava.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_RABBIT);
					lava.monster[go] = MonsterFactory::CreateMonster("Rabbit");
					break;

				case '2':
					lava.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_BIRD);
					lava.monster[go] = MonsterFactory::CreateMonster("Bird");
					break;

				case '3':
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
				lava.velocity[go] = (RNGdestination - lava.position[go]).Normalized() * 2.f;
				lava.monster[go]->m_velocity = lava.velocity[go];
				lava.appearance[go].scale.Set(1, 1, 1);
				std::cout << lava.monster[go]->m_position << lava.monster[go]->m_destination << std::endl;

			}
		}
	}
	//monster = createGO(&lava);
	//lava.mask[monster] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_AI;
	//lava.position[monster].SetZero();
	//lava.velocity[monster].Set(0, 0, 1);
	//lava.hitbox[monster].m_origin = lava.position[monster] + Vector3(0, 0.75f, -0.3);
	//lava.hitbox[monster].m_scale.Set(1.5f, 1.5f, 1.75f);
	//lava.appearance[monster].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_RABBIT);
	//lava.appearance[monster].scale.Set(1, 1, 1);
	//
	//rock = createGO(&lava);
	//lava.mask[rock] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX;
	//lava.position[rock].Set(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z);
	//lava.appearance[rock].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE);
	//lava.appearance[rock].scale.Set(1, 1, 1);
	//
	//net = createGO(&lava);
	//lava.mask[net] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE;
	//lava.position[net].SetZero();
	//lava.appearance[net].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET);
	//lava.appearance[net].scale.Set(2, 2, 2);
	////HITBOX.m_origin = Vector3(0, 5, 0);
	////HITBOX.m_scale = Vector3(10, 10, 10);

	counter = 3;
	b_Rocks = true;
	b_Nets = false;
	b_Baits = false;

	f_RotateRock = 0;
	f_RotateNet = 0;
	f_RotateBait = 0;
}
static double counter = 0;

void SceneLava::Update(double dt)
{
	//===============================================================================================================================//
	//                                                            Updates                                                            //
	//===============================================================================================================================//

	//Update Projectiles
	itemProjectile->UpdateProjectile(dt);
	rockProjectile->UpdateRockProjectile(dt);
	netProjectile->UpdateNetProjectile(dt);
	baitProjectile->UpdateBaitProjectile(dt);

	//Movement update for Gameobjects
	UpdateGameObjects(&lava, dt);

	//Camera Update
	camera.Update(dt);

	//Player Update
	SharedData::GetInstance()->player->Update(dt);
	SharedData::GetInstance()->inputManager->Update();

	//Monster Update
	for (GameObject ai = 0; ai < lava.GAMEOBJECT_COUNT; ++ai)
	{
		if (lava.monster[ai])
		{
			lava.monster[ai]->m_position = lava.position[ai];
			lava.monster[ai]->Update(dt);
			lava.velocity[ai] = lava.monster[ai]->m_velocity;
		}
	}

	//===============================================================================================================================//
	//                                                            Checks                                                             //
	//===============================================================================================================================//

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

    for (vector<ItemProjectile*>::iterator it = ItemProjectile::ItemProjectileList.begin(); it != ItemProjectile::ItemProjectileList.end(); ++it){
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

    RenderLavaScene();

    //visable hitbox
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
    modelStack.Scale(1, 1, 1);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TRAP), false);
    modelStack.PopMatrix();

    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_ENTER].isPressed)
        std::cout << "asd" << std::endl;
}

void SceneLava::RenderLavaScene()
{
    //Ground mesh
    modelStack.PushMatrix();
    modelStack.Translate(0, 0, 0);
    modelStack.Rotate(-90, 1, 0, 0);
    modelStack.Scale(200, 200, 100);
    //RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GRASS), true);
    modelStack.PopMatrix();

    //RenderGameObjects(&lava);

    //Skyplane
    modelStack.PushMatrix();
    modelStack.Translate(500, 2800, -500);
    //modelStack.Rotate(0, 0,0,0);
    //RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GRASSZONESKYPLANE), false);
    modelStack.PopMatrix();
}

bool SceneLava::ViewCheckPosition(Vector3 pos, float degree)
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

void SceneLava::Exit()
{

}

//========================
// == OBJECTS TO RENDER
//========================
