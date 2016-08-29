#ifndef SCENE_ZOO_H
#define SCENE_ZOO_H

#include "../Scene.h"
#include "../../GameObject/ItemProjectiles.h"
#include "../../General/ZooCamera.h"
#include "../../General/MousePicker.h"
#include "../../GameObject/Items.h"

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

    //Probably needs to be in SharedData or Player class
    const int AREA_MAX_SIZE;

    float grassAreaSize;
    float fireAreaSize;
    float rockAreaSize;
    float swampAreaSize;

    enum AREA
    {
        AREA_OVERVIEW,
        AREA_GRASS,
        AREA_FIRE,
        AREA_ROCK,
        AREA_SWAMP,

        NUM_AREAS
    };

    enum STATE
    {
        STATE_ZOO,
        STATE_ENCLOSURE,
        STATE_SHOP
    };
    
    enum SHOP
    {
        SHOP_MAIN,
        SHOP_BUY,
        SHOP_SELL,
        SHOP_UPGRADE,

        NUM_SHOP
    };

    STATE currentState;
    AREA currentArea;
    SHOP currentShop;

    bool isFollowingMonster;
    GameObject targetedMonster;
    GameObject iter;

    void DisplayMonsterStats(Monster* monster);
    void CycleThroughZoneArea(std::vector<GameObject> area);
    void UpgradeEnclosureSize(AREA area);
    void RenderEnclosures();

    void PurchaseItem(Item item);
    void SellItem(Item item);

    void RenderShopInterface();
    void RenderEnclosureInterface();
    void RenderShopkeeperText();

    float updown, leftright;
};

#endif