#pragma once

class cmdline_handler : public commandline_handler_metadb_handle
{
public:
	result on_token(const char * token);	
	void on_files_done();
	void on_file(const metadb_handle_ptr & ptr);
	static bool g_find_value(const char * string, const char * name, pfc::string_base & value);
	static bool g_find_command_by_name(const char * command, grabber::provider_ptr & provider, unsigned & index);

private:
	grabber::provider_ptr m_provider;
	unsigned m_index;
	metadb_handle_list m_handles;
};
