#include "inventory_box.h"
#include "res.h"
#include "object.h"

InventoryBox::InventoryBox()
	: _bg(*sprite_map[L"inventory"]),
	_item_vec(),
	_ptr_pos(0U),
	_hand(*sprite_map[L"hand"])
{
	_item_vec.reserve(MAX_NR_ITEM_PER_RUNNER);

	_bg.setPosition(700.f, 50.f);
}

void InventoryBox::Reset()
{
	_item_vec.clear();
	ResetPtrPos(0U);
}

bool InventoryBox::AddItem(item_id_t item_id)
{
	if(_item_vec.size() >= MAX_NR_ITEM_PER_RUNNER) return false;
	item_t new_item(item_id, *Item::GetSprite(item_id));
	_item_vec.push_back(new_item);
	RecalculatePos(_item_vec.size() - 1U);

	return true;
}

bool InventoryBox::SpendItem(item_id_t * id_ptr)
{
	if(_item_vec.empty()) return false;

	*id_ptr = _item_vec[_ptr_pos].id;

	vector<item_t>::iterator iter(_item_vec.begin());
	advance(iter, _ptr_pos);
	_item_vec.erase(iter);
	
	if(!_item_vec.empty() && _ptr_pos >= _item_vec.size())
		ResetPtrPos(_item_vec.size() - 1);

	RecalculatePos(_ptr_pos);

	sfx_mgr->Play(Item::GetSFX(*id_ptr));

	return true;
}

void InventoryBox::HandleNumberKeyInput(int number)
{
	int index = number - 1;
	if(index < 0 || index >= static_cast<int>(_item_vec.size()))
		return;

	ResetPtrPos(index);
}

void InventoryBox::draw(RenderTarget & target, RenderStates states) const
{
	target.draw(_bg);
	for(auto iter = _item_vec.begin(); iter != _item_vec.end(); ++iter)
		target.draw(iter->sprite);

	if(!_item_vec.empty())
		target.draw(_hand);
}

void InventoryBox::RecalculatePos(size_t begin)
{
	for(; begin < _item_vec.size(); ++begin)
	{
		_item_vec[begin].sprite.setPosition(720.f,
											66.f + 59.f * begin);
	}
}

void InventoryBox::ResetPtrPos(size_t new_pos)
{
	_ptr_pos = new_pos;
	_hand.setPosition(660.f, 70.f + 59.f * _ptr_pos);
}