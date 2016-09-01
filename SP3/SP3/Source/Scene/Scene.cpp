/******************************************************************************/
/*!
\file	Scene.cpp
\author Foo Jing Ting
\par	email: 152856H\@mymail.nyp.edu.sg
\brief
Abstract class for scenes in gameplay
*/
/******************************************************************************/
#include "GL\glew.h"

#include "../../Source/Scene/Scene.h"
#include "../General/SharedData.h"
#include "../General/LoadFile.h"
#include "MyMath.h"

#include "../GameObject/AI_Strategy.h"
#include "../GameObject/ItemProjectiles.h"

#include <sstream>

LevelGenerationMap Scene::m_levelGenerationData = {};
char** Scene::m_levelMap = 0;

Scene::Scene()
{
}

Scene::Scene(std::string name) : m_sceneName(name)
{
    b_Rocks = true;
    b_Nets = false;
    b_Baits = false;
    b_Traps = false;

    f_RotateRock = 0.f;
    f_RotateNet = 0.f;
    f_RotateBait = 0.f;
    f_RotateTrap = 0.f;

    f_HighlightPos = -20.f;
}

Scene::~Scene()
{
}

void Scene::LoadLevelGenerationData(const char* file_path)
{
    LoadFile(file_path, FILE_LEVELGENERATIONDATA);
}

void Scene::LoadLevelMap(const char* file_path)
{
    LoadFile(file_path, FILE_LEVELMAPDATA);
}

void Scene::AddToMap(char tileCount, GraphicsLoader::GEOMETRY_TYPE meshType, const std::vector<COMPONENTS>& componentsList)
{
    std::pair<GraphicsLoader::GEOMETRY_TYPE, std::vector<COMPONENTS>> pair(meshType, componentsList);
    m_levelGenerationData.insert(std::pair<char, std::pair<GraphicsLoader::GEOMETRY_TYPE, std::vector<COMPONENTS>>>(tileCount, pair));
}

void Scene::RenderText(Mesh* mesh, std::string text, Color color)
{
	if (!mesh || mesh->textureID <= 0)
		return;

	glDisable(GL_DEPTH_TEST);
	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_TEXT_ENABLED), 1);
	glUniform3fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_TEXT_COLOR), 1, &color.r);
	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_LIGHTENABLED), 0);
	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_COLOR_TEXTURE_ENABLED), 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_COLOR_TEXTURE), 0);

	float translationOffset = 0.f;  // stores font width of each character
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 0.05f + translationOffset, 0, 0); //i * 0.1f is to let all the characters start from point of render; not point of //render middle of the text
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_MVP), 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);

		translationOffset += (float)(mesh->fontSize[text[i]] / 64.f);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_TEXT_ENABLED), 0);
	glEnable(GL_DEPTH_TEST);
}

void Scene::RenderTextCentralised(Mesh* mesh, std::string text, Color color)
{
	if (!mesh || mesh->textureID <= 0)
		return;

	glDisable(GL_DEPTH_TEST);
	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_TEXT_ENABLED), 1);
	glUniform3fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_TEXT_COLOR), 1, &color.r);
	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_LIGHTENABLED), 0);
	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_COLOR_TEXTURE_ENABLED), 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_COLOR_TEXTURE), 0);

	float halfOffset = 0.f;
	if (text != "") {
		for (unsigned i = 1; i < text.length() - 1; ++i)
		{
			halfOffset += (float)(mesh->fontSize[text[i]] / 64.f);
		}
	}

	halfOffset *= 0.5f;

	float translationOffset = 0.f;  // stores font width of each character
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(translationOffset - halfOffset, 0, 0); //i * 0.1f is to let all the characters start from point of render; not point of //render middle of the text
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_MVP), 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);

		translationOffset += (float)(mesh->fontSize[text[i]] / 64.f);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_TEXT_ENABLED), 0);
	glEnable(GL_DEPTH_TEST);
}

void Scene::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0)
		return;

	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10);
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity();
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	modelStack.Translate(x, y, 0);
	modelStack.Scale(size, size, size);
	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_TEXT_ENABLED), 1);
	glUniform3fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_TEXT_COLOR), 1, &color.r);
	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_LIGHTENABLED), 0);
	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_COLOR_TEXTURE_ENABLED), 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_COLOR_TEXTURE), 0);

	float translationOffset = 0.f;  // stores font width of each character
    for (unsigned i = 0; i < text.length(); ++i)
    {
        Mtx44 characterSpacing;
        characterSpacing.SetToTranslation(translationOffset + 0.5f, 0.5f, 0); //1.0f is the spacing of each character, you may change this value
        Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
        glUniformMatrix4fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_MVP), 1, GL_FALSE, &MVP.a[0]);

        mesh->Render((unsigned)text[i] * 6, 6);

        float offsetIncrease = 0.f;
        if (mesh->fontSize[text[i]] == 0) {   // there's no font data
            offsetIncrease = 1.f;
        }
        else {
            offsetIncrease = (float)(mesh->fontSize[text[i]] / 64.f) + 0.1f;
        }
        translationOffset += offsetIncrease;
    }
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_TEXT_ENABLED), 0);
	modelStack.PopMatrix();
	viewStack.PopMatrix();
	projectionStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void Scene::RenderMesh(Mesh *mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;

	//// rendering for light depth
	//if (SharedData::GetInstance()->graphicsLoader->GetRenderPass() == GraphicsLoader::RENDER_PASS_PRE)
	//{
	//    Mtx44 lightDepthMVP = m_lightDepthProj * m_lightDepthView * modelStack.Top();
	//    glUniformMatrix4fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_LIGHT_DEPTH_MVP_GPASS), 1, GL_FALSE, &lightDepthMVP.a[0]);
	//
	//    for (int i = 0; i < Mesh::MAX_TEXTURES; ++i)
	//    {
	//        if (mesh->textureArray[i] > 0)
	//        {
	//            glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(static_cast<GraphicsLoader::UNIFORM_TYPE>(GraphicsLoader::U_SHADOW_COLOR_TEXTURE_ENABLED + i)), 1);
	//            glActiveTexture(GL_TEXTURE0 + i);
	//            glBindTexture(GL_TEXTURE_2D, mesh->textureArray[i]);
	//            glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(static_cast<GraphicsLoader::UNIFORM_TYPE>(GraphicsLoader::U_SHADOW_COLOR_TEXTURE + i)), i);
	//        }
	//        else {
	//            glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(static_cast<GraphicsLoader::UNIFORM_TYPE>(GraphicsLoader::U_SHADOW_COLOR_TEXTURE_ENABLED + i)), 0);
	//        }
	//
	//    }
	//
	//    mesh->Render();
	//
	//    return;
	//}

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_MVP), 1, GL_FALSE, &MVP.a[0]);

	//...to here
	//have fog all the time, regardless of whether light is enabled
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_MODELVIEW), 1, GL_FALSE, &modelView.a[0]);

	if (enableLight)
	{
		glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_LIGHTENABLED), 1);

		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_MODELVIEW_INVERSE_TRANSPOSE), 1, GL_FALSE, &modelView.a[0]);

		//// Light shadows
		//Mtx44 lightDepthMVP = m_lightDepthProj * m_lightDepthView * modelStack.Top();
		//glUniformMatrix4fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_LIGHT_DEPTH_MVP), 1, GL_FALSE, &lightDepthMVP.a[0]);

		//load material
		glUniform3fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_MATERIAL_AMBIENT), 1, &mesh->material.kAmbient.r);
		glUniform3fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_MATERIAL_DIFFUSE), 1, &mesh->material.kDiffuse.r);
		glUniform3fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_MATERIAL_SPECULAR), 1, &mesh->material.kSpecular.r);
		glUniform1f(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_MATERIAL_SHININESS), mesh->material.kShininess);
	}
	else
	{
		glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_LIGHTENABLED), 0);
	}

	for (int i = 0; i < Mesh::MAX_TEXTURES; ++i) {
		if (mesh->textureArray[i] > 0) {
			glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(static_cast<GraphicsLoader::UNIFORM_TYPE>(GraphicsLoader::U_COLOR_TEXTURE_ENABLED + i)), 1);  //MUST BE IN SEQUENCE
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, mesh->textureArray[i]);
			glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(static_cast<GraphicsLoader::UNIFORM_TYPE>(GraphicsLoader::U_COLOR_TEXTURE + i)), i);
		}
		else {
			glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(static_cast<GraphicsLoader::UNIFORM_TYPE>(GraphicsLoader::U_COLOR_TEXTURE_ENABLED + i)), 0);
		}
	}

	mesh->Render();
}

