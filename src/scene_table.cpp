#include "scene.h"

#include "intro_scene.h"
#include "starting_scene.h"
#include "play_scene.h"

// 모든 scene의 instance를 여기서 정의합니다.
void SceneMgr::CreateSceneTable()
{
	new IntroScene(INTRO_SCENE);
	new StartingScene(STARTING_SCENE);
	new PlayScene(PLAY_SCENE);
}