/******************************************************************************/
/*!
\file	Camera.cpp
\author Wen Sheng Tang
\par	email: tang_wen_sheng\@nyp.edu.sg
\brief
Class that controls field of view of scene
*/
/******************************************************************************/
#include "Camera.h"
#include "Application.h"
#include "Mtx44.h"
#include "SharedData.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();
}

static const float CAMERA_SPEED = 5.0f;

void Camera::Update()
{
    position = SharedData::GetInstance()->player->GetPositionVector();
    target = position + SharedData::GetInstance()->player->GetViewVector() * 5.f;
    up = SharedData::GetInstance()->player->GetUpVector();

	//if(Application::IsKeyPressed('A'))
	//{
    //    MoveLeft(dt);
	//}
	//if(Application::IsKeyPressed('D'))
	//{
    //    MoveRight(dt);
	//}
	//if(Application::IsKeyPressed('W'))
	//{
    //    MoveForward(dt);
	//}
	//if(Application::IsKeyPressed('S'))
	//{
    //    MoveBackward(dt);
	//}
    //
	//if(Application::IsKeyPressed(VK_LEFT))
	//{
	//	Vector3 view = (target - position).Normalized();
	//	float yaw = (float)(CAMERA_SPEED * (float)dt);
	//	Mtx44 rotation;
	//	rotation.SetToRotation(yaw, 0, 1, 0);
	//	view = rotation * view;
	//	target = position + view;
	//	Vector3 right = view.Cross(up);
	//	right.y = 0;
	//	right.Normalize();
	//	up = right.Cross(view).Normalized();
	//}
	//if(Application::IsKeyPressed(VK_RIGHT))
	//{
	//	Vector3 view = (target - position).Normalized();
	//	float yaw = (float)(-CAMERA_SPEED * (float)dt);
	//	Mtx44 rotation;
	//	rotation.SetToRotation(yaw, 0, 1, 0);
	//	view = rotation * view;
	//	target = position + view;
	//	Vector3 right = view.Cross(up);
	//	right.y = 0;
	//	right.Normalize();
	//	up = right.Cross(view).Normalized();
	//}
	//if(Application::IsKeyPressed(VK_UP))
	//{
	//	float pitch = (float)(CAMERA_SPEED * (float)dt);
	//	Vector3 view = (target - position).Normalized();
	//	Vector3 right = view.Cross(up);
	//	right.y = 0;
	//	right.Normalize();
	//	up = right.Cross(view).Normalized();
	//	Mtx44 rotation;
	//	rotation.SetToRotation(pitch, right.x, right.y, right.z);
	//	view = rotation * view;
	//	target = position + view;
	//}
	//if(Application::IsKeyPressed(VK_DOWN))
	//{
	//	float pitch = (float)(-CAMERA_SPEED * (float)dt);
	//	Vector3 view = (target - position).Normalized();
	//	Vector3 right = view.Cross(up);
	//	right.y = 0;
	//	right.Normalize();
	//	up = right.Cross(view).Normalized();
	//	Mtx44 rotation;
	//	rotation.SetToRotation(pitch, right.x, right.y, right.z);
	//	view = rotation * view;
	//	target = position + view;
	//}
	////Update the camera direction based on mouse move
	//// left-right rotate
    //double x, y;
    //Application::GetCursorPos(&x, &y);
    //
    //if (x != Application::cursorXPos)
    //{
    //    double diff_xpos = Application::cursorXPos - x;
    //
	//	Vector3 view = (target - position).Normalized();
    //    float yaw = (float)(diff_xpos * CAMERA_SPEED * (float)dt);
	//	Mtx44 rotation;
	//	rotation.SetToRotation(yaw, 0, 1, 0);
	//	view = rotation * view;
	//	target = position + view;
	//	Vector3 right = view.Cross(up);
	//	right.y = 0;
	//	right.Normalize();
	//	up = right.Cross(view).Normalized();
	//}
    //if (y != Application::cursorYPos - 0.5)
    //{
    //    double diff_ypos = Application::cursorYPos - y;
    //
    //    float pitch = (float)(diff_ypos * CAMERA_SPEED * (float)dt);
	//	Vector3 view = (target - position).Normalized();
	//	Vector3 right = view.Cross(up);
	//	right.y = 0;
	//	right.Normalize();
	//	up = right.Cross(view).Normalized();
	//	Mtx44 rotation;
	//	rotation.SetToRotation(pitch, right.x, right.y, right.z);
	//	view = rotation * view;
	//	target = position + view;
	//}
}

void Camera::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}

void Camera::MoveForward(const double dt)
{
    Vector3 view = (target - position).Normalized();
    position.x += view.x * CAMERA_SPEED * (float)dt;
    position.z += view.z * CAMERA_SPEED * (float)dt;
    target.x += view.x * CAMERA_SPEED * (float)dt;
    target.z += view.z * CAMERA_SPEED * (float)dt;
}
void Camera::MoveBackward(const double dt)
{
    Vector3 view = (target - position).Normalized();
    position.x -= view.x * CAMERA_SPEED * (float)dt;
    position.z -= view.z * CAMERA_SPEED * (float)dt;
    target.x -= view.x * CAMERA_SPEED * (float)dt;
    target.z -= view.z * CAMERA_SPEED * (float)dt;
}

void Camera::MoveLeft(const double dt)
{
    Vector3 view = (target - position).Normalized();
    Vector3 right = view.Cross(up);
    right.y = 0;
    right.Normalize();
    position -= right * CAMERA_SPEED * (float)dt;
    target -= right * CAMERA_SPEED * (float)dt;
}

void Camera::MoveRight(const double dt)
{
    Vector3 view = (target - position).Normalized();
    Vector3 right = view.Cross(up);
    right.y = 0;
    right.Normalize();
    position += right * CAMERA_SPEED * (float)dt;
    target += right * CAMERA_SPEED * (float)dt;
}