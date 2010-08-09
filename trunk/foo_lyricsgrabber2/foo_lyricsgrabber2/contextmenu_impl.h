#pragma once

#include "ui_main_conf.h"


class lyrics_grabber_contextmenu : public contextmenu_item_v2
{
public:
	unsigned get_num_items();
	contextmenu_item_node_root * instantiate_item(unsigned p_index,metadb_handle_list_cref p_data,const GUID & p_caller);
	GUID get_item_guid(unsigned p_index);
	void get_item_name(unsigned p_index,pfc::string_base & p_out);
	void get_item_default_path(unsigned p_index,pfc::string_base & p_out) { p_out = "Lyrics Grabber"; }
	bool get_item_description(unsigned p_index,pfc::string_base & p_out);
	t_enabled_state get_enabled_state(unsigned p_index) { return contextmenu_item::DEFAULT_ON; }
	void item_execute_simple(unsigned p_index,const GUID & p_node,metadb_handle_list_cref p_data,const GUID & p_caller);

	//
	static unsigned g_find_provider_by_menu_index(const grabber::provider_ptr_list & p_list, unsigned p_index, grabber::provider_ptr & p_out);
	//! Only used to indentify location of sub-menu
	static bool g_find_provider_by_menu_guid(const grabber::provider_ptr_list & p_list, const GUID & p_guid, grabber::provider_ptr & p_out);
	static void g_new_lookup(const grabber::provider_ptr p_provider, unsigned p_index, metadb_handle_list_cref p_data);
};


class contextmenu_leaf : public contextmenu_item_node_root_leaf
{
public:
	contextmenu_leaf(const grabber::provider_ptr & p_owner,unsigned p_index) : m_owner(p_owner), m_index(p_index) {}

	bool get_display_data(pfc::string_base & p_out,unsigned & p_displayflags,metadb_handle_list_cref p_data,const GUID & p_caller)
	{
		m_owner->get_menu_item_name(m_index, p_out);
		return true;
	}

	void execute(metadb_handle_list_cref p_data,const GUID & p_caller) { lyrics_grabber_contextmenu::g_new_lookup(m_owner, m_index, p_data); }
	bool get_description(pfc::string_base & p_out) { return m_owner->get_menu_item_description(m_index, p_out); }
	GUID get_guid() { return pfc::guid_null; }
	bool is_mappable_shortcut() { return true; }

private:
	grabber::provider_ptr m_owner;
	unsigned m_index;
};

static contextmenu_item_factory_t<lyrics_grabber_contextmenu> g_myitem_factory;

class contextmenu_leaf_look : public contextmenu_item_node_root_leaf
{
public:
	bool get_display_data(pfc::string_base & p_out,unsigned & p_displayflags,metadb_handle_list_cref p_data,const GUID & p_caller);

	void execute(metadb_handle_list_cref p_data,const GUID & p_caller)
	{
		g_look_all();
	}

	bool get_description(pfc::string_base & p_out) { g_get_description(p_out); return true; }
	GUID get_guid() { return pfc::guid_null; }
	bool is_mappable_shortcut() { return true; }

	//
	static void g_get_name(pfc::string_base & p_out) { p_out = "Search all"; }
	static void g_get_description(pfc::string_base & p_out) { p_out = "Searches all available sites for lyrics.";}
	static void g_look_all();

private:
};


class contextmenu_popup : public contextmenu_item_node_root_popup
{
public:
	contextmenu_popup(const grabber::provider_ptr & p_owner) : m_owner(p_owner)
	{
		for (t_size i = 0; i < m_owner->get_menu_item_count(); i++)
		{
			m_sublist.add_item(new contextmenu_sub_leaf(m_owner, i));
		}
	}

	bool get_display_data(pfc::string_base & p_out,unsigned & p_displayflags,metadb_handle_list_cref p_data,const GUID & p_caller)
	{
		m_owner->get_provider_name(p_out);
		return true;
	}

	t_size get_children_count() { return m_owner->get_menu_item_count(); }
	contextmenu_item_node * get_child(t_size p_index) { pfc::dynamic_assert(p_index < m_sublist.get_count()); return m_sublist[p_index]; }
	GUID get_guid() { return m_owner->get_provider_guid(); }
	bool is_mappable_shortcut() { return true; }

private:
	class contextmenu_sub_leaf : public contextmenu_item_node_leaf
	{
	public:
		contextmenu_sub_leaf(const grabber::provider_ptr & p_owner,unsigned p_index) : m_owner(p_owner), m_index(p_index) {}

		bool get_display_data(pfc::string_base & p_out,unsigned & p_displayflags,metadb_handle_list_cref p_data,const GUID & p_caller)
		{
			m_owner->get_menu_item_name(m_index, p_out);
			return true;
		}

		void execute(metadb_handle_list_cref p_data,const GUID & p_caller) { lyrics_grabber_contextmenu::g_new_lookup(m_owner, m_index, p_data); }
		bool get_description(pfc::string_base & p_out) { return m_owner->get_menu_item_description(m_index, p_out); }
		GUID get_guid() { return pfc::guid_null; }
		bool is_mappable_shortcut() { return true; }

	private:
		grabber::provider_ptr m_owner;
		unsigned m_index;
	};

	pfc::ptr_list_t<contextmenu_sub_leaf> m_sublist;
	grabber::provider_ptr m_owner;
};
