#include "Particle.h"

ParticleObject::ParticleObject() :
type(P_HIDDENBONUS),
pos(0, 0, 0),
scale(1, 1, 1),
vel(0, 0, 0),
rotation(0),
rotationSpeed(0),
active(false),
b_toDelete(false)
{
}

ParticleObject::~ParticleObject()
{
}

void ParticleObject::UpdateParticle(double dt)
{
    switch (this->type)
    {
    case P_HIDDENBONUS:
        this->vel.y -= 3.f * (float)dt;
        this->vel.x += Math::RandFloatMinMax(-3.2f, 3.2f) * float(dt);
        this->vel.z += Math::RandFloatMinMax(-3.2f, 3.2f) * float(dt);
        this->pos += vel * float(dt);
        //this->pos.y += vel.y * float(dt);
        //this->pos.x += Math::RandFloatMinMax(-5.f, 5.f) * float(dt);
        //this->pos.z += Math::RandFloatMinMax(-5.f, 5.f) * float(dt);
        this->rotation += Math::RandIntMinMax(-this->rotationSpeed, this->rotationSpeed) * (float)dt * 10.0f;

        if (this->pos.y < 0.f/*(ReadHeightMap(m_heightMap, particle->pos.x / 4000.0f, particle->pos.z / 4000.0f) * 350.0f)*/)
        {
            this->b_toDelete = true;
        }
        break;

    case P_VOLCANOSPARK:
        this->vel.y += 0.001f * (float)dt;
        this->pos.y += this->vel.y * float(dt);
        this->rotation += Math::RandIntMinMax(-this->rotationSpeed, this->rotationSpeed) * (float)dt * 10.0f;
        if (this->pos.y > 20.f)
        {
            this->b_toDelete = true;
        }
        break;
	case P_FALLINGLEAF:
		this->vel.y -= 0.5f * (float)dt;
		this->pos.y += this->vel.y * float(dt);
		this->pos.x += Math::RandIntMinMax(-2, 2) * (float)dt;
		this->rotation += Math::RandIntMinMax(-this->rotationSpeed, this->rotationSpeed) * (float)dt * 10.0f;
		if (this->pos.y < 0.f)
		{
			this->b_toDelete = true;
		}
		break;
	case P_ROCK:
		this->vel.y -= 0.5f * (float)dt;
		this->pos.y += this->vel.y * float(dt);
		this->rotation += Math::RandIntMinMax(-this->rotationSpeed, this->rotationSpeed) * (float)dt * 10.0f;
		if (this->pos.y < 0.f)
		{
			this->b_toDelete = true;
		}
		break;
	case P_DUST:
		this->vel.y += 0.001f * (float)dt;
		this->pos.y += this->vel.y * float(dt);
		this->rotation += Math::RandIntMinMax(-this->rotationSpeed, this->rotationSpeed) * (float)dt * 10.0f;
		if (this->pos.y > 2.f)
		{
			this->b_toDelete = true;
		}
		break;
	case P_MUDBUBBLE:
		this->vel.y += 0.001f * (float)dt;
		this->pos.y += this->vel.y * float(dt);
		this->scale += 0.001f * (float)dt;
		this->rotation += Math::RandIntMinMax(-this->rotationSpeed, this->rotationSpeed) * (float)dt * 10.0f;
		if (this->scale.x >= 5 ||
			this->scale.y >= 5 ||
			this->scale.z >= 5)
		{
			this->b_toDelete = true;
		}
		break;
    }
    
}