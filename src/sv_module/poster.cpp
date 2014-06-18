#include <assert.h>
#include <thread>
#include <curl/curl.h>
#include <curl/easy.h>

#include "poster.h"
#include "../util.h"

using namespace std;

Poster::Poster(const string & proxy_url, const string & data)
	: _proxy_url(proxy_url), _data(data)
{
}

Poster::~Poster()
{
}

void Post_(const string & proxy_url, const string & data)
{
	CURL * curl = curl_easy_init();
	assert(curl && "Failed to initiate curl");
	curl_easy_setopt(curl, CURLOPT_URL, proxy_url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)data.length());
	
	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		ErrorMsg(L"curl_easy_perform ½ÇÆÐ : %hs", curl_easy_strerror(res));
	}
	curl_easy_cleanup(curl);
}

void Poster::Post()
{
	thread t(Post_, _proxy_url, _data);
	t.detach();
}