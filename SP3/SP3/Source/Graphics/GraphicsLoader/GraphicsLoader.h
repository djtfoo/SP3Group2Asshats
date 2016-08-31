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
		GEO_CUBE,
        GEO_PLAYERBOX,

        //Game HUD
        GEO_BOX_TRANSLUCENT,
		GEO_HUD,
		GEO_HUDHIGHLIGHT,

		//SceneGrass
		GEO_GRASS_TERRAIN,
		GEO_GRASS_SKYPLANE,
		GEO_MONSTER_BIRD,
		GEO_MONSTER_RABBIT,
		GEO_BOSS_FAIRY,
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
        GEO_MEAT,

		//SceneSwamp
		GEO_SWAMP_TERRAIN,
		GEO_SWAMP_SKYPLANE,
        GEO_SWAMP_MUD,
        GEO_SWAMP_PLANT,
        GEO_SWAMP_ROOT,
        GEO_SWAMP_DEADTREE,
        GEO_MONSTER_SEAMONSTER,
        GEO_MONSTER_GRIMEJAM,
        GEO_BOSS_MUKBOSS,

		//SceneRock
		GEO_ROCK_TERRAIN,
		GEO_ROCK_SKYPLANE,
        GEO_ROCK_BUSH,
        GEO_ROCK_ROCK,
        GEO_ROCK_PILLAR,
        GEO_MONSTER_FOSSIL,
        GEO_MONSTER_GOLEM,
        GEO_BOSS_ROCKSNAKE,

		//SceneLava
		GEO_LAVA_TERRAIN,
		GEO_LAVA,
		GEO_RED_CRYSTAL,
		GEO_LAVA_SKYPLANE,
		GEO_MONSTER_FIREBUG,
		GEO_MONSTER_MAGMA,
		GEO_BOSS_MAGMA_BERZEKER,
		GEO_VOLCANO,

        GEO_TEXT_IMPACT,

		// Bonus
		GEO_MONEY_TREE,
		GEO_COIN,

		// Capturing monsters
		GEO_TEXT_CAPTURE,
		GEO_TEXT_MISS,

		// Bars above monsters
		GEO_HPBAR,
		GEO_CAPTUREBAR,

		// Menu stuffs
		GEO_STARTGAME,
		GEO_STARTGAME_HL,
		GEO_STARTGAME_ICON,

		GEO_LOADGAME,
		GEO_LOADGAME_HL,
		GEO_LOADGAME_ICON,

		GEO_INSTRUCTIONS,
		GEO_INSTRUCTIONS_HL,
		GEO_INSTRUCTIONS_ICON,

		GEO_RESUMEGAME,
		GEO_RESUMEGAME_HL,
		GEO_RESUMEGAME_ICON,

		GEO_QUEST,
		GEO_QUEST_HL,
		GEO_QUEST_ICON,

		GEO_BACKTOTOWN,
		GEO_BACKTOTOWN_HL,
		GEO_BACKTOTOWN_ICON,

		GEO_OPTIONS,
		GEO_OPTIONS_HL,
		GEO_OPTIONS_ICON,

		GEO_STORY,
		GEO_STORY_HL,
		GEO_STORY_ICON,

		GEO_CREDITS,
		GEO_CREDITS_HL,
		GEO_CREDITS_ICON,

		GEO_BACK,
		GEO_BACK_HL,
		GEO_BACK_ICON,

		GEO_EXIT,
		GEO_EXIT_HL,
		GEO_EXIT_ICON,

		GEO_MENUBOARD,

		GEO_GREENBUTTON,

		GEO_PAUSEGAME,

		GEO_GAMETITLE,

		// Particles
		GEO_HIDDENBONUS_PARTICLE,
		GEO_VOLCANOSPARK_PARTICLE,

        // Monster states
        GEO_MONSTERSTATE_EXCLAMATION_MARK,
        GEO_MONSTERSTATE_QUESTION_MARK,
		GEO_MONSTERSTATE_RAMPAGE,

        // Zoo
        GEO_SHOP_BACKGROUND,
        GEO_SHOP_SELECTION,
        GEO_SHOP_SELECTION_ALT,

        GEO_ZOO_GRASS_GROUND,
        GEO_ZOO_SWAMP_GROUND,
        GEO_ZOO_ROCK_GROUND,
        GEO_ZOO_LAVA_GROUND,

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