void Scene::RenderMeshIn2D(Mesh *mesh, bool enableLight, float sizeX, float sizeY, float x, float y)
{
	viewStack.PushMatrix();
	viewStack.LoadIdentity();
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	modelStack.Translate(x, y, 0);
	modelStack.Scale(sizeX, sizeY, 1);

	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_MVP), 1, GL_FALSE, &MVP.a[0]);

	glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_LIGHTENABLED), 0);
	if (mesh->textureID > 0)
	{
		glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_COLOR_TEXTURE_ENABLED), 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_COLOR_TEXTURE), 0);
	}
	else
	{
		glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_COLOR_TEXTURE_ENABLED), 0);
	}
	mesh->Render();
	if (mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	//for (int i = 0; i < Mesh::MAX_TEXTURES; ++i) {
	//	if (mesh->textureArray[i] > 0) {
	//		glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(static_cast<GraphicsLoader::UNIFORM_TYPE>(GraphicsLoader::U_COLOR_TEXTURE_ENABLED + i)), 1);  //MUST BE IN SEQUENCE
	//		glActiveTexture(GL_TEXTURE0 + i);
	//		glBindTexture(GL_TEXTURE_2D, mesh->textureArray[i]);
	//		glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(static_cast<GraphicsLoader::UNIFORM_TYPE>(GraphicsLoader::U_COLOR_TEXTURE + i)), i);
	//	}
	//	else {
	//		glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(static_cast<GraphicsLoader::UNIFORM_TYPE>(GraphicsLoader::U_COLOR_TEXTURE_ENABLED + i)), 0);
	//	}
	//}
	//
	//mesh->Render();
	//if (mesh->textureArray[0] > 0)
	//{
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//}

	modelStack.PopMatrix();
	viewStack.PopMatrix();
}

void Scene::RenderUI(Mesh* mesh, float size, float x, float y, float rotatex, float rotatey, float rotatez, bool enableLight)
{
	//glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -100, 100); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(x, y, 97);
	modelStack.Rotate(rotatex, 1, 0, 0);
	modelStack.Rotate(rotatey, 0, 1, 0);
	modelStack.Rotate(rotatez, 0, 0, 1);
	modelStack.Scale(size, size, size);
	if (enableLight)
	{
		glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_LIGHTENABLED), 1);
	}
	else
	{
		glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_LIGHTENABLED), 0);
	}
	if (mesh->textureArray[0] > 0)
	{
		glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_COLOR_TEXTURE_ENABLED), 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureArray[0]);
		glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_COLOR_TEXTURE), 0);
	}
	else
	{
		glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_COLOR_TEXTURE_ENABLED), 0);
	}

	Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_MVP), 1, GL_FALSE, &MVP.a[0]);

	mesh->Render();

	glBindTexture(GL_TEXTURE_2D, 0);
	modelStack.PopMatrix();
	viewStack.PopMatrix();
	projectionStack.PopMatrix();
	//glEnable(GL_DEPTH_TEST);
}
//
//void Scene::RenderMeshIn2D(Mesh *mesh, bool enableLight, float size, float x, float y, bool rotate, float angle)
//{
//
//	viewStack.PushMatrix();
//	viewStack.LoadIdentity();
//	modelStack.PushMatrix();
//	modelStack.LoadIdentity();
//	modelStack.Translate(x, y, 0);
//	modelStack.Rotate(angle, 0, 0, 1);
//	modelStack.Scale(size, size, size);
//	//if (rotate)
//	//	modelStack.Rotate(rotateAngle, 0, 0, 1);
//
//
//	Mtx44 MVP, modelView, modelView_inverse_transpose;
//
//	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
//	glUniformMatrix4fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_MVP), 1, GL_FALSE, &MVP.a[0]);
//	if (mesh->textureID > 0)
//	{
//		glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_COLOR_TEXTURE_ENABLED), 1);
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
//		glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_COLOR_TEXTURE), 0);
//	}
//	else
//	{
//		glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_COLOR_TEXTURE_ENABLED), 0);
//	}
//	mesh->Render();
//	if (mesh->textureID > 0)
//	{
//		glBindTexture(GL_TEXTURE_2D, 0);
//	}
//
//	modelStack.PopMatrix();
//	viewStack.PopMatrix();
//
//}

bool Scene::SetHUD(const bool m_bHUDmode)
{
	if (m_bHUDmode)
	{
		if (m_bIsInOrthogonalMode == true)
			return false;

		Mtx44 ortho;
		ortho.SetToOrtho(-80, 80, -60, 60, -10, 10);
		projectionStack.PushMatrix();
		projectionStack.LoadMatrix(ortho);
		m_bIsInOrthogonalMode = true;
	}
	else
	{
		if (m_bIsInOrthogonalMode == false)
			return false;
		projectionStack.PopMatrix();
		m_bIsInOrthogonalMode = false;
	}
	return true;
}

