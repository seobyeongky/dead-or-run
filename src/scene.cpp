#include "scene.h"

Packet Scene::_packet;

Scene::Scene(scene_id_t id) :
	_id(id)
{
 	SceneMgr::S_Bind(id, this);
}

Scene::~Scene()
{
}

void SceneMgr::S_Create()
{
	new SceneMgr();
}

void SceneMgr::S_Delete()
{
	delete _instance;
}

void SceneMgr::S_Bind(scene_id_t id, Scene *scene)
{
	auto &scene_container = _instance->_scene_container;
	assert(static_cast<size_t>(id) < scene_container.size());
	assert(scene_container[id] == nullptr);
	scene_container[id] = scene;
}

Scene *SceneMgr::S_GetScene(scene_id_t id)
{
	auto &scene_container = _instance->_scene_container;
	assert(static_cast<size_t>(id) < scene_container.size());
	assert(scene_container[id] != nullptr);
	return scene_container[id];
}

SceneMgr *SceneMgr::_instance = nullptr;

SceneMgr::SceneMgr()
	: _scene_container(NR_SCENE)
{
	_instance = this;

	_scene_container.resize(NR_SCENE, nullptr);

	CreateSceneTable();
}

SceneMgr::~SceneMgr()
{
	for(auto it = _scene_container.begin(); it != _scene_container.end(); ++it)
		delete *it;
}