#pragma once

namespace lyrics_grabber_sdk
{
	//! Use static_api_ptr_t to instantiate
	class NOVTABLE host : public service_base
	{
	public:
		// Get global config_item
		virtual const config_item & get_global_config() const = 0;

		FB2K_MAKE_SERVICE_INTERFACE_ENTRYPOINT(host);
	};
}
