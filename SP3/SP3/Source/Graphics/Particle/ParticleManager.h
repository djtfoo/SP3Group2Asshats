#ifndef PARTICLEMANAGER_H
#define PARTICLEMANAGER_H

#include "Particle.h"
#include <vector>

class ParticleManager
{
public:
    ParticleManager();
    ~ParticleManager();

    const unsigned MAX_PARTICLES;
    int m_particleCount;
    double d_timeCounter;
    std::vector<ParticleObject*> m_particleList;
    
    void Init();
    void Update(double dt);
    void Exit();

    void ClearParticles();

    ParticleObject* GetParticle();
    void SpawnParticle(const Vector3& position, ParticleObject::PARTICLEOBJECT_TYPE type);
    void DeleteParticle(ParticleObject* particle);
};

#endif