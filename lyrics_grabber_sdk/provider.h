#pragma once

namespace lyrics_grabber_sdk
{
	// Base interface for lyric providers
	// Use provider_factory<T> to instantiate
	class NOVTABLE provider_base : public service_base
	{
	public:
		// Provider basics
		virtual void get_provider_name(pfc::string_base & p_out) = 0;
		virtual GUID get_provider_guid() = 0;
		virtual bool get_provider_description(pfc::string_base & p_out) { p_out = ""; return false; }
		virtual bool get_provider_url(pfc::string_base & p_out) { p_out = ""; return false; }

		// Context menu items
		virtual bool is_menu_popup() { return false; }
		virtual unsigned get_menu_item_count() = 0;
		// By default, guid is based on menu item name and its index
		virtual GUID get_menu_item_guid(unsigned p_index)
		{
			static_api_ptr_t<hasher_md5> hasher;
			pfc::string8_fast_aggressive buf;

			get_menu_item_name(p_index, buf);
			buf << p_index;
			return hasher->process_single_guid(buf, buf.get_length());
		}
		virtual void get_menu_item_name(unsigned p_index, pfc::string_base & p_out) = 0;
		virtual bool get_menu_item_description(unsigned p_index, pfc::string_base & p_out) { p_out = ""; return false; }


		// Querying for lyrics (execute command)
		// Return value should allocated in this function, and freed in on_done
		// Could return NULL so pointer_delete() would not be called
		virtual pfc::string_list_impl * lookup(unsigned p_index, metadb_handle_list_cref p_meta, threaded_process_status & p_status, abort_callback & p_abort) = 0;
		virtual pfc::string8 lookup_one(unsigned p_index, const metadb_handle_ptr & p_meta, threaded_process_status & p_status, abort_callback & p_abort) {return "";}

		// IMPORTANT: Implement this to free pointer correctly (cross DLL safety)
		// Called when p is not NULL
		virtual void pointer_delete(void * p) = 0;

		// Configuration
		virtual bool have_config_popup() { return false; }
		virtual bool show_config_popup(HWND wnd_parent) { return false; }
		// Caution: this is always called, even no configuration available, in that case, p_size = 0 and p_reader is NULL
		virtual void set_config(stream_reader * p_reader, t_size p_size, abort_callback & p_abort) {}
		virtual void get_config(stream_writer * p_writer, abort_callback & p_abort) {}

		static const GUID menu_item_configuration;

		FB2K_MAKE_SERVICE_INTERFACE_ENTRYPOINT(provider_base)
	};

	template<typename T>
	class provider_factory : public service_factory_single_t<T> {};

	typedef service_ptr_t<provider_base> provider_ptr;
	typedef pfc::list_t<provider_ptr> provider_ptr_list;
}
