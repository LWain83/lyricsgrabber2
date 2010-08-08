#include "stdafx.h"
#include "builtin_providers.h"

#if !NO_BUILTIN_PROVIDERS
#include "debug.h"
#include "helper.h"
#include "curl_wrapper.h"
#include "deelx.h"

#include "host_impl.h"


//static grabber::provider_factory<provider_lyricsplugin> g_lyricsplugin;
static grabber::provider_factory<provider_searchall> g_searchall;
static grabber::provider_factory<provider_darklyrics> g_darklyrics;

FORCEINLINE void how_to_sleep(t_size p_items)
{
	// 'Cause We all don't wanto flood server
	const DWORD magic_sleep[] = {300, 700, 1200, 1800, 2500, 3300, 4200, 5200, 5500};
	const t_size magic_items[] = {0, 60, 140, 270, 390, 500, 620, 880, 1000, infinite};

	for (int i = 0; i < _countof(magic_items); i++)
	{
		if (magic_items[i] < p_items && p_items <= magic_items[i])
		{
			Sleep(magic_sleep[i]);
			break;
		}
	}
}

pfc::string_list_impl * provider_searchall::lookup(unsigned p_index, metadb_handle_list_cref p_meta, threaded_process_status & p_status, abort_callback & p_abort)
{
	pfc::string_list_impl * str_list = new pfc::string_list_impl;
	const grabber::provider_ptr_list list = host_impl::g_get_provider_list();

	pfc::string8 provider;
	pfc::string8 buffer;
	try
	{
		for (t_size i = 0; i < p_meta.get_count(); ++i)
		{
			if (p_abort.is_aborting())
				break;

			bool found = false;

			// Sleep
			how_to_sleep(i);

			buffer.reset();

			const metadb_handle_ptr & p = p_meta.get_item(i);

			if (p.is_empty())
			{
				str_list->add_item("");
				continue;
			}

			// Set progress
			pfc::string8_fast path = file_path_canonical(p->get_path());

			// add subsong index?
			if (p->get_subsong_index() > 0)
			{
				path.add_string(" /index:");
				path.add_string(pfc::format_uint(p->get_subsong_index()));
			}

			p_status.set_item_path(path);

			for (t_size j = 0; j < list.get_count(); j++)
			{
				if (list[j] == this)
					continue;

				p_status.set_progress((i+1)*(j+1), p_meta.get_count() * list.get_count());

				list[j]->get_provider_name(provider);

				if (provider.find_first("Python") == -1)
				{
					buffer = list[j]->lookup_one(j, p, p_status, p_abort);
					
					if (buffer != "")
						break;
				}
			}	

			p_status.set_progress(i + 1, p_meta.get_count());

			str_list->add_item(buffer);
		}
	}
	catch (pfc::exception & e)
	{
		console_error(e.what());
		delete str_list;
		return NULL;
	}
	catch (...)
	{
		delete str_list;
		return NULL;
	}

	return str_list;
}

