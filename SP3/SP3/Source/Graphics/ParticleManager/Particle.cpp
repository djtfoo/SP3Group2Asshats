#include "Particle.h"

ParticleObject::ParticleObject() :
type(P_HIDDENBONUS),
pos(0, 0, 0),
scale(1, 1, 1),
vel(0, 0, 0),
rotation(0),
rotationSpeed(0),
active(false)
{
}



ParticleObject::~ParticleObject()
{
}



ParticleObject* ParticleObject::GetParticle()
{
	for (std::vector<ParticleObject*>::iterator it = particleList.begin(); it != particleList.end(); ++it)
	{
		ParticleObject* particle = (ParticleObject*)*it;
		if (!particle->active)
		{
			particle->active = true;
			m_particleCount++;
			return particle;
		}
	}

	for (unsigned i = 0; i < 10; ++i)
	{
		ParticleObject* particle = new ParticleObject();
		particleList.push_back(particle);
	}

	ParticleObject* particle = particleList.back();
	particle->active = true;
	m_particleCount++;
	return particle;
}

void ParticleObject::UpdateParticle(double dt, Vector3 position, ParticleObject_TYPE type)
{
	if (m_particleCount < MAX_PARTICLE)
	{
		if (timecounter > 5)
		{
			ParticleObject* particle = GetParticle();
			particle->type = type;
			particle->scale.Set(4, 4, 4);
			particle->vel.Set(0.f, 1.f, 0.f);
			particle->rotationSpeed = Math::RandFloatMinMax(20.0f, 40.0f);
			particle->pos.Set(position.x, 5, position.z);
			timecounter = 0;
		}
		else
		{
			timecounter++;
		}
	}

	for (std::vector<ParticleObject* >::iterator it = particleList.begin(); it != particleList.end(); ++it)
	{
		ParticleObject* particle = (ParticleObject*)*it;
		if (particle->active)
		{
			if (particle->type == type)
			{
				particle->vel.y += -0.05f * (float)dt;
				particle->pos.y += particle->vel.y * float(dt);
				particle->pos.x += Math::RandFloatMinMax(-2.5, 2.5) * float(dt);
				particle->pos.z += Math::RandFloatMinMax(-2.5, 2.5) * float(dt);
				particle->rotation += Math::RandIntMinMax(-particle->rotationSpeed, particle->rotationSpeed) * (float)dt * 10.0f;
			}

			if (particle->pos.y < 0.f/*(ReadHeightMap(m_heightMap, particle->pos.x / 4000.0f, particle->pos.z / 4000.0f) * 350.0f)*/)
			{
				particle->active = false;
				m_particleCount--;
			}
		}
	}
}