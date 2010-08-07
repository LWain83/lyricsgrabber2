#pragma once

#define CURL_STATICLIB
#include <curl/curl.h>
#include <curl/easy.h>
#include <string>

class web_downloader
{
public:
	PFC_DECLARE_EXCEPTION(exception_curl		, pfc::exception, "Curl Error")
	PFC_DECLARE_EXCEPTION(exception_curl_init	, exception_curl, "Global libcurl initialisation failed")
	PFC_DECLARE_EXCEPTION(exception_curl_session, exception_curl, "Start easy Session failed")
	PFC_DECLARE_EXCEPTION(exception_curl_perform, exception_curl, "File transfer failed")

	// Constructor
	web_downloader();
	web_downloader(long flags);

	// Destructor
	~web_downloader();

	// Raw handle for curl
	CURL * get_curl_handle() { return m_curl; }

	template <typename T>
	void set_option(CURLoption option, T param)
	{
		curl_easy_setopt(m_curl, option, param);
	}

	std::string do_transfer(const char * url);

public:
	std::string escape(const char * str, size_t len = 0);

private:
	void _init();
	static size_t _write_memory_callback(void * ptr, size_t size, size_t nmemb, void * data);

private:
	CURL * m_curl;
	char m_error[CURL_ERROR_SIZE];
};

