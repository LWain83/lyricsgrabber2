#pragma once

class lyric_lookup_task : public threaded_process_callback
{
public:
	lyric_lookup_task(const grabber::provider_ptr & p_provider, unsigned p_index, metadb_handle_list_cref p_data);
	void run(threaded_process_status & p_status,abort_callback & p_abort);
	void start();
	void on_done(HWND p_wnd,bool p_was_aborted);

private:
	metadb_handle_list m_handles;
	grabber::provider_ptr m_provider;
	pfc::string_list_impl * m_values_ptr;
	pfc::string8 m_name;
	unsigned m_index;
};
