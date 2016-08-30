/****************************************************************************/
/*!
\file Sound.h
\author Bek Kunta
\par email: xiaobek\@gmail.com
\brief
A Class that is define for the Sound in the game
*/
/****************************************************************************/
#ifndef SOUND_H
#define SOUND_H

//#include "../../irrKlang-1.5.0/include/irrKlang.h"
#include "../../irrKlang-1.5.0/include/irrKlang.h"
//#include "Camera.h"
#include <string>
using std::string;
#pragma comment(lib, "irrKlang.lib")

/****************************************************************************/
/*!
Class Sound:
\brief Defines a Sound and its methods,functions and soundEngines used.
*/
/****************************************************************************/

class Sound
{
public:

	Sound();
	~Sound();
	void Init();
	void playMusic(string Music);
	void stopMusic(string Music);
	void playSoundEffect(string Music);
	//void playSoundEffect3D(string Music, irrklang::vec3df pos, irrklang::vec3df TargetPos);
	void playSoundEffect3D(string Music, irrklang::vec3df pos, irrklang::vec3df view, irrklang::vec3df TargetPos,bool repeat = 0);
	void stopSoundEffect3D();
private: 
	irrklang::ISoundEngine* musicEngine;
	irrklang::ISoundEngine* SoundEffect3D;
	irrklang::ISound* Song;
	irrklang::ISound* SoundEffect;
	
};
#endif// ! SOUND_H
