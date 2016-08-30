#include "GL\glew.h"
//#include "../../shader.hpp"

#include "SceneSwamp.h"
#include "../../General/Application.h"
//#include "../../Graphics/LoadOBJ/LoadTGA.h"
//#include "../../Graphics/Mesh/MeshBuilder.h"
#include "../../General/SharedData.h"
#include "../../GameObject/MonsterFactory.h"

#include <sstream>

SceneSwamp::SceneSwamp()
{
}

SceneSwamp::~SceneSwamp()
{
}

void SceneSwamp::Init()
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
    memset(&swamp, 0, sizeof(swamp));

    // Load map
    Scene::LoadLevelMap("GameData/SwampScene.csv");
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
                GameObject go = createGO(&swamp);

                for (unsigned i = 0; i < (it->second).second.size(); ++i)
                {
                    swamp.mask[go] = swamp.mask[go] | (it->second).second[i];
                }

                swamp.position[go].Set(cols * Scene::tileSize, 0.f, rows * Scene::tileSize);
                swamp.hitbox[go].m_origin = swamp.position[go] + Vector3(0, 2, 0);
                swamp.hitbox[go].m_scale.Set(4.f, 8.f, 4.f);
                swamp.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh((it->second).first);
                swamp.appearance[go].scale.Set(Math::RandFloatMinMax(0.8f, 1.f), Math::RandFloatMinMax(0.5f, 1.f), Math::RandFloatMinMax(0.8f, 1.f));
                swamp.appearance[go].angle = Math::RandFloatMinMax(0.f, 360.f);
                //swamp.appearance[go].scale.Set(1, 1, 1);
            }
            else if (tile >= '1' && tile <= '9')
            {
                GameObject go = createGO(&swamp);
                swamp.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_AI | COMPONENT_OBSTACLE;
                //swamp.velocity[go].Set(Math::RandFloatMinMax(0.f, 1.f), 0, Math::RandFloatMinMax(0.f, 1.f));
                //swamp.velocity[go].SetZero();
                swamp.position[go].Set(cols * tileSize + Math::RandFloatMinMax(0.f, 1.f), 0.f, rows * tileSize + Math::RandFloatMinMax(0.f, 1.f));
                swamp.hitbox[go].m_origin = swamp.position[go] + Vector3(0, 0.75f, -0.3f);
                switch (tile)
                {
                case '1':
                    swamp.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_SEAMONSTER);
                    swamp.monster[go] = MonsterFactory::CreateMonster("SeaMonster");
                    swamp.hitbox[go].m_scale.Set(1.5f, 2.f, 1.75f);
                    swamp.appearance[go].scale.Set(1.5f, 1.5f, 1.5f);
                    break;

                case '2':
                    swamp.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_GRIMEJAM);
                    swamp.monster[go] = MonsterFactory::CreateMonster("Grimejam");
                    swamp.hitbox[go].m_scale.Set(2.f, 2.f, 2.f);
                    swamp.appearance[go].scale.Set(2.f, 2.f, 2.f);
                    break;

                case '3':
                    swamp.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BOSS_MUKBOSS);
                    swamp.monster[go] = MonsterFactory::CreateMonster("MukBoss");
                    swamp.hitbox[go].m_scale.Set(3.f, 3.f, 3.f);
                    swamp.appearance[go].scale.Set(6, 6, 6);
                    break;
                }
                swamp.monster[go]->m_position = swamp.position[go];
                swamp.appearance[go].angle = 0.f;

                int randCol = cols + Math::RandIntMinMax(3, 5) * Math::RandIntMinMax(-1, 1);
                int randRow = rows + Math::RandIntMinMax(3, 5) * Math::RandIntMinMax(-1, 1);
                while (randCol < 0 || randCol >= 40 || randRow < 0 || randRow >= 40 || Scene::m_levelMap[randRow][randCol] != '0')
                {
                    randCol = cols + Math::RandIntMinMax(3, 5) * Math::RandIntMinMax(-1, 1);
                    randRow = rows + Math::RandIntMinMax(3, 5) * Math::RandIntMinMax(-1, 1);
                }

                Vector3 RNGdestination(randCol * Scene::tileSize + Scene::tileSize * 0.5f, 0, randRow * Scene::tileSize + Scene::tileSize * 0.5f);

                swamp.monster[go]->m_destination = RNGdestination;
                swamp.velocity[go] = (RNGdestination - swamp.position[go]).Normalized() * 2.f;
                swamp.monster[go]->m_velocity = swamp.velocity[go];

            }
        }
    }
    
    //monster = createGO(&grass);
    //grass.mask[monster] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_AI;
    //grass.position[monster].SetZero();
    //grass.velocity[monster].Set(0, 0, 1);
    //grass.hitbox[monster].m_origin = grass.position[monster] + Vector3(0, 0.75f, -0.3);
    //grass.hitbox[monster].m_scale.Set(1.5f, 1.5f, 1.75f);
    //grass.appearance[monster].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_RABBIT);
    //grass.appearance[monster].scale.Set(1, 1, 1);
    //
    //rock = createGO(&grass);
    //grass.mask[rock] = COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_HITBOX;
    //grass.position[rock].Set(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z);
    //grass.appearance[rock].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE);
    //grass.appearance[rock].scale.Set(1, 1, 1);
    //
    //net = createGO(&grass);
    //grass.mask[net] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE;
    //grass.position[net].SetZero();
    //grass.appearance[net].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET);
    //grass.appearance[net].scale.Set(2, 2, 2);
    ////HITBOX.m_origin = Vector3(0, 5, 0);
    ////HITBOX.m_scale = Vector3(10, 10, 10);
	
	// Fog
	fog.color.Set(0.3f, 0.6f, 0.3f);
	fog.start = 10;
	fog.end = 1000;
	fog.density = 0.008f;
	fog.type = Fog::FOG_EXPONENTIAL;
	fog.enabled = true;
	glUniform3fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_FOG_COLOR), 1, &fog.color.r);

	glUniform1f(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_FOG_START), fog.start);
	glUniform1f(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_FOG_END), fog.end);
	glUniform1f(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_FOG_DENSITY), fog.density);
	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_FOG_TYPE), fog.type);
	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_FOG_ENABLED), fog.enabled);

    b_capturing = false;
    b_captured = false;
    captureCounter = 0;

    camera.Update();
}
static double counter = 0;

