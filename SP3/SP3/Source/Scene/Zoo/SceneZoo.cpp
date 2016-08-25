#include "GL\glew.h"
//#include "../../shader.hpp"

#include "SceneZoo.h"
#include "../../General/Application.h"
//#include "../../Graphics/LoadOBJ/LoadTGA.h"
//#include "../../Graphics/Mesh/MeshBuilder.h"
#include "../../General/SharedData.h"

#include <sstream>

SceneZoo::SceneZoo()
{
}

SceneZoo::~SceneZoo()
{
}

void SceneZoo::Init()
{
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 1000 units
    Mtx44 perspective;
    perspective.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
    //perspective.SetToOrtho(-80, 80, -60, 60, -1000, 1000);
    projectionStack.LoadMatrix(perspective);

    zooCamera.Init(Vector3(0, 100, -100), Vector3(0, 0, 0), Vector3(0, 1, 0));

    Application::GetCursorPos(&Application::cursorXPos, &Application::cursorYPos);
    Application::SetCursorPos(Application::GetWindowWidth() / 2.f, Application::GetWindowHeight() / 2.f);

    bLButtonState = false;

    b_capturing = false;
    b_captured = false;
    captureCounter = 0;
    counter = 0;

    memset(&zooWorld, 0, sizeof(zooWorld));

    for (unsigned i = 0; i < 20; ++i)
    {
        GameObject go = createGO(&zooWorld);
        zooWorld.mask[go] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_HITBOX;
        zooWorld.position[go].Set(Math::RandFloatMinMax(-100.f, 100.f), 2.5f, Math::RandFloatMinMax(-100.f, 100.f));
        zooWorld.hitbox[go].m_origin = zooWorld.position[go];
        zooWorld.hitbox[go].m_scale.Set(5.f, 5.f, 5.f);
        zooWorld.appearance[go].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE);
        zooWorld.appearance[go].scale.Set(1, 1, 1);
    }
}
static double counter = 0;

void SceneZoo::Update(double dt)
{
    fps = (float)(1.f / dt);

    //===============================================================================================================================//
    //                                                            Updates                                                            //
    //===============================================================================================================================//

    //Movement update for Gameobjects
    //UpdateGameObjects(&zooWorld, dt);

    //Camera Update
    zooCamera.Update(dt);

    //Player Update
    //SharedData::GetInstance()->player->Update(dt);

    //Inputmanager Update
    SharedData::GetInstance()->inputManager->Update();

    //===============================================================================================================================//
    //                                                            Checks                                                             //
    //===============================================================================================================================//

    //===============================================================================================================================//
    //                                                            Key Inputs                                                         //
    //===============================================================================================================================//

    counter += dt;
}

void SceneZoo::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Mtx44 perspective;
    perspective.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
    //perspective.SetToOrtho(-80, 80, -60, 60, -1000, 1000);
    projectionStack.LoadMatrix(perspective);

    // Camera matrix
    viewStack.LoadIdentity();
    viewStack.LookAt(
        zooCamera.position.x, zooCamera.position.y, zooCamera.position.z,
        zooCamera.target.x, zooCamera.target.y, zooCamera.target.z,
        zooCamera.up.x, zooCamera.up.y, zooCamera.up.z
        );
    // Model matrix : an identity matrix (model will be at the origin)
    modelStack.LoadIdentity();

    //shows pressed locations
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_SPACE].isPressed)
    {
        Vector3 temp;
        temp = zooCamera.target;
        temp.y = 0;
        tempStore.push_back(temp);
    }

    //for checking stuff
    for (unsigned i = 0; i < tempStore.size(); ++i)
    {
        modelStack.PushMatrix();
        modelStack.Translate(tempStore[i].x, tempStore[i].y, tempStore[i].z);
        RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
        modelStack.PopMatrix();
    }

    //for camera.target
    modelStack.PushMatrix();
    modelStack.Translate(zooCamera.target.x, zooCamera.target.y, zooCamera.target.z);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
    modelStack.PopMatrix();

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

    //Render World
    RenderZooScene();

    for (GameObject ai = 0; ai < zooWorld.GAMEOBJECT_COUNT; ++ai)
    {

    }

    for (GameObject tallGrass = 0; tallGrass < zooWorld.GAMEOBJECT_COUNT; ++tallGrass)
    {
        if ((zooWorld.mask[tallGrass] & COMPONENT_HITBOX) == COMPONENT_HITBOX)
        {
            modelStack.PushMatrix();
            modelStack.Translate(zooWorld.hitbox[tallGrass].m_origin.x, zooWorld.hitbox[tallGrass].m_origin.y, zooWorld.hitbox[tallGrass].m_origin.z);
            modelStack.Scale(zooWorld.hitbox[tallGrass].m_scale.x, zooWorld.hitbox[tallGrass].m_scale.y, zooWorld.hitbox[tallGrass].m_scale.z);
            //modelStack.Rotate(0, 0, 0, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            modelStack.PopMatrix();
        }
    }

}

void SceneZoo::RenderZooScene()
{
    //Ground mesh
    modelStack.PushMatrix();
    modelStack.Translate(0, 0, 0);
    modelStack.Rotate(-90, 1, 0, 0);
    modelStack.Scale(200, 200, 100);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCK_TERRAIN), true);
    modelStack.PopMatrix();

    //Skyplane
    modelStack.PushMatrix();
    modelStack.Translate(500, 2800, -500);
    //modelStack.Rotate(0, 0,0,0);
    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_GRASS_SKYPLANE), false);
    modelStack.PopMatrix();
}

void SceneZoo::Exit()
{

}

//========================
// == OBJECTS TO RENDER
//========================





//Mouse picking stuff//
////Step1 - ViewPort space
//float x = 1.0f - (2.0f * Application::cursorYPos) / 1080;
//float y = (2.0f * Application::cursorXPos) / 1920 - 1.0f; 
//float z = 1.0f;
//Vector3 ray_nds;
//ray_nds.Set(x, y, z);

////Step 2 - Clip space
//Mtx44 ray_clip;
//ray_clip.SetToPerspective(ray_nds.x, ray_nds.y, -1.0f, 0.0f);

////Step3 - Camera space
//Mtx44 ray_cam = ray_clip * perspective.GetInverse();

////Step4 - World space 
//Mtx44 view_matrix;
//view_matrix.SetToLookAt(zooCamera.position.x, zooCamera.position.y, zooCamera.position.z,
//    zooCamera.target.x, zooCamera.target.y, zooCamera.target.z,
//    zooCamera.up.x, zooCamera.up.y, zooCamera.up.z);

//Mtx44 ray_wor = (view_matrix.GetInverse() * ray_cam);

//Vector3 ray_world = ray_wor * Vector3(1,1,1);
//ray_world = ray_world.Normalized();

////std::cout << ray_world << std::endl;

////end of Mouse picking stuff//





////ground normal
//Vector3 ground_normal;
//ground_normal.Set(0, 1, 0);

//Vector3 ray;
//ray = zooCamera.position + ray_world;
//
//float distance = 100;

//Vector3 result;
//result = ray_world * distance;

//std::cout << result << std::endl;

//if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_SPACE].isHeldDown)
//{
//    result.y = 0;
//    tempStore.push_back(result);
//}

//for (unsigned i = 0; i < tempStore.size(); ++i)
//{
//    modelStack.PushMatrix();
//    modelStack.Translate(tempStore[i].x, tempStore[i].y, tempStore[i].z);
//    RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CUBE), false);
//    modelStack.PopMatrix();
//}