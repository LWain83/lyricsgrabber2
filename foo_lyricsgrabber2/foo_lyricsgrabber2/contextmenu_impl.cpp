#include "stdafx.h"
#include "contextmenu_impl.h"
#include "host_impl.h"
#include "task.h"
#include "ui_main_conf.h"

#include "debug.h"

static contextmenu_group_popup_factory g_contextmenu(lyrics_grabber_contextmenu::class_guid, contextmenu_groups::root, "Lyrics Grabber", 0);
CMainConf * contextmenu_leaf_conf::g_conf_dlg = NULL;

unsigned lyrics_grabber_contextmenu::get_num_items()
{
	unsigned count = 1;
	const grabber::provider_ptr_list list = host_impl::g_get_provider_list();

	for (t_size i = 0; i < list.get_count(); i++)
	{
		count += list[i]->is_menu_popup() ? 1 : list[i]->get_menu_item_count();
	}

	return count;
}

contextmenu_item_node_root * lyrics_grabber_contextmenu::instantiate_item(unsigned p_index,metadb_handle_list_cref p_data,const GUID & p_caller)
{
	grabber::provider_ptr ptr;

	if (g_find_provider_by_menu_index(host_impl::g_get_provider_list(), p_index, ptr) != infinite )
	{
		if (ptr->is_menu_popup())
		{
			// Create popup menu item
			return new contextmenu_popup(ptr);
		}
		else
		{
			// Create normal command menu item
			return new contextmenu_leaf(ptr, p_index);
		}
	}
	else
	{
		// Configuration
		return new contextmenu_leaf_conf;
	}
}

GUID lyrics_grabber_contextmenu::get_item_guid(unsigned p_index)
{
	grabber::provider_ptr ptr;
	unsigned i;

	if ((i = g_find_provider_by_menu_index(host_impl::g_get_provider_list(), p_index, ptr)) != infinite)
	{
		if (ptr->is_menu_popup())
		{
			return ptr->get_provider_guid();
		}
		else
		{
			return ptr->get_menu_item_guid(p_index - i);
		}
	}
	else
	{
		// Configuration
		return grabber::provider_base::menu_item_configuration;
	}
}

void lyrics_grabber_contextmenu::get_item_name(unsigned p_index,pfc::string_base & p_out)
{
	grabber::provider_ptr ptr;
	unsigned i;

	if ((i = g_find_provider_by_menu_index(host_impl::g_get_provider_list(), p_index, ptr)) != infinite)
	{
		if (ptr->is_menu_popup())
		{
			ptr->get_provider_name(p_out);
		}
		else
		{
			ptr->get_menu_item_name(p_index - i, p_out);
		}
	}
	else
	{
		// Configuration
		contextmenu_leaf_conf::g_get_name(p_out);
	}
}

bool lyrics_grabber_contextmenu::get_item_description(unsigned p_index,pfc::string_base & p_out)
{
	grabber::provider_ptr ptr;
	unsigned i;

	if ((i = g_find_provider_by_menu_index(host_impl::g_get_provider_list(), p_index, ptr)) != infinite)
	{
		if (ptr->is_menu_popup())
		{
			p_out = "";
			return false;
		}
		else
		{
			ptr->get_menu_item_description(p_index - i, p_out);
		}
	}
	else
	{
		// Configuration
		contextmenu_leaf_conf::g_get_name(p_out);
	}

	return true;
}

void lyrics_grabber_contextmenu::item_execute_simple(unsigned p_index,const GUID & p_node,metadb_handle_list_cref p_data,const GUID & p_caller)
{
	grabber::provider_ptr ptr;

	if (p_node == pfc::guid_null)
	{
		// Default leaf
		unsigned i;

		if ((i = g_find_provider_by_menu_index(host_impl::g_get_provider_list(), p_index, ptr)) != infinite)
		{
			// Start new query
			g_new_lookup(ptr, p_index - i, p_data);
		}
		else
		{
			// Configuration
			contextmenu_leaf_conf::g_show_conf_win();
		}
	}
	else
	{
		// Sub menu
		if (g_find_provider_by_menu_guid(host_impl::g_get_provider_list(), p_node, ptr))
		{
			// Start new query
			g_new_lookup(ptr, p_index, p_data);
		}
	}
}

unsigned lyrics_grabber_contextmenu::g_find_provider_by_menu_index(const grabber::provider_ptr_list & p_list, unsigned p_index, grabber::provider_ptr & p_out)
{
	unsigned start = 0;
	unsigned end = 0;

	for (t_size i = 0; i < p_list.get_count(); i++)
	{
		unsigned count = p_list[i]->is_menu_popup() ? 1 : p_list[i]->get_menu_item_count();
		end = start + count - 1;

		if (start <= p_index && p_index <= end)
		{
			p_out = p_list[i];
			return start;
		}

		start = end + 1;
	}

	return infinite;
}

bool lyrics_grabber_contextmenu::g_find_provider_by_menu_guid(const grabber::provider_ptr_list & p_list, const GUID & p_guid, grabber::provider_ptr & p_out)
{
	for (t_size i = 0; i < p_list.get_count(); i++)
	{
		grabber::provider_ptr ptr = p_list[i];

		if (!ptr->is_menu_popup())
			continue;

		if (ptr->get_provider_guid() == p_guid)
		{
			p_out = ptr;
			return true;
		}
	}

	return false;
}

void lyrics_grabber_contextmenu::g_new_lookup(const grabber::provider_ptr p_provider, unsigned p_index, metadb_handle_list_cref p_data)
{
	service_ptr_t<lyric_lookup_task> task = new service_impl_t<lyric_lookup_task>(p_provider, p_index, p_data);
	task->start();
}

void contextmenu_leaf_conf::g_show_conf_win()
{
	if (!g_conf_dlg || !g_conf_dlg->IsWindow())
	{
		g_conf_dlg = new CMainConf;
		g_conf_dlg->Create(core_api::get_main_window());
	}

	g_conf_dlg->CenterWindow(core_api::get_main_window());
	g_conf_dlg->ShowWindow(SW_SHOW);
}

bool contextmenu_leaf_conf::get_display_data(pfc::string_base & p_out,unsigned & p_displayflags,metadb_handle_list_cref p_data,const GUID & p_caller)
{
	g_get_name(p_out);

	if (g_conf_dlg && g_conf_dlg->IsWindow())
	{
		p_displayflags = FLAG_GRAYED;
	}
	else
	{
		p_displayflags = 0;
	}

	return true;
}

bool contextmenu_leaf_look::get_display_data(pfc::string_base & p_out,unsigned & p_displayflags,metadb_handle_list_cref p_data,const GUID & p_caller)
{
	g_get_name(p_out);

	p_displayflags = 0;

	return true;
}

// function to scan through all lookups
void contextmenu_leaf_look::g_look_all()
{
}
