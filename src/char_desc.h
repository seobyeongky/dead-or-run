/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	char_desc.h

	custom SFML CharDesc class의 header file 입니다.
=======================================================*/

#pragma once

#include "internal.h"
#include "runner.h"

class CharDesc : public Drawable
{
public:
	CharDesc();

	void Reset();
	void Update(Runner * runner);

private:
	virtual void draw(RenderTarget& target, RenderStates states) const;

private:
	Sprite						_face;
	Text						_desc_text;
	RectangleShape				_desc_text_bg;

	void AddDesc(wstring & desc, const wchar_t * prefix, skill_id_t sid);
	void SetFace(const wchar_t * face_name);
	void SetDesc(const wchar_t * desc);
};