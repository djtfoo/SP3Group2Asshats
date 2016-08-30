/****************************************************************************/
/*!
\file Sound.h
\author Bek Kunta
\par email: xiaobek\@gmail.com
\brief
A Class that is define for the ItemProjectile in the game
*/
/****************************************************************************/
#ifndef ITEMPROJECTILE_H
#define ITEMPROJECTILE_H

#include "Vector3.h"
//#include AABB
//#include "../Audio/Sound.h"
#include <vector>

using std::vector;
/****************************************************************************/
/*!
Class ItemProjectile:
\brief Defines a ItemProjectile and its methods,functions.
*/
/****************************************************************************/
class ItemProjectile
{
public:
	ItemProjectile(Vector3 pos, Vector3 dir, float r, float s, int d);
	~ItemProjectile();

	void Update(double dt);
	void UpdateProjectile(double dt);
	void UpdateRockProjectile(double dt);
	void UpdateNetProjectile(double dt);
	void UpdateBaitProjectile(double dt);

	static vector<ItemProjectile*> ItemProjectileList;
	static vector<ItemProjectile*> RockProjectileList;
	static vector<ItemProjectile*> NetProjectileList;
	static vector<ItemProjectile*> BaitProjectileList;

	static double d_rockCounter;
	static double d_netCounter;
	static double d_baitCounter;

	static double d_rockCooldown;
	static double d_netCooldown;
	static double d_baitCooldown;

	Vector3 position;
	bool deleteBullet;
private:
	Vector3 direction;

	float range;
	float speed;
	float distanceTravelled;
	float vSpeed;

	int damage;

	//Sound sound;
};

#endif// ! ITEMPROJECTILE_H