#define RENDER_MASK (COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE)
void Scene::RenderGameObjects(World* world)
{
    for (unsigned GO = 0; GO < world->GAMEOBJECT_COUNT; ++GO)
    {
        if ((world->mask[GO] & RENDER_MASK) == RENDER_MASK)
        {
            Vector3 *pos;
            Mesh *mesh;
    
            pos = &(world->position[GO]);
            mesh = (world->appearance[GO].mesh);
    
            modelStack.PushMatrix();
            modelStack.Translate(world->position[GO].x, world->position[GO].y, world->position[GO].z);
            modelStack.Rotate(world->appearance[GO].angle, 0, 1, 0);
            modelStack.Scale(world->appearance[GO].scale.x, world->appearance[GO].scale.y, world->appearance[GO].scale.z);
			if (world->appearance[GO].billboard)
			{
				modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.x - world->position[GO].x, camera.position.z - world->position[GO].z)), 0, 1, 0);
				RenderMesh(world->appearance[GO].mesh, false);
			}
			else
			{
				if (world->velocity[GO].LengthSquared() > Math::EPSILON)
					modelStack.Rotate(Math::RadianToDegree(atan2(world->velocity[GO].x, world->velocity[GO].z)), 0, 1, 0);

				RenderMesh(world->appearance[GO].mesh, true);
			}
			modelStack.PopMatrix();
        }

        if (m_sceneName != "Zoo" && SharedData::GetInstance()->sceneManager->GetGameState() == SceneManager::GAMESTATE_GAMEPLAY && world->monster[GO])
        {
            Vector3 pos;

            pos = (world->position[GO]);

            // Render HP and capture rate
			float hpBar = world->monster[GO]->GetHealthStat() / 80.f;
			float captureBar = world->monster[GO]->GetCaptureRateStat() / 100.f;

			modelStack.PushMatrix();
			modelStack.Translate(pos.x, pos.y + 1.5f + 2.f * world->appearance[GO].scale.y, pos.z);
			modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.x - pos.x, camera.position.z - pos.z)), 0, 1, 0);
			//modelStack.Translate(0.5f * hpBar, 0.f, 0.f);
			modelStack.Scale(hpBar, 0.2, 1);
			RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_HPBAR), false);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(pos.x, pos.y + 2.5f + 2.f * world->appearance[GO].scale.y, pos.z);
			modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.x - pos.x, camera.position.z - pos.z)), 0, 1, 0);
			//modelStack.Translate(0.5f * hpBar, 0.f, 0.f);
			modelStack.Scale(captureBar, captureBar, captureBar);
			RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_CAPTUREBAR), false);
			modelStack.PopMatrix();

            switch (world->monster[GO]->GetStrategyState())
            {
            case AI_Strategy::STATE_ALERT:

                modelStack.PushMatrix();
                modelStack.Translate(pos.x, pos.y + 5.f + 2.f * world->appearance[GO].scale.y, pos.z);
                modelStack.Scale(4, 4, 4);
                modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.x - pos.x, camera.position.z - pos.z)), 0, 1, 0);
                RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTERSTATE_QUESTION_MARK), false);
                modelStack.PopMatrix();
                break;

            case AI_Strategy::STATE_ATTACK:
            case AI_Strategy::STATE_RUN:

                modelStack.PushMatrix();
                modelStack.Translate(pos.x, pos.y + 5.f + 2.f * world->appearance[GO].scale.y, pos.z);
                modelStack.Scale(4, 4, 4);
                modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.x - pos.x, camera.position.z - pos.z)), 0, 1, 0);
                RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTERSTATE_EXCLAMATION_MARK), false);
                modelStack.PopMatrix();
                break;

            case AI_Strategy::STATE_BAITED:

                modelStack.PushMatrix();
                modelStack.Translate(pos.x, pos.y + 5.f + 2.f * world->appearance[GO].scale.y, pos.z);
                modelStack.Scale(2, 2, 2);
                modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.x - pos.x, camera.position.z - pos.z)), 0, 1, 0);
                RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BAIT), false);
                modelStack.PopMatrix();
                break;

			case AI_Strategy::STATE_RAMPAGE:

				modelStack.PushMatrix();
				modelStack.Translate(pos.x, pos.y + 5.f + 2.f * world->appearance[GO].scale.y, pos.z);
				modelStack.Scale(2, 2, 2);
				modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.x - pos.x, camera.position.z - pos.z)), 0, 1, 0);
				RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTERSTATE_RAMPAGE), false);
				modelStack.PopMatrix();
				break;
            }
        }
    }
}

#define MOVEMENT_MASK (COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY)
void Scene::UpdateGameObjects(World* world, double dt)
{
    for (unsigned GO = 0; GO < world->GAMEOBJECT_COUNT; ++GO)
    {
        Vector3 *pos;
        pos = &(world->position[GO]);

        Vector3 *vel;
        vel = &(world->velocity[GO]);

        if ((world->mask[GO] & MOVEMENT_MASK) == MOVEMENT_MASK)
        {
            pos->x += vel->x * (float)dt;
            pos->y += vel->y * (float)dt;
            pos->z += vel->z * (float)dt;
            //std::cout << "asd" << std::endl;
        }

        if ((world->mask[GO] & COMPONENT_HITBOX) == COMPONENT_HITBOX)
        {
            Vector3 *origin;
            origin = &(world->hitbox[GO].m_origin);

            //origin->x = pos->x;
            //origin->y = pos->y;
            //origin->z = pos->z;
            origin->x += vel->x * (float)dt;
            origin->y += vel->y * (float)dt;
            origin->z += vel->z * (float)dt;
        }

    }
}

void Scene::RenderUI(Mesh* mesh, float sizex, float sizey, float sizez, float x, float y, float rotatex, float rotatey, float rotatez, bool enableLight)
{
    glDisable(GL_DEPTH_TEST);
    Mtx44 ortho;
    ortho.SetToOrtho(0, 80, 0, 60, -100, 100); //size of screen UI
    projectionStack.PushMatrix();
    projectionStack.LoadMatrix(ortho);
    viewStack.PushMatrix();
    viewStack.LoadIdentity(); //No need camera for ortho mode
    modelStack.PushMatrix();
    modelStack.LoadIdentity(); //Reset modelStack
    modelStack.Translate(x, y, 97);
    modelStack.Rotate(rotatex, 1, 0, 0);
    modelStack.Rotate(rotatey, 0, 1, 0);
    modelStack.Rotate(rotatez, 0, 0, 1);
    modelStack.Scale(sizex, sizey, sizez);
    if (enableLight)
    {
        glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_LIGHTENABLED), 1);
    }
    else
    {
        glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_LIGHTENABLED), 0);
    }
    if (mesh->textureArray[0] > 0)
    {
        glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_COLOR_TEXTURE_ENABLED), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh->textureArray[0]);
        glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_COLOR_TEXTURE), 0);
    }
    else
    {
        glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_COLOR_TEXTURE_ENABLED), 0);
    }

    Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
    glUniformMatrix4fv(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_MVP), 1, GL_FALSE, &MVP.a[0]);

    mesh->Render();

    glBindTexture(GL_TEXTURE_2D, 0);
    projectionStack.PopMatrix();
    viewStack.PopMatrix();
    modelStack.PopMatrix();
    glEnable(GL_DEPTH_TEST);
}

