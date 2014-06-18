#pragma once

#include <string>

class Poster
{
public:
	Poster(const std::string & proxy_url, const std::string & data);
	~Poster();

	void Post();

private:
	std::string _proxy_url;
	std::string _data;
};