#include "ParticleManager.h"

ParticleManager::ParticleManager() : MAX_PARTICLES(1000), m_particleCount(0), d_timeCounter(0.0)
{
}

ParticleManager::~ParticleManager()
{
}

void ParticleManager::Init()
{
    m_particleCount = 0;
    d_timeCounter = 0.0;
}

void ParticleManager::Update(double dt)
{
    for (std::vector<ParticleObject*>::iterator it = m_particleList.begin(); it != m_particleList.end(); ++it)
    {
        ParticleObject* particle = (ParticleObject*)(*it);
        if (particle->active)
        {
            particle->UpdateParticle(dt);
            if (particle->b_toDelete)
            {
                DeleteParticle(particle);
            }
        }

    }
}

void ParticleManager::Exit()
{
    ClearParticles();
}

void ParticleManager::ClearParticles()
{
    m_particleCount = 0;

    while (m_particleList.size() > 0)
    {
        ParticleObject *particle = m_particleList.back();
        delete particle;
        m_particleList.pop_back();
    }
}

void ParticleManager::SpawnParticle(const Vector3& position, ParticleObject::PARTICLEOBJECT_TYPE type)
{
    if (m_particleCount < MAX_PARTICLES)
    {
        ParticleObject* particle = GetParticle();
        particle->type = type;

        switch (type)
        {
        case ParticleObject::P_HIDDENBONUS:
            particle->scale.Set(4, 4, 4);
            particle->vel.Set(0.f, 6.f, 0.f);
            particle->rotationSpeed = Math::RandFloatMinMax(20.0f, 40.0f);
            particle->pos.Set(position.x, 5, position.z);
            break;
        case ParticleObject::P_VOLCANOSPARK:
			particle->scale.Set(10.f, 4.f, 10.f);
            particle->vel.Set(0.f, 5.f, 0.f);
            particle->rotationSpeed = Math::RandFloatMinMax(20.0f, 40.0f);
            particle->pos.Set(position.x + Math::RandFloatMinMax(-0.1f, 0.1f), 5, position.z + Math::RandFloatMinMax(-0.1f, 0.1f));
            break;
		case ParticleObject::P_FALLINGLEAF:
			particle->scale.Set(2.f, 2.f, 2.f);
			particle->vel.Set(0.f, -1.f, 0.f);
			particle->rotationSpeed = Math::RandFloatMinMax(20.0f, 40.0f);
			particle->pos.Set(position.x + Math::RandFloatMinMax(-0.1f, 0.1f), 3.f, position.z + Math::RandFloatMinMax(-0.1f, 0.1f));
			break;
		case ParticleObject::P_ROCK:
			particle->scale.Set(2.f, 2.f, 2.f);
			particle->vel.Set(0.f, -5.f, 0.f);
			particle->rotationSpeed = Math::RandFloatMinMax(20.0f, 40.0f);
			particle->pos.Set(position.x + Math::RandFloatMinMax(-0.1f, 0.1f), 50, position.z + Math::RandFloatMinMax(-0.1f, 0.1f));
			break;
		case ParticleObject::P_DUST:
			particle->scale.Set(2.f, 2.f, 2.f);
			particle->vel.Set(0.f, 0.5f, 0.f);
			particle->rotationSpeed = Math::RandFloatMinMax(20.0f, 40.0f);
			particle->pos.Set(position.x + Math::RandFloatMinMax(-0.1f, 0.1f), 5, position.z + Math::RandFloatMinMax(-0.1f, 0.1f));
			break;
		case ParticleObject::P_MUDBUBBLE:
			particle->scale.Set(2.f, 2.f, 2.f);
			particle->vel.Set(0.f, 0.001f, 0.f);
			particle->rotationSpeed = Math::RandFloatMinMax(20.0f, 40.0f);
			particle->pos.Set(position.x + Math::RandFloatMinMax(-0.1f, 0.1f), 5, position.z + Math::RandFloatMinMax(-0.1f, 0.1f));
			break;
        }
    }
}

ParticleObject* ParticleManager::GetParticle()
{
    for (std::vector<ParticleObject*>::iterator it = m_particleList.begin(); it != m_particleList.end(); ++it)
    {
        ParticleObject* particle = (ParticleObject*)*it;
        if (!particle->active)
        {
            particle->active = true;
            particle->b_toDelete = false;
            ++m_particleCount;
            return particle;
        }
    }

    for (unsigned i = 0; i < 10; ++i)
    {
        ParticleObject* particle = new ParticleObject();
        m_particleList.push_back(particle);
    }

    ParticleObject* particle = m_particleList.back();
    particle->active = true;
    particle->b_toDelete = false;
    ++m_particleCount;
    return particle;
}

void ParticleManager::DeleteParticle(ParticleObject *particle)
{
    particle->active = false;
    --m_particleCount;
}