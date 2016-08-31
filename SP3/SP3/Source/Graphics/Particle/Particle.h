#ifndef PARTICLE_H
#define PARTICLE_H

#include "Vector3.h"

class ParticleObject
{
public:
    enum PARTICLEOBJECT_TYPE
    {
        //P_WATER = 0,
        P_HIDDENBONUS = 0,
        P_VOLCANOSPARK,
		P_FALLINGLEAF,
		P_ROCK,
		P_DUST,
		P_MUDBUBBLE,
        P_TOTAL
    };

	ParticleObject();
	~ParticleObject();

	PARTICLEOBJECT_TYPE type;

	Vector3 pos;
	Vector3 vel;
	Vector3 scale;
	float rotation;
	float rotationSpeed;

	bool active;
    bool b_toDelete;

	void UpdateParticle(double dt);
};



#endif