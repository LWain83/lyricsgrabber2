#include "stdafx.h"
#include "cfg.h"


// Save
void cfg_lyrics_grabber::get_data_raw(stream_writer * p_stream,abort_callback & p_abort)
{
	try
	{
		// Write version
		unsigned ver = grabber::VERSION_CURRENT;
		p_stream->write_lendian_t(ver, p_abort);
		// Others
		p_stream->write_lendian_t(m_val.skip_exist, p_abort);
		p_stream->write_string(m_val.lyric_field_name, p_abort);
		p_stream->write_lendian_t(m_val.proxy_type, p_abort);
		p_stream->write_string(m_val.proxy_hostport, p_abort);
		p_stream->write_lendian_t(m_val.proxy_need_auth, p_abort);
		p_stream->write_string(m_val.proxy_username, p_abort);
		p_stream->write_string(m_val.proxy_password, p_abort);
		p_stream->write_lendian_t(m_val.title_formatting, p_abort);
	}
	catch (...)
	{
		return;
	}
}

// Load
void cfg_lyrics_grabber::set_data_raw(stream_reader * p_stream,t_size p_sizehint,abort_callback & p_abort)
{
	t_uint32 read_version;

	set_default_value();

	if (p_sizehint < sizeof(t_uint32))
	{
		return;
	}

	try
	{
		// Read version from stream
		p_stream->read_lendian_t(read_version, p_abort);

		switch (read_version)
		{
		case grabber::VERSION_CURRENT:
			p_stream->read_lendian_t(m_val.skip_exist, p_abort);
			p_stream->read_string(m_val.lyric_field_name, p_abort);
			p_stream->read_lendian_t(m_val.proxy_type, p_abort);
			p_stream->read_string(m_val.proxy_hostport, p_abort);
			p_stream->read_lendian_t(m_val.proxy_need_auth, p_abort);
			p_stream->read_string(m_val.proxy_username, p_abort);
			p_stream->read_string(m_val.proxy_password, p_abort);
			p_stream->read_lendian_t(m_val.title_formatting, p_abort);
			break;

		default:
			// Reset
			set_default_value();
			return;
			break;
		}
	}
	catch (...)
	{
		// Reset
		set_default_value();
		return;
	}
}

void cfg_lyrics_grabber::set_default_value()
{
	m_val.skip_exist = true;
	m_val.lyric_field_name = "LYRICS";
	m_val.title_formatting = false;
	m_val.proxy_type = grabber::PROXY_NONE;
	m_val.proxy_hostport = "";
	m_val.proxy_need_auth = false;
	m_val.proxy_username = "";
	m_val.proxy_password = "";
}