//************************************************************************
//*                            LyricsPlugin                              *
//************************************************************************
pfc::string_list_impl * provider_lyricsplugin::lookup(unsigned p_index, metadb_handle_list_cref p_meta, threaded_process_status & p_status, abort_callback & p_abort)
{
	// Regular Expression Class
	CRegexpT<char> regexp;
	MatchResult match;

	// Buffer
	pfc::string8 buff;
	pfc::string_list_impl * str_list = new pfc::string_list_impl;

	try
	{
		// Init fetcher
		curl_wrapper_simple fetcher(&m_config_item);

		for (t_size i = 0; i < p_meta.get_count(); ++i)
		{
			if (p_abort.is_aborting())
				break;

			// Sleep
			how_to_sleep(i);
			// Clear buff
			buff.reset();

			const metadb_handle_ptr & p = p_meta.get_item(i);

			if (p.is_empty())
			{
				str_list->add_item("");
				continue;
			}

			// Set progress
			pfc::string8_fast path = file_path_canonical(p->get_path());

			// add subsong index?
			if (p->get_subsong_index() > 0)
			{
				path.add_string(" /index:");
				path.add_string(pfc::format_uint(p->get_subsong_index()));
			}

			p_status.set_item_path(path);
			p_status.set_progress(i + 1, p_meta.get_count());

			pfc::string8_fast artist, title;
			static_api_ptr_t<titleformat_compiler> compiler;
			service_ptr_t<titleformat_object> script;

			// Get ARTIST
			compiler->compile_safe(script, "[%artist%]");
			p->format_title(NULL, artist, script, NULL);
			// Get TITLE
			compiler->compile_safe(script, "[%title%]");
			p->format_title(NULL, title, script, NULL);

			// Fetching from HTTP
			// Set HTTP Address
			pfc::string8_fast url("http://www.lyricsplugin.com/winamp03/plugin/content.php?artist=");
			pfc::string8_fast referer("http://www.lyricsplugin.com/winamp03/plugin/?artist=");
			
			// URL = http://www.lyricsplugin.com/winamp03/plugin/content.php?artist=<artist>&title=<title>&td=<timestamp>
			// REFERER = http://www.lyricsplugin.com/winamp03/plugin/?artist=<artist>&title=<title>
			
			url += fetcher.quote(artist);
			url += "&title=";
			url += fetcher.quote(title);

			referer += fetcher.quote(artist);
			referer += "&title=";
			referer += fetcher.quote(title);

			// Get it now
			try
			{
				fetcher.fetch(url, referer, buff);
			}
			catch (pfc::exception & e)
			{
				console_error(e.what());
				str_list->add_item("");
				continue;
			}
			catch (...)
			{
				str_list->add_item("");
				continue;
			}

			const char * regex_lyricbox = "<div\\s+id\\s*?=\\s*?\"lyrics\"\\s*?>[\\r\\n]*(.*?)[\\r\\n]*</div>";

			// expression for extract lyrics
			regexp.Compile(regex_lyricbox, SINGLELINE);

			// match
			MatchResult result = regexp.Match(buff.get_ptr());

			// Get Group
			if (result.IsMatched())
			{
				int nStart = result.GetGroupStart(1);
				int nEnd = result.GetGroupEnd(1);
				int index;
				pfc::string8_fast lyric(buff.get_ptr() + nStart, nEnd - nStart);

				convert_html_to_plain(lyric);

				index = lyric.find_first("www.tunerankings.com");

				if (index == 0)
				{
					str_list->add_item("");
					continue;
				}
				else if (index != -1)
				{
					lyric.remove_chars(index, 20);
				}

				if (string_trim(lyric).get_length() > 0)
				{
					str_list->add_item(lyric);
					continue;
				}
			}
			str_list->add_item("");
		}
	}
	catch (pfc::exception & e)
	{
		console_error(e.what());
		delete str_list;
		return NULL;
	}
	catch (...)
	{
		delete str_list;
		return NULL;
	}

	return str_list;
}

