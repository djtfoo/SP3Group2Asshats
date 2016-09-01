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
    SceneZoo(std::string name);
    ~SceneZoo();

    virtual void Init();
    virtual void Update(double dt);
    virtual void Render();
    virtual void RenderZooScene();
    virtual void Exit();

    virtual void SpawnSceneParticles();
    virtual void SceneEnvironmentEffect() {};

    void populateMonsterList();

    World zooWorld;
    ZooCamera zooCamera;

    Vector3 grassAreaPosition;
    Vector3 fireAreaPosition;
    Vector3 rockAreaPosition;
    Vector3 swampAreaPosition;

    Vector3 treeRands[50];
    Vector3 volcanoRands[50];
    Vector3 rockRands[50];
    Vector3 swampPlantRands[50];

    const int AREA_MAX_SIZE;

    static int grassAreaSize;
    static int fireAreaSize;
    static int rockAreaSize;
    static int swampAreaSize;

    std::vector<GameObject> grassZone;
    std::vector<GameObject> fireZone;
    std::vector<GameObject> rockZone;
    std::vector<GameObject> swampZone;

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
        STATE_SHOP,
        STATE_CHANGE_SCENE,

        STATE_MENU,
        STATE_QUEST,
        STATE_OPTIONS,

        NUM_STATE
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
        TEXT_INSUFFICIENT_MEAT,
        TEXT_AT_MAX_UPGRADE,

        NUM_TEXT
    };

    STATE currentState;
    AREA currentArea;
    SHOP currentShop;
    Item::TYPE currentItem;
    AREA changeSceneTo;

    bool isFollowingMonster;
    GameObject targetedMonster;
    GameObject cycleIter;

    float f_rotateMonster;  // for viewing of quest

    void RenderOverviewInterface();

    void DisplayMonsterInterface(Monster* monster);
    bool UpgradeEnclosureSize(AREA area);
    void RenderEnclosures();
    void RenderEnclosureInterface();

    void SellMonster();
    void SlaughterMonster();

    int transactionCounter;
    bool isInTransaction;
    void RenderTransactionInterface();
    void RenderShopInterface();

    void RenderMenuInterface();
    void RenderQuestInterface();
    void RenderOptionsInterface();

    void RenderUpgradeInterface(Item::TYPE item);

    SHOPKEEPER_TEXT shopKeeperTextChoice;
    void RenderShopkeeperText();

    //Upgrade enclosure values
    int upgradeCost;

    void RenderHuntingScenesInterface();

    //shit for debugging
    float updown, leftright;

    //HUD stuff
    void RenderHUD();
    float f_Rotate;
    float rotateEnclosureIcon1;
    float rotateEnclosureIcon2;

    bool b_savedGame;
    bool b_unableToGo;
    double d_textTimer;

    bool b_returnToMainMenu;
};

#endif