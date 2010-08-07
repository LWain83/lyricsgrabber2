#include "stdafx.h"
#include "cfg.h"
#include "tag_writer.h"
#include "host_impl.h"
#include "debug.h"


void main_thread_callback_tag_writer::callback_run()
{
	//CTaggerDialog * m_tagger_dlg = new CTaggerDialog(m_handles, m_values);
	if (m_tagger_dlg)
	{
		m_tagger_dlg->Create(core_api::get_main_window());
		m_tagger_dlg->CenterWindow();
		m_tagger_dlg->ShowWindow(SW_SHOWNORMAL);
	}
	else
	{
		popup_message_error("Create tagger dialog failed.");
	}
}

tag_writter::tag_writter(metadb_handle_list_cref p_handles, const pfc::string_list_impl & p_values, const pfc::list_base_const_t<const metadb_handle *> & p_skipped) 
: m_data(p_values)
{
	pfc::dynamic_assert(p_handles.get_count() == p_values.get_count());
	pfc::array_t<t_size> order;

	m_fieldname = host_impl::g_get_config_ref().lyric_filed_name;
	order.set_size(p_handles.get_count());
	order_helper::g_fill(order.get_ptr(), order.get_size());
	p_handles.sort_get_permutation_t(pfc::compare_t<metadb_handle_ptr, metadb_handle_ptr>, order.get_ptr());
	m_handles.set_count(order.get_size());
	m_values.set_count(order.get_size());

	bool has_skipped_items = p_skipped.get_count() > 0;

	for(t_size n = 0; n < order.get_count(); ++n) 
	{
		metadb_handle_ptr ptr = p_handles[order[n]];

		if (has_skipped_items)
		{
			t_size index = p_skipped.find_item(ptr.get_ptr());

			if (index != infinite)
				continue;
		}

		m_handles.add_item(ptr);
		m_values.add_item(m_data[order[n]]);
	}
}

bool tag_writter::apply_filter(metadb_handle_ptr p_location,t_filestats p_stats,file_info & p_info)
{
	t_size index;

	if (m_handles.bsearch_t(pfc::compare_t<metadb_handle_ptr,metadb_handle_ptr>,p_location,index))
	{
		// Null string, skip writing.
		if (!m_values[index] || !*m_values[index])
			return false;
		
		p_info.meta_set(m_fieldname, m_values[index]);
		return true;
	}

	return false;
}
