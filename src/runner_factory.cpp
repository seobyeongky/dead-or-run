#include <algorithm>

#include "runner_factory.h"
#include "res.h"
#include "util.h"

RunnerFactory::RunnerFactory()
	: _texture(),
	_height_size(0U),
	_runner_list()
{
	_texture.create(DEFAULT_RUNNER_TEXTURE_WIDTH, DEFAULT_RUNNER_TEXTURE_HEIGHT);
}

RunnerFactory::~RunnerFactory()
{
	for(auto it = _runner_list.begin(); it != _runner_list.end(); ++it)
	{
		delete it->runner;
	}
}

Runner * RunnerFactory::CreateRunner(const wchar_t * cl_name, runner_id_t id, const Color & color)
{
	Runner * runner = nullptr;
	switch(id)
	{
	case SEO:	runner = new Seo(cl_name, &_texture, color); break;
	case PARK:	runner = new Park(cl_name, &_texture, color); break;
	case OH:	runner = new Oh(cl_name, &_texture, color); break;
	case JEONG:	runner = new Jeong(cl_name, &_texture, color); break;
	case KIM:	runner = new Kim(cl_name, &_texture, color); break;
	case YONG:	runner = new Yong(cl_name, &_texture, color); break;
	case YOON:	runner = new Yoon(cl_name, &_texture, color); break;
	case RANDOM_RUNNER: runner = new RandomRunner(cl_name, &_texture, color); break;
	default:
		ErrorMsg(L"RunnerFactory : CreateRunner : 알 수 없는 runner_id(%d)", id);
		return nullptr;
	}

	const Sprite *	sprite = sprite_map[runner->GetName()];
	IntRect			rect = sprite->getTextureRect();

	// 필요하다면 텍스처 크기를 적절하게 늘립니다.
	bool			recreate = false;
	unsigned int	recreate_width = 0U;

	if(_texture.getSize().x <
		static_cast<unsigned int>(rect.width))
	{
		recreate = true;
		recreate_width = static_cast<unsigned int>(rect.width);
	}
	else recreate_width = _texture.getSize().x;

	unsigned int	recreate_height = 0U;

	if(_texture.getSize().y < _height_size + rect.height)
	{
		recreate = true;
		recreate_height = _height_size + rect.height;
	}
	else recreate_height = _texture.getSize().y;

	if(recreate)
	{
		_texture.create(recreate_width, recreate_height);
	}
	
	// 이제 
	Image & full_image = texture_map[L"chars.png"]->copyToImage();
	Image portion_image;
	portion_image.create(sprite->getTextureRect().width, sprite->getTextureRect().height);
	portion_image.copy(full_image, 0U, 0U, sprite->getTextureRect(), false);
	portion_image.createMaskFromColor(Color(255, 0, 255));

	for(unsigned int i = 0; i <portion_image.getSize().y; ++i)
	{
		for(unsigned int j = 0; j < portion_image.getSize().x; ++j)
		{
			Color & c = portion_image.getPixel(j, i);
			if(c == Color(0, 255, 0))
				portion_image.setPixel(j, i, color);
		}
	}

	_texture.update(portion_image, 0U, _height_size);
	
	Vector2i base(0, static_cast<int>(_height_size));
	runner->ResetTexture(&_texture);
	runner->ResetTextureBase(base);
	
	// 그리고
	_height_size += portion_image.getSize().y;

	context_t context;
	context.runner = runner;
	context.color = color;
	context.height = rect.height;
	_runner_list.push_back(context);

	return runner;
}

void RunnerFactory::DeleteRunner(Runner * runner)
{
	unsigned int accum_height = 0U;

	for(auto iter = _runner_list.begin();
		iter != _runner_list.end();
		++iter)
	{
		if(iter->runner == runner)
		{
			DeleteRunner(iter, accum_height);
			break;
		}
		else accum_height += iter->height;
	}
	delete runner;
}

void RunnerFactory::DeleteRunner(list<context_t>::iterator iter, unsigned int accum_height)
{
	// 우선 texture부터 갱신합니다.
	Image & original_texture_image = _texture.copyToImage();
	Image portion_image;
	IntRect portion;
	portion.left = 0U;
	portion.top = accum_height + iter->height;
	portion.width = original_texture_image.getSize().x;
	portion.height = original_texture_image.getSize().y - portion.top;
	portion_image.create(portion.width, portion.height);
	portion_image.copy(	original_texture_image,
						0U,
						0U,
						portion,
						false );
			
	_texture.update(portion_image, 0U, accum_height);
/*	static int cnt = 0;
	char buf[32];
	sprintf(buf, "tex%d.png", cnt++);
	_texture.copyToImage().saveToFile(buf);
	*/
	iter = _runner_list.erase(iter);
	
	for(; iter != _runner_list.end(); ++iter)
	{
		iter->runner->ResetTexture(&_texture);
		iter->runner->ResetTextureBase(
			Vector2i(0, accum_height) );

		accum_height += iter->height;
	}

	_height_size = accum_height;
}

void RunnerFactory::Clear()
{
	_height_size = 0U;
	for_each(_runner_list.begin(), _runner_list.end(), [](const context_t & c){delete c.runner;});
	_runner_list.clear();
}

void RunnerFactory::DumpTexture()
{
	Image & image = _texture.copyToImage();
	image.saveToFile("texture.png");
}