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
	m_meshList[GEO_BOX_06] = MeshBuilder::GenerateCone("test", Color(1, 0, 0), 36, 1.f, 1.f);
    
	m_meshList[GEO_CUBE] = MeshBuilder::GenerateCube("Cube", Color(0, 1, 0), 1.0f);

	m_meshList[GEO_PLAYERBOX] = MeshBuilder::GenerateCube("Cube", Color(1, 0, 0), 1.0f);

	m_meshList[GEO_GRASS_TERRAIN] = MeshBuilder::GenerateQuad("GRASS_DARKGREEN", Color(0, 1,0), 1.f, 100);
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

	m_meshList[GEO_ROCKS1] = MeshBuilder::GenerateOBJ("Rock", "OBJ//GrassZone//Rocks.obj");
	m_meshList[GEO_ROCKS1]->textureArray[0] = LoadTGA("Image//GrassZone//Rock3.tga");

	m_meshList[GEO_TRAP] = MeshBuilder::GenerateOBJ("Trap", "OBJ//GrassZone//Traps.obj");
	m_meshList[GEO_TRAP]->textureArray[0] = LoadTGA("Image//GrassZone//Steel.tga");

	m_meshList[GEO_BAIT] = MeshBuilder::GenerateCube("Cube", Color(1, 0.75, 0.79), 1.0f);

	m_meshList[GEO_NET] = MeshBuilder::GenerateOBJ("Net", "OBJ//GrassZone//Net.obj");
	m_meshList[GEO_NET]->textureArray[0] = LoadTGA("Image//GrassZone//Net.tga");

	//m_meshList[GEO_ROCKS2] = MeshBuilder::GenerateOBJ("Bird", "OBJ//stone_2.obj");
	//m_meshList[GEO_ROCKS2]->textureArray[0] = LoadTGA("Image//Rock2.tga");

	//m_meshList[GEO_ROCKS3] = MeshBuilder::GenerateOBJ("Bird", "OBJ//stone_3.obj");
	//m_meshList[GEO_ROCKS3]->textureArray[0] = LoadTGA("Image//Rock3.tga");

	//m_meshList[GEO_ROCKS4] = MeshBuilder::GenerateOBJ("Bird", "OBJ//stone_4.obj");
	//m_meshList[GEO_ROCKS4]->textureArray[0] = LoadTGA("Image//Rock4.tga");

	//m_meshList[GEO_ROCKS5] = MeshBuilder::GenerateOBJ("Bird", "OBJ//stone_5.obj");
	//m_meshList[GEO_ROCKS5]->textureArray[0] = LoadTGA("Image//Rock5.tga");

	m_meshList[GEO_SWAMP_TERRAIN] = MeshBuilder::GenerateQuad("Swamp Terrain", Color(1, 1, 1), 1.f, 20);
	m_meshList[GEO_SWAMP_TERRAIN]->textureArray[0] = LoadTGA("Image//SwampZone/swamp_terrain.tga");
	
	m_meshList[GEO_SWAMP_SKYPLANE] = MeshBuilder::GenerateSkyPlane("skyplane", Color(1, 1, 1), 128, 400.0f, 3000.0f, 1.0f, 1.0f);
	m_meshList[GEO_SWAMP_SKYPLANE]->textureArray[0] = LoadTGA("Image//SwampZone//swamp_skyplane.tga");

    m_meshList[GEO_SWAMP_PLANT] = MeshBuilder::GenerateOBJ("swamp plant", "OBJ//SwampZone/swamp_plant.obj");
    //m_meshList[GEO_SWAMP_PLANT] = MeshBuilder::GenerateCone("cone", Color(1, 1, 1), 36, 1.f, 1.f);
    m_meshList[GEO_SWAMP_PLANT]->textureArray[0] = LoadTGA("Image//SwampZone/swamp_plant.tga");

    m_meshList[GEO_SWAMP_ROOT] = MeshBuilder::GenerateOBJ("swamp root", "OBJ//SwampZone/swamp_root.obj");
    //m_meshList[GEO_SWAMP_ROOT] = MeshBuilder::GenerateCube("cube", Color(1, 1, 0), 1.f);
    m_meshList[GEO_SWAMP_ROOT]->textureArray[0] = LoadTGA("Image//SwampZone/swamp_root.tga");

	m_meshList[GEO_ROCK_TERRAIN] = MeshBuilder::GenerateQuad("Rock Terrain", Color(1, 1, 1), 1.f, 100);
	//m_meshList[GEO_ROCK_TERRAIN]->textureArray[0] = LoadTGA();

	m_meshList[GEO_LAVA_TERRAIN] = MeshBuilder::GenerateQuad("Lava Terrain", Color(1, 1, 1), 1.f, 100);
	//m_meshList[GEO_LAVA_TERRAIN]->textureArray[0] = LoadTGA();

    // Fonts
    m_meshList[GEO_TEXT_IMPACT] = MeshBuilder::GenerateText("Font - Impact", 16, 16);
    m_meshList[GEO_TEXT_IMPACT]->textureID = LoadTGA("Image//Fonts/Font_Impact.tga");
    m_meshList[GEO_TEXT_IMPACT]->LoadFontData("Image//Fonts/FontData_Impact.csv");
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