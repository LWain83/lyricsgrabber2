#pragma once

#include "curl/curl.h"
#include "curl/types.h"
#include "curl/easy.h"


PFC_DECLARE_EXCEPTION(exception_curl, pfc::exception, "cURL error")

class curl_wrapper_simple
{
public:
	curl_wrapper_simple(const grabber::config_item * p_config = NULL);
	~curl_wrapper_simple() throw() { _cleanup(); }
	void set_proxy(const grabber::config_item & p_config) throw();
	pfc::string8_fast quote(const char * str) throw();
	void fetch(const pfc::string_base & p_url, pfc::string_base & p_out);
	void fetch(const pfc::string_base & p_url,const pfc::string_base & p_referer,  pfc::string_base & p_out);
	void fetch_googleluck(const pfc::string_base & p_site, const pfc::string_base & p_keywords, pfc::string_base & p_out);

	static size_t g_write_memory_callback(void *ptr, size_t size, size_t nmemb, void *data) throw();

private:
	void _init();
	void _cleanup() throw();

private:
	pfc::string8_fast m_buff;
	CURL * m_curl_handle;
};
