#include "protocol.h"

#include <vector>
using namespace std;

class CharacterTable
{
private:
	typedef runner_info_t info_t;

private:
	vector<info_t>	_arr;

public:
	CharacterTable()
	{
		_arr.resize(NR_RUNNER);

_arr[SEO] = info_t(BACK_ATTACK_SKILL,
				   LEAP_SKILL,
				   INVALID_SKILL,
				   INVALID_SKILL);
_arr[PARK] = info_t(BAYABA_SKILL,
					INVALID_SKILL,
					INVALID_SKILL,
					INVALID_SKILL);
_arr[OH] = info_t(MAD_DOG_SKILL,
				  SMOKE_SKILL,
				  INVALID_SKILL,
				  INVALID_SKILL);
_arr[JEONG] = info_t(GRUDGE_SKILL,
					 FLASH_TRANSFER,
					 INVALID_SKILL,
					 INVALID_SKILL);
_arr[KIM] = info_t(GOD_COME_SKILL,
				   GHOST_SKILL,
				   INVALID_SKILL,
				   INVALID_SKILL);
_arr[YONG] = info_t(HELL_GATE_SKILL,
					GROUND_ATTACK_SKILL,
					INVALID_SKILL,
					INVALID_SKILL);
_arr[YOON] = info_t(MAGIC_BOOGER_SKILL,
					PROVOKING_SKILL,
					INVALID_SKILL,
					INVALID_SKILL);
_arr[RANDOM_RUNNER] = info_t(INVALID_SKILL,
							 INVALID_SKILL,
							 INVALID_SKILL,
							 INVALID_SKILL);
	}

	const runner_info_t & operator [](runner_id_t rid) const
	{
		return _arr[rid];
	}
};

static CharacterTable _character_table;

const runner_info_t & CHARACTER(runner_id_t rid)
{
	return _character_table[rid];
}