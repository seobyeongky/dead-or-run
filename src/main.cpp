#include "internal.h"
#include "edit_text.h"
#include "scene.h"
#include "res.h"
#include <curl/curl.h>

RenderWindow				window;
Font						normal_font;
int							present_time;
int							delta_time;
audiere::AudioDevicePtr		audio_device;
audiere::OutputStreamPtr	bg_music;
opzSFXMgr *					sfx_mgr;

void DetectLeak()
{
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);  
	VLDEnable();
//	_CrtSetBreakAlloc(4963);
//	_CrtSetBreakAlloc(4962);
//	_CrtSetBreakAlloc(4975);
//	_CrtSetBreakAlloc(5808);
//	_CrtSetBreakAlloc(5809);
//	_CrtSetBreakAlloc(8156);
//	_CrtSetBreakAlloc(8163);

//	_CrtSetBreakAlloc(294);
//	_CrtSetBreakAlloc(373);
//	_CrtSetBreakAlloc(3366);
#endif
}

int main()
{	DetectLeak();
	if(!LoadResources())
		return -1;

	sfx_mgr = new opzSFXMgr();
	
#ifndef _DEBUG
	srand(static_cast<unsigned int>(time(nullptr)));
#endif

	Scene *			scene;
	Clock			clock;
	int				last_time = 0;
	
	sf::Event		event;
	client_msg_t	cl_msg;

	window.create(sf::VideoMode(800, 600), "Run", Style::Titlebar);
//	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60U);

	BeginClient();
	
	if(bg_music)
	{
		bg_music->setRepeat(true);
		bg_music->play();
	}
	SceneMgr::S_Create();

	scene = SceneMgr::S_GetScene(INTRO_SCENE);
	scene->OnInit();

	clock.restart();

    while (window.isOpen())
    {
		// calc time
		present_time = clock.getElapsedTime().asMilliseconds();
		delta_time = present_time - last_time;
		last_time = present_time;

		window.clear(sf::Color::Black);

		// poll sfml window msg
        while (window.pollEvent(event))
        {
			switch(event.type)
			{
			case Event::TextEntered:
				if(!(scene->HandleTextEntered(event.text.unicode)))
				{
					if(event.text.unicode == UNICODE_ESC) window.close();
				}
				break;

			case Event::KeyPressed:
				if(!(scene->HandleKeyPressed(event.key)))
				{
				}
				break;

			case Event::IMEUpdated:
				scene->HandleIMECompMsg(event.ime.unicode, event.ime.done);
				break;

			case Event::Closed:
				window.close();
				break;
			}
			if(scene->GetNextScene() != INVALID_SCENE) goto END_OF_SCENE_HANDLING;
        }

		// poll opznet client msg
		while(GetClientMsg(&cl_msg))
		{
			scene->HandleClientMsg(cl_msg);
			if(scene->GetNextScene() != INVALID_SCENE) goto END_OF_SCENE_HANDLING;
		}

		scene->Go();
		
END_OF_SCENE_HANDLING:
		if(scene->GetNextScene() != INVALID_SCENE)
		{
			scene = SceneMgr::S_GetScene(scene->GetNextScene());
			scene->OnInit();
		}

        window.display();

		_ASSERTE( _CrtCheckMemory( ) );
    }

	SceneMgr::S_Delete();

	EndClient();

	delete sfx_mgr;

	ReleaseResource();
	
	_CrtDumpMemoryLeaks();

    return 0;
}