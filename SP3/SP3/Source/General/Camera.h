/******************************************************************************/
/*!
\file	Camera.h
\author Wen Sheng Tang
\par	email: tang_wen_sheng\@nyp.edu.sg
\brief
Class that controls field of view of scene
*/
/******************************************************************************/
#ifndef CAMERA_3_H
#define CAMERA_3_H

#include "Vector3.h"

class Camera
{
public:
	enum CAMERA_STATE
	{
		CAMERA_FPS,
		CAMERA_ISOMETRIC,
		CAMERA_STATES_TOTAL
	};

	Vector3 position;
	Vector3 target;
	Vector3 up;

	Vector3 defaultPosition;
	Vector3 defaultTarget;
	Vector3 defaultUp;

	Camera();
	~Camera();
	void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	void Update();
	void Reset();

    void MoveForward(const double dt);
    void MoveBackward(const double dt);
    void MoveLeft(const double dt);
    void MoveRight(const double dt);
};

#endif