void Scene::UpdatePlayer(double dt, World *world)
{
    SharedData::GetInstance()->inputManager->Update();
    SharedData::GetInstance()->player->Update(dt);

    Vector3 playerPos = SharedData::GetInstance()->player->GetPositionVector();

    if (m_sceneName == "Swamp")
    {
        if (30.f * ReadHeightMap(SharedData::GetInstance()->graphicsLoader->m_heightMapSwamp, (playerPos.x - 100.f) / 300.f, (playerPos.z - 100.f) / 300.f) < 5.f && !SharedData::GetInstance()->player->IsJumping()) {
            std::cout << "ON MUD!! ";
            SceneEnvironmentEffect();
        }
    }
    else if (m_sceneName == "Lava")
    {
        if (30.f * ReadHeightMap(SharedData::GetInstance()->graphicsLoader->m_heightMapSwamp, (playerPos.x - 100.f) / 300.f, (playerPos.z - 100.f) / 300.f) < 5.f && !SharedData::GetInstance()->player->IsJumping()) {
            std::cout << "ON LAVA!! ";
            SceneEnvironmentEffect();
        }
    }

    bool collision = false;
    playerPos += SharedData::GetInstance()->player->GetVelocityVector();
    playerPos.y = 0.f;
    for (GameObject obstacle = 0; obstacle < world->GAMEOBJECT_COUNT; ++obstacle)
    {
        if ((world->mask[obstacle] & COMPONENT_OBSTACLE) == COMPONENT_OBSTACLE)
        {
            if (world->hitbox[obstacle].CheckCollision(playerPos))
            {
                collision = true;
                break;
            }
        }
    }
    if (!collision) {
        SharedData::GetInstance()->player->Move(dt);
    }
    SharedData::GetInstance()->player->UpdateNoiseFactor();
}

void Scene::UpdateMonsters(double dt, World *world)
{
    for (GameObject ai = 0; ai < world->GAMEOBJECT_COUNT; ++ai)
    {
        if (world->monster[ai])
        {
            world->monster[ai]->m_position = world->position[ai];
            world->monster[ai]->Update(dt);

            if (world->monster[ai]->GetStrategyState() == AI_Strategy::STATE_ALERT)
            {
                world->monster[ai]->m_velocity.SetZero();
            }

            world->velocity[ai] = world->monster[ai]->m_velocity;
        }
    }
}

void Scene::DestroyText(World *world)
{
    for (GameObject text = 0; text < world->GAMEOBJECT_COUNT; ++text)
    {
        if ((world->mask[text] & COMPONENT_TEXT) == COMPONENT_TEXT)
        {
            if (world->position[text].y > 9.f)
            {
                destroyGO(world, text);
            }
        }

    }
}

