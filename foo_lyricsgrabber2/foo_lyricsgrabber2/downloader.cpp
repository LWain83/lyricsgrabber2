#include "stdafx.h"
#include "downloader.h"


web_downloader::web_downloader()
{
	if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK)
		throw exception_curl_init();

	_init();
}

web_downloader::web_downloader(long flags)
{
	if (curl_global_init(flags) != CURLE_OK)
		throw exception_curl_session();

	_init();
}

web_downloader::~web_downloader()
{
	curl_easy_cleanup(m_curl);
}

std::string web_downloader::do_transfer(const char * url)
{
	std::string data;

	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, _write_memory_callback);
	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &data);
	curl_easy_setopt(m_curl, CURLOPT_URL, url);

	CURLcode nCode = curl_easy_perform(m_curl);

	if (nCode != CURLE_OK)
	{
		throw exception_curl_perform(curl_easy_strerror(nCode));
	}

	return data;
}

size_t web_downloader::_write_memory_callback(void * ptr, size_t size, size_t nmemb, void * data)
{
	size_t realsize = size * nmemb;
	std::string * str = reinterpret_cast<std::string *>(data);

	if (str) 
		str->append(reinterpret_cast<const char *>(ptr), realsize);

	return realsize;
}

void web_downloader::_init()
{
	m_curl = curl_easy_init();
	if (!m_curl)
		throw exception_curl_session();
}

std::string web_downloader::escape( const char * str, size_t len )
{
	char * pstr = curl_easy_escape(m_curl, str, len);
	std::string s(pstr);
	
	curl_free(pstr);
	return s;
}
