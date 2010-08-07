#pragma once

#include "ui_tagger.h"


class main_thread_callback_tag_writer : public main_thread_callback
{
public:
	main_thread_callback_tag_writer(const pfc::string_base & p_name, metadb_handle_list_cref p_handles, const pfc::string_list_impl & p_values)
		: m_tagger_dlg(new CTaggerDialog(p_name, p_handles, p_values))
	{
	}

	void callback_run();

private:
	CTaggerDialog * m_tagger_dlg;
};

class tag_writter : public file_info_filter
{
public:
	tag_writter(metadb_handle_list_cref p_handles, const pfc::string_list_impl & p_values, const pfc::list_base_const_t<const metadb_handle *> & p_skipped);
	bool apply_filter(metadb_handle_ptr p_location,t_filestats p_stats,file_info & p_info);

private:
	metadb_handle_list m_handles;
	pfc::list_t<const char *> m_values;
	pfc::string_list_impl m_data;
	pfc::string8 m_fieldname;
};
