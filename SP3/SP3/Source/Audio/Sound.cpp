/****************************************************************************/
/*!
\file Sound.cpp
\author Bek Kunta
\par email: xiaobek\@gmail.com
\brief
A Class that is define for the Sound in the game
*/
/****************************************************************************/
#include "Sound.h"
#include "../General/SharedData.h"
/****************************************************************************/
/*!
\brief
Sound Contructor and creating of irrklang device
*/
/****************************************************************************/
Sound::Sound()
{
    b_playSound = true;
    b_playMusic = true;
	musicEngine = irrklang::createIrrKlangDevice();
	SoundEffect3D = irrklang::createIrrKlangDevice();
}
/****************************************************************************/
/*!
\brief
Sound Destructor
*/
/****************************************************************************/
Sound::~Sound()
{

}

/****************************************************************************/
/*!
\brief Init()
Setting the Min and Max Distance for the 3d Sound and its volume.
*/
/****************************************************************************/
void Sound::Init()
{
    SoundEffect3D->setDefault3DSoundMinDistance(10.0f);
    SoundEffect3D->setDefault3DSoundMaxDistance(100.0);
    SoundEffect3D->setSoundVolume(1.0f);
}

void Sound::Exit()
{
    Song->stop();
    Song->drop();

    SoundEffect->stop();
    SoundEffect->drop();

    musicEngine->drop();
    SoundEffect3D->drop();
}

/****************************************************************************/
/*!
\brief
playMusic(string Music)
\param Music
a string to take in music file to play.
*/
/****************************************************************************/
void Sound::PlayMusic(string Music,float Volume)
{
    if (b_playMusic)
    {
        musicEngine->setSoundVolume(Volume);
        Song = musicEngine->play2D(Music.c_str(), true, false, true);
    }
}
/****************************************************************************/
/*!
\brief
playSoundEffect(string Music)
\param Music
a string to take in music file to play.
*/
/****************************************************************************/
void Sound::PlaySoundEffect(string Music)
{
    if (b_playSound)
    {
        SoundEffect = musicEngine->play2D(Music.c_str(), false, false, false);
    }
}

/****************************************************************************/
/*!
\brief
playSoundEffect3D(string Music, irrklang::vec3df pos, irrklang::vec3df TargetPos)
\param Music
a string to take in music file to play.
\param pos
a vec3df to take in the position to hear the sound 
\param TargetPos
a vec3df to take in the position of where the sound is comming from.
SoundEffect3D->play3D(Music.c_str(), pos); Gets the music and your position
SoundEffect3D->setListenerPosition(pos, TargetPos); Listen to TargetPos from your pos
*/
/****************************************************************************/
void Sound::PlaySoundEffect3D(string Music, irrklang::vec3df pos, irrklang::vec3df view, irrklang::vec3df TargetPos,bool repeat)
{
    if (b_playSound)
    {
        SoundEffect3D->play3D(Music.c_str(), TargetPos);
        SoundEffect3D->setListenerPosition(pos, view);
    }
}

void Sound::StopSoundEffect3D()
{
	SoundEffect3D->stopAllSounds();
}

void Sound::StopAllSounds()
{
	SoundEffect3D->stopAllSounds();
	musicEngine->stopAllSounds();
}

/****************************************************************************/
/*!
\brief
playSoundEffect(string Music)
\param Music
a string to take in music file to stop.
*/
/****************************************************************************/
void Sound::StopMusic(string Music)
{
	Song->stop();
}

void Sound::PlayBGM()
{
    musicEngine->setSoundVolume(1.0);

    if (SharedData::GetInstance()->sceneManager->GetGameState() == SceneManager::GAMESTATE_MAINMENU)
    {
        Song = musicEngine->play2D("Sound//Menu/MenuBGM.wav", true, false, true);
    }

    else
    {
        if (SharedData::GetInstance()->sceneManager->m_scene->m_sceneName == "Grass")
        {
            SharedData::GetInstance()->sound->PlayMusic("Sound//GrassZone//GrassScene.wav");
        }
        else if (SharedData::GetInstance()->sceneManager->m_scene->m_sceneName == "Swamp")
        {
            SharedData::GetInstance()->sound->PlayMusic("Sound//SwampZone//SwampScene.wav");
        }
        else if (SharedData::GetInstance()->sceneManager->m_scene->m_sceneName == "Rock")
        {
            SharedData::GetInstance()->sound->PlayMusic("Sound//RockZone//RockScene.wav");
        }
        else if (SharedData::GetInstance()->sceneManager->m_scene->m_sceneName == "Lava")
        {
            SharedData::GetInstance()->sound->PlayMusic("Sound//LavaZone//LavaScene.wav");
        }
        else if (SharedData::GetInstance()->sceneManager->m_scene->m_sceneName == "Zoo")
        {
            SharedData::GetInstance()->sound->PlayMusic("Sound//Zoo//ZooScene.wav");
        }
    }
}