void Scene::UpdateRockProjectiles(World *world)
{
    if (ItemProjectile::RockProjectileList.size())
    {
        for (unsigned i = 0; i < ItemProjectile::RockProjectileList.size(); ++i)
        {
            for (GameObject ai = 0; ai < world->GAMEOBJECT_COUNT; ++ai)
            {
                if ((world->mask[ai] & COMPONENT_AI) == COMPONENT_AI)
                {
                    if (world->hitbox[ai].CheckCollision(ItemProjectile::RockProjectileList[i]->position))
                    {
                        // Get hit by rock
                        //grass.velocity[ai] = (grass.position[ai] - camera.position).Normalized();
                        //grass.velocity[ai].y = 0;
                        ItemProjectile::RockProjectileList[i]->deleteBullet = true;
                        if (world->monster[ai]->GetStrategyState() != AI_Strategy::STATE_CAPTURED) {
                            world->monster[ai]->TakeDamage(SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].GetEffectiveness());
                            std::cout << "HEALTH: " << world->monster[ai]->GetHealthStat() << std::endl;
                            SharedData::GetInstance()->sound->PlaySoundEffect3D("Sound//Hit.mp3",
                                irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
                                irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
                                irrklang::vec3df(world->position[ai].x, world->position[ai].y, world->position[ai].z));

                            if (world->monster[ai]->GetHealthStat() == 0)
                            {
                                GameObject meat = createGO(world);
                                world->mask[meat] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_MEAT;
                                world->position[meat] = world->monster[ai]->m_position;
                                world->appearance[meat].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MEAT);
                                world->appearance[meat].scale.Set(1, 1, 1);
                                world->appearance[meat].angle = 0.f;
                                world->appearance[meat].billboard = false;

                                destroyGO(world, ai);
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
}

void Scene::UpdateNetProjectiles(World *world)
{
    if (ItemProjectile::NetProjectileList.size())
    {
        for (unsigned i = 0; i < ItemProjectile::NetProjectileList.size(); ++i)
        {
            for (GameObject ai = 0; ai < world->GAMEOBJECT_COUNT; ++ai)
            {
                if ((world->mask[ai] & COMPONENT_AI) == COMPONENT_AI && world->hitbox[ai].CheckCollision(ItemProjectile::NetProjectileList[i]->position)
                    && world->monster[ai]->GetStrategyState() != AI_Strategy::STATE_CAPTURED)
                {
                    ItemProjectile::NetProjectileList[i]->deleteBullet = true;
                    GameObject net = createGO(world);
                    world->mask[net] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_CAPTURE;
                    world->capture[net].capturingMonster = true;
                    world->capture[net].capturedMonster = false;
                    world->capture[net].timeBeforeCapture = 3;
                    world->appearance[net].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET);
                    //world->appearance[net].scale.Set(2, 2, 2);
                    world->appearance[net].scale = world->appearance[ai].scale;
                    world->appearance[net].angle = 0.f;
                    world->appearance[net].billboard = false;

                    if (world->capture[net].capturingMonster == true)
                    {
                        world->position[net] = world->position[ai];
                        int v1 = rand() % 100;
                        std::cout << v1 << std::endl;
                        if (v1 < world->monster[ai]->GetCaptureRateStat())
                        {
                            world->capture[net].capturedMonster = true;//Captured!
                            GameObject text_C = createGO(world);
                            world->mask[text_C] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_VELOCITY | COMPONENT_TEXT;
                            world->appearance[text_C].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_CAPTURE);
                            world->position[text_C] = world->position[net];
                            world->position[text_C].y += world->appearance[ai].scale.y;
                            world->velocity[text_C] = (world->position[text_C] - camera.position).Normalized();
                            world->velocity[text_C].y = 3;
                            world->appearance[text_C].scale.Set(2, 2, 2);
                            world->appearance[text_C].billboard = true;
                            world->appearance[text_C].angle = 0.f;
                        }
                        else
                        {
                            std::cout << "HAHAHAHAHHA FAILED CAPTURED!" << std::endl;
                            GameObject text_M = createGO(world);
                            world->mask[text_M] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_VELOCITY | COMPONENT_TEXT;
                            world->appearance[text_M].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_MISS);
                            world->position[text_M] = world->position[net];
                            world->position[text_M].y += world->appearance[ai].scale.y;
                            world->velocity[text_M] = (world->position[text_M] - camera.position).Normalized();
                            world->velocity[text_M].y = 3;
                            world->appearance[text_M].scale.Set(2, 2, 2);
                            world->appearance[text_M].billboard = true;
                            world->appearance[text_M].angle = 0.f;
                            SharedData::GetInstance()->sound->PlaySoundEffect3D("Sound//Escaped.wav",
                                irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
                                irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
                                irrklang::vec3df(world->position[ai].x, world->position[ai].y, world->position[ai].z));
                            destroyGO(world, net);
                            world->capture[net].capturingMonster = false;//fail capture

                        }
                    }
                    if (world->capture[net].capturedMonster == true)
                    {
                        std::cout << "HAHAHAHAHHA CAPTURED!" << std::endl;
                        world->velocity[ai].SetZero();
                        world->monster[ai]->m_velocity = world->velocity[ai];
                        world->monster[ai]->GetCaptured();
                        world->position[net] = world->position[ai];
                        world->capture[net].caughtMonster = ai;
                        SharedData::GetInstance()->sound->PlaySoundEffect3D("Sound//Netted.wav",
                            irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
                            irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
                            irrklang::vec3df(world->position[ai].x, world->position[ai].y, world->position[ai].z));
                    }
                    break;
                }
            }
        }
    }
}

void Scene::UpdateBaitProjectiles(World *world)
{
    if (ItemProjectile::BaitProjectileList.size())
    {
        for (unsigned i = 0; i < ItemProjectile::BaitProjectileList.size(); ++i)
        {
            if (ItemProjectile::BaitProjectileList[i]->position.y <= 1)
            {
                Vector3 tempo = ItemProjectile::BaitProjectileList[i]->position;
                ItemProjectile::BaitProjectileList[i]->deleteBullet = true;

                GameObject bait = createGO(world);
                world->mask[bait] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_BAIT;
                world->position[bait] = tempo;
                world->appearance[bait].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BAIT);
                world->appearance[bait].scale.Set(1, 1, 1);
                world->appearance[bait].billboard = false;
                world->bait[bait].scentRadius = 200;
                world->bait[bait].foundRadius = 5;
                world->bait[bait].eattingBait = false;
                world->bait[bait].foundBait = false;
                world->bait[bait].timeEatting = 3.0f;
                world->bait[bait].b_PlayBaitSoundOnes = false;
            }
        }
    }
}

void Scene::UpdateBait(World *world, double dt)
{
    for (GameObject bait = 0; bait < world->GAMEOBJECT_COUNT; ++bait)
    {
        if ((world->mask[bait] & COMPONENT_BAIT) == COMPONENT_BAIT)
        {

            for (GameObject ai = 0; ai < world->GAMEOBJECT_COUNT; ++ai)
            {
                if ((world->mask[ai] & COMPONENT_AI) == COMPONENT_AI && world->monster[ai])
                {
                    if ((world->monster[ai]->GetStrategyState() == AI_Strategy::STATE_IDLE || world->monster[ai]->GetStrategyState() == AI_Strategy::STATE_ALERT) && (world->position[ai] - world->position[bait]).LengthSquared() < world->bait[bait].scentRadius && world->bait[bait].eattingBait == false)
                    {
                        world->bait[bait].foundBait = true;
                        //std::cout << " FOUND BAIT " + grass.bait[bait].foundBait << std::endl;
                        if (world->bait[bait].foundBait == true)
                        {
                            //world->velocity[ai] = (grass.position[bait] - grass.position[ai]).Normalized();
                            //world->velocity[ai].y = 0;
                            //world->monster[ai]->m_velocity = grass.velocity[ai];
                            if (world->monster[ai]->GetStrategyState() == AI_Strategy::STATE_ALERT || world->monster[ai]->GetStrategyState() == AI_Strategy::STATE_IDLE || world->monster[ai]->GetStrategyState() == AI_Strategy::STATE_BAITED)
                            {
                                world->monster[ai]->GetBaited(world->position[bait]);
                                world->bait[bait].baitedMonsters.push_back(ai);
                            }
                        }
                    }
                    if ((world->monster[ai]->GetStrategyState() == AI_Strategy::STATE_BAITED) && (world->position[ai] - world->position[bait]).LengthSquared() < world->bait[bait].foundRadius)
                    {
                        world->bait[bait].foundBait = false;
                        world->bait[bait].eattingBait = true;
                        world->velocity[ai].SetZero();
                        world->monster[ai]->m_velocity.SetZero();
                    }
                    if (world->monster[ai]->GetStrategyState() == AI_Strategy::STATE_BAITED && world->bait[bait].eattingBait == true
                        && (world->position[ai] - world->position[bait]).LengthSquared() < world->bait[bait].foundRadius)
                    {
                        world->bait[bait].timeEatting -= (float)(dt);
                        if (world->bait[bait].timeEatting > 0.1f) {
                            float scale = world->bait[bait].timeEatting / 3.f;
                            world->appearance[bait].scale.Set(scale, scale, scale);
                        }
                        if (!world->bait[bait].b_PlayBaitSoundOnes)
                        {
                            SharedData::GetInstance()->sound->PlaySoundEffect3D("Sound//Eat1.wav",
                                irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
                                irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
                                irrklang::vec3df(world->position[bait].x, world->position[bait].y, world->position[bait].z), true);
                            world->bait[bait].b_PlayBaitSoundOnes = true;
                        }
                    }

                    if (world->bait[bait].timeEatting <= 0)
                    {
                        //world->bait[bait].eattingBait = false;
                        world->bait[bait].finishedBait = true;
                        //world->velocity[ai] = 2;
                        //world->velocity[ai].y = 0;
                        SharedData::GetInstance()->sound->StopSoundEffect3D();
                        world->bait[bait].b_PlayBaitSoundOnes = false;
                    }
                }
            }

            //for (std::vector<GameObject>::iterator it = world->bait[bait].baitedMonsters.begin(); it != world->bait[bait].baitedMonsters.end(); )
            //{
            //    if (world->monster[*it]->GetStrategyState() != AI_Strategy::STATE_BAITED)
            //    {
            //        it = world->bait[bait].baitedMonsters.erase(it);
            //    }
            //    else
            //    {
            //        ++it;
            //    }
            //}

            if (world->bait[bait].finishedBait == true) {
                //PUT WHATEVER THE RABBIT DO NORMALLY HERE :D (DONE EATTING BAIT)
                for (unsigned i = 0; i < world->bait[bait].baitedMonsters.size(); ++i) {
                    GameObject ai = world->bait[bait].baitedMonsters[i];
                    if (world->monster[ai] && world->monster[ai]->GetStrategyState() == AI_Strategy::STATE_BAITED) {
                        world->monster[ai]->SetIdleState();
                        world->velocity[ai] = world->monster[ai]->m_velocity;
                    }
                }
                destroyGO(world, bait);
            }
        }
    }
}

void Scene::UpdateTrap(World *world, double dt)
{
    for (GameObject trap = 0; trap < world->GAMEOBJECT_COUNT; ++trap)
    {
        if ((world->mask[trap] & COMPONENT_TRAP) == COMPONENT_TRAP)
        {
            if (!world->trap[trap].activated)
            {
                for (GameObject ai = 0; ai < world->GAMEOBJECT_COUNT; ++ai)
                {
                    if ((world->mask[ai] & COMPONENT_AI) == COMPONENT_AI && world->monster[ai] && world->monster[ai]->GetStrategyState() != AI_Strategy::STATE_TRAPPED
                        && world->monster[ai]->GetStrategyState() != AI_Strategy::STATE_CAPTURED)
                    {
                        if ((world->position[trap] - world->position[ai]).LengthSquared() < world->trap[trap].radius)
                        {
                            world->trap[trap].caughtMonsterVel = world->velocity[ai];
                            world->trap[trap].caughtMonster = ai;
                            world->trap[trap].activated = true;
                            //grass.velocity[ai].SetZero();
                            if (world->velocity[ai].LengthSquared() > Math::EPSILON) {
                                world->velocity[ai] = world->velocity[ai].Normalized() * 0.01f;
                                world->monster[ai]->m_velocity = world->velocity[ai];
                                world->monster[ai]->GetTrapped();
                            }
                            SharedData::GetInstance()->sound->PlaySoundEffect3D("Sound//TrapActive.wav",
                                irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
                                irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
                                irrklang::vec3df(world->position[ai].x, world->position[ai].y, world->position[ai].z));
                        }
                    }
                }
            }
            else
            {
                if (world->trap[trap].triggerTimer < world->trap[trap].triggerDuration)
                {
                    world->trap[trap].triggerTimer += dt;
                }
                else
                {
                    if (world->monster[world->trap[trap].caughtMonster]->GetStrategyState() != AI_Strategy::STATE_CAPTURED) {
                        world->velocity[world->trap[trap].caughtMonster] = world->trap[trap].caughtMonsterVel;
                        world->monster[world->trap[trap].caughtMonster]->m_velocity = world->trap[trap].caughtMonsterVel;
                        world->monster[world->trap[trap].caughtMonster]->SetIdleState();
                    }
                    destroyGO(world, trap);
                }
            }
        }
    }
}

void Scene::CheckMonsterAttack(World *world)
{
    for (GameObject ai = 0; ai < world->GAMEOBJECT_COUNT; ++ai)
    {
        if ((world->mask[ai] & COMPONENT_AI) == COMPONENT_AI && world->monster[ai] && world->monster[ai]->GetStrategyState() == AI_Strategy::STATE_ATTACK)
        {
            //if ((grass.position[ai] - SharedData::GetInstance()->player->GetPositionVector()).LengthSquared() <
            //    0.25f * (SharedData::GetInstance()->player->PlayerHitBox.m_scale.x + grass.hitbox[ai].m_scale.x) * (SharedData::GetInstance()->player->PlayerHitBox.m_scale.z + grass.hitbox[ai].m_scale.z))
            if (world->hitbox[ai].CheckCollision(SharedData::GetInstance()->player->PlayerHitBox))
            {
                std::cout << "Distance:" << (world->hitbox[ai].m_origin - SharedData::GetInstance()->player->PlayerHitBox.m_origin).Length() << std::endl;
                std::cout << std::endl;

                world->monster[ai]->AttackPlayer();
                world->monster[ai]->ResetAggression();
                world->monster[ai]->SetIdleState();
            }

        }

    }
}

void Scene::ShootRock()
{
    if (ItemProjectile::d_rockCounter > ItemProjectile::d_rockCooldown)
    {
        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed && SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].Use())
        {
            ItemProjectile::RockProjectileList.push_back(new ItemProjectile(
                Vector3(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z),
                Vector3(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
                500,
                50,
                10
                ));
            SharedData::GetInstance()->sound->PlaySoundEffect3D("Sound//Throwing.wav",
                irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
                irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
                irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z));

            ItemProjectile::d_rockCounter = 0.0;
        }
    }
}

