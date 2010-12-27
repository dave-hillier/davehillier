#ifdef WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include "SDL.h"
#include "SDLTimer.h"
#include "GLMain.h"
#include "Console.h"
#include "ConsoleVars.h"
#include "ConsoleCmds.h"
#include "Font.h"
#include "Input.h"
#include "ModelManager.h"
#include "EventScheduler.h"

// defs
#define APP_NAME "Kodama"
#define APP_VERSION 1.0
#define WIDTH 640
#define HEIGHT 480

// globals
bool quit = false;

// prototypes
void handle_event(SDL_Event event);
void main_loop ();
Font *font1;


int main(int argc, char *argv[])
{
	int width = WIDTH, height = HEIGHT;
	SDL_Event event;
	unsigned int vidmodes = SDL_OPENGL;
	
	{
		FILE *newfp;
		
		newfp = freopen("error.txt", "w", stderr);
		if ( newfp == NULL ) {	// This happens on NT apparently 
#if !defined(stderr)
			stderr = fopen("error.txt", "w");
#else
			newfp = fopen("error.txt", "w");
			if ( newfp ) {
				*stderr = *newfp;
			}
#endif
		}
	}

	// Initialize SDL for video output
	if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 ) {
	//	con->Printf("Unable to initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	{
		char buff[128];
		sprintf(buff, "%s %.2f", APP_NAME, APP_VERSION);
		SDL_WM_SetCaption (buff, NULL);
	}

	


	// init here
	con = new Console();
	input = new Input();
	{
		ConsoleVar *var;
		var = cvars->Add("fullscreen", "0");
		var = cvars->Add("width", "640");
		var = cvars->Add("height", "480");

		command->ReadSettings("default.cfg");

		if (var && var->Value() == 1.0f)
			vidmodes  |= SDL_FULLSCREEN;
	}		


#ifdef _DEBUG	
//	vidmodes  |= ~SDL_FULLSCREEN; // |=
#endif

	// Create an OpenGL screen  16 bit maybe? 
	if ( SDL_SetVideoMode(width, height, 32, vidmodes) == NULL ) {
	//	con->Printf("Unable to create OpenGL screen: %s\n", SDL_GetError());
		SDL_Quit();
		exit(2);
	}	



	mman = new ModelManager();
	glapp = new GLMain(width, height);
	
	font1 = new Font("font1.tga", 16, 16, 14);
	srand(SDL_GetTicks()); // initilise random number generator
	
	while (!quit) {
		
		main_loop();	
		
		while ( SDL_PollEvent(&event) )
			handle_event(event); 
	}
	
	SDL_Quit();

	delete glapp;
	delete input;
	delete mman;

	con->Printf("Normal exit\n");
	delete font1;
	delete con;

	return 0;
}

void handle_event(SDL_Event event) {

	// TODO JOYSTICK MOTION
	switch (event.type)
	{
	case SDL_QUIT:
		quit = true;
		break;
	case SDL_KEYDOWN:
		input->KeyDown(event.key.keysym.sym);
		break;
	case SDL_KEYUP:
		input->KeyUp(event.key.keysym.sym);
		break;
	case SDL_MOUSEBUTTONDOWN:
		input->KeyDown(event.button.button+KEY_MOUSE1-1);
		break;
	case SDL_MOUSEBUTTONUP:
		input->KeyUp(event.button.button+KEY_MOUSE1-1);
		break;
	}
}


void main_loop()
{
	timer.Update();
	events->Update();

	// display
	glapp->display();
	// input?

	SDL_GL_SwapBuffers();

}

char *GetPasteData( void )
{
#ifdef WIN32
	char *data = NULL;
	char *cliptext;

	if ( OpenClipboard( NULL ) != 0 )
	{
		HANDLE hClipboardData;

		if ( ( hClipboardData = GetClipboardData( CF_TEXT ) ) != 0 )
		{
			if ( ( cliptext = (char *)GlobalLock( hClipboardData ) ) != 0 ) 
			{
				data = new char[GlobalSize( hClipboardData ) + 1];//malloc( GlobalSize( hClipboardData ) + 1 );
				strcpy( data, cliptext );
				GlobalUnlock( hClipboardData );
			}
		}
		CloseClipboard();
	}
	return data;
#else
	return NULL;
#endif
}

void CmdQuit (int argc, char *argv[])
{
	quit = true;
}
