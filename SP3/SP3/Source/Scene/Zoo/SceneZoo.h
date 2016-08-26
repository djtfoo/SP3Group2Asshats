#ifndef SCENE_ZOO_H
#define SCENE_ZOO_H

#include "../Scene.h"
#include "../../GameObject/ItemProjectiles.h"
#include "../../General/ZooCamera.h"
#include "../../General/MousePicker.h"

class SceneZoo : public Scene
{
public:
    SceneZoo();
    ~SceneZoo();

    virtual void Init();
    virtual void Update(double dt);
    virtual void Render();
    virtual void RenderZooScene();
    virtual void Exit();

    void populateMonsterList();

    World zooWorld;
    ZooCamera zooCamera;

    std::vector<GameObject> grassZone;
    std::vector<GameObject> fireZone;
    std::vector<GameObject> rockZone;
    std::vector<GameObject> swampZone;

    Vector3 grassAreaPosition;
    Vector3 fireAreaPosition;
    Vector3 rockAreaPosition;
    Vector3 swampAreaPosition;

    enum AREA
    {
        AREA_OVERVIEW,
        AREA_GRASS,
        AREA_FIRE,
        AREA_ROCK,
        AREA_SWAMP,

        NUM_AREAS
    };

    AREA currentArea;

    bool isFollowing;
    GameObject followingGO;
    GameObject iter;

    std::vector<Vector3> tempStore;

    void DisplayMonsterStats(Monster* monster);
    void CycleThroughZoneArea(std::vector<GameObject> area);
};

#endif