void Scene::ShootNet()
{
    if (ItemProjectile::d_netCounter > ItemProjectile::d_netCooldown)
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
            SharedData::GetInstance()->sound->PlaySoundEffect3D("Sound//ThrowingNet.wav",
                irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
                irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
                irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z));

            ItemProjectile::d_netCounter = 0.0;
        }
    }
}

void Scene::ShootBait()
{
    if (ItemProjectile::d_baitCounter > ItemProjectile::d_baitCooldown)
    {
        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed && SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].Use())
        {
            ItemProjectile::BaitProjectileList.push_back(new ItemProjectile(
                Vector3(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z),
                Vector3(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
                500,
                50,
                10
                ));
            SharedData::GetInstance()->sound->PlaySoundEffect3D("Sound//Throwing.wav",
                irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
                irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
                irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z));

            ItemProjectile::d_baitCounter = 0.0;
        }
    }
}

void Scene::PlaceTrap(World *world)
{
    if (ItemProjectile::d_trapCounter > ItemProjectile::d_trapCooldown)
    {
        if (SharedData::GetInstance()->inputManager->keyState[InputManager::MOUSE_L].isPressed && SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP].Use())
        {
            GameObject trap = createGO(world);

            world->mask[trap] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_TRAP;

            world->position[trap].Set(SharedData::GetInstance()->player->GetPositionVector().x + SharedData::GetInstance()->player->GetViewVector().x * 20, 0, SharedData::GetInstance()->player->GetPositionVector().z + SharedData::GetInstance()->player->GetViewVector().z * 20);
            world->appearance[trap].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TRAP);
            world->appearance[trap].scale.Set(3, 3, 3);
            world->appearance[trap].angle = 0.f;

            world->trap[trap].triggerDuration = 3.0f;
            world->trap[trap].triggerTimer = 0.f;
            world->trap[trap].radius = 2.5f;
            world->trap[trap].activated = false;
            world->trap[trap].caughtMonster = 0;
            world->trap[trap].caughtMonsterVel.SetZero();

            SharedData::GetInstance()->sound->PlaySoundEffect3D("Sound//SetTrap.wav",
                irrklang::vec3df(SharedData::GetInstance()->player->GetPositionVector().x, SharedData::GetInstance()->player->GetPositionVector().y, SharedData::GetInstance()->player->GetPositionVector().z),
                irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
                irrklang::vec3df(world->position[trap].x, world->position[trap].y, world->position[trap].z));

            ItemProjectile::d_trapCounter = 0.0;
        }
    }
}

void Scene::UpdateInventory()
{
    //Rocks
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_1].isPressed)
    {
        b_Rocks = true;
        b_Baits = false;
        b_Nets = false;
        b_Traps = false;
        f_HighlightPos = -20.f;
    }
    //Nets
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_2].isPressed)
    {
        b_Nets = true;
        b_Rocks = false;
        b_Baits = false;
        b_Traps = false;
        f_HighlightPos = -10.f;
    }
    //Baits
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_3].isPressed)
    {
        b_Baits = true;
        b_Rocks = false;
        b_Nets = false;
        b_Traps = false;

        f_HighlightPos = 0.f;
    }
    //Traps
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_4].isPressed)
    {
        b_Baits = false;
        b_Rocks = false;
        b_Nets = false;
        b_Traps = true;

        f_HighlightPos = 10.f;
    }
}

