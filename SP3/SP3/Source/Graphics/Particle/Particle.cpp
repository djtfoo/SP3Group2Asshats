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
        vel.y -= 3.f * (float)dt;
        vel.x += Math::RandFloatMinMax(-3.2f, 3.2f) * float(dt);
        vel.z += Math::RandFloatMinMax(-3.2f, 3.2f) * float(dt);
        pos += vel * float(dt);
        //this->pos.y += vel.y * float(dt);
        //this->pos.x += Math::RandFloatMinMax(-5.f, 5.f) * float(dt);
        //this->pos.z += Math::RandFloatMinMax(-5.f, 5.f) * float(dt);
        rotation += Math::RandIntMinMax(-rotationSpeed, rotationSpeed) * (float)dt * 10.0f;

        if (pos.y < 0.f/*(ReadHeightMap(m_heightMap, particle->pos.x / 4000.0f, particle->pos.z / 4000.0f) * 350.0f)*/)
        {
            b_toDelete = true;
        }
        break;

    case P_VOLCANOSPARK:
        vel.y += 0.001f * (float)dt;
        pos.y += vel.y * float(dt);
        rotation += Math::RandIntMinMax(-rotationSpeed, rotationSpeed) * (float)dt * 10.0f;
        if (pos.y > 20.f)
        {
            b_toDelete = true;
        }
        break;
	case P_FALLINGLEAF:
		vel.y -= (float)dt;
        vel.x += Math::RandFloatMinMax(-5.f, 5.f) * (float)dt;
		//this->pos.y += this->vel.y * float(dt);
		//this->pos.x += Math::RandFloatMinMax(-2.f, 2.f) * (float)dt;
        pos += vel * float(dt);
		rotation += Math::RandIntMinMax(-rotationSpeed, rotationSpeed) * (float)dt * 10.0f;
		if (pos.y < -scale.y)
		{
			b_toDelete = true;
		}
		break;
	case P_ROCK:
		vel.y -= 0.5f * (float)dt;
		pos.y += this->vel.y * float(dt);
		rotation += Math::RandIntMinMax(-rotationSpeed, rotationSpeed) * (float)dt * 10.0f;
		if (pos.y < 0.f)
		{
			b_toDelete = true;
		}
		break;
	case P_DUST:
		vel.y += 0.001f * (float)dt;
		pos.y += vel.y * float(dt);
		rotation += Math::RandIntMinMax(-rotationSpeed, rotationSpeed) * (float)dt * 10.0f;
		if (pos.y > 2.f)
		{
			b_toDelete = true;
		}
		break;
	case P_MUDBUBBLE:
		vel.y += 0.001f * (float)dt;
		pos.y += this->vel.y * float(dt);
		scale += 0.001f * (float)dt;
		rotation += Math::RandIntMinMax(-rotationSpeed, rotationSpeed) * (float)dt * 10.0f;
		if (scale.x >= 5 ||
			scale.y >= 5 ||
			scale.z >= 5)
		{
			b_toDelete = true;
		}
		break;
    }
    
}