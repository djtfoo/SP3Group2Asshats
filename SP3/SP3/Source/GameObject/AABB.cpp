#include "AABB.h"

/******************************************************************************/
/*!
\brief	Overloaded contructor

\param pos
	position of the AABB
\param scale
	scale of the AABB
*/
/******************************************************************************/
AABB::AABB(Vector3 pos, Vector3 scale) :
m_origin(pos),
m_scale(scale)
{
}

/******************************************************************************/
/*!
\brief	Default contructor
/******************************************************************************/
AABB::AABB() :
m_origin(Vector3(0, 0, 0)),
m_scale(Vector3(0, 0, 0))
{
}

/******************************************************************************/
/*!
\brief	Default destructor
/******************************************************************************/
AABB::~AABB()
{
}

/******************************************************************************/
/*!
\brief	AABB collision check of AABB against Vector3 point

\param	vecPoint
	Vector3 point to test for collision

*/
/******************************************************************************/
bool AABB::CheckCollision(const Vector3& vecPoint)
{
	Vector3 max = m_origin + m_scale * 0.5;
    Vector3 min = m_origin - m_scale * 0.5;

	//Check if the point is less than max and greater than min
	if (vecPoint.x > min.x && vecPoint.x < max.x &&
		vecPoint.y > min.y && vecPoint.y < max.y &&
		vecPoint.z > min.z && vecPoint.z < max.z)
		return true;
	//If not, then return false
	else
		return false;
}

/******************************************************************************/
/*!
\brief	AABB collision check of AABB against another AABB

\param rhsBox
	Second AABB hitbox to test for collision

*/
/******************************************************************************/
bool AABB::CheckCollision(const AABB& rhsBox)
{
	Vector3 LhsMax = m_origin + m_scale * 0.5;
	Vector3 LhsMin = m_origin - m_scale * 0.5;
	Vector3 RhsMax = rhsBox.m_origin + rhsBox.m_scale * 0.5;
	Vector3 RhsMin = rhsBox.m_origin - rhsBox.m_scale * 0.5;

	//Check if lhsBox's max is greater than rhsBox's min and lhsBox's min is less than rhsBox's max
	if (LhsMax.x > RhsMin.x &&
		LhsMin.x < RhsMax.x &&
		LhsMax.y > RhsMin.y &&
		LhsMin.y < RhsMax.y &&
		LhsMax.z > RhsMin.z &&
		LhsMin.z < RhsMax.z)
		return true;
	//If not, then return false
	else
		return false;
}