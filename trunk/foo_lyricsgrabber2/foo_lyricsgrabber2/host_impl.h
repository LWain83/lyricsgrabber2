#pragma once

#include "cfg.h"


class initquit_impl : public initquit
{
public:
	void on_init();
	void on_quit();
};

class host_impl : public grabber::host
{
public:
	const grabber::config_item & get_global_config() const;

	static grabber::provider_ptr_list & g_get_provider_list() throw();
	static grabber::config_item & g_get_config_ref() throw();

	static void g_load_providers() throw();
	static void g_save_providers() throw();

private:
	static grabber::provider_ptr_list provider_ptr_list;
	static const GUID guid_config;
	static bool m_inited;
};
