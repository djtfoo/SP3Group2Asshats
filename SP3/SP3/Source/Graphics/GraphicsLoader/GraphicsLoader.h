/******************************************************************************/
/*!
\file	GraphicsLoader.h
\author Muhammad Amirul Bin Zaol-kefli
\par	email: 150509T@mymail.nyp.edu.sg
\brief
Load Graphics for every instance
*/
/******************************************************************************/
#ifndef GRAPHICS_LOADER_H
#define GRAPHICS_LOADER_H

#include "../Mesh/Mesh.h"
#include "MatrixStack.h"
#include "../Lighting/Light.h"
#include "../Lighting/DepthFBO.h"
#include "../Mesh/LoadHmap.h"
#include "../ParticleManager/Particle.h"
#include <math.h>

#include <vector>

/******************************************************************************/
/*!
\brief
GraphicsLoader class
*/
/******************************************************************************/
class GraphicsLoader
{
public:
	GraphicsLoader();
	~GraphicsLoader();

	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_LIGHTENABLED,
		U_NUMLIGHTS,
		U_LIGHT0_TYPE,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,
		U_LIGHT1_TYPE,
		U_LIGHT1_POSITION,
		U_LIGHT1_COLOR,
		U_LIGHT1_POWER,
		U_LIGHT1_KC,
		U_LIGHT1_KL,
		U_LIGHT1_KQ,
		U_LIGHT1_SPOTDIRECTION,
		U_LIGHT1_COSCUTOFF,
		U_LIGHT1_COSINNER,
		U_LIGHT1_EXPONENT,

		// BASE NUMBER + OFFSET = RESULT
		// U_COLOR_TEXTURE_ENABLED + i
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE_ENABLED1,
		U_COLOR_TEXTURE,
		U_COLOR_TEXTURE1,

		U_FOG_COLOR,
		U_FOG_START,
		U_FOG_END,
		U_FOG_DENSITY,
		U_FOG_TYPE,
		U_FOG_ENABLED,

		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};

	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_CROSSHAIR,
		GEO_LIGHTBALL,
		//GEO_SPHERE,
		//GEO_SPHERE2,
		//GEO_SPHERE3,
		//GEO_SPHERE4,
		//GEO_SPHERE5,
		//GEO_SPHERE6,
		//GEO_SPHERE7,
		//GEO_SPHERE8,
		//GEO_SPHERE9,
		//GEO_QUAD,
		GEO_CUBE,
		//GEO_RING,
		//GEO_CONE,
		//GEO_LEFT,
		//GEO_RIGHT,
		//GEO_TOP,
		//GEO_BOTTOM,
		//GEO_FRONT,
		//GEO_BACK,
		//GEO_GRASS_DARKGREEN,
		//GEO_GRASS_LIGHTGREEN,
		GEO_SKYPLANE,
		//GEO_DEVIL,
		//GEO_SKELETON,
		//GEO_GRAVESTONE,
		//GEO_ENEMY,
		//GEO_OBJECT,
		//GEO_CASTLE,
		//GEO_TREE,
		//GEO_RAIN,
		//GEO_TERRAIN,
		//GEO_LAVA,
		//GEO_TEXT,
		//GEO_SPRITE_ANIMATION,
		//GEO_PARTICLE_WATER,
		GEO_BOX_06,

		GEO_PLAYERBOX,

		GEO_HUD,
		GEO_HUDHIGHLIGHT,

		//SceneGrass
		GEO_GRASS_TERRAIN,
		GEO_GRASS_SKYPLANE,
		GEO_MONSTER_BIRD,
		GEO_MONSTER_RABBIT,
		GEO_GRASS1,
		GEO_GRASS2,
		GEO_TREE1,
        GEO_TREE2,
		//GEO_ROCKS2,
		//GEO_ROCKS3,
		//GEO_ROCKS4,
		//GEO_ROCKS5,

		GEO_ROCKS1,
		GEO_TRAP,
		GEO_NET,
		GEO_BAIT,

		//SceneSwamp
		GEO_SWAMP_TERRAIN,
		GEO_SWAMP_SKYPLANE,
        GEO_SWAMP_PLANT,
        GEO_SWAMP_ROOT,

		//SceneRock
		GEO_ROCK_TERRAIN,
		GEO_ROCK_SKYPLANE,

		//SceneLava
		GEO_LAVA_TERRAIN,
		GEO_LAVA_SKYPLANE,
		GEO_MONSTER_FIREBUG,
		GEO_MONSTER_MAGMA,
		GEO_MONSTER_MAGMA_BERZEKER,
		GEO_VOLCANO,

        GEO_TEXT_IMPACT,

		NUM_GEOMETRY,
	};

	void Init();
	void Exit();

	Mesh* GetMesh(GEOMETRY_TYPE m_geotype);
	unsigned GetParameters(UNIFORM_TYPE m_parameters);

	unsigned GetProgramID();
	//unsigned GetGPassShaderID();
	//DepthFBO GetLightDepthFBO();

	Light GetLights();
	//RENDER_PASS GetRenderPass();

	/*Mtx44*/

private:

	unsigned m_vertexArrayID;
	Mesh* m_meshList[NUM_GEOMETRY];

	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	std::vector<unsigned char> m_heightMap;

	unsigned m_gPassShaderID;
	DepthFBO m_lightDepthFBO;

	Mtx44 m_lightDepthProj;
	Mtx44 m_lightDepthView;

	Light m_lights;

	//RENDER_PASS m_renderPass;
};

#endif