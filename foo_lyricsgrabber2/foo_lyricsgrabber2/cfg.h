#pragma once

class cfg_lyrics_grabber : public cfg_var
{
public:
	explicit inline cfg_lyrics_grabber(const GUID & p_guid) : cfg_var(p_guid)
	{
		set_default_value();
	}

	inline grabber::config_item & val() {return m_val;}
	inline grabber::config_item const & val() const {return m_val;}

protected:
	void set_default_value();
	void get_data_raw(stream_writer * p_stream,abort_callback & p_abort);
	void set_data_raw(stream_reader * p_stream,t_size p_sizehint,abort_callback & p_abort);
	
	grabber::config_item m_val;
};