void Scene::UpdateParticles(World *world, double dt)
{
    if (SharedData::GetInstance()->particleManager->d_timeCounter > 2.0)
    {
        SpawnSceneParticles();
        for (GameObject GO = 0; GO < world->GAMEOBJECT_COUNT; ++GO)
        {
            if ((world->mask[GO] & COMPONENT_MONEYTREE) == COMPONENT_MONEYTREE)
            {
                SharedData::GetInstance()->particleManager->SpawnParticle(world->position[GO], ParticleObject::P_HIDDENBONUS);
            }
        }

        SharedData::GetInstance()->particleManager->d_timeCounter = 0.0;
    }
    SharedData::GetInstance()->particleManager->Update(dt);
}

bool Scene::CheckPickUpCaughtMonster(World *world, GameObject GO)
{
    GameObject ai = world->capture[GO].caughtMonster;
    if ((camera.position - world->position[ai]).LengthSquared() < 150)
    {
        if (ViewCheckPosition(world->position[ai], 45.f) == true)
        {
            std::cout << "CAUGHT THE MONSTER" << std::endl;
            SharedData::GetInstance()->player->monsterList.push_back(world->monster[ai]->GetName());
            SharedData::GetInstance()->sound->PlaySoundEffect3D("Sound//Captured.mp3",
                irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
                irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
                irrklang::vec3df(world->position[ai].x, world->position[ai].y, world->position[ai].z));
            destroyGO(world, ai);
            // destroy net
            destroyGO(world, GO);

            return true;
        }
    }

    return false;
}

bool Scene::CheckInteractMoneyTree(World *world, GameObject GO)
{
    if ((camera.position - world->position[GO]).LengthSquared() < 150)
    {
        if (ViewCheckPosition(world->position[GO], 45.f) == true)
        {
            std::cout << "MoneyTree Found" << std::endl;
            world->mask[GO] = COMPONENT_DISPLACEMENT | COMPONENT_APPEARANCE | COMPONENT_HITBOX | COMPONENT_COIN;
            world->appearance[GO].mesh = SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_COIN);
            world->appearance[GO].scale.Set(5, 5, 5);

            return true;
        }
    }

    return false;
}

bool Scene::CheckPickUpCoin(World *world, GameObject GO)
{
    if ((camera.position - world->position[GO]).LengthSquared() < 150)
    {
        if (ViewCheckPosition(world->position[GO], 45.f) == true)
        {
            std::cout << "coin picked up" << std::endl;
            SharedData::GetInstance()->sound->PlaySoundEffect3D("Sound//PickUp.wav",
                irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
                irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
                irrklang::vec3df(world->position[GO].x, world->position[GO].y, world->position[GO].z));
            destroyGO(world, GO);

            return true;
        }
    }

    return false;
}

bool Scene::CheckPickUpMeat(World *world, GameObject GO)
{
    if ((camera.position - world->position[GO]).LengthSquared() < 150)
    {
        if (ViewCheckPosition(world->position[GO], 45.f) == true)
        {
            SharedData::GetInstance()->player->inventory[Item::TYPE_MEAT].Add(1);
            std::cout << "meat picked up" << std::endl;
            SharedData::GetInstance()->sound->PlaySoundEffect3D("Sound//PickUp.wav",
                irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
                irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
                irrklang::vec3df(world->position[GO].x, world->position[GO].y, world->position[GO].z));
            destroyGO(world, GO);

            return true;
        }
    }

    return false;
}

bool Scene::CheckPickUpRock(World *world, GameObject GO)
{
    if ((camera.position - world->position[GO]).LengthSquared() < 150)
    {
        if (ViewCheckPosition(world->position[GO], 45.f) == true)
        {
            std::cout << "rocks picked up" << std::endl;
			SharedData::GetInstance()->sound->PlaySoundEffect3D("Sound//PickUp.wav",
				irrklang::vec3df(camera.position.x, camera.position.y, camera.position.z),
				irrklang::vec3df(SharedData::GetInstance()->player->GetViewVector().x, SharedData::GetInstance()->player->GetViewVector().y, SharedData::GetInstance()->player->GetViewVector().z),
				irrklang::vec3df(world->position[GO].x, world->position[GO].y, world->position[GO].z));
            destroyGO(world, GO);
            SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].Add(1);
            // insert pick up rock sound

            return true;
        }
    }

    return false;
}

bool Scene::ViewCheckPosition(Vector3 pos, float degree)
{
    if (pos != SharedData::GetInstance()->player->GetPositionVector())
    {
        Vector3 view = (pos - SharedData::GetInstance()->player->GetPositionVector()).Normalized();

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

    return true;
}

void Scene::RenderProjectiles()
{
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
}

void Scene::RenderPressEText(World *world)
{
    for (GameObject GO = 0; GO < world->GAMEOBJECT_COUNT; ++GO)
    {
        if ((world->mask[GO] & COMPONENT_MONEYTREE) == COMPONENT_MONEYTREE)
        {
            if ((camera.position - world->position[GO]).LengthSquared() < 150)
            {
                if (ViewCheckPosition(world->position[GO], 45.f) == true)
                {
                    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "PRESS E TO CUT DOWN", Color(1, 1, 0), 3, 30, 30);
    
                }
            }
        }
        else if ((world->mask[GO] & COMPONENT_COIN) == COMPONENT_COIN)
        {
            if ((camera.position - world->position[GO]).LengthSquared() < 150)
            {
                if (ViewCheckPosition(world->position[GO], 45.f) == true)
                {
                    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "PRESS E TO COLLECT", Color(1, 1, 0), 3, 30, 30);
                }
            }
        }
        else if ((world->mask[GO] & COMPONENT_CAPTURE) == COMPONENT_CAPTURE)   // net
        {
            GameObject ai = world->capture[GO].caughtMonster;
            if ((camera.position - world->position[ai]).LengthSquared() < 150)
            {
                if (ViewCheckPosition(world->position[ai], 45.f) == true)
                {
                    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "PRESS E TO COLLECT", Color(1, 1, 0), 3, 30, 30);
                }
            }
        }
        else if ((world->mask[GO] & COMPONENT_MEAT) == COMPONENT_MEAT)   // net
        {
            if ((camera.position - world->position[GO]).LengthSquared() < 150)
            {
                if (ViewCheckPosition(world->position[GO], 45.f) == true)
                {
                    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "PRESS E TO COLLECT", Color(1, 1, 0), 3, 30, 30);
                }
            }
        }
        else if ((world->mask[GO] & COMPONENT_ROCKS) == COMPONENT_ROCKS)
        {
            if ((camera.position - world->position[GO]).LengthSquared() < 150)
            {
                if (ViewCheckPosition(world->position[GO], 45.f) == true)
                {
                    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), "PRESS E TO COLLECT", Color(1, 1, 0), 3, 30, 30);
                }
            }
        }
    
    }
    
}

