/****************************************************************************/
/*!
\file ItemProjectiles.cpp
\author Bek Kunta
\par email: xiaobek\@gmail.com
\brief
A Class that is define for the ItemProjectiles in the game
*/
/****************************************************************************/
#include "ItemProjectiles.h"
#include "../General/WorldValues.h"

vector<ItemProjectile*> ItemProjectile::ItemProjectileList;
vector<ItemProjectile*> ItemProjectile::RockProjectileList;
vector<ItemProjectile*> ItemProjectile::NetProjectileList;
vector<ItemProjectile*> ItemProjectile::BaitProjectileList;

double ItemProjectile::d_rockCounter = 0.3;
double ItemProjectile::d_netCounter = 3.0;
double ItemProjectile::d_baitCounter = 0.3;
double ItemProjectile::d_trapCounter = 1.0;

double ItemProjectile::d_rockCooldown = 0.3;
double ItemProjectile::d_netCooldown = 3.0;
double ItemProjectile::d_baitCooldown = 0.3;
double ItemProjectile::d_trapCooldown = 1.0;

/****************************************************************************/
/*!
\brief 

\param pos
		position of projectile
\param dir
		direction of projectile
\param r 
		range of the projectile till destroy
\param s 
		speed of the projectile
\param d 
		damage done by the projectile
ItemProjectiles Contructor and Initing ItemProjectiles
*/
/****************************************************************************/
ItemProjectile::ItemProjectile(Vector3 pos, Vector3 dir, float r, float s, int d)
{
	position = pos;
	direction = dir;
	deleteBullet = false;

	range = r;
	speed = s;
	damage = d;
	vSpeed = 0;

	distanceTravelled = 0.f;
}
/****************************************************************************/
/*!
\brief
ItemProjectiles Destructor
*/
/****************************************************************************/
ItemProjectile::~ItemProjectile()
{
}
/****************************************************************************/
/*!
\brief Update(double dt)

\param dt 
		dt to update projectiles
*/
/****************************************************************************/
void ItemProjectile::Update(double dt)
{
	distanceTravelled += dt * speed;
	position += direction * dt * speed;
	vSpeed += (WV_GRAVITY / 200.f) * dt;

	position.y += vSpeed;

	//if (Hitbox::CheckHitBox(position, damage)){
	//	deleteBullet = true;

	//	
	//}

	if (distanceTravelled >= range){
		deleteBullet = true;
	}

	if (position.y < -2)
	{
		deleteBullet = true;
	}
}
/****************************************************************************/
/*!
\brief UpdateProjectile(double dt)

\param dt	
	dt to update projectiles in other scene
*/
/****************************************************************************/
void ItemProjectile::UpdateProjectile(double dt)
{
	for (vector<ItemProjectile*>::iterator it = ItemProjectileList.begin(); it != ItemProjectileList.end();) {
		if ((*it)->deleteBullet == true){
			delete *it;
			it = ItemProjectile::ItemProjectileList.erase(it);
		}
		else
		{
			(*it)->Update(dt);
			it++;
		}
	}
}
/****************************************************************************/
/*!
\brief UpdateRockProjectile(double dt)

\param dt
dt to update rockprojectiles in other scene
*/
/****************************************************************************/
void ItemProjectile::UpdateRockProjectile(double dt)
{
	for (vector<ItemProjectile*>::iterator it = RockProjectileList.begin(); it != RockProjectileList.end();){
		if ((*it)->deleteBullet == true){
			delete *it;
			it = ItemProjectile::RockProjectileList.erase(it);
		}
		else
		{
			(*it)->Update(dt);
			it++;
		}
	}
}
/****************************************************************************/
/*!
\brief UpdateNetProjectile(double dt)

\param dt
dt to update netprojectiles in other scene
*/
/****************************************************************************/
void ItemProjectile::UpdateNetProjectile(double dt)
{
	for (vector<ItemProjectile*>::iterator it = NetProjectileList.begin(); it != NetProjectileList.end();){
		if ((*it)->deleteBullet == true){
			delete *it;
			it = ItemProjectile::NetProjectileList.erase(it);
		}
		else
		{
			(*it)->Update(dt);
			it++;
		}
	}
}
/****************************************************************************/
/*!
\brief UpdateBaitProjectile(double dt)

\param dt
dt to update baitprojectiles in other scene
*/
/****************************************************************************/
void ItemProjectile::UpdateBaitProjectile(double dt)
{
	for (vector<ItemProjectile*>::iterator it = BaitProjectileList.begin(); it != BaitProjectileList.end();){
		if ((*it)->deleteBullet == true){
			delete *it;
			it = ItemProjectile::BaitProjectileList.erase(it);
		}
		else
		{
			(*it)->Update(dt);
			it++;
		}
	}
}