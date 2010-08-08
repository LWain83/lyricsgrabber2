#pragma once

#include "py_class_wrapper.h"


enum t_version_info
{
	VERSION_100 = 0x100,
};

struct script_info
{
	std::string filename;
	std::string name;
	std::string author;
	std::string description;
	std::string url;
	std::string version;
};

struct config_info_custom
{
	std::string extra_libpath;
	std::string extra_scriptpath;
};

class config_info : public pfc::list_t<script_info>, public config_info_custom
{
private:
	static const int VERSION_CURRENT = VERSION_100;

public:
	void read_from_stream(stream_reader * p_reader, t_size p_size, abort_callback & p_abort)
	{
		int version;

		remove_all();

		if (p_size <= sizeof(int) + sizeof(t_size))
			return;

		try
		{
			// Read version
			p_reader->read_lendian_t(version, p_abort);

			switch (version)
			{
			case VERSION_CURRENT:
				t_size count;
				p_reader->read_lendian_t(count, p_abort);

				for (t_size i = 0; i < count; i++)
				{
					script_info info;

					info.filename = p_reader->read_string(p_abort).get_ptr();
					info.name = p_reader->read_string(p_abort).get_ptr();
					info.author = p_reader->read_string(p_abort).get_ptr();
					info.version = p_reader->read_string(p_abort).get_ptr();
					info.description = p_reader->read_string(p_abort).get_ptr();
					info.url = p_reader->read_string(p_abort).get_ptr();
					add_item(info);
				}

				// Custom Info
				extra_libpath = p_reader->read_string(p_abort).get_ptr();
				extra_scriptpath = p_reader->read_string(p_abort).get_ptr();
				break;

			default:
				remove_all();
				return;
			}
		}
		catch (...)
		{
			remove_all();
			return;
		}
	}

	void write_to_stream(stream_writer * p_writer, abort_callback & p_abort)
	{
		// Write version
		p_writer->write_lendian_t(VERSION_CURRENT, p_abort);
		// Write size
		p_writer->write_lendian_t(get_count(), p_abort);
		//
		try
		{
			for (t_size i = 0; i < get_count(); i++)
			{
				script_info & info_ref = m_buffer[i];

				p_writer->write_string(info_ref.filename.c_str(), p_abort);
				p_writer->write_string(info_ref.name.c_str(), p_abort);
				p_writer->write_string(info_ref.author.c_str(), p_abort);
				p_writer->write_string(info_ref.version.c_str(), p_abort);
				p_writer->write_string(info_ref.description.c_str(), p_abort);
				p_writer->write_string(info_ref.url.c_str(), p_abort);
			}

			// Custom Info
			p_writer->write_string(extra_libpath.c_str(), p_abort);
			p_writer->write_string(extra_scriptpath.c_str(), p_abort);
		}
		catch (...)
		{
			// Do nothing here
		}
	}
};

class py_site
{
public:
	~py_site() { _cleanup(); }

	inline unsigned get_script_count() { return m_config_info.get_count(); }
	inline const char * get_script_name(unsigned p_index) { return m_config_info[p_index].name.c_str(); }
	inline config_info & get_config() { return m_config_info; }
	bool invoke(unsigned p_index, metadb_handle_list_cref p_meta, threaded_process_status & p_status, abort_callback & p_abort, pfc::string_list_impl & p_data);
	bool refresh();	
	void init();
	void exec_file(const char * filename, PyObject * global, PyObject * local);
	void enum_scripts(const char * p_folder, pfc::list_t<script_info> & p_out);

private:
	void _cleanup();

	pfc::string8 m_grabber_profile_path;
	config_info m_config_info;
	py_stdout_redirector m_stdout_redirector;
	py_stderr_receiver m_stderr_receiver;
	boost::python::object m_main_module;
	boost::python::dict m_global_dict;
	critical_section2 m_cs;
};
