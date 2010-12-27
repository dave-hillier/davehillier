#ifndef __AUDIO__
#define __AUDIO__
#include <alut.h>

class AudioEnv
{
private:
   ALuint buffer[256];
   ALuint source[256];
   ALuint nextBuffer, nextSource;
public:
   AudioEnv ();
   ~AudioEnv ();
   void Init ();
   void ListenerPosition (ALfloat*, ALfloat*);
//   void UpdateObstruction (int);
   int LoadFile (char *, bool);
   int PlayFile (char *, bool);
   void SetSourcePosition (int, float *);
   void SetSourceVelocity (int, float *);
   void Play (int);
   void Stop (int);
};
extern AudioEnv Audio;

#endif // __AUDIO__