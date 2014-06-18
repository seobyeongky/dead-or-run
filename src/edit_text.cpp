#include "edit_text.h"
#include <Windows.h>

#define BACKSPACE_KEY	8

EditText::EditText(const Font & font, size_t char_size) :
	_text(),
	_caret(),
	_string(),
	_caret_pos(0U),
	_comp(false),
	_ignore_next_char(false),
	_draw_caret(true)
{
	_text.setFont(font);
	setCharSize(char_size);
	_caret.setSize(Vector2f(3.f, 15.f));
}

EditText::~EditText()
{
}

void EditText::setTextColor(const Color &color)
{
	_text.setColor(color);
}

void EditText::setCaretColor(const Color &color)
{
	_caret.setFillColor(color);
}

void EditText::setCharSize(size_t char_size)
{
	_text.setCharacterSize(char_size);
}

void EditText::setPosition(float x, float y)
{
	Transformable::setPosition(x, y);
	updateCaret();
}

void EditText::drawCaret(bool b)
{
	_draw_caret = b;
}

bool EditText::handleIMEUpdated(wchar_t unicode, bool done)
{
	if (unicode == 27 || unicode == BACKSPACE_KEY)
	{ // ESC or backsapce
		_comp = false;
		_string.erase(_caret_pos);
	}
	else
	{
		if (_comp)
		{
			_string[_caret_pos] = unicode;
		}
		else
		{
			if (!done)
				_string.insert(_caret_pos, unicode);
		}

		if (done)
		{
			_comp = false;
			_ignore_next_char = true;
			++_caret_pos;
		}		
		else
			_comp = true;

		updateCaret();
	}

	_text.setString(_string);

	return true;
}

bool EditText::handleEnteredKey(wchar_t unicode)
{
	if(_ignore_next_char)
	{
		_ignore_next_char = false;
		return true;
	}

	if(unicode == BACKSPACE_KEY)
	{
		if(_caret_pos > 0U)
		{
			_string.erase(_caret_pos - 1);
			--_caret_pos;
		}
	}
	else if(unicode == L'\r')
	{
		return false;
	}
	else if(unicode == L'\x1b')
	{
		return false;
	}
	else if(unicode == 22)
	{ // Ctrl + v
		if( !OpenClipboard( NULL ) ) return false;
		HANDLE handle = GetClipboardData( CF_UNICODETEXT );
		if( handle )
		{
			// Convert the ANSI string to Unicode, then
			// insert to our buffer.
			WCHAR* pwszText = ( WCHAR* )GlobalLock( handle );
			if( pwszText )
			{
				// Copy all characters up to null.
				size_t len = wcslen(pwszText);
				for(size_t i = 0U; i < len; ++i)
				{	
					_string.insert(_caret_pos, pwszText[i]);
					++_caret_pos;
				}
				GlobalUnlock( handle );
			}
		}
		CloseClipboard(); 
	}
	else
	{
		_string.insert(_caret_pos, unicode);
		++_caret_pos;
	}
	
	_text.setString(_string);

	updateCaret();

	return true;
}

bool EditText::handleKeyPress(Event::KeyEvent key)
{
	if(key.code == Keyboard::Left)
	{
		_comp = false;
		if(_caret_pos > 0U)
			--_caret_pos;
	}
	else if(key.code == Keyboard::Right)
	{
		_comp = false;
		if(_caret_pos < _string.getSize())
			++_caret_pos;
	}
	else if(key.code == Keyboard::Delete)
	{
		_comp = false;
		if(_caret_pos < _string.getSize())
		{
			_string.erase(_caret_pos);
			_text.setString(_string);
		}
	}
	else
	{
		return false;
	}

	updateCaret();

	return true;
}

void EditText::updateCaret()
{
	Vector2f next_pos;
	if (!_string.isEmpty())
	{

		next_pos = _text.findCharacterPos(_caret_pos);
		auto bounds = _text.getLocalBounds();
		next_pos.x += bounds.left;
		next_pos.y += bounds.top;

		_caret.setSize(Vector2f(3.f, static_cast<float>(bounds.height)));
	}

	_caret.setPosition(next_pos);
}

void EditText::draw(RenderTarget& target, RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(_text, states);
	if (_draw_caret)
		target.draw(_caret, states);
}

void EditText::clear()
{
	_string.clear();
	_text.setString("");
	_caret_pos = 0U;
	updateCaret();
	_comp = false;
	_ignore_next_char = false;
}

FloatRect EditText::getLocalBounds() const
{
	return _text.getLocalBounds();
}