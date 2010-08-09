#include "stdafx.h"
#include "cmdline_handler.h"
#include "host_impl.h"
#include "task.h"
#include "contextmenu_impl.h"


static commandline_handler_factory_t<cmdline_handler> g_cmdline_handler;

cmdline_handler::result cmdline_handler::on_token(const char * token)
{
	const char exec_prefix[] = "/grabber_exec:";
	const char conf_prefix[] = "/grabber_conf";

	// HACK: initquit called later than commandline_handler
	host_impl::g_load_providers();

	// /exec_grabber:name=commandname <file1> [<file2> <file3> ...]
	if (strncmp(token, exec_prefix, _countof(exec_prefix) - 1) == 0)
	{
		if (strlen(token) <= _countof(exec_prefix))
		{
			return RESULT_NOT_OURS;
		}

		const char * p = token + _countof(exec_prefix) - 1;
		const grabber::config_item & config = host_impl::g_get_config_ref();
		pfc::string8 cmd;
		bool name_status;

		name_status = g_find_value(p, "name", cmd);

		if (name_status)
		{
			if (g_find_command_by_name(cmd, m_provider, m_index))
				return RESULT_PROCESSED_EXPECT_FILES;
		}

		return RESULT_NOT_OURS;
	}
	// Configuration window
	else if (strncmp(token, conf_prefix, _countof(conf_prefix) - 1) == 0)
	{
		//contextmenu_leaf_conf::g_show_conf_win();

		return RESULT_PROCESSED;
	}

	return RESULT_NOT_OURS;
}

void cmdline_handler::on_files_done()
{
	service_ptr_t<lyric_lookup_task> task = new service_impl_t<lyric_lookup_task>(m_provider, m_index, m_handles);
	task->start();

	// stub
	m_handles.remove_all();
}

void cmdline_handler::on_file(const metadb_handle_ptr & ptr)
{
	m_handles.add_item(ptr);
}

bool cmdline_handler::g_find_value(const char * string, const char * name, pfc::string_base & value)
{
	pfc::string8_fast tmp;
	const char sym_equal = '=';
	const char sep = ';';
	t_size pos;
	t_size end;

	tmp = name;
	tmp.add_char(sym_equal);
	pos = pfc::string_find_first(string, tmp);

	if (pos != infinite && (pos == 0 || string[pos - 1] == ';'))
	{
		pos += tmp.length();
		end = pfc::string_find_first(string, sep, pos);

		if (end == infinite)
		{
			value.set_string(string + pos);
			return true;
		}
		else
		{
			value.set_string(string + pos, end - pos);
			return true;
		}
	}

	return false;
}

bool cmdline_handler::g_find_command_by_name(const char * command, grabber::provider_ptr & provider, unsigned & index)
{
	// Taked from pfc
	service_enum_t<grabber::provider_base> e;
	grabber::provider_ptr ptr;
	unsigned i = 0;

	if (e.first(ptr)) do
	{
		bool is_popup = ptr->is_menu_popup();
		unsigned num_actions = ptr->get_menu_item_count();

		for(unsigned action = 0; action < num_actions; action++)
		{
			pfc::string8_fast path, name;

			if (is_popup)
			{
				ptr->get_provider_name(path);
			}

			ptr->get_menu_item_name(action, name);

			if (!path.is_empty())
				path += "/";

			path += name;

			if (!stricmp_utf8(command, path))
			{
				provider = ptr;
				index = action;
				return true;
			}
		}
	} while(e.next(ptr));

	return false;
}
