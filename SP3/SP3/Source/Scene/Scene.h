#ifndef SCENE_H
#define SCENE_H


#include "Mtx44.h"
#include "MatrixStack.h"
#include "../Graphics/Mesh/Mesh.h"
#include "../Graphics/Vertex/Vertex.h"
#include "../General/Camera.h"

#include "../Graphics/GraphicsLoader/GraphicsLoader.h"
#include "../GameObject/GameObject.h"
#include "../Graphics/Particle/Particle.h"

#include <vector>
#include <map>

typedef std::map<char, std::pair<GraphicsLoader::GEOMETRY_TYPE, std::vector<COMPONENTS>> > LevelGenerationMap;

class Scene
{
    friend class MainMenu;

public:
    static LevelGenerationMap m_levelGenerationData;
    static void AddToMap(char tileCount, GraphicsLoader::GEOMETRY_TYPE meshType, const std::vector<COMPONENTS>& componentsList);
    static void LoadLevelGenerationData(const char* file_path);
    static void LoadLevelMap(const char* file_path);

    static char** m_levelMap;    // store for pathfinding

    static const int m_rows = 40;
    static const int m_cols = 40;
    static const int tileSize = 5;

	Scene() {}
	~Scene() {}

	virtual void Init() = 0;
	virtual void Update(double dt) = 0;
	virtual void Render() = 0;
	virtual void Exit() = 0;

	MS modelStack;
	MS viewStack;
	MS projectionStack;

	Camera camera;
	float fps;

    //Item Switch
    bool b_Rocks;
    bool b_Nets;
    bool b_Baits;

    // GAME HUD INVENTORY
    float f_RotateRock;
    float f_RotateNet;
    float f_RotateBait;

    float f_HighlightPos;

    // Generic Updates
    void UpdatePlayer(double dt, World *world);
    void UpdateMonsters(double dt, World *world);
    void DestroyText(World *world);
    void UpdateRockProjectiles(World *world);
    void UpdateNetProjectiles(World *world);
    void UpdateBaitProjectiles(World *world);

    void UpdateBait(World *world, double dt);
    void UpdateTrap(World *world, double dt);
    void CheckMonsterAttack(World *world);

    void ShootRock();
    void ShootNet();
    void ShootBait();
    void PlaceTrap(World *world);

    void UpdateParticles(World *world, double dt);

    bool CheckPickUpCaughtMonster(World *world, GameObject GO);
    virtual bool CheckInteractMoneyTree(World *world, GameObject GO);
    bool CheckPickUpCoin(World *world, GameObject GO);
    bool CheckPickUpMeat(World *world, GameObject GO);
    bool CheckPickUpRock(World *world, GameObject GO);

    bool ViewCheckPosition(Vector3 pos, float degree);
    
    void RenderProjectiles();
    void RenderPressEText(World *world);
    void RenderHUD(World *world);
    void RenderParticle(ParticleObject *particle);

	// To render
	// ideal case is put objects in a vector; then use for loop to go through the list to render them

	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextCentralised(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	void RenderMesh(Mesh *mesh, bool enableLight);
	//void RenderMeshIn2D(Mesh *mesh, bool enableLight, float size = 1.0f, float x = 0.0f, float y = 0.0f, bool rotate = false, float angle = 0.0f);
    void RenderMeshIn2D(Mesh *mesh, bool enableLight, float sizeX, float sizeY, float x, float y);
	void RenderUI(Mesh* mesh, float size, float x, float y, float rotatex = 0, float rotatey = 0, float rotatez = 0, bool enableLight = false);
    void RenderUI(Mesh* mesh, float sizex, float sizey, float sizez, float x, float y, float rotatex, float rotatey, float rotatez, bool enableLight);

	bool m_bIsInOrthogonalMode;
	bool SetHUD(const bool m_bHUDmode);

    //Gameobject handlers
    void RenderGameObjects(World* world);
    void UpdateGameObjects(World* world, double dt);
};

#endif