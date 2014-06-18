#include "char_desc.h"
#include "res.h"
#include "util.h"
#include "play_scene_layout.h"

CharDesc::CharDesc() :
	_face(),
	_desc_text(L"...", normal_font, 15U),
	_desc_text_bg()
{
	_desc_text.setColor(Color(255, 255, 255));

	_desc_text_bg.setSize(Vector2f(325.f, 90.f));
	_desc_text_bg.setPosition(	static_cast<float>(window.getSize().x - VEHUMET),
								static_cast<float>(SHINING_ONE + 170.f + TROG));
	_desc_text_bg.setOrigin(325.f, 0.f);
	_desc_text_bg.setFillColor(Color(255, 0, 0, 127));
}

void CharDesc::Reset()
{
	SetFace(L"unknown-face");
	SetDesc(L"...");
}

void CharDesc::Update(Runner * runner)
{
	SetFace(runner->GetFaceName());

	wstring desc;
	desc.append(runner->GetDesc());
	desc.push_back(L'\n');
	auto & char_info = CHARACTER(runner->GetId());
	if (char_info.q_skill != INVALID_SKILL) AddDesc(desc, L"Q스킬:", char_info.q_skill);
	if (char_info.w_skill != INVALID_SKILL) AddDesc(desc, L"W스킬:", char_info.w_skill);
	if (char_info.e_skill != INVALID_SKILL) AddDesc(desc, L"E스킬:", char_info.e_skill);
	if (char_info.e_skill != INVALID_SKILL) AddDesc(desc, L"R스킬:", char_info.r_skill);
	
	SetDesc(desc.c_str());
}

void CharDesc::draw(RenderTarget& target, RenderStates states) const
{
	target.draw(_face, states);	
	target.draw(_desc_text, states);
	target.draw(_desc_text_bg, states);
}

void CharDesc::AddDesc(wstring & desc, const wchar_t * prefix, skill_id_t sid)
{
	auto skill = SKILL(sid);
	if (skill == nullptr) return;
	desc.append(prefix);
	desc.push_back(L' ');
	desc.append(skill->name);
	desc.push_back(L'(');
	desc.append(to_wstring(skill->cost));
	desc.push_back(L')');
	desc.push_back(L'\n');
}

void CharDesc::SetFace(const wchar_t * face_name)
{
	_face = *sprite_map[face_name];
	_face.setOrigin(0.f, 0.f);
	SetScaleToSize(&_face, 200.f, 170.f);
	_face.setPosition(	static_cast<float>(window.getSize().x - VEHUMET - 200), 
						static_cast<float>(SHINING_ONE));
}

void CharDesc::SetDesc(const wchar_t * desc)
{
	_desc_text.setString(desc);
	_desc_text.setOrigin(325.f, 0.f);
	_desc_text.setPosition(	static_cast<float>(window.getSize().x - VEHUMET),
							static_cast<float>(SHINING_ONE + 170.f + TROG));
}