#pragma once

namespace lyrics_grabber_sdk
{
	enum t_proxy_type
	{
		PROXY_NONE = 0,
		PROXY_HTTP,
		PROXY_SOCKS4,
		PROXY_SOCKS5,
	};

	enum t_config_version
	{
		VERSION_CURRENT = 0x400,
		VERSION_0107 = 0x107,
	};

	struct config_item
	{
		pfc::string8 lyric_field_name;
		bool title_formatting;
		t_uint32 proxy_type;
		bool proxy_need_auth;
		pfc::string8 proxy_hostport, proxy_username, proxy_password;
		bool skip_exist;
		bool quiet_mode;
	};
}
