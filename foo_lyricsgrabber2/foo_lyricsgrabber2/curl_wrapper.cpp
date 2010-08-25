#include "stdafx.h"
#include "curl_wrapper.h"


curl_wrapper_simple::curl_wrapper_simple(const grabber::config_item * p_config) throw(): m_curl_handle(NULL)
{
	_init();

	if (p_config)
		set_proxy(*p_config);

	// Set Timeout for connect
	// 10 sec
	curl_easy_setopt(m_curl_handle, CURLOPT_CONNECTTIMEOUT, 10);

	// Set low speed
	curl_easy_setopt(m_curl_handle, CURLOPT_LOW_SPEED_LIMIT, (long)0); 
	curl_easy_setopt(m_curl_handle, CURLOPT_LOW_SPEED_TIME, (long)15);

	// send all data to this function
	curl_easy_setopt(m_curl_handle, CURLOPT_WRITEFUNCTION, g_write_memory_callback);

	// we pass our 'chunk' struct to the callback function 
	curl_easy_setopt(m_curl_handle, CURLOPT_WRITEDATA, &m_buff);
}

void curl_wrapper_simple::_init()
{
	if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK)
	{
		throw exception_curl("curl_global_init");
	}

	// init curl session
	m_curl_handle = curl_easy_init();

	if (!m_curl_handle)
	{
		throw exception_curl("curl_easy_init() faield");
	}
}

void curl_wrapper_simple::_cleanup()
{
	curl_easy_cleanup(m_curl_handle);
}

void curl_wrapper_simple::set_proxy(const grabber::config_item & p_config)
{
	// Basic curl setting
	// Set Proxy
	if (p_config.proxy_type != grabber::PROXY_NONE)
	{
		curl_proxytype proxy_type;

		switch (p_config.proxy_type)
		{
		case grabber::PROXY_SOCKS4:
			proxy_type = CURLPROXY_SOCKS4;
			break;

		case grabber::PROXY_SOCKS5:
			proxy_type = CURLPROXY_SOCKS5;
			break;

		case grabber::PROXY_HTTP:
		default:
			proxy_type = CURLPROXY_HTTP;
			break;
		}

		// proxy type
		curl_easy_setopt(m_curl_handle, CURLOPT_PROXYTYPE, proxy_type);

		// proxy address:port
		curl_easy_setopt(m_curl_handle, CURLOPT_PROXY, p_config.proxy_hostport.get_ptr());

		if (p_config.proxy_need_auth)
		{
			pfc::string8_fast proxy = p_config.proxy_username;

			proxy.add_char(':');
			proxy.add_string(p_config.proxy_password);
			curl_easy_setopt(m_curl_handle, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
			curl_easy_setopt(m_curl_handle, CURLOPT_PROXYUSERPWD, proxy.get_ptr());
		}
	}
}

pfc::string8_fast curl_wrapper_simple::quote(const char * str)
{
	char * temp = curl_easy_escape(m_curl_handle, str, 0);
	pfc::string8_fast buffer;

	if (temp)
	{
		buffer = temp;
		curl_free(temp);
		return buffer;
	}

	return "";
}

size_t curl_wrapper_simple::g_write_memory_callback(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size * nmemb;
	pfc::string_base *str = reinterpret_cast<pfc::string_base *>(data);

	if (str) 
		str->add_string(reinterpret_cast<const char *>(ptr), realsize);

	return realsize;
}

void curl_wrapper_simple::fetch(const pfc::string_base & p_url, pfc::string_base & p_out)
{
	CURLcode nCode;

	curl_easy_setopt(m_curl_handle, CURLOPT_URL, p_url.get_ptr());
	nCode = curl_easy_perform(m_curl_handle);

	if (nCode != CURLE_OK)
	{
		m_buff.reset();
		throw exception_curl(curl_easy_strerror(nCode));
	}
	else
	{
		p_out = m_buff;
		m_buff.reset();
	}
}

void curl_wrapper_simple::fetch(const pfc::string_base & p_url,const pfc::string_base & p_referer,  pfc::string_base & p_out)
{
	CURLcode nCode;

	curl_slist list;

	

	curl_easy_setopt(m_curl_handle, CURLOPT_URL, p_url.get_ptr());
	curl_easy_setopt(m_curl_handle, CURLOPT_REFERER, p_referer.get_ptr());

	nCode = curl_easy_perform(m_curl_handle);

	if (nCode != CURLE_OK)
	{
		m_buff.reset();
		throw exception_curl(curl_easy_strerror(nCode));
	}
	else
	{
		p_out = m_buff;
		m_buff.reset();
	}
}

void curl_wrapper_simple::fetch_googleluck(const pfc::string_base & p_site, const pfc::string_base & p_keywords, pfc::string_base & p_out)
{
	CURLcode nCode;
	
	pfc::string8_fast url = "http://www.google.com/search?hl=en&ie=UTF-8&oe=UTF-8&q=";
	url += p_keywords;
	url += "+";
	url += p_site;
	url += "&btnI=I%27m+Feeling+Lucky";

	curl_easy_setopt(m_curl_handle, CURLOPT_FOLLOWLOCATION, 1);

	fetch(url, url, p_out);
}
