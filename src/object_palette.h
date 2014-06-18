/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	object_palette.h

	ObjectPalette class�� header file�Դϴ�.
=======================================================*/

#pragma once

#include "internal.h"
#include "camera.h"
#include "object_interface.h"

class ObjectPalette : public Drawable {
/*	game�� �����ϴ� object���� �����ϴ� �����̳��Դϴ�.*/

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