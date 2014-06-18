#include "protocol.h"

#include <vector>
#include <string>
using namespace std;

class SkillTable
{
private:
	typedef skill_info_t info_t;

private:
	vector<skill_info_t> _arr;

public:
	SkillTable()
	{
		_arr.resize(INVALID_SKILL);
_arr[BACK_ATTACK_SKILL] =
	info_t(L"��¦ ������", 20, 1, L"back-attack-icon", L"...");
_arr[LEAP_SKILL] = 
	info_t(L"����", 50, 3, L"leap-icon", L"...");
_arr[BAYABA_SKILL] =
	info_t(L"�پ߹� ����", 40, 3, L"bayaba-icon", L"...");
_arr[MAD_DOG_SKILL] =
	info_t(L"��ģ�� ����", 35, 15, L"mad-dog-icon", L"...");
_arr[SMOKE_SKILL] =
	info_t(L"����", 20, 1, L"smoke-icon", L"...");
_arr[GRUDGE_SKILL] = 
	info_t(L"����", 35, 2, L"grudge-icon", L"...");
_arr[FLASH_TRANSFER] =
	info_t(L"���� �̵�", 30, 1, L"flash-transfer-icon", L"...");
_arr[GOD_COME_SKILL] =
	info_t(L"�ų���", 40, 5, L"god-come-icon", L"...");
_arr[GHOST_SKILL] =
	info_t(L"��üȭ", 30, 3, L"ghost-icon", L"...");
_arr[HELL_GATE_SKILL] =
	info_t(L"������ ����", 30, 3, L"hell-gate-icon", L"...");
_arr[GROUND_ATTACK_SKILL] =
	info_t(L"���� ��Ÿ", 20, 1, L"ground-attack-icon", L"...");
_arr[MAGIC_BOOGER_SKILL] =
	info_t(L"�ź��� ȭ��", 5, 1, L"magic-booger-icon", L"...");
_arr[PROVOKING_SKILL] =
	info_t(L"����(���� �̱���)", 0, 5, L"provoking-icon", L"...");
	}

	const skill_info_t * operator [](skill_id_t sid)
	{
		if (sid < 0 || sid >= static_cast<int>(_arr.size()))
			return nullptr;
		else
			return &_arr[sid];
	}
};

static SkillTable _icon_table;

const skill_info_t * SKILL(skill_id_t sid)
{
	return _icon_table[sid];
}