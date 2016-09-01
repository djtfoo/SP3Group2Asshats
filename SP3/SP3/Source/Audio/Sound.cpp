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
/****************************************************************************/
/*!
\brief
Sound Contructor and creating of irrklang device
*/
/****************************************************************************/
Sound::Sound()
{
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
	musicEngine->setSoundVolume(Volume);
	Song = musicEngine->play2D(Music.c_str(), true, false, true);
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
	SoundEffect = musicEngine->play2D(Music.c_str(), false, false, false);
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
	SoundEffect3D->play3D(Music.c_str(), TargetPos);
	SoundEffect3D->setListenerPosition(pos, view);
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