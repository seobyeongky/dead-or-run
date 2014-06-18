/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	runner_texture_mgr.h

	Runner Texture 관리 class의 header file입니다.
=======================================================*/

#pragma once

#include "internal.h"
#include "runner.h"

#define DEFAULT_RUNNER_TEXTURE_WIDTH	500
#define DEFAULT_RUNNER_TEXTURE_HEIGHT	1000

class RunnerFactory {

public:
			 RunnerFactory();
			~RunnerFactory();

	Runner * CreateRunner(const wchar_t * cl_name, runner_id_t id, const Color & color);
	void	 DeleteRunner(Runner * runner);

	void	 Clear();
	
private:
	struct context_t
	{
		Runner *		runner;
		Color			color;
		unsigned int	height;
	};
	
private:
	void	DeleteRunner(list<context_t>::iterator iter, unsigned int accum_height);

	// FOR DEBUG
	void	DumpTexture();

private:
	Texture						_texture;
	unsigned int				_height_size;
	list<context_t>				_runner_list;
};