/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	object_palette.h

	ObjectPalette class의 header file입니다.
=======================================================*/

#pragma once

#include "internal.h"
#include "camera.h"
#include "object_interface.h"

class ObjectPalette : public Drawable {
/*	game상에 등장하는 object들을 저장하는 컨테이너입니다.*/

public:
				 ObjectPalette();
				~ObjectPalette();

	void		 InsertObject(IObject * object);
	void		 EraseObject(IObject * object);

	void		 Update();

	void		 Clear() { _vector.clear(); }

private:
	void		 draw(RenderTarget& target, RenderStates states) const;

private:
	vector<IObject *>	_vector;
};