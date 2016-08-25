#ifndef FOG_H
#define FOG_H

#include "../Vertex/Vertex.h"

struct Fog
{
    enum FOG_TYPE
    {
        FOG_LINEAR,
        FOG_EXPONENTIAL,
        FOG_EXPONENTIALSQUARED,
        FOG_TOTAL
    };

    Color color;
    float start;
    float end;
    float density;
    FOG_TYPE type;
    bool enabled;
	int fog_change;

    Fog()
    {
        color.Set(0.5f, 0.5f, 0.5f);
        start = 0.f;
        end = 0.f;
        density = 0.f;
        type = FOG_LINEAR;
        enabled = false;
		fog_change = 1;
    }
};

#endif