void SceneSwamp::UpdateFog(double dt)
{
	fog.density += 0.0005f * dt * fog.fog_change;
	if (fog.density <= 0.001f || fog.density >= 0.009f) {
		fog.fog_change *= -1;
	}
	glUniform1f(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_FOG_DENSITY), fog.density);

}

void SceneSwamp::Update(double dt)
{
    fps = (float)(1.f / dt);

    //===============================================================================================================================//
    //                                                            Updates                                                            //
    //===============================================================================================================================//

	UpdateFog(dt);

    //Update Projectiles
    itemProjectile->UpdateProjectile(dt);

    //Movement update for Gameobjects
    UpdateGameObjects(&swamp, dt);

    //Camera Update
    camera.Update();

    //Player Update
    SharedData::GetInstance()->player->Update(dt);

    SharedData::GetInstance()->inputManager->Update();

    //===============================================================================================================================//
    //                                                            Checks                                                             //
    //===============================================================================================================================//

    counter += dt;
}

void SceneSwamp::Render()
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

    RenderSwampScene();

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


    //=============
    // RENDER FPS
    //=============
    std::stringstream ss;
    ss << "FPS: " << fps;
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3, 0, 3);
}

void SceneSwamp::RenderSwampScene()
{
	modelStack.PushMatrix();
	modelStack.Translate(100, -7, 100);
	modelStack.Scale(300.0f, 30.0f, 300.0f);
	RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SWAMP_TERRAIN), true);
	modelStack.PopMatrix();

    RenderGameObjects(&swamp);

    //Skyplane
    modelStack.PushMatrix();
    modelStack.Translate(500, 2800, -500);
    //modelStack.Rotate(0, 0,0,0);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_SWAMP_SKYPLANE), false);
    modelStack.PopMatrix();
}

bool SceneSwamp::ViewCheckPosition(Vector3 pos, float degree)
{
    if (pos != camera.position)
    {
        Vector3 view = (pos - camera.position).Normalized();

        float angleX = Math::RadianToDegree(acos(view.Dot(SharedData::GetInstance()->player->GetViewVector())));

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

void SceneSwamp::Exit()
{

}

//========================
// == OBJECTS TO RENDER
//========================
