#pragma once

#include "py_site.h"


class py_provider : public grabber::provider_base
{
public:
	py_provider() : m_conf_dlg(NULL)
	{
	}

	//! Provider basics
	void get_provider_name(pfc::string_base & p_out) { p_out = "Python"; };
	GUID get_provider_guid();
	bool get_provider_description(pfc::string_base & p_out);
	bool get_provider_url(pfc::string_base & p_out);

	// Context menu items
	bool is_menu_popup() { return true; }
	unsigned get_menu_item_count();

	void get_menu_item_name(unsigned p_index, pfc::string_base & p_out);
	bool get_menu_item_description(unsigned p_index, pfc::string_base & p_out) { p_out = ""; return false; }

	// Querying for lyrics (execute command)
	pfc::string_list_impl * lookup(unsigned p_index, metadb_handle_list_cref p_meta, threaded_process_status & p_status, abort_callback & p_abort);
	void pointer_delete(void * p) { delete p; }

	// Configuration
	bool have_config_popup() { return true; }
	bool show_config_popup(HWND wnd_parent);
	void set_config(stream_reader * p_reader, t_size p_size, abort_callback & p_abort);
	void get_config(stream_writer * p_writer, abort_callback & p_abort);

private:
	py_site m_site;
	CConfDlg * m_conf_dlg;
};
