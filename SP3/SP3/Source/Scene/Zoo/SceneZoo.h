#ifndef SCENE_ZOO_H
#define SCENE_ZOO_H

#include "../Scene.h"
#include "../../GameObject/ItemProjectiles.h"
#include "../../General/ZooCamera.h"
#include "../../General/MousePicker.h"
#include "../../GameObject/Items.h"

struct PopUpMessage
{
    std::string m_message;

    float m_posX;
    float m_posY;

    float direction;

    double m_duration;
    double m_lifetime;
};

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

    //
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

    enum SHOPKEEPER_TEXT
    {
        TEXT_NONE,

        TEXT_TIP_1,
        TEXT_TIP_2,
        TEXT_TIP_3,
        TEXT_TIP_4,
        TEXT_TIP_5,

        TEXT_THANK,
        TEXT_INSUFFICIENT_COINS,
        TEXT_AT_MAX_UPGRADE,

        NUM_TEXT
    };

    STATE currentState;
    AREA currentArea;
    SHOP currentShop;
    Item::TYPE currentItem;

    bool isFollowingMonster;
    GameObject targetedMonster;
    GameObject iter;

    void DisplayMonsterStats(Monster* monster);
    void CycleThroughZoneArea(std::vector<GameObject> area);
    bool UpgradeEnclosureSize(AREA area);
    void RenderEnclosures();
    void RenderEnclosureInterface();
    
    int transactionCounter;
    bool isInTransaction;
    void RenderTransactionInterface();
    void RenderShopInterface();

    void RenderUpgradeInterface(Item::TYPE item);

    short shopKeeperTextChoice;
    void RenderShopkeeperText();

    //Upgrade enclosure values
    int upgradeCost;

    //shit for debugging
    float updown, leftright;
    
    //HUD stuff
    void RenderHUD();
    float f_Rotate;
};

#endif