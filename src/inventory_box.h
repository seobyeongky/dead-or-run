/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	inventory_box.h

	InventoryBox class의 header file입니다.
=======================================================*/

#pragma once

#include "internal.h"

class InventoryBox : public Drawable
{
public:
			InventoryBox();

	void	Reset();

	bool	AddItem(item_id_t item_id);
	bool	SpendItem(item_id_t * id_ptr);

	void	HandleNumberKeyInput(int number);
	
private:
	virtual void draw(RenderTarget& target, RenderStates states) const;

	struct item_t
	{
		item_id_t	id;
		Sprite		sprite;

		item_t(void) : id(ITEM_INVALID), sprite()
		{
		}

		item_t(item_id_t _id, const Sprite & _sprite)
			: id(_id), sprite(_sprite)
		{
		}
	};

private:
	void	RecalculatePos(size_t begin);
	void	ResetPtrPos(size_t new_pos);

private:
	Sprite			_bg;
	vector<item_t>	_item_vec;
	size_t			_ptr_pos;
	Sprite			_hand;
};