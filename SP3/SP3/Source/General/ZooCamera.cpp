/******************************************************************************/
/*!
\file	Camera.cpp
\author Wen Sheng Tang
\par	email: tang_wen_sheng\@nyp.edu.sg
\brief
Class that controls field of view of scene
*/
/******************************************************************************/
#include "ZooCamera.h"
#include "Application.h"
#include "Mtx44.h"
#include "SharedData.h"

ZooCamera::ZooCamera()
{
}

ZooCamera::~ZooCamera()
{
}

void ZooCamera::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();
}

static const float CAMERA_SPEED = 50.0f;

void ZooCamera::Update(double dt)
{
    ////X scroll
    //if (Application::cursorXPos > Application::GetWindowWidth() * 0.8)
    //{        
    //    position -= Vector3(CAMERA_SPEED, 0, 0) * dt;
    //    target -= Vector3(CAMERA_SPEED, 0, 0) * dt;

    //    if (Application::cursorXPos > Application::GetWindowWidth() * 0.9)
    //    {
    //        position -= Vector3(CAMERA_SPEED, 0, 0) * dt;
    //        target -= Vector3(CAMERA_SPEED, 0, 0) * dt;
    //    }
    //}
    //else if (Application::cursorXPos < Application::GetWindowWidth() * 0.2)
    //{       
    //    position += Vector3(CAMERA_SPEED, 0, 0) * dt;
    //    target += Vector3(CAMERA_SPEED, 0, 0) * dt;

    //    if (Application::cursorXPos < Application::GetWindowWidth() * 0.1)
    //    {
    //        position += Vector3(CAMERA_SPEED, 0, 0) * dt;
    //        target += Vector3(CAMERA_SPEED, 0, 0) * dt;
    //    }
    //}

    ////Y scroll
    //if (Application::cursorYPos > Application::GetWindowHeight() * 0.8)
    //{
    //    position -= Vector3(0, 0, CAMERA_SPEED) * dt;
    //    target -= Vector3(0, 0, CAMERA_SPEED) * dt;
    //    if (Application::cursorYPos > Application::GetWindowHeight() * 0.9)
    //    {
    //        position -= Vector3(0, 0, CAMERA_SPEED) * dt;
    //        target -= Vector3(0, 0, CAMERA_SPEED) * dt;
    //    }
    //}
    //else if (Application::cursorYPos < Application::GetWindowHeight() * 0.2)
    //{       
    //    position += Vector3(0, 0, CAMERA_SPEED) * dt;
    //    target += Vector3(0, 0, CAMERA_SPEED) * dt;

    //    if (Application::cursorYPos < Application::GetWindowHeight() * 0.1)
    //    {
    //        position += Vector3(0, 0, CAMERA_SPEED) * dt;
    //        target += Vector3(0, 0, CAMERA_SPEED) * dt;
    //    }
    //}

    ////Zoom in/out
    //if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_W].isHeldDown && position.y > 20)
    //{
    //    position.y -= CAMERA_SPEED* dt;
    //}
    //else if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_S].isHeldDown)
    //{
    //    position.y += CAMERA_SPEED* dt;
    //}

}

void ZooCamera::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}