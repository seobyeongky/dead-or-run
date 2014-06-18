#include "object_palette.h"

ObjectPalette::ObjectPalette()
	: _vector()
{
}

ObjectPalette::~ObjectPalette()
{
}

void ObjectPalette::InsertObject(IObject * object)
{
	_vector.push_back(object);
}

void ObjectPalette::EraseObject(IObject * object)
{
	for(auto iter = _vector.begin(); iter != _vector.end(); ++iter)
	{
		if(*iter == object)
		{
			_vector.erase(iter);
			return;
		}
	}
}

void ObjectPalette::Update()
{
	// pos correction
	for(unsigned int i = 0U; i < _vector.size(); ++i)
	{
		_vector[i]->CorrectPos();
	}

	// runner를 y 좌표 오름차순으로 정렬합니다.
	bool changed;

	for(unsigned int i = 0U; i < _vector.size(); ++i)
	{
		changed = false;

		for(unsigned int j = 0U; j < _vector.size() - i; ++j)
		{
			if(j + 1 >= _vector.size()) break;
			if(_vector[j + 1]->GetPos().y <
				_vector[j]->GetPos().y)
			{
				swap(_vector[j + 1], _vector[j]);
				changed = true;
			}
		}

		if(!changed) break;
	}
}


void ObjectPalette::draw(RenderTarget& target, RenderStates states) const
{
	for(auto iter = _vector.begin(); iter != _vector.end(); ++iter)
	{
		if((*iter)->GetPriority() == IObject::FALL_DOWN_PRIORITY)
			(*iter)->Draw(target);
	}
	for(auto iter = _vector.begin(); iter != _vector.end(); ++iter)
	{
		if((*iter)->GetPriority() == IObject::GROUND_PRIORITY)
			(*iter)->Draw(target);
	}
	for(auto iter = _vector.begin(); iter != _vector.end(); ++iter)
	{
		if((*iter)->GetPriority() == IObject::AIR_PRIORITY)
			(*iter)->Draw(target);
	}
}