void Scene::RenderHUD(World *world)
{
    glUniform1i(SharedData::GetInstance()->graphicsLoader->GetParameters(GraphicsLoader::U_FOG_ENABLED), false);

    SetHUD(true);

    std::stringstream ss;
    ss << "FPS: " << fps;
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3, 0, 3);

    ss.str("");
    ss << "Noise factor:" << SharedData::GetInstance()->player->GetNoiseFactor();
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3, 0, 6);

    ss.str("");
    ss << "PLAYER HEALTH:" << SharedData::GetInstance()->player->GetHealth();
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss.str(), Color(1, 1, 0), 3, 0, 9);

    RenderPressEText(world);

    std::stringstream ss1, ss2, ss3, ss4, ss5;
    ss1 << "Rocks: " << SharedData::GetInstance()->player->inventory[Item::TYPE_ROCK].GetCount();
    ss2 << "Nets: " << SharedData::GetInstance()->player->inventory[Item::TYPE_NET].GetCount();
    ss3 << "Baits: " << SharedData::GetInstance()->player->inventory[Item::TYPE_BAIT].GetCount();
    ss4 << "Traps: " << SharedData::GetInstance()->player->inventory[Item::TYPE_TRAP].GetCount();
    ss5 << "Meat: " << SharedData::GetInstance()->player->inventory[Item::TYPE_MEAT].GetCount();

    RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_HUDHIGHLIGHT), false, 11.f, 12.f, f_HighlightPos, -48);
    RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_HUD), false, 50.f, 12.f, 0, -48);

    // background
    RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BOX_TRANSLUCENT), false, 50.f, 12.f, 0, -48);

    // 1: Rock
    if (b_Rocks) {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCKS1), 2.5f, 30.f, 6.5f, f_RotateRock, f_RotateRock, 0, false);
		RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss1.str(), Color(1, 1, 0), 1, 28.f, 3.5f);
    }
    else 
	{
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_ROCKS1), 2, 30.f, 6.5f, f_RotateRock, f_RotateRock, 0, false);
		RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss1.str(), Color(1, 1, 0), 1, 28.f, 3.5f);
    }
	
    // 2: Net
    if (b_Nets) {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET), 1.25f, 35.f, 5.5f, 0, f_RotateNet, 0, false);
		RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss2.str(), Color(1, 1, 0), 1, 33.f, 3.5f);
    }
    else {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_NET), 1, 35.f, 5.5f, 0, f_RotateNet, 0, false);
		RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss2.str(), Color(1, 1, 0), 1, 33.f, 3.5f);
    }
    
    // 3: Bait
    if (b_Baits) {
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BAIT), 2.5f, 40.f, 6.5f, f_RotateBait, f_RotateBait, 0, false);
		RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss3.str(), Color(1, 1, 0), 1, 38.f, 3.5f);
    }
    else 
	{
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BAIT), 2, 40.f, 6.5f, f_RotateBait, f_RotateBait, 0, false);
		RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss3.str(), Color(1, 1, 0), 1, 38.f, 3.5f);
    }
 
    // 4: Trap
    if (b_Traps) 
	{
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TRAP), 2.5f, 45.f, 6.5f, 0, f_RotateTrap, 0, false);
		RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss4.str(), Color(1, 1, 0), 1, 43.f, 3.5f);
    }
    else
	{
        RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TRAP), 2, 45.f, 6.5f, 0, f_RotateTrap, 0, false);
		RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss4.str(), Color(1, 1, 0), 1, 43.f, 3.5f);
    }
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss4.str(), Color(1, 1, 0), 1, 43.f, 3.5f);
    // 5: Meat
    RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MEAT), 2, 50.f, 6.2f, 45.f, 45.f, 0, false);
    RenderTextOnScreen(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_TEXT_IMPACT), ss5.str(), Color(1, 1, 0), 1, 48.f, 3.5f);
    //RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_BAIT), false, 30.f, 10, -30, false);
    //RenderMeshIn2D(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_HUDHIGHLIGHT), true, 10.0f, 40, -48);

	RenderUI(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MONSTER_RABBIT), 2, 75.f, 20.5f, 0, f_RotateTrap, 0, false);

    SetHUD(false);
}

void Scene::RenderParticles()
{
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_ONE, GL_ONE);
    for (std::vector<ParticleObject* >::iterator it = SharedData::GetInstance()->particleManager->m_particleList.begin(); it != SharedData::GetInstance()->particleManager->m_particleList.end(); ++it)
    {
        ParticleObject* particle = (ParticleObject*)(*it);
        if (particle->active)
        {
            RenderParticle(particle);
        }
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);
}

void Scene::RenderParticle(ParticleObject* particle)
{
    switch (particle->type)
    {
    case ParticleObject::P_HIDDENBONUS:
        modelStack.PushMatrix();
        modelStack.Translate(particle->pos.x, particle->pos.y, particle->pos.z);
        modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.x - particle->pos.x, camera.position.z - particle->pos.z)), 0, 1, 0);
        modelStack.Scale(particle->scale.x, particle->scale.y, particle->scale.z);
        RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_HIDDENBONUS_PARTICLE), false);
        modelStack.PopMatrix();
        break;
	case ParticleObject::P_VOLCANOSPARK:
		modelStack.PushMatrix();
		modelStack.Translate(particle->pos.x, particle->pos.y, particle->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.x - particle->pos.x, camera.position.z - particle->pos.z)), 0, 1, 0);
		modelStack.Scale(particle->scale.x, particle->scale.y, particle->scale.z);
		RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_VOLCANOSPARK_PARTICLE), false);
		modelStack.PopMatrix();
		break;
	case ParticleObject::P_FALLINGLEAF:
		modelStack.PushMatrix();
		modelStack.Translate(particle->pos.x, particle->pos.y, particle->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.x - particle->pos.x, camera.position.z - particle->pos.z)), 0, 1, 0);
		modelStack.Scale(particle->scale.x, particle->scale.y, particle->scale.z);
		RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FALLINGLEAF_PARTICLE), false);
		modelStack.PopMatrix();
		break;
	case ParticleObject::P_ROCK:
		modelStack.PushMatrix();
		modelStack.Translate(particle->pos.x, particle->pos.y, particle->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.x - particle->pos.x, camera.position.z - particle->pos.z)), 0, 1, 0);
		modelStack.Scale(particle->scale.x, particle->scale.y, particle->scale.z);
		RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_FALLINGROCK_PARTICLE), false);
		modelStack.PopMatrix();
		break;
	case ParticleObject::P_DUST:
		modelStack.PushMatrix();
		modelStack.Translate(particle->pos.x, particle->pos.y, particle->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.x - particle->pos.x, camera.position.z - particle->pos.z)), 0, 1, 0);
		modelStack.Scale(particle->scale.x, particle->scale.y, particle->scale.z);
		RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_DUST_PARTICLE), false);
		modelStack.PopMatrix();
		break;
	case ParticleObject::P_MUDBUBBLE:
		modelStack.PushMatrix();
		modelStack.Translate(particle->pos.x, particle->pos.y, particle->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.x - particle->pos.x, camera.position.z - particle->pos.z)), 0, 1, 0);
		modelStack.Scale(particle->scale.x, particle->scale.y, particle->scale.z);
		RenderMesh(SharedData::GetInstance()->graphicsLoader->GetMesh(GraphicsLoader::GEO_MUDBUBBLE_PARTICLE), false);
		modelStack.PopMatrix();
		break;
    default:
        break;
    }
}