pfc::string8 provider_lyricsplugin::lookup_one(unsigned p_index, const metadb_handle_ptr & p_meta, threaded_process_status & p_status, abort_callback & p_abort)
{
	// Regular Expression Class
	CRegexpT<char> regexp;
	MatchResult match;

	// Buffer
	pfc::string8 buff;

	try
	{
		// Init fetcher
		curl_wrapper_simple fetcher(&m_config_item);

		// Clear buff
		buff.reset();

		const metadb_handle_ptr & p = p_meta;

		if (p.is_empty())
		{
			return "";
		}

		pfc::string8_fast artist, title;
		static_api_ptr_t<titleformat_compiler> compiler;
		service_ptr_t<titleformat_object> script;

		// Get ARTIST
		compiler->compile_safe(script, "[%artist%]");
		p->format_title(NULL, artist, script, NULL);
		// Get TITLE
		compiler->compile_safe(script, "[%title%]");
		p->format_title(NULL, title, script, NULL);

		// Fetching from HTTP
		// Set HTTP Address
		pfc::string8_fast url("http://www.lyricsplugin.com/winamp03/plugin/content.php?artist=");
		pfc::string8_fast referer("http://www.lyricsplugin.com/winamp03/plugin/?artist=");

		// URL = http://www.lyricsplugin.com/winamp03/plugin/content.php?artist=<artist>&title=<title>&td=<timestamp>
		// REFERER = http://www.lyricsplugin.com/winamp03/plugin/?artist=<artist>&title=<title>

		url += fetcher.quote(artist);
		url += "&title=";
		url += fetcher.quote(title);

		referer += fetcher.quote(artist);
		referer += "&title=";
		referer += fetcher.quote(title);

		// Get it now
		try
		{
			fetcher.fetch(url, referer, buff);
		}
		catch (pfc::exception & e)
		{
			console_error(e.what());
			return "";
		}
		catch (...)
		{
			return "";
		}

		const char * regex_lyricbox = "<div\\s+id\\s*?=\\s*?\"lyrics\"\\s*?>[\\r\\n]*(.*?)[\\r\\n]*</div>";

		// expression for extract lyrics
		regexp.Compile(regex_lyricbox, SINGLELINE);

		// match
		MatchResult result = regexp.Match(buff.get_ptr());

		// Get Group
		if (result.IsMatched())
		{
			int nStart = result.GetGroupStart(1);
			int nEnd = result.GetGroupEnd(1);
			int index;
			pfc::string8_fast lyric(buff.get_ptr() + nStart, nEnd - nStart);

			convert_html_to_plain(lyric);

			index = lyric.find_first("www.tunerankings.com");

			if (index == 0)
			{
				return "";
			}
			else if (index != -1)
			{
				lyric.remove_chars(index, 20);
			}

			if (string_trim(lyric).get_length() > 0)
			{
				return lyric;
			}
		}
	}
	catch (pfc::exception & e)
	{
		console_error(e.what());
		return "";
	}
	catch (...)
	{
		return "";
	}

	return "";
}

//************************************************************************
//*                             Dark Lyrics                              *
//************************************************************************
pfc::string_list_impl * provider_darklyrics::lookup(unsigned p_index, metadb_handle_list_cref p_meta, threaded_process_status & p_status, abort_callback & p_abort)
{
	const float threshold = 0.8f;

	// Regular Expression Class
	CRegexpT<char> regexp;
	MatchResult match;

	// Buffer
	pfc::string8 buff;
	pfc::string_list_impl * str_list = new pfc::string_list_impl;

	try
	{
		// Init fetcher
		curl_wrapper_simple fetcher(&m_config_item);

		for (t_size i = 0; i < p_meta.get_count(); ++i)
		{
			if (p_abort.is_aborting())
				break;

			// Sleep
			how_to_sleep(i);
			// Clear buff
			buff.reset();

			const metadb_handle_ptr & p = p_meta.get_item(i);

			if (p.is_empty())
			{
				str_list->add_item("");
				continue;
			}

			// Set progress
			pfc::string8_fast path = file_path_canonical(p->get_path());

			// add subsong index?
			if (p->get_subsong_index() > 0)
			{
				path.add_string(" /index:");
				path.add_string(pfc::format_uint(p->get_subsong_index()));
			}

			p_status.set_item_path(path);
			p_status.set_progress(i + 1, p_meta.get_count());

			pfc::string8_fast artist, title, album;

			file_info_impl info;
			p->get_info(info);

			// Get count of artists
			t_size count = info.meta_get_count_by_name("artist");

			// Get Album
			album = info.meta_get("album", 0);

			// Get TITLE
			title = info.meta_get("title", 0);

			bool found = false;

			// Iterate through all artists listed
			for (int j = 0; j < count && !found; j++)
			{
				// Get Artist
				artist = info.meta_get("artist", j);
			
				//Fetching from HTTP
				// Set HTTP Address
				pfc::string8_fast url("http://www.darklyrics.com/lyrics/");

				// URL = http://www.darklyrics.com/lyrics/ensiferum/victorysongs.html

				string_helper::convert_to_lower_case(artist);
				string_helper::convert_to_lower_case(album);
				string_helper::remove_char(album, ' ');

				url += fetcher.quote(artist);
				url += "/";
				url += fetcher.quote(album);
				url += ".html";

				// Get it now
				try
				{
					fetcher.fetch(url, buff);
				}
				catch (pfc::exception & e)
				{
					console_error(e.what());
					continue;
				}
				catch (...)
				{
					continue;
				}


				const char * regex_ahref = "<a\\shref=\"#(?P<no>\\d+)\">(?P<text>.+?)</a>";

				// expression for extract lyrics
				regexp.Compile(regex_ahref, IGNORECASE);

				// match
				MatchResult result = regexp.Match(buff.get_ptr());

				int noGroup = regexp.GetNamedGroupNumber("no");
				int textGroup = regexp.GetNamedGroupNumber("text");

				int jump_to = 0;
				pfc::string8_fast compare = title;
				compare.insert_chars(0, ". ");
				float good;
				float best = 0.0f;


				while (result.IsMatched())
				{
					int gStart = result.GetGroupStart(noGroup);
					int gEnd = result.GetGroupEnd(noGroup);
					pfc::string8_fast temp(buff.get_ptr() + gStart, gEnd - gStart);
					int no = StrToIntA(temp);

					gStart = result.GetGroupStart(textGroup);
					gEnd = result.GetGroupEnd(textGroup);

					temp = pfc::string8_fast(buff.get_ptr()+gStart, gEnd - gStart);

					int levDist = LD(compare, compare.get_length(), temp, temp.get_length());

					good = 1.0f - (levDist / (float)compare.get_length());

					if (good >= threshold && good > best)
					{
						jump_to = no;
						best = good;
					}
					result = regexp.Match(buff.get_ptr(),result.GetEnd());
				}

				if (jump_to == 0)
				{
					continue;
				}

				char regex_lyrics[100];

				sprintf(regex_lyrics, "<a\\s+name=%d><font*(.*?)</font*(.*?)>(?P<lyrics>.+?)<font", jump_to);

				// expression for extract lyrics
				regexp.Compile(regex_lyrics, IGNORECASE | SINGLELINE);

				noGroup = regexp.GetNamedGroupNumber("lyrics");

				result = regexp.Match(buff.get_ptr());

				if (result.IsMatched())
				{
					int nStart = result.GetGroupStart(noGroup);
					int nEnd = result.GetGroupEnd(noGroup);
					pfc::string8_fast lyric(buff.get_ptr() + nStart, nEnd - nStart);

					convert_html_to_plain(lyric);

					if (jump_to == 1)
						lyric.remove_chars(0, 3);
					else
						lyric.remove_chars(0, 4);

					lyric.remove_chars(lyric.get_length()-4, 4);

					if (string_trim(lyric).get_length() > 0)
					{
						str_list->add_item(lyric);
						found = true;
						continue;
					}
				}
			}
			if (found)
				continue;
			else
				str_list->add_item("");
		}
	}
	catch (pfc::exception & e)
	{
		console_error(e.what());
		delete str_list;
		return NULL;
	}
	catch (...)
	{
		delete str_list;
		return NULL;
	}

	return str_list;
}
pfc::string8 provider_darklyrics::lookup_one(unsigned p_index, const metadb_handle_ptr & p_meta, threaded_process_status & p_status, abort_callback & p_abort)
{
	const float threshold = 0.8f;

	// Regular Expression Class
	CRegexpT<char> regexp;
	MatchResult match;

	// Buffer
	pfc::string8 buff;

	try
	{
		// Init fetcher
		curl_wrapper_simple fetcher(&m_config_item);

		const metadb_handle_ptr & p = p_meta;

		if (p.is_empty())
		{
			return "";
		}

		pfc::string8_fast artist, title, album;
		static_api_ptr_t<titleformat_compiler> compiler;
		service_ptr_t<titleformat_object> script;

		file_info_impl info;
		p->get_info(info);

		// Get count of artists
		t_size count = info.meta_get_count_by_name("artist");

		// Get Album
		album = info.meta_get("album", 0);

		// Get TITLE
		title = info.meta_get("title", 0);


		// Iterate through all artists listed
		for (int j = 0; j < count; j++)
		{
			// Get Artist
			artist = info.meta_get("artist", j);		//Fetching from HTTP

			// Set HTTP Address
			pfc::string8_fast url("http://www.darklyrics.com/lyrics/");

			// URL = http://www.darklyrics.com/lyrics/ensiferum/victorysongs.html

			string_helper::convert_to_lower_case(artist);
			string_helper::convert_to_lower_case(album);
			string_helper::remove_char(album, ' ');

			url += fetcher.quote(artist);
			url += "/";
			url += fetcher.quote(album);
			url += ".html";

			// Get it now
			try
			{
				fetcher.fetch(url, buff);
			}
			catch (pfc::exception & e)
			{
				console_error(e.what());
				continue;
			}
			catch (...)
			{
				continue;
			}

			const char * regex_ahref = "<a\\shref=\"#(?P<no>\\d+)\">(?P<text>.+?)</a>";

			// expression for extract lyrics
			regexp.Compile(regex_ahref, IGNORECASE);

			// match
			MatchResult result = regexp.Match(buff.get_ptr());

			int noGroup = regexp.GetNamedGroupNumber("no");
			int textGroup = regexp.GetNamedGroupNumber("text");

			int jump_to = 0;
			pfc::string8_fast compare = title;
			compare.insert_chars(0, ". ");
			float good;
			float best = 0.0f;


			while (result.IsMatched())
			{
				int gStart = result.GetGroupStart(noGroup);
				int gEnd = result.GetGroupEnd(noGroup);
				pfc::string8_fast temp(buff.get_ptr() + gStart, gEnd - gStart);
				int no = StrToIntA(temp);

				gStart = result.GetGroupStart(textGroup);
				gEnd = result.GetGroupEnd(textGroup);

				temp = pfc::string8_fast(buff.get_ptr()+gStart, gEnd - gStart);

				int levDist = LD(compare, compare.get_length(), temp, temp.get_length());

				good = 1.0f - (levDist / (float)compare.get_length());

				if (good >= threshold && good > best)
				{
					jump_to = no;
					best = good;
				}
				result = regexp.Match(buff.get_ptr(),result.GetEnd());
			}

			if (jump_to == 0)
			{
				continue;
			}

			char regex_lyrics[100];

			sprintf(regex_lyrics, "<a\\s+name=%d><font*(.*?)</font*(.*?)>(?P<lyrics>.+?)<font", jump_to);

			// expression for extract lyrics
			regexp.Compile(regex_lyrics, IGNORECASE | SINGLELINE);

			noGroup = regexp.GetNamedGroupNumber("lyrics");

			result = regexp.Match(buff.get_ptr());

			if (result.IsMatched())
			{
				int nStart = result.GetGroupStart(noGroup);
				int nEnd = result.GetGroupEnd(noGroup);
				pfc::string8_fast lyric(buff.get_ptr() + nStart, nEnd - nStart);

				convert_html_to_plain(lyric);

				if (jump_to == 1)
					lyric.remove_chars(0, 3);
				else
					lyric.remove_chars(0, 4);

				lyric.remove_chars(lyric.get_length()-4, 4);

				if (string_trim(lyric).get_length() > 0)
				{
					return lyric;
				}
			}
		}
	}
	catch (pfc::exception & e)
	{
		console_error(e.what());
		return "";
	}
	catch (...)
	{
		return "";
	}

	return "";
}

#endif
