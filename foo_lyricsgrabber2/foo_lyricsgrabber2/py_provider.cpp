#include "stdafx.h"
#include "ui_conf.h"
#include "py_provider.h"


static grabber::provider_factory<py_provider> g_provider;

GUID py_provider::get_provider_guid()
{
	// {6323D7E6-328D-4168-A325-CE78C0120BDE}
	static const GUID guid = 
	{ 0x6323d7e6, 0x328d, 0x4168, { 0xa3, 0x25, 0xce, 0x78, 0xc0, 0x12, 0xb, 0xde } };

	return guid;
}

bool py_provider::get_provider_description(pfc::string_base & p_out)
{
	p_out = "Add support for user-defined provider in python scripting language\n\n\n\n";
	p_out += "Copyright (c) 2001-2008 Python Software Foundation.\nAll Rights Reserved.\n\n";
	p_out += "Copyright (c) 2000 BeOpen.com.\nAll Rights Reserved.\n\n";
	p_out += "Copyright (c) 1995-2001 Corporation for National Research Initiatives.\nAll Rights Reserved.\n\n";
	p_out += "Copyright (c) 1991-1995 Stichting Mathematisch Centrum, Amsterdam.\nAll Rights Reserved\n\n";
	return true;
}

bool py_provider::get_provider_url(pfc::string_base & p_out)
{
	p_out = "http://code.google.com/p/lyricsgrabber2/";
	return true;
}

unsigned py_provider::get_menu_item_count()
{
	return m_site.get_script_count();
}

void py_provider::get_menu_item_name(unsigned p_index, pfc::string_base & p_out)
{
	p_out = m_site.get_script_name(p_index);
}

pfc::string_list_impl * py_provider::lookup(unsigned p_index, metadb_handle_list_cref p_meta, threaded_process_status & p_status, abort_callback & p_abort)
{
	TRACK_CALL_TEXT("py_provider::lookup");
	pfc::string_list_impl * data = new pfc::string_list_impl;
	bool status;

	status = m_site.invoke(p_index, p_meta, p_status, p_abort, *data);

	if (status == false)
	{
		delete data;
		data = NULL;
	}

	return data;
}

bool py_provider::show_config_popup(HWND wnd_parent)
{
	if (!m_conf_dlg || !m_conf_dlg->IsWindow())
	{
		m_conf_dlg = new CConfDlg(m_site);
		m_conf_dlg->Create(wnd_parent);
	}

	m_conf_dlg->ShowWindow(SW_SHOW);
	return true;
}

void py_provider::set_config(stream_reader * p_reader, t_size p_size, abort_callback & p_abort)
{
	m_site.get_config().read_from_stream(p_reader, p_size, p_abort);
	// HACK: For services is not prepared on DLL loading
	m_site.init();
}

void py_provider::get_config(stream_writer * p_writer, abort_callback & p_abort)
{
	m_site.get_config().write_to_stream(p_writer, p_abort);
}
