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

char** Scene::m_levelMap = 0;

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
            modelStack.Scale(world->appearance[GO].scale.x, world->appearance[GO].scale.y, world->appearance[GO].scale.z);
            //modelStack.Rotate(0, 0, 0, 0);
            RenderMesh(world->appearance[GO].mesh, true);
            modelStack.PopMatrix();
        }
    }
}

#define MOVEMENT_MASK (COMPONENT_DISPLACEMENT | COMPONENT_VELOCITY)
void Scene::UpdateGameObjects(World* world, double dt)
{
    for (unsigned GO = 0; GO < world->GAMEOBJECT_COUNT; ++GO)
    {
        Vector3 *vel;
        vel = &(world->velocity[GO]);

        if ((world->mask[GO] & MOVEMENT_MASK) == MOVEMENT_MASK)
        {
            Vector3 *pos;
            pos = &(world->position[GO]);

            pos->x += vel->x * dt;
            pos->y += vel->y * dt;
            pos->z += vel->z * dt;
            //std::cout << "asd" << std::endl;
        }

        if ((world->mask[GO] & COMPONENT_HITBOX) == COMPONENT_HITBOX)
        {
            Vector3 *origin;
            origin = &(world->hitbox[GO].m_origin);

            origin->x += vel->x * dt;
            origin->y += vel->y * dt;
            origin->z += vel->z * dt;
        }

    }
}