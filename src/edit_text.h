/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	edit_text.h

	EditText class의 header file입니다.
=======================================================*/

#pragma once

#include <SFML\Graphics.hpp>

using namespace sf;

class EditText : public Drawable, public Transformable {
/*	입력용 text */

public:
							  EditText(const Font & font, size_t char_size);
							 ~EditText();

	void					  drawCaret(bool b);
	void					  setCharSize(size_t char_size);
	void					  setTextColor(const Color &color);
	void					  setCaretColor(const Color &color);
	void					  setPosition(float x, float y);

	bool					  handleIMEUpdated(wchar_t unicode, bool done);
	bool					  handleEnteredKey(wchar_t unicode);
	bool					  handleKeyPress(Event::KeyEvent key);

	const String &			  getString() {return _string;}

	void					  clear();

	FloatRect				  getLocalBounds() const;

private:
	virtual void draw(RenderTarget& target, RenderStates states) const;

private:
	Text			_text;
	String			_string;
	size_t			_caret_pos;
	
	RectangleShape	_caret;

	bool			_comp;

	bool			_ignore_next_char;

	bool			_draw_caret;

	void			updateCaret();
};