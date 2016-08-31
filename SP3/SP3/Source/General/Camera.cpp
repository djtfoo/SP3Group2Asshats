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

    rotateAngle = 0.f;
}

static const float CAMERA_SPEED = 5.0f;

void Camera::Update()
{
    position = SharedData::GetInstance()->player->GetPositionVector();
    target = position + SharedData::GetInstance()->player->GetViewVector() * 5.f;
    up = SharedData::GetInstance()->player->GetUpVector();
}

void Camera::UpdateForMenu(double dt)
{
    rotateAngle += 0.3f * (float)(dt);
    target.Set(100, 0, 100);
    position = target + Vector3(150.f * sinf(rotateAngle), 50, 150.f * cosf(rotateAngle));

    Vector3 view = (target - position).Normalized();
    Vector3 right = view.Cross(up);
    right.y = 0;
    right.Normalize();
    up = right.Cross(view).Normalized();
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