#include <stdio.h>
#include "Audio.h"

AudioEnv Audio;
// constructor
AudioEnv::AudioEnv ()
{
	nextBuffer = 0;
	nextSource = 0;
}

// destructor
AudioEnv::~AudioEnv ()
{
	alutExit();
}

// init
void AudioEnv::Init ()
{
   alutInit (NULL, 0); // init OpenAL

   // global settings
   alListenerf(AL_GAIN, 1.0);
   alDopplerFactor(1.0); 
   alDopplerVelocity(343); 

   // load all the waves here

}

// ListenerPostition -- update listener's position and direction
void AudioEnv::ListenerPosition (ALfloat* position, ALfloat* angle)
{
	alListenerfv(AL_POSITION, position);
	alListenerfv(AL_ORIENTATION, angle);
}

// LoadFile -- loads a file into a buffer and source
int AudioEnv::LoadFile (char *filename, bool loop)
{
   // create buffer
   alGetError(); /* clear */
   alGenBuffers(1, &buffer[nextBuffer]);
   if(alGetError() != AL_NO_ERROR) {
	   return 0;
   }

   // create source
   alGetError(); /* clear */
   alGenSources(1, &source[nextSource]);
   if(alGetError() != AL_NO_ERROR) {
	   return 0;
   }

   // load data into buffer
   ALsizei size, freq;
   ALenum format;
   ALvoid *data;
   ALboolean looping;
   
   alutLoadWAVFile(filename, &format, &data, &size, &freq, &looping);
   alBufferData (buffer[nextBuffer], format, data, size, freq);

   // set static source properties
   alSourcei(source[nextSource], AL_BUFFER, buffer[nextBuffer]);
   alSourcei(source[nextSource], AL_LOOPING, 1);
   alSourcef(source[nextSource], AL_REFERENCE_DISTANCE, 10);

   nextBuffer++;
   nextSource++;
   return nextBuffer;
}

// Playfile -- loads a file into a buffer and source, then plays it
int AudioEnv::PlayFile (char *filename, bool loop)
{
	int loadhandle;

	loadhandle = LoadFile(filename, loop);

	if (loadhandle != 0) { 
		Play(loadhandle); 
	} else
	{
		return 0;
	}

	return loadhandle;
}

//SetSourcePosition
void AudioEnv::SetSourcePosition (int handle, float *position)
{
	alSourcefv(source[handle-1], AL_POSITION, position);
}

//SetSourceVelocity
void AudioEnv::SetSourceVelocity (int handle, float *velocity)
{
	alSourcefv(source[handle-1], AL_VELOCITY, velocity);
}

// Play
void AudioEnv::Play(int handle)
{
	alSourcePlay(source[handle-1]); 
}

// Stop
void AudioEnv::Stop(int handle)
{
	alSourceStop(source[handle-1]);
}
