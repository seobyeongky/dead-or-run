#include "runner.h"
#include "res.h"
#include "util.h"
#include "coord.h"

RandomRunner::RandomRunner(const wchar_t * cl_name, const Texture * texture, const Color & color) :
	Runner(cl_name, texture, color)
{
	_sprite.setOrigin(35.f, 42.f);
	IntRect rect(0, 0, 70, 85);
	_sprite.setTextureRect(_base_tex_vec + rect);
}

RandomRunner::~RandomRunner()
{
}

runner_id_t RandomRunner::GetId() const
{
	return RANDOM_RUNNER;
}

const wchar_t * RandomRunner::GetName() const
{
	return L"random-runner";
}

const wchar_t * RandomRunner::GetFaceName() const
{
	return L"unknown-face";
}

const wchar_t * RandomRunner::GetDesc() const
{
	return L"임의로 캐릭터가 골라집니다.";
}

void RandomRunner::SetState(state_t new_state)
{
	_state = new_state;
}

void RandomRunner::Update()
{
}

void RandomRunner::Yell() const
{
}

void RandomRunner::Hit() const
{
}