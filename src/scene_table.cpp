#include "scene.h"

#include "intro_scene.h"
#include "starting_scene.h"
#include "play_scene.h"

// ��� scene�� instance�� ���⼭ �����մϴ�.
void SceneMgr::CreateSceneTable()
{
	new IntroScene(INTRO_SCENE);
	new StartingScene(STARTING_SCENE);
	new PlayScene(PLAY_SCENE);
}