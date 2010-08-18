#include "stdafx.h"
#include "task.h"
#include "tag_writer.h"
#include "debug.h"
#include "host_impl.h"


void lyric_lookup_task::run(threaded_process_status & p_status,abort_callback & p_abort)
{
	TRACK_CALL_TEXT("lyric_lookup_task::run");

	if (m_handles.get_count() == 0)
	{
		return;
	}

	// Quering
	if (m_provider.is_valid())
	{
		TRACK_CODE("lookup", m_values_ptr = m_provider->lookup(m_index, m_handles, p_status, p_abort));
	}
	else
	{
		popup_message_error("No provider presents.");
	}
}

void lyric_lookup_task::start()
{
	m_provider->get_menu_item_name(m_index, m_name);

	threaded_process::g_run_modeless(
		this,
		threaded_process::flag_show_abort |
		threaded_process::flag_show_progress |
		threaded_process::flag_show_item |
		threaded_process::flag_show_delayed |
		threaded_process::flag_no_focus,
		core_api::get_main_window(),
		pfc::string_formatter() << "Querying " << m_name << " for lyrics..."
		);
}

lyric_lookup_task::lyric_lookup_task(const grabber::provider_ptr & p_provider, unsigned p_index, metadb_handle_list_cref p_data)
: m_provider(p_provider)
, m_index(p_index)
, m_values_ptr(NULL)
{
	grabber::config_item & item = host_impl::g_get_config_ref();
	static_api_ptr_t<titleformat_compiler> compiler;
	service_ptr_t<titleformat_object> script;
	
	if (item.skip_exist)
	{
		pfc::string8 field_name;

		// Skip tracks which contains lyric
		for (t_size i = 0; i < p_data.get_count(); i++)
		{
			if (item.title_formatting)
			{
				compiler->compile_safe(script, item.lyric_field_name);
				p_data[i]->format_title(NULL, field_name, script, NULL);
			}
			else
				field_name = item.lyric_field_name;

			metadb_handle_ptr & ptr = p_data[i];
			const file_info * pinfo;

			ptr->metadb_lock();

			if (ptr->get_info_locked(pinfo))
			{
				if (!pinfo->meta_exists(field_name))
				{
					m_handles.add_item(ptr);
				}
			}

			ptr->metadb_unlock();
		}
	}
	else
	{
		m_handles.add_items(p_data);
	}
}

void lyric_lookup_task::on_done(HWND p_wnd,bool p_was_aborted)
{
	TRACK_CALL_TEXT("lyric_lookup_task::on_done");

	if (!m_values_ptr)
	{
		return;
	}

	if (!p_was_aborted)
	{
		if (host_impl::g_get_config_ref().quiet_mode)
		{
			pfc::list_t<const metadb_handle *> dummy;

			static_api_ptr_t<metadb_io_v2>()->update_info_async(m_handles, 
				new service_impl_t<tag_writter>(m_handles, *m_values_ptr, dummy), 
				core_api::get_main_window(), metadb_io_v2::op_flag_delay_ui, NULL);
		}
		else
		{
			// Add tag updating task to a main thread, which will create modeless dialog there
			static_api_ptr_t<main_thread_callback_manager>()->add_callback(new service_impl_t<main_thread_callback_tag_writer>(m_name, m_handles, *m_values_ptr));
		}
	}

	TRACK_CODE("pointer_delete", m_provider->pointer_delete(m_values_ptr));
}
