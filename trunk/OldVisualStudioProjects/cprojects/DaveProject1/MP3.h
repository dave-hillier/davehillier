// MP3.h

#include <windows.h>
#include <mmsystem.h>
#include <Dshow.h>

class Mp3
{
private:
 IBaseFilter   *  pif;
 IGraphBuilder *  pigb;
 IMediaControl *  pimc;
 IMediaEventEx *  pimex;

 bool    ready;

public:
 Mp3();
 ~Mp3();

 void Load(LPSTR filename);
 void Cleanup();

 void Play();
 void Pause();
 void Stop();
};

