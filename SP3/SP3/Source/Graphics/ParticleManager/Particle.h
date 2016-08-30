#ifndef PARTICLE_H
#define PARTICLE_H

#include "Vector3.h"
#include <vector>

enum ParticleObject_TYPE
{
	//P_WATER = 0,
	P_HIDDENBONUS = 0,
	P_TOTAL
};

class ParticleObject
{
public:
	ParticleObject();
	~ParticleObject();

	ParticleObject_TYPE type;

	Vector3 pos;
	Vector3 vel;
	Vector3 scale;
	float rotation;
	float rotationSpeed;
	float timecounter = 0;

	bool active;
	std::vector<ParticleObject*> particleList;
	int m_particleCount = 0;
	const unsigned MAX_PARTICLE = 1000;

	void UpdateParticle(double dt, Vector3 position, ParticleObject_TYPE type);
	ParticleObject* GetParticle(void);
	//void RenderParticle(ParticleObject particle);
};



#endif