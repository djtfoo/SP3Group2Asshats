#include "../GraphicsLoader/GraphicsLoader.h"
#include "../../shader.hpp"
#include "../Mesh/MeshBuilder.h"
#include "../LoadOBJ/LoadTGA.h"
#include "../LoadOBJ/LoadOBJ.h"

GraphicsLoader::GraphicsLoader()
{
}

GraphicsLoader::~GraphicsLoader()
{
}

void GraphicsLoader::Init()
{
	// Black background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	m_programID = LoadShaders("Shader//Shadow.vertexshader", "Shader//Shadow.fragmentshader");

	// Get a handle for our uniform
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	//m_parameters[U_MODEL] = glGetUniformLocation(m_programID, "M");
	//m_parameters[U_VIEW] = glGetUniformLocation(m_programID, "V");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");
	m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
	m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
	m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
	m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
	m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
	m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
	m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
	m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
	m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
	m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
	m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");
	// Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled[0]");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture[0]");
	m_parameters[U_COLOR_TEXTURE_ENABLED1] = glGetUniformLocation(m_programID, "colorTextureEnabled[1]");
	m_parameters[U_COLOR_TEXTURE1] = glGetUniformLocation(m_programID, "colorTexture[1]");
	// Get a handle for our "textColor" uniform
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

	m_parameters[U_FOG_COLOR] = glGetUniformLocation(m_programID, "fogParam.color");
	m_parameters[U_FOG_START] = glGetUniformLocation(m_programID, "fogParam.start");
	m_parameters[U_FOG_END] = glGetUniformLocation(m_programID, "fogParam.end");
	m_parameters[U_FOG_DENSITY] = glGetUniformLocation(m_programID, "fogParam.density");
	m_parameters[U_FOG_TYPE] = glGetUniformLocation(m_programID, "fogParam.type");
	m_parameters[U_FOG_ENABLED] = glGetUniformLocation(m_programID, "fogParam.enabled");

	// Use our shader
	glUseProgram(m_programID);

	m_lights.type = Light::LIGHT_DIRECTIONAL;
	m_lights.position.Set(0, 100, 10);
	m_lights.color.Set(1, 1, 1);
	m_lights.power = 1.f;
	m_lights.kC = 1.f;
	m_lights.kL = 0.01f;
	m_lights.kQ = 0.001f;
	m_lights.cosCutoff = cos(Math::DegreeToRadian(45));
	m_lights.cosInner = cos(Math::DegreeToRadian(30));
	m_lights.exponent = 3.f;
	m_lights.spotDirection.Set(0.f, 1.f, 0.f);

	glUniform1i(m_parameters[U_NUMLIGHTS], 1);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);

	glUniform1i(m_parameters[U_LIGHT0_TYPE], m_lights.type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &m_lights.color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], m_lights.power);
	glUniform1f(m_parameters[U_LIGHT0_KC], m_lights.kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], m_lights.kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], m_lights.kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], m_lights.cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], m_lights.cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], m_lights.exponent);

	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		m_meshList[i] = NULL;
	}
	m_meshList[GEO_AXES] = MeshBuilder::GenerateAxes("axes", 1000, 1000, 1000);
    m_meshList[GEO_CUBE] = MeshBuilder::GenerateCube("Cube", Color(1, 0, 0), 1.0f);
    m_meshList[GEO_PLAYERBOX] = MeshBuilder::GenerateCube("Cube", Color(1, 0, 0), 1.0f);

    // HUD
    m_meshList[GEO_BOX_TRANSLUCENT] = MeshBuilder::GenerateQuad("test", Color(1, 1, 1, 0.6f), 1.f);

	m_meshList[GEO_HUD] = MeshBuilder::GenerateQuad("HUD", Color(1, 1, 1), 1.f);
	m_meshList[GEO_HUD]->textureID = LoadTGA("Image//HUD//HUD.tga");

	m_meshList[GEO_HUDHIGHLIGHT] = MeshBuilder::GenerateQuad("HUDHIGHLIGHT", Color(0, 1, 0), 1.f);
	m_meshList[GEO_HUDHIGHLIGHT]->textureID = LoadTGA("Image//HUD//HudHighlights.tga");

	m_meshList[GEO_HP_UI] = MeshBuilder::GenerateQuad("HP_UI", Color(0, 1, 0), 1.f);
	m_meshList[GEO_HP_UI]->textureID = LoadTGA("Image//HUD//HP_UI.tga");

	m_meshList[GEO_HP] = MeshBuilder::GenerateQuad("HP", Color(1, 0, 0), 1.f);

    // Grass Scene
	m_meshList[GEO_GRASS_TERRAIN] = MeshBuilder::GenerateTerrain("GEO_TERRAIN", "Image//HeightMap.raw",m_heightMapGrass);
	m_meshList[GEO_GRASS_TERRAIN]->textureArray[0] = LoadTGA("Image//GrassZone//Grass.tga");

	m_meshList[GEO_GRASS_SKYPLANE] = MeshBuilder::GenerateSkyPlane("skyplane", Color(1, 1, 1), 128, 400.0f, 3000.0f, 1.0f, 1.0f);
	m_meshList[GEO_GRASS_SKYPLANE]->textureArray[0] = LoadTGA("Image//GrassZone//SkyPlane.tga");

	m_meshList[GEO_MONSTER_RABBIT] = MeshBuilder::GenerateOBJ("Rabbit", "OBJ//GrassZone//Rabbit.obj");
	m_meshList[GEO_MONSTER_RABBIT]->textureArray[0] = LoadTGA("Image//GrassZone//Rabbit.tga");

	m_meshList[GEO_MONSTER_BIRD] = MeshBuilder::GenerateOBJ("Bird", "OBJ//GrassZone//Bird.obj");
	m_meshList[GEO_MONSTER_BIRD]->textureArray[0] = LoadTGA("Image//GrassZone//Bird.tga");

	m_meshList[GEO_GRASS1] = MeshBuilder::GenerateOBJ("Grass1", "OBJ//GrassZone//Grass_02.obj");
    //m_meshList[GEO_GRASS1] = MeshBuilder::GenerateCone("test cube", Color(1, 1, 1), 36, 1.f, 1.f);
	m_meshList[GEO_GRASS1]->textureArray[0] = LoadTGA("Image//GrassZone//Leafs.tga");

	m_meshList[GEO_GRASS2] = MeshBuilder::GenerateOBJ("Grass2", "OBJ//GrassZone//Grass_03.obj");
   // m_meshList[GEO_GRASS2] = MeshBuilder::GenerateCone("test cube", Color(1, 1, 1), 36, 1.f, 1.f);
	m_meshList[GEO_GRASS2]->textureArray[0] = LoadTGA("Image//GrassZone//Leafs.tga");

    m_meshList[GEO_TREE1] = MeshBuilder::GenerateOBJ("Grass2", "OBJ//GrassZone//TreeBush2.obj");
    //m_meshList[GEO_TREE1] = MeshBuilder::GenerateSphere("test cube", Color(1, 1, 1), 18, 36, 1.f);
    m_meshList[GEO_TREE1]->textureArray[0] = LoadTGA("Image//GrassZone//Trees.tga");

    m_meshList[GEO_TREE2] = MeshBuilder::GenerateOBJ("Grass2", "OBJ//GrassZone//TreeBush4.obj");
   // m_meshList[GEO_TREE2] = MeshBuilder::GenerateSphere("test cube", Color(1, 1, 1), 18, 36, 1.f);
    m_meshList[GEO_TREE2]->textureArray[0] = LoadTGA("Image//GrassZone//Trees.tga");

	//meshList[GEO_TREES] = MeshBuilder::GenerateOBJ("Bird", "OBJ//Birdv2.obj");
	//meshList[GEO_TREES]->textureArray[0] = LoadTGA("Image//grass_lightgreen.tga");

	m_meshList[GEO_ROCKS1] = MeshBuilder::GenerateOBJ("Rock", "OBJ//Rocks.obj");
	m_meshList[GEO_ROCKS1]->textureArray[0] = LoadTGA("Image//Rock3.tga");

	m_meshList[GEO_TRAP] = MeshBuilder::GenerateOBJ("Trap", "OBJ//Traps.obj");
	m_meshList[GEO_TRAP]->textureArray[0] = LoadTGA("Image//Steel.tga");

	m_meshList[GEO_BAIT] = MeshBuilder::GenerateCube("Cube", Color(1, 0.75, 0.79), 1.0f);

	m_meshList[GEO_NET] = MeshBuilder::GenerateOBJ("Net", "OBJ//Net.obj");
	m_meshList[GEO_NET]->textureArray[0] = LoadTGA("Image//Net.tga");

    m_meshList[GEO_MEAT] = MeshBuilder::GenerateOBJ("Meat", "OBJ//meat.obj");
    m_meshList[GEO_MEAT]->textureArray[0] = LoadTGA("Image//meat.tga");

	m_meshList[GEO_BOSS_FAIRY] = MeshBuilder::GenerateOBJ("Net", "OBJ//GrassZone//Fairy.obj");
	m_meshList[GEO_BOSS_FAIRY]->textureArray[0] = LoadTGA("Image//GrassZone//Fairy.tga");

	m_meshList[GEO_TEXT_MISS] = MeshBuilder::GenerateQuad("Miss", Color(0, 1, 0), 1.f);
	m_meshList[GEO_TEXT_MISS]->textureArray[0] = LoadTGA("Image//Miss.tga");

	m_meshList[GEO_TEXT_CAPTURE] = MeshBuilder::GenerateQuad("CAPTURE", Color(0, 1, 0), 1.f);
	m_meshList[GEO_TEXT_CAPTURE]->textureArray[0] = LoadTGA("Image//Captured.tga");

    // Swamp Scene
	m_meshList[GEO_SWAMP_TERRAIN] = MeshBuilder::GenerateTerrain("GEO_TERRAIN", "Image//SwampZone/Swamp_HeightMap.raw", m_heightMapSwamp);
	m_meshList[GEO_SWAMP_TERRAIN]->textureArray[0] = LoadTGA("Image//SwampZone/swamp_terrain.tga");
	
	m_meshList[GEO_SWAMP_SKYPLANE] = MeshBuilder::GenerateSkyPlane("skyplane", Color(1, 1, 1), 128, 400.0f, 3000.0f, 1.0f, 1.0f);
	m_meshList[GEO_SWAMP_SKYPLANE]->textureArray[0] = LoadTGA("Image//SwampZone//swamp_skyplane.tga");

    m_meshList[GEO_SWAMP_MUD] = MeshBuilder::GenerateQuad("Swamp Mud", Color(1, 1, 1), 1.f, 10);
    m_meshList[GEO_SWAMP_MUD]->textureArray[0] = LoadTGA("Image//SwampZone/swamp_mud.tga");

    m_meshList[GEO_SWAMP_PLANT] = MeshBuilder::GenerateOBJ("swamp plant", "OBJ//SwampZone/swamp_plant.obj");
    m_meshList[GEO_SWAMP_PLANT]->textureArray[0] = LoadTGA("Image//SwampZone/swamp_plant.tga");

    m_meshList[GEO_SWAMP_ROOT] = MeshBuilder::GenerateOBJ("swamp root", "OBJ//SwampZone/swamp_root.obj");
    m_meshList[GEO_SWAMP_ROOT]->textureArray[0] = LoadTGA("Image//SwampZone/swamp_root.tga");

    m_meshList[GEO_SWAMP_DEADTREE] = MeshBuilder::GenerateOBJ("swamp dead tree", "OBJ//SwampZone/dead tree.obj");
    m_meshList[GEO_SWAMP_DEADTREE]->textureArray[0] = LoadTGA("Image//SwampZone/dead tree 3.tga");

    m_meshList[GEO_MONSTER_SEAMONSTER] = MeshBuilder::GenerateOBJ("SeaMonster", "OBJ//SwampZone/SeaMonster.obj");
    m_meshList[GEO_MONSTER_SEAMONSTER]->textureArray[0] = LoadTGA("Image//SwampZone/SeaMonster.tga");

    m_meshList[GEO_MONSTER_GRIMEJAM] = MeshBuilder::GenerateOBJ("Grimejam", "OBJ//SwampZone/Grimejam.obj");
    m_meshList[GEO_MONSTER_GRIMEJAM]->textureArray[0] = LoadTGA("Image//SwampZone/Grimejam.tga");

    m_meshList[GEO_BOSS_MUKBOSS] = MeshBuilder::GenerateOBJ("Rabbit", "OBJ//SwampZone/MukBoss.obj");
    m_meshList[GEO_BOSS_MUKBOSS]->textureArray[0] = LoadTGA("Image//SwampZone/MukBoss.tga");

    // Rock Scene
    m_meshList[GEO_ROCK_TERRAIN] = MeshBuilder::GenerateTerrain("GEO_TERRAIN", "Image//HeightMap.raw", m_heightMapRock);
    m_meshList[GEO_ROCK_TERRAIN]->textureArray[0] = LoadTGA("Image//RockZone/RockZone_Terrain.tga");

    m_meshList[GEO_ROCK_SKYPLANE] = MeshBuilder::GenerateSkyPlane("Rock Skyplane", Color(1, 1, 1), 128, 400.0f, 3000.0f, 1.0f, 1.0f);
    m_meshList[GEO_ROCK_SKYPLANE]->textureArray[0] = LoadTGA("Image//RockZone/RockZone_Skyplane.tga");

    m_meshList[GEO_ROCK_BUSH] = MeshBuilder::GenerateOBJ("Volcano", "OBJ//RockZone/RockZone_Bush.obj");
    m_meshList[GEO_ROCK_BUSH]->textureArray[0] = LoadTGA("Image//RockZone/RockZone_OldGrass.tga");

    m_meshList[GEO_ROCK_ROCK] = MeshBuilder::GenerateOBJ("Volcano", "OBJ//RockZone/RockZone_Rocks.obj");
    m_meshList[GEO_ROCK_ROCK]->textureArray[0] = LoadTGA("Image//RockZone/RockZone_Rocks.tga");

    m_meshList[GEO_ROCK_PILLAR] = MeshBuilder::GenerateOBJ("Volcano", "OBJ//RockZone/RockZone_Pillar.obj");
    m_meshList[GEO_ROCK_PILLAR]->textureArray[0] = LoadTGA("Image//RockZone/RockZone_Pillar.tga");

    m_meshList[GEO_MONSTER_FOSSIL] = MeshBuilder::GenerateOBJ("Volcano", "OBJ//RockZone/Fossil.obj");
    m_meshList[GEO_MONSTER_FOSSIL]->textureArray[0] = LoadTGA("Image//RockZone/Fossil.tga");

    m_meshList[GEO_MONSTER_GOLEM] = MeshBuilder::GenerateOBJ("Volcano", "OBJ//RockZone/Golem.obj");
    m_meshList[GEO_MONSTER_GOLEM]->textureArray[0] = LoadTGA("Image//RockZone/Golem.tga");

    m_meshList[GEO_BOSS_ROCKSNAKE] = MeshBuilder::GenerateOBJ("Volcano", "OBJ//RockZone/RockSnake.obj");
    m_meshList[GEO_BOSS_ROCKSNAKE]->textureArray[0] = LoadTGA("Image//RockZone/RockSnake.tga");

    // Lava Scene
	m_meshList[GEO_LAVA_TERRAIN] = MeshBuilder::GenerateTerrain("GEO_TERRAIN", "Image//FireZone/heightmaplava.raw", m_heightMapLava);
	m_meshList[GEO_LAVA_TERRAIN]->textureArray[0] = LoadTGA("Image//FireZone/lava_terrain.tga");

	m_meshList[GEO_LAVA] = MeshBuilder::GenerateQuad("GEO_TERRAIN", Color(1,1,1), 1.f, 10);
	m_meshList[GEO_LAVA]->textureArray[0] = LoadTGA("Image//FireZone/LavaTexture.tga");

	m_meshList[GEO_LAVA_SKYPLANE] = MeshBuilder::GenerateSkyPlane("skyplane", Color(1, 1, 1), 128, 400.0f, 3000.0f, 1.0f, 1.0f);
	m_meshList[GEO_LAVA_SKYPLANE]->textureArray[0] = LoadTGA("Image//FireZone//fire_skyplane.tga");

	m_meshList[GEO_VOLCANO] = MeshBuilder::GenerateOBJ("Volcano", "OBJ//FireZone/Volcano.obj");
	m_meshList[GEO_VOLCANO]->textureArray[0] = LoadTGA("Image//FireZone/volcano_terrain.tga");

	m_meshList[GEO_MONSTER_MAGMA] = MeshBuilder::GenerateOBJ("Magma", "OBJ//FireZone/Magma.obj");
	m_meshList[GEO_MONSTER_MAGMA]->textureArray[0] = LoadTGA("Image//FireZone/Magma.tga");

	m_meshList[GEO_MONSTER_FIREBUG] = MeshBuilder::GenerateOBJ("Magma", "OBJ//FireZone/FireBug.obj");
	m_meshList[GEO_MONSTER_FIREBUG]->textureArray[0] = LoadTGA("Image//FireZone/FireBug.tga");

	m_meshList[GEO_BOSS_MAGMA_BERZEKER] = MeshBuilder::GenerateOBJ("Magma", "OBJ//FireZone/MagmaBezerker.obj");
	m_meshList[GEO_BOSS_MAGMA_BERZEKER]->textureArray[0] = LoadTGA("Image//FireZone/MagmaBezerker.tga");

	// Bonuses
	m_meshList[GEO_MONEY_TREE] = MeshBuilder::GenerateOBJ("Money Tree", "OBJ//GrassZone/BonusTree.obj");
	m_meshList[GEO_MONEY_TREE]->textureArray[0] = LoadTGA("Image//GrassZone/BonusTree.tga");

	m_meshList[GEO_COIN] = MeshBuilder::GenerateOBJ("Coins", "OBJ//coin.obj");
	m_meshList[GEO_COIN]->textureArray[0] = LoadTGA("Image//coin.tga");

	m_meshList[GEO_RED_CRYSTAL] = MeshBuilder::GenerateOBJ("Red Crystal", "OBJ//FireZone/RedCrystal.obj");
	m_meshList[GEO_RED_CRYSTAL]->textureArray[0] = LoadTGA("Image//FireZone/RedCrystal.tga");

	// Bars above monsters
	m_meshList[GEO_HPBAR] = MeshBuilder::GenerateQuad("HP BAR", Color(1, 0, 0), 1.f);
	//m_meshList[GEO_CAPTUREBAR] = MeshBuilder::GenerateSphere("CAPTURE BAR", Color(0.5f, 0, 0.5f), 18, 36, 1.f);
    m_meshList[GEO_CAPTUREBAR] = MeshBuilder::GenerateRing("CAPTURE BAR", Color(0.5f, 0, 1.f), 36);

	// Menu stuffs
	m_meshList[GEO_STARTGAME] = MeshBuilder::GenerateQuad("NewGame", Color(1, 0, 0), 1.f);
	m_meshList[GEO_STARTGAME]->textureID = LoadTGA("Image//MenuText//NewGame.tga");

	m_meshList[GEO_STARTGAME_HL] = MeshBuilder::GenerateQuad("NewGameHL", Color(1, 0, 0), 1.f);
	m_meshList[GEO_STARTGAME_HL]->textureID = LoadTGA("Image//MenuText//NewGameHL.tga");

	m_meshList[GEO_LOADGAME] = MeshBuilder::GenerateQuad("LoadGame", Color(1, 0, 0), 1.f);
	m_meshList[GEO_LOADGAME]->textureID = LoadTGA("Image//MenuText//LoadGame.tga");

	m_meshList[GEO_LOADGAME_HL] = MeshBuilder::GenerateQuad("LoadGameHL", Color(1, 0, 0), 1.f);
	m_meshList[GEO_LOADGAME_HL]->textureID = LoadTGA("Image//MenuText//LoadGameHL.tga");

	m_meshList[GEO_INSTRUCTIONS] = MeshBuilder::GenerateQuad("Intructions", Color(1, 0, 0), 1.f);
	m_meshList[GEO_INSTRUCTIONS]->textureID = LoadTGA("Image//MenuText//Intructions.tga");

	m_meshList[GEO_INSTRUCTIONS_HL] = MeshBuilder::GenerateQuad("IntructionsH/l", Color(1, 0, 0), 1.f);
	m_meshList[GEO_INSTRUCTIONS_HL]->textureID = LoadTGA("Image//MenuText//IntructionsHL.tga");

	m_meshList[GEO_RESUMEGAME] = MeshBuilder::GenerateQuad("ResumeGame", Color(1, 0, 0), 1.f);
	m_meshList[GEO_RESUMEGAME]->textureID = LoadTGA("Image//MenuText//ResumeGame.tga");

	m_meshList[GEO_RESUMEGAME_HL] = MeshBuilder::GenerateQuad("ResumeGameHL", Color(1, 0, 0), 1.f);
	m_meshList[GEO_RESUMEGAME_HL]->textureID = LoadTGA("Image//MenuText//ResumeGameHL.tga");

	m_meshList[GEO_QUEST] = MeshBuilder::GenerateQuad("Quest", Color(1, 0, 0), 1.f);
	m_meshList[GEO_QUEST]->textureID = LoadTGA("Image//MenuText//Quest.tga");

	m_meshList[GEO_QUEST_HL] = MeshBuilder::GenerateQuad("QuestHL", Color(1, 0, 0), 1.f);
	m_meshList[GEO_QUEST_HL]->textureID = LoadTGA("Image//MenuText//QuestHL.tga");

	m_meshList[GEO_BACKTOTOWN] = MeshBuilder::GenerateQuad("Backtotown", Color(1, 0, 0), 1.f);
	m_meshList[GEO_BACKTOTOWN]->textureID = LoadTGA("Image//MenuText//Backtotown.tga");

	m_meshList[GEO_BACKTOTOWN_HL] = MeshBuilder::GenerateQuad("BacktotownHL", Color(1, 0, 0), 1.f);
	m_meshList[GEO_BACKTOTOWN_HL]->textureID = LoadTGA("Image//MenuText//BacktotownHL.tga");

	m_meshList[GEO_OPTIONS] = MeshBuilder::GenerateQuad("Options", Color(1, 0, 0), 1.f);
	m_meshList[GEO_OPTIONS]->textureID = LoadTGA("Image//MenuText//Options.tga");

	m_meshList[GEO_OPTIONS_HL] = MeshBuilder::GenerateQuad("OptionsHL", Color(1, 0, 0), 1.f);
	m_meshList[GEO_OPTIONS_HL]->textureID = LoadTGA("Image//MenuText//OptionsHL.tga");

	m_meshList[GEO_STORY] = MeshBuilder::GenerateQuad("Story", Color(1, 0, 0), 1.f);
	m_meshList[GEO_STORY]->textureID = LoadTGA("Image//MenuText//Story.tga");

	m_meshList[GEO_STORY_HL] = MeshBuilder::GenerateQuad("StoryHL", Color(1, 0, 0), 1.f);
	m_meshList[GEO_STORY_HL]->textureID = LoadTGA("Image//MenuText//StoryHL.tga");

	m_meshList[GEO_CREDITS] = MeshBuilder::GenerateQuad("Credits", Color(1, 0, 0), 1.f);
	m_meshList[GEO_CREDITS]->textureID = LoadTGA("Image//MenuText//Credits.tga");

	m_meshList[GEO_CREDITS_HL] = MeshBuilder::GenerateQuad("CreditsHL", Color(1, 0, 0), 1.f);
	m_meshList[GEO_CREDITS_HL]->textureID = LoadTGA("Image//MenuText//CreditsHL.tga");

	m_meshList[GEO_EXIT] = MeshBuilder::GenerateQuad("Exit", Color(1, 0, 0), 1.f);
	m_meshList[GEO_EXIT]->textureID = LoadTGA("Image//MenuText//Exit.tga");

	m_meshList[GEO_EXIT_HL] = MeshBuilder::GenerateQuad("ExitHL", Color(1, 0, 0), 1.f);
	m_meshList[GEO_EXIT_HL]->textureID = LoadTGA("Image//MenuText//ExitHL.tga");

	m_meshList[GEO_BACK] = MeshBuilder::GenerateQuad("Back", Color(1, 0, 0), 1.f);
	m_meshList[GEO_BACK]->textureID = LoadTGA("Image//MenuText//Back.tga");

	m_meshList[GEO_BACK_HL] = MeshBuilder::GenerateQuad("BackHL", Color(1, 0, 0), 1.f);
	m_meshList[GEO_BACK_HL]->textureID = LoadTGA("Image//MenuText//BackHL.tga");

	m_meshList[GEO_MENUBOARD] = MeshBuilder::GenerateQuad("MenuBoard", Color(1, 0, 0), 1.f);
	m_meshList[GEO_MENUBOARD]->textureID = LoadTGA("Image//MenuText//MenuBoard.tga");

	m_meshList[GEO_GREENBUTTON] = MeshBuilder::GenerateQuad("GreenBoard", Color(1, 0, 0), 1.f);
	m_meshList[GEO_GREENBUTTON]->textureID = LoadTGA("Image//MenuText//GreenButton.tga");

	m_meshList[GEO_PAUSEGAME] = MeshBuilder::GenerateQuad("PauseGame", Color(1, 0, 0), 1.f);
	m_meshList[GEO_PAUSEGAME]->textureID = LoadTGA("Image//MenuText//PauseGame.tga");

	m_meshList[GEO_GAMETITLE] = MeshBuilder::GenerateQuad("GameTitle", Color(1, 0, 0), 1.f);
	m_meshList[GEO_GAMETITLE]->textureID = LoadTGA("Image//MenuText//GameTitle.tga");
	
	m_meshList[GEO_CONTROLS] = MeshBuilder::GenerateQuad("Controls", Color(1, 0, 0), 1.f);
	m_meshList[GEO_CONTROLS]->textureID = LoadTGA("Image//MenuText//Controls.tga");

	//Menu & Pause Icon 

	m_meshList[GEO_STARTGAME_ICON] = MeshBuilder::GenerateQuad("NewGameIcon", Color(1, 0, 0), 1.f);
	m_meshList[GEO_STARTGAME_ICON]->textureID = LoadTGA("Image//MenuText//NewGameIcon.tga");

	m_meshList[GEO_LOADGAME_ICON] = MeshBuilder::GenerateQuad("LoadGameIcon", Color(1, 0, 0), 1.f);
	m_meshList[GEO_LOADGAME_ICON]->textureID = LoadTGA("Image//MenuText//LoadGameIcon.tga");

	m_meshList[GEO_INSTRUCTIONS_ICON] = MeshBuilder::GenerateQuad("IntructionsIcon", Color(1, 0, 0), 1.f);
	m_meshList[GEO_INSTRUCTIONS_ICON]->textureID = LoadTGA("Image//MenuText//IntructionsIcon.tga");

	m_meshList[GEO_RESUMEGAME_ICON] = MeshBuilder::GenerateQuad("ResumeGameIcon", Color(1, 0, 0), 1.f);
	m_meshList[GEO_RESUMEGAME_ICON]->textureID = LoadTGA("Image//MenuText//ResumeGameIcon.tga");

	m_meshList[GEO_QUEST_ICON] = MeshBuilder::GenerateQuad("QuestIcon", Color(1, 0, 0), 1.f);
	m_meshList[GEO_QUEST_ICON]->textureID = LoadTGA("Image//MenuText//QuestIcon.tga");

	m_meshList[GEO_BACKTOTOWN_ICON] = MeshBuilder::GenerateQuad("BacktotownIcon", Color(1, 0, 0), 1.f);
	m_meshList[GEO_BACKTOTOWN_ICON]->textureID = LoadTGA("Image//MenuText//BacktotownIcon.tga");

	m_meshList[GEO_OPTIONS_ICON] = MeshBuilder::GenerateQuad("OptionsIcon", Color(1, 0, 0), 1.f);
	m_meshList[GEO_OPTIONS_ICON]->textureID = LoadTGA("Image//MenuText//OptionsIcon.tga");

	m_meshList[GEO_STORY_ICON] = MeshBuilder::GenerateQuad("StoryIcon", Color(1, 0, 0), 1.f);
	m_meshList[GEO_STORY_ICON]->textureID = LoadTGA("Image//MenuText//StoryIcon.tga");

	m_meshList[GEO_CREDITS_ICON] = MeshBuilder::GenerateQuad("CreditsIcon", Color(1, 0, 0), 1.f);
	m_meshList[GEO_CREDITS_ICON]->textureID = LoadTGA("Image//MenuText//CreditsIcon.tga");

	m_meshList[GEO_EXIT_ICON] = MeshBuilder::GenerateQuad("ExitIcon", Color(1, 0, 0), 1.f);
	m_meshList[GEO_EXIT_ICON]->textureID = LoadTGA("Image//MenuText//ExitIcon.tga");

	m_meshList[GEO_BACK_ICON] = MeshBuilder::GenerateQuad("BackIcon", Color(1, 0, 0), 1.f);
	m_meshList[GEO_BACK_ICON]->textureID = LoadTGA("Image//MenuText//BackIcon.tga");
	//End Menu & Pause stuffs


	// Monster State
    m_meshList[GEO_MONSTERSTATE_EXCLAMATION_MARK] = MeshBuilder::GenerateQuad("Exclamation mark", Color(1, 0, 0), 1.f);
    m_meshList[GEO_MONSTERSTATE_EXCLAMATION_MARK]->textureArray[0] = LoadTGA("Image//monster_exclamation.tga");

    m_meshList[GEO_MONSTERSTATE_QUESTION_MARK] = MeshBuilder::GenerateQuad("Question Mark", Color(0, 1, 0), 1.f);
    m_meshList[GEO_MONSTERSTATE_QUESTION_MARK]->textureArray[0] = LoadTGA("Image//monster_questionmark.tga");

	m_meshList[GEO_MONSTERSTATE_RAMPAGE] = MeshBuilder::GenerateQuad("Rampage State", Color(0, 1, 0), 1.f);
	m_meshList[GEO_MONSTERSTATE_RAMPAGE]->textureArray[0] = LoadTGA("Image//monster_rampage.tga");

    m_meshList[GEO_MONSTERSTATE_TRAPPED] = MeshBuilder::GenerateQuad("Trapped State", Color(0, 1, 0), 1.f);
    m_meshList[GEO_MONSTERSTATE_TRAPPED]->textureArray[0] = LoadTGA("Image//monster_trapped.tga");

    // Zoo
    m_meshList[GEO_ZOO_GRASS_GROUND] = MeshBuilder::GenerateQuad("Grass Ground", Color(0, 1, 0), 1.f, 10);
    m_meshList[GEO_ZOO_GRASS_GROUND]->textureArray[0] = LoadTGA("Image//GrassZone/Grass.tga");

    m_meshList[GEO_ZOO_SWAMP_GROUND] = MeshBuilder::GenerateQuad("Grass Ground", Color(0, 1, 0), 1.f, 10);
    m_meshList[GEO_ZOO_SWAMP_GROUND]->textureArray[0] = LoadTGA("Image//SwampZone/swamp_terrain.tga");

    m_meshList[GEO_ZOO_ROCK_GROUND] = MeshBuilder::GenerateQuad("Grass Ground", Color(0, 1, 0), 1.f, 10);
    m_meshList[GEO_ZOO_ROCK_GROUND]->textureArray[0] = LoadTGA("Image//RockZone/RockZone_Terrain.tga");

    m_meshList[GEO_ZOO_LAVA_GROUND] = MeshBuilder::GenerateQuad("Grass Ground", Color(0, 1, 0), 1.f, 10);
    m_meshList[GEO_ZOO_LAVA_GROUND]->textureArray[0] = LoadTGA("Image//FireZone/lava_terrain.tga");

    m_meshList[GEO_ZOO_DIRT_GROUND] = MeshBuilder::GenerateQuad("Dirt Ground", Color(0, 1, 0), 1.f, 10);
    m_meshList[GEO_ZOO_DIRT_GROUND]->textureArray[0] = LoadTGA("Image//ZooIcons/DirtGround.tga");

    // Shop stuff
    m_meshList[GEO_SHOP_BACKGROUND] = MeshBuilder::GenerateQuad("Shop Background", Color(0.5f, 0.5f, 0.5f));
    m_meshList[GEO_SHOP_BACKGROUND]->textureArray[0] = LoadTGA("Image//ZooIcons/ShopBackground.tga");

    m_meshList[GEO_SHOP_SELECTION] = MeshBuilder::GenerateQuad("Shop selection", Color(0, 0, 0));
    m_meshList[GEO_SHOP_SELECTION]->textureArray[0] = LoadTGA("Image//MenuText/MenuBoard.tga");

    m_meshList[GEO_SHOP_SELECTION_ALT] = MeshBuilder::GenerateQuad("Shop selection alt", Color(1, 1, 1));
    m_meshList[GEO_SHOP_SELECTION_ALT]->textureArray[0] = LoadTGA("Image//MenuText/MenuBoardHL.tga");

    m_meshList[GEO_ENCLOSURE_INTERFACE] = MeshBuilder::GenerateQuad("Enclosure interface", Color(1, 1, 1));
    m_meshList[GEO_ENCLOSURE_INTERFACE]->textureArray[0] = LoadTGA("Image//ZooIcons/EnclosureInterface.tga");

    m_meshList[GEO_ICON_LEFT_ARROW] = MeshBuilder::GenerateQuad("Arrow icon left", Color(1, 1, 1));
    m_meshList[GEO_ICON_LEFT_ARROW]->textureArray[0] = LoadTGA("Image//ZooIcons/ArrowIconLeft.tga");

    m_meshList[GEO_ICON_RIGHT_ARROW] = MeshBuilder::GenerateQuad("Arrow icon right", Color(1, 1, 1));
    m_meshList[GEO_ICON_RIGHT_ARROW]->textureArray[0] = LoadTGA("Image//ZooIcons/ArrowIconRight.tga");

    m_meshList[GEO_ICON_EYE] = MeshBuilder::GenerateQuad("eye icon", Color(1, 1, 1));
    m_meshList[GEO_ICON_EYE]->textureArray[0] = LoadTGA("Image//ZooIcons/EyeIcon.tga");

    m_meshList[GEO_FENCE] = MeshBuilder::GenerateOBJ("fence", "OBJ//Fence.obj");
    m_meshList[GEO_FENCE]->textureArray[0] = LoadTGA("Image//ZooIcons/FenceTexture.tga");

    m_meshList[GEO_HUNTING_GROUNDS_INTERFACE] = MeshBuilder::GenerateQuad("Hunting grounds interface", Color(1, 1, 1));
    m_meshList[GEO_HUNTING_GROUNDS_INTERFACE]->textureArray[0] = LoadTGA("Image//ZooIcons/HuntingGroundsInterface.tga");

    m_meshList[GEO_ICON_GRASS] = MeshBuilder::GenerateQuad("grass icon", Color(1, 1, 1));
    m_meshList[GEO_ICON_GRASS]->textureArray[0] = LoadTGA("Image//ZooIcons/GrassIcon.tga");

    m_meshList[GEO_ICON_FIRE] = MeshBuilder::GenerateQuad("fire icon", Color(1, 1, 1));
    m_meshList[GEO_ICON_FIRE]->textureArray[0] = LoadTGA("Image//ZooIcons/FireIcon.tga");

    m_meshList[GEO_ICON_ROCK] = MeshBuilder::GenerateQuad("rock icon", Color(1, 1, 1));
    m_meshList[GEO_ICON_ROCK]->textureArray[0] = LoadTGA("Image//ZooIcons/RockIcon.tga");

    m_meshList[GEO_ICON_SWAMP] = MeshBuilder::GenerateQuad("swamp icon", Color(1, 1, 1));
    m_meshList[GEO_ICON_SWAMP]->textureArray[0] = LoadTGA("Image//ZooIcons/SwampIcon.tga");

    m_meshList[GEO_ICON_GRASS_ALT] = MeshBuilder::GenerateQuad("grass icon Alt", Color(1, 1, 1));
    m_meshList[GEO_ICON_GRASS_ALT]->textureArray[0] = LoadTGA("Image//ZooIcons/GrassIconAlt.tga");

    m_meshList[GEO_ICON_FIRE_ALT] = MeshBuilder::GenerateQuad("fire icon Alt", Color(1, 1, 1));
    m_meshList[GEO_ICON_FIRE_ALT]->textureArray[0] = LoadTGA("Image//ZooIcons/FireIconAlt.tga");

    m_meshList[GEO_ICON_ROCK_ALT] = MeshBuilder::GenerateQuad("rock icon Alt", Color(1, 1, 1));
    m_meshList[GEO_ICON_ROCK_ALT]->textureArray[0] = LoadTGA("Image//ZooIcons/RockIconAlt.tga");

    m_meshList[GEO_ICON_SWAMP_ALT] = MeshBuilder::GenerateQuad("swamp icon Alt", Color(1, 1, 1));
    m_meshList[GEO_ICON_SWAMP_ALT]->textureArray[0] = LoadTGA("Image//ZooIcons/SwampIconAlt.tga");

    m_meshList[GEO_GRASSZONE_CAPTURE] = MeshBuilder::GenerateQuad("grass scene icon", Color(1, 1, 1));
    m_meshList[GEO_GRASSZONE_CAPTURE]->textureArray[0] = LoadTGA("Image//ZooIcons/GrassScene.tga");

    m_meshList[GEO_GRASSZONE_CAPTURE_ALT] = MeshBuilder::GenerateQuad("grass scene icon alt", Color(1, 1, 1));
    m_meshList[GEO_GRASSZONE_CAPTURE_ALT]->textureArray[0] = LoadTGA("Image//ZooIcons/GrassSceneAlt.tga");

    m_meshList[GEO_FIREZONE_CAPTURE] = MeshBuilder::GenerateQuad("fire scene icon", Color(1, 1, 1));
    m_meshList[GEO_FIREZONE_CAPTURE]->textureArray[0] = LoadTGA("Image//ZooIcons/LavaScene.tga");

    m_meshList[GEO_FIREZONE_CAPTURE_ALT] = MeshBuilder::GenerateQuad("fire scene icon alt", Color(1, 1, 1));
    m_meshList[GEO_FIREZONE_CAPTURE_ALT]->textureArray[0] = LoadTGA("Image//ZooIcons/LavaSceneAlt.tga");

    m_meshList[GEO_ROCKZONE_CAPTURE] = MeshBuilder::GenerateQuad("rock scene icon", Color(1, 1, 1));
    m_meshList[GEO_ROCKZONE_CAPTURE]->textureArray[0] = LoadTGA("Image//ZooIcons/RockScene.tga");

    m_meshList[GEO_ROCKZONE_CAPTURE_ALT] = MeshBuilder::GenerateQuad("rock scene icon alt", Color(1, 1, 1));
    m_meshList[GEO_ROCKZONE_CAPTURE_ALT]->textureArray[0] = LoadTGA("Image//ZooIcons/RockSceneAlt.tga");

    m_meshList[GEO_SWAMPZONE_CAPTURE] = MeshBuilder::GenerateQuad("swamp scene icon", Color(1, 1, 1));
    m_meshList[GEO_SWAMPZONE_CAPTURE]->textureArray[0] = LoadTGA("Image//ZooIcons/SwampScene.tga");

    m_meshList[GEO_SWAMPZONE_CAPTURE_ALT] = MeshBuilder::GenerateQuad("swamp scene icon alt", Color(1, 1, 1));
    m_meshList[GEO_SWAMPZONE_CAPTURE_ALT]->textureArray[0] = LoadTGA("Image//ZooIcons/SwampSceneAlt.tga");

	// Particle Loading
	m_meshList[GEO_HIDDENBONUS_PARTICLE] = MeshBuilder::GenerateQuad("Hidden Bonus", Color(0, 1, 0), 1.f);
	m_meshList[GEO_HIDDENBONUS_PARTICLE]->textureArray[0] = LoadTGA("Image//Particles/HiddenBonus.tga");

	m_meshList[GEO_VOLCANOSPARK_PARTICLE] = MeshBuilder::GenerateQuad("Volcano Spark", Color(0, 1, 0), 1.f);
	m_meshList[GEO_VOLCANOSPARK_PARTICLE]->textureArray[0] = LoadTGA("Image//Particles/VolcanoSpark.tga");

	m_meshList[GEO_FALLINGLEAF_PARTICLE] = MeshBuilder::GenerateQuad("Falling Leaf", Color(0, 1, 0), 1.f);
	m_meshList[GEO_FALLINGLEAF_PARTICLE]->textureArray[0] = LoadTGA("Image//Particles/LeafParticle.tga");

	m_meshList[GEO_DUST_PARTICLE] = MeshBuilder::GenerateQuad("Dust", Color(0, 1, 0), 1.f);
	m_meshList[GEO_DUST_PARTICLE]->textureArray[0] = LoadTGA("Image//Particles/DustParticle.tga");

	m_meshList[GEO_FALLINGROCK_PARTICLE] = MeshBuilder::GenerateQuad("Falling Rocks", Color(0, 1, 0), 1.f);
	m_meshList[GEO_FALLINGROCK_PARTICLE]->textureArray[0] = LoadTGA("Image//Particles/RockParticle.tga");

	m_meshList[GEO_MUDBUBBLE_PARTICLE] = MeshBuilder::GenerateQuad("Mud Bubble", Color(0, 1, 0), 1.f);
	m_meshList[GEO_MUDBUBBLE_PARTICLE]->textureArray[0] = LoadTGA("Image//Particles/MudBubbleParticle.tga");

    // Fonts
    m_meshList[GEO_TEXT_IMPACT] = MeshBuilder::GenerateText("Font - Impact", 16, 16);
    m_meshList[GEO_TEXT_IMPACT]->textureID = LoadTGA("Image//Fonts/Font_Impact.tga");
    m_meshList[GEO_TEXT_IMPACT]->LoadFontData("Image//Fonts/FontData_Impact.csv");
}

void GraphicsLoader::Exit()
{
    // Cleanup VBO
    for (int i = 0; i < NUM_GEOMETRY; ++i)
    {
        if (m_meshList[i])
            delete m_meshList[i];
    }
}

Mesh* GraphicsLoader::GetMesh(GEOMETRY_TYPE m_geotype)
{
	return this->m_meshList[m_geotype];
}

unsigned GraphicsLoader::GetParameters(UNIFORM_TYPE parameter)
{
	return m_parameters[parameter];
}

unsigned GraphicsLoader::GetProgramID()
{
	return this->m_programID;
}

Light GraphicsLoader::GetLights()
{
	return m_lights;
}