#pragma once

#define NO_BUILTIN_PROVIDERS 0

#if !NO_BUILTIN_PROVIDERS


class provider_searchall : public grabber::provider_base
{
public:
	void get_provider_name(pfc::string_base & p_out) { p_out = "Search All"; }
	bool get_provider_description(pfc::string_base & p_out)
	{ 
		p_out = "Searches through every provider until lyrics are found";
		return true; 
	}
	GUID get_provider_guid()
	{
		// {D261C737-9297-4751-84CF-2F157B3FCAA2}
		static const GUID provider_guid = 
		{ 0x64A58280, 0x7E09, 0x4147, { 0xA0, 0x9d, 0x0E, 0x41, 0xE4, 0xE1, 0xE8, 0x44 } };

		return provider_guid;
	}
	bool get_provider_url(pfc::string_base & p_out)
	{
		p_out = "";
		return false;
	}

	unsigned get_menu_item_count() { return 1; }
	void get_menu_item_name(unsigned p_index, pfc::string_base & p_out)
	{
		p_out = "Search all";
	}

	pfc::string_list_impl * lookup(unsigned p_index, metadb_handle_list_cref p_meta, threaded_process_status & p_status, abort_callback & p_abort);
	void pointer_delete(void * p) { delete p; }

	void set_config(stream_reader * p_reader, t_size p_size, abort_callback & p_abort)
	{
		m_config_item = static_api_ptr_t<grabber::host>()->get_global_config();
	}

private:
	grabber::config_item m_config_item;
};

class provider_darklyrics : public grabber::provider_base
{
public:
	void get_provider_name(pfc::string_base & p_out) { p_out = "Dark Lyrics"; }
	bool get_provider_description(pfc::string_base & p_out)
	{ 
		p_out = "Dark Lyrics provide many Lyrics in the Metal Genre.";
		return true; 
	}
	GUID get_provider_guid()
	{
		// {C3CBAFEF-0597-434F-8F02-EF65F507FBD0}
		static const GUID provider_guid = 
		{ 0xC3CBAFEF, 0x0597, 0x434F, { 0x8F, 0x02, 0xEF, 0x65, 0xF5, 0x07, 0xFB, 0xD0 } };

		return provider_guid;
	}
	bool get_provider_url(pfc::string_base & p_out)
	{
		p_out = "http://www.darklyrics.com";
		return true;
	}

	unsigned get_menu_item_count() { return 1; }
	void get_menu_item_name(unsigned p_index, pfc::string_base & p_out)
	{
		p_out = "Dark Lyrics";
	}

	pfc::string_list_impl * lookup(unsigned p_index, metadb_handle_list_cref p_meta, threaded_process_status & p_status, abort_callback & p_abort);
	pfc::string8 lookup_one(unsigned p_index, const metadb_handle_ptr & p_meta, threaded_process_status & p_status, abort_callback & p_abort);
	void pointer_delete(void * p) { delete p; }

	void set_config(stream_reader * p_reader, t_size p_size, abort_callback & p_abort)
	{
		m_config_item = static_api_ptr_t<grabber::host>()->get_global_config();
	}

private:
	grabber::config_item m_config_item;
};

class provider_azlyrics : public grabber::provider_base
{
public:
	void get_provider_name(pfc::string_base & p_out) { p_out = "AZ Lyrics"; }
	bool get_provider_description(pfc::string_base & p_out)
	{ 
		p_out = "AZ Lyrics.";
		return true; 
	}
	GUID get_provider_guid()
	{
		// {943674B6-EC4E-44E2-84CF-7F2315352577}
		static const GUID provider_guid = 
		{ 0x943674B6, 0xEC4E, 0x44E2, { 0x84, 0xCF, 0x7F, 0x23, 0x15, 0x35, 0x25, 0x77 } };

		return provider_guid;
	}
	bool get_provider_url(pfc::string_base & p_out)
	{
		p_out = "http://www.azlyrics.com";
		return true;
	}

	unsigned get_menu_item_count() { return 1; }
	void get_menu_item_name(unsigned p_index, pfc::string_base & p_out)
	{
		p_out = "AZ Lyrics";
	}

	pfc::string_list_impl * lookup(unsigned p_index, metadb_handle_list_cref p_meta, threaded_process_status & p_status, abort_callback & p_abort);
	pfc::string8 lookup_one(unsigned p_index, const metadb_handle_ptr & p_meta, threaded_process_status & p_status, abort_callback & p_abort);
	void pointer_delete(void * p) { delete p; }

	void set_config(stream_reader * p_reader, t_size p_size, abort_callback & p_abort)
	{
		m_config_item = static_api_ptr_t<grabber::host>()->get_global_config();
	}

private:
	grabber::config_item m_config_item;
};

class provider_lyrdb : public grabber::provider_base
{
public:
	void get_provider_name(pfc::string_base & p_out) { p_out = "LyrDB"; }
	bool get_provider_description(pfc::string_base & p_out)
	{ 
		p_out = "LyrDB contains more than 1,000,000 lyrics and them are catalogued by title and artist. Also, you'll be able to find songs"
        "arranged with the original CD's. The artists' discography will let you to find the album you were looking for, and you'll"
        "discover new artists by following LyrDB's suggested related artists.";
		return true; 
	}
	GUID get_provider_guid()
	{
		// {CA95529F-7D73-4638-A7AB-E409DA3D0957}
		static const GUID provider_guid = 
		{ 0xCA95529F, 0x7D73, 0x4638, { 0xA7, 0xAB, 0xE4, 0x09, 0xDA, 0x3D, 0x09, 0x57 } };

		return provider_guid;
	}
	bool get_provider_url(pfc::string_base & p_out)
	{
		p_out = "http://www.lyrdb.com";
		return true;
	}

	unsigned get_menu_item_count() { return 1; }
	void get_menu_item_name(unsigned p_index, pfc::string_base & p_out)
	{
		p_out = "LyrDB";
	}

	pfc::string_list_impl * lookup(unsigned p_index, metadb_handle_list_cref p_meta, threaded_process_status & p_status, abort_callback & p_abort);
	pfc::string8 lookup_one(unsigned p_index, const metadb_handle_ptr & p_meta, threaded_process_status & p_status, abort_callback & p_abort);
	void pointer_delete(void * p) { delete p; }

	void set_config(stream_reader * p_reader, t_size p_size, abort_callback & p_abort)
	{
		m_config_item = static_api_ptr_t<grabber::host>()->get_global_config();
	}

private:
	grabber::config_item m_config_item;
};

class provider_lyricwiki : public grabber::provider_base
{
public:
	void get_provider_name(pfc::string_base & p_out) { p_out = "LyricWiki"; }
	bool get_provider_description(pfc::string_base & p_out)
	{ 
		p_out = "LyricWiki is a free wiki websize where anyone can get reliable lyrics for any song by any artist.";
		return true; 
	}
	GUID get_provider_guid()
	{
		// {72E6B3AE-1494-401E-861C-AD2ABE37E0DD}
		static const GUID provider_guid = 
		{ 0x72E6B3AE, 0x1494, 0x401E, { 0x86, 0x1C, 0xAD, 0x2A, 0xBE, 0x37, 0xE0, 0xDD } };

		return provider_guid;
	}
	bool get_provider_url(pfc::string_base & p_out)
	{
		p_out = "http://lyrics.wikia.com";
		return true;
	}

	unsigned get_menu_item_count() { return 1; }
	void get_menu_item_name(unsigned p_index, pfc::string_base & p_out)
	{
		p_out = "LyricWiki";
	}

	pfc::string_list_impl * lookup(unsigned p_index, metadb_handle_list_cref p_meta, threaded_process_status & p_status, abort_callback & p_abort);
	pfc::string8 lookup_one(unsigned p_index, const metadb_handle_ptr & p_meta, threaded_process_status & p_status, abort_callback & p_abort);
	void pointer_delete(void * p) { delete p; }

	void set_config(stream_reader * p_reader, t_size p_size, abort_callback & p_abort)
	{
		m_config_item = static_api_ptr_t<grabber::host>()->get_global_config();
	}

private:
	grabber::config_item m_config_item;
};

class provider_leoslyrics : public grabber::provider_base
{
public:
	void get_provider_name(pfc::string_base & p_out) { p_out = "LeosLyrics"; }
	bool get_provider_description(pfc::string_base & p_out)
	{ 
		p_out = "Leo's Lyrics";
		return true; 
	}
	GUID get_provider_guid()
	{
		// {D261C737-9297-4751-84CF-2F157B3FCAA2}
		static const GUID provider_guid = 
		{ 0xd261c737, 0x9297, 0x4751, { 0x84, 0xcf, 0x2f, 0x15, 0x7b, 0x3f, 0xca, 0xa2 } };

		return provider_guid;
	}
	bool get_provider_url(pfc::string_base & p_out)
	{
		p_out = "http://leoslyrics.com";
		return true;
	}

	unsigned get_menu_item_count() { return 1; }
	void get_menu_item_name(unsigned p_index, pfc::string_base & p_out)
	{
		p_out = "Leo's Lyrics";
	}

	pfc::string_list_impl * lookup(unsigned p_index, metadb_handle_list_cref p_meta, threaded_process_status & p_status, abort_callback & p_abort);
	pfc::string8 lookup_one(unsigned p_index, const metadb_handle_ptr & p_meta, threaded_process_status & p_status, abort_callback & p_abort);
	void pointer_delete(void * p) { delete p; }

	void set_config(stream_reader * p_reader, t_size p_size, abort_callback & p_abort)
	{
		m_config_item = static_api_ptr_t<grabber::host>()->get_global_config();
	}

private:
	grabber::config_item m_config_item;
};

class provider_lyricsplugin : public grabber::provider_base
{
public:
	void get_provider_name(pfc::string_base & p_out) { p_out = "LyricsPlugin"; }
	bool get_provider_description(pfc::string_base & p_out)
	{ 
		p_out = "LyricsPlugin.com is a great site providing a good many high quality lyrics for all genres.";
		return true; 
	}
	GUID get_provider_guid()
	{
		// {D261C737-9297-4751-84CF-2F157B3FCAA2}
		static const GUID provider_guid = 
		{ 0xd261c737, 0x9297, 0x4751, { 0x84, 0xcf, 0x2f, 0x15, 0x7b, 0x3f, 0xca, 0xa2 } };

		return provider_guid;
	}
	bool get_provider_url(pfc::string_base & p_out)
	{
		p_out = "http://lyricsplugin.com";
		return true;
	}

	unsigned get_menu_item_count() { return 1; }
	void get_menu_item_name(unsigned p_index, pfc::string_base & p_out)
	{
		p_out = "Lyrics Plugin";
	}

	pfc::string_list_impl * lookup(unsigned p_index, metadb_handle_list_cref p_meta, threaded_process_status & p_status, abort_callback & p_abort);
	pfc::string8 lookup_one(unsigned p_index, const metadb_handle_ptr & p_meta, threaded_process_status & p_status, abort_callback & p_abort);
	void pointer_delete(void * p) { delete p; }

	void set_config(stream_reader * p_reader, t_size p_size, abort_callback & p_abort)
	{
		m_config_item = static_api_ptr_t<grabber::host>()->get_global_config();
	}

private:
	grabber::config_item m_config_item;
};


#endif
