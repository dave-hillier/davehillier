// MP3.cpp

#include "Mp3.h"

Mp3::Mp3()
{
 pif = NULL;
 pigb = NULL;
 pimc = NULL;
 pimex = NULL;
 ready = false;

 CoInitialize(NULL);
}

Mp3::~Mp3()
{
 Cleanup();
}

void Mp3::Cleanup()
{
 CoUninitialize();

 if (pimc)
  pimc->Stop();


 
 if(pif)
 {
  pif->Release();
  pif = NULL;
 }

 if(pigb)
 {
  pigb->Release();
  pigb = NULL;
 }

 if(pimc)
 {
  pimc->Release();
  pimc = NULL;
 }
 if(pimex)
 {
  pimex->Release();
  pimex = NULL;
 }

}

void Mp3::Load(LPSTR szFile)
{
 WCHAR wFile[MAX_PATH];
 MultiByteToWideChar(CP_ACP, 0, szFile, -1, wFile, MAX_PATH);

 if (SUCCEEDED(CoCreateInstance( CLSID_FilterGraph,
         NULL,
         CLSCTX_INPROC_SERVER,
         IID_IGraphBuilder,
         (void **)&this->pigb)))
 {
  pigb->QueryInterface(IID_IMediaControl, (void **)&pimc);
  pigb->QueryInterface(IID_IMediaEventEx, (void **)&pimex);

  if (SUCCEEDED(pigb->RenderFile(wFile, NULL)))
  {
   ready = true;
  }
 }
}

void Mp3::Play()
{
 if (ready)
 {
  pimc->Run();
 }
}

void Mp3::Pause()
{
 if (ready)
 {
  pimc->Pause();
 }
}

void Mp3::Stop()
{
 if (ready)
 {
  pimc->Stop();
 }
}

