/******************************************************************************/
/*!
\file	Camera.h
\author Wen Sheng Tang
\par	email: tang_wen_sheng\@nyp.edu.sg
\brief
Class that controls field of view of scene
*/
/******************************************************************************/
#ifndef ZOO_CAMERA_H
#define ZOO_CAMERA_H

#include "Vector3.h"

class ZooCamera
{
public:

	Vector3 position;
	Vector3 target;
	Vector3 up;

	Vector3 defaultPosition;
	Vector3 defaultTarget;
	Vector3 defaultUp;

    ZooCamera();
    ~ZooCamera();
	void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	void Update(double dt);
	void Reset();
};

#endif