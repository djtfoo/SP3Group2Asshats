#include "SharedData.h"
#include "../GameObject/MonsterFactory.h"
#include "../Scene/Scene.h"

SharedData::SharedData()
{
}

SharedData::~SharedData()
{
}

void SharedData::Init()
{
    player = new Player();
    graphicsLoader = new GraphicsLoader();
    inputManager = new InputManager();
    graphicsLoader->Init();
    sound = new Sound();
    sceneManager = new SceneManager();
	particleManager = new ParticleManager();
    questManager = new QuestManager();
    questManager->Init();

    MonsterFactory::LoadMonsterData("GameData/MonstersData.csv");
    Scene::LoadLevelGenerationData("GameData/LevelGenerationData.csv");
}

void SharedData::Exit()
{
    if (player)
        delete player;

    if (graphicsLoader)
    {
        graphicsLoader->Exit();
        delete graphicsLoader;
    }

    if (inputManager)
        delete inputManager;

	if (sound)
		delete sound;

    if (sceneManager)
    {
        sceneManager->Exit();
        delete sceneManager;
    }

    if (particleManager)
    {
        particleManager->Exit();
        delete particleManager;
    }
}