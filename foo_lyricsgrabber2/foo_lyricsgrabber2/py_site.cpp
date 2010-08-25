#include "stdafx.h"
#include "py_site.h"
#include <boost/python/module.hpp>

using namespace boost::python;
using namespace pfc::stringcvt;


BOOST_PYTHON_MODULE(grabber)
{
	// metadb_handle_ptr
	py_metadb_handle_ptr_wrapper::g_register();

	// metadb_handle_list
	py_metadb_handle_list_wrapper::g_register();

	// threaded_process_status
	py_process_status_wrapper::g_register();

	// abort_callback
	py_abort_callback_wrapper::g_register();

	// provider wrapper
	py_provider_base_wrapper::g_register();
}

void py_site::init()
{
	insync2(m_cs);

	m_grabber_profile_path = file_path_display(core_api::get_profile_path());
	m_grabber_profile_path.fix_dir_separator('\\');
	m_grabber_component_path = m_grabber_profile_path;
	m_grabber_component_path += "user-components\\foo_lyricsgrabber2\\pygrabber";
	m_grabber_profile_path += "pygrabber";

	// Initialize the interpreter
	Py_Initialize();

	// Set encoding as utf-8, is it evil?
	if (PyUnicode_SetDefaultEncoding("utf-8") != 0)
	{
		PyErr_Clear();
	}

	pfc::string8_fast s;
	
	{
		string_os_from_utf8 os;

		os.convert(m_grabber_component_path);
		// Set sys.path
		if (!PathFileExists(os))
		{	
			s = m_grabber_profile_path;
			s << "\\system\\python25.zip;";
			s << m_grabber_profile_path << "\\system;";
			s << m_grabber_profile_path << "\\libs";
			componentPath = false;
		}
		else
		{
			s = m_grabber_component_path;
			s << "\\system\\python25.zip;";
			s << m_grabber_component_path << "\\system;";
			s << m_grabber_component_path << "\\libs";
			componentPath = true;
		}
	}
	if (!m_config_info.extra_libpath.empty())
	{
		s << ";" << m_config_info.extra_libpath.c_str();
	}

	PySys_SetPath((char *)string_ansi_from_utf8(s).get_ptr());

	// Create profile path if they are not exists
	if (!componentPath)
	{
		string_os_from_utf8 os;
		s = m_grabber_profile_path;
		s << "\\libs";
		os.convert(s);
		if (!PathFileExists(os))
			SHCreateDirectory(core_api::get_main_window(), os);
		s = m_grabber_profile_path;
		s << "\\scripts";
		os.convert(s);
		if (!PathFileExists(os))
			SHCreateDirectory(core_api::get_main_window(), os);
	}
	else
	{
		string_os_from_utf8 os;
		s = m_grabber_component_path;
		s << "\\libs";
		os.convert(s);
		if (!PathFileExists(os))
			SHCreateDirectory(core_api::get_main_window(), os);
		s = m_grabber_component_path;
		s << "\\scripts";
		os.convert(s);
		if (!PathFileExists(os))
			SHCreateDirectory(core_api::get_main_window(), os);
	}

	// Init custom module
	initgrabber();
	
	// Import module __main__ and __dict__
	m_main_module = import("__main__");
	m_global_dict = extract<dict>(m_main_module.attr("__dict__"));

	// Prepare the interpreter
	s = 
		"import __builtin__, sys, cStringIO\n"
		"sys.argv = ['']\n"
		// Prevent stdin blocking interpreter
		"sys.stdin = cStringIO.StringIO()\n"

		"import traceback\n"
		;

	try
	{
		// Redirector & Receiver
		m_global_dict["py_stdout_redirector"] = class_<py_stdout_redirector>("stdout_redirector", no_init)
			.def("write", &py_stdout_redirector::write)
			;

		m_global_dict["py_stderr_receiver"] = class_<py_stderr_receiver>("stderr_receiver", no_init)
			.def("write", &py_stderr_receiver::write)
			;

		// Import sys
		object sys_module = import("sys");
		sys_module.attr("stdout") = m_stdout_redirector;
		sys_module.attr("stderr") = m_stderr_receiver;

		exec(s.get_ptr(), m_global_dict, m_global_dict);

		// Execute autoexec.py
		if (!componentPath)
		{
			s = m_grabber_profile_path;
			s << "\\system\\autoexec.py";
		}
		else
		{
			s = m_grabber_component_path;
			s << "\\system\\autoexec.py";
		}

		boost::python::exec_file(string_ansi_from_utf8(s).get_ptr(), m_global_dict, m_global_dict);
	}
	catch (std::exception const & e)
	{
		popup_message::g_complain("foo_grabber_python", e.what());
	}
	catch (error_already_set const &)
	{
		py_stderr_receiver::g_show_error();
	}
}

void py_site::_cleanup()
{
	// Should not call this
	//Py_Finalize();
}

bool py_site::invoke(unsigned p_index, metadb_handle_list_cref p_meta, threaded_process_status & p_status, abort_callback & p_abort, pfc::string_list_impl & p_data)
{
	if (p_index >= m_config_info.get_count())
		return false;

	while (m_cs.enter_timeout(2000) == 0)
	{
		if (p_abort.is_aborting())
			return false;
	}

	py_metadb_handle_list_wrapper meta_wrapper(p_meta);
	py_process_status_wrapper status_wrapper(p_status, p_meta);
	py_abort_callback_wrapper abort_wrapper(p_abort);

	try
	{
		dict local_dict = m_global_dict.copy();

		TRACK_CALL_TEXT("m_config_info[p_index].filename.c_str()");

		exec_file(m_config_info[p_index].filename.c_str(), local_dict.ptr(), local_dict.ptr());

		if (!local_dict.has_key("LyricProviderInstance"))
		{
			m_cs.leave();
			return false;
		}

		object instance = local_dict["LyricProviderInstance"];

		// Call query method
		list str_list = call_method<list>(instance.ptr(), "Query", boost::ref(meta_wrapper), boost::ref(status_wrapper), boost::ref(abort_wrapper));

		int length = boost::python::len(str_list);

		for (int i = 0; i < length; i++)
		{
			std::string str = extract<std::string>(str_list[i]);

			p_data.add_item(str.c_str());
		}
	}
	catch (std::exception const & e)
	{
		m_cs.leave();
		popup_message::g_complain("foo_grabber_python", e.what());
		return false;
	}
	catch (error_already_set const &)
	{
		m_cs.leave();
		py_stderr_receiver::g_show_error();
		return false;
	}

	m_cs.leave();
	return true;
}

bool py_site::refresh()
{
	if (m_cs.try_enter() == 0)
	{
		return false;
	}

	m_config_info.remove_all();

	pfc::list_t<script_info> scripts;

	if (!componentPath)
		enum_scripts(pfc::string_formatter(m_grabber_profile_path) << "\\scripts", scripts);
	else
		enum_scripts(pfc::string_formatter(m_grabber_component_path) << "\\scripts", scripts);
	

	{
		pfc::string_list_impl data;
		pfc::splitStringSimple_toList(data, ";", m_config_info.extra_scriptpath.c_str());

		for (t_size i = 0; i < data.get_count(); i++)
		{
			enum_scripts(data[i], scripts);
		}
	}

	// Validate script
	for (t_size i = 0; i < scripts.get_count(); i++)
	{
		script_info & info = scripts[i];

		try
		{
			dict local_dict = m_global_dict.copy();

			exec_file(info.filename.c_str(), local_dict.ptr(), local_dict.ptr());

			if (!local_dict.has_key("LyricProviderInstance"))
				continue;

			object instance = local_dict["LyricProviderInstance"];

			// Get Information
			info.name = call_method<std::string>(instance.ptr(), "GetName");
			info.version = call_method<std::string>(instance.ptr(), "GetVersion");
			info.description = call_method<std::string>(instance.ptr(), "GetDescription");
			info.url = call_method<std::string>(instance.ptr(), "GetURL");
			info.author = call_method<std::string>(instance.ptr(), "GetAuthor");
		}
		catch (std::exception const & e)
		{
			popup_message::g_complain("foo_grabber_python", e.what());
			continue;
		}
		catch (error_already_set const &)
		{
			py_stderr_receiver::g_show_error();
			continue;
		}

		if (info.name.empty())
		{
			continue;
		}

		// Okay
		m_config_info.add_item(info);
	}

	m_cs.leave();
	return true;
}

void py_site::exec_file(const char * filename, PyObject * global, PyObject * local)
{
	// Taked from boost::python
	PyObject * pyfile = PyFile_FromString((char *)string_ansi_from_utf8(filename).get_ptr(), "r");

	if (!pyfile)
	{
		throw std::invalid_argument(std::string(filename) + " : no such file");
		return;
	}

	boost::python::handle<> file(pyfile);
	PyObject * result = PyRun_File(PyFile_AsFile(file.get()),
		filename,
		Py_file_input,
		global, local);

	if (!result)
	{
		throw_error_already_set();
	}
}

void py_site::enum_scripts(const char * p_folder, pfc::list_t<script_info> & p_out)
{
	pfc::string8 folder = p_folder;
	folder.fix_dir_separator('\\');

	pfc::string8_fast path = folder;
	puFindFile findfile;

	// Enum file names
	path += "*.py*";
	findfile = uFindFirstFile(path);

	if (findfile) do
	{
		if (findfile->IsDirectory())
			continue;

		script_info si;

		si.filename = folder;
		si.filename += findfile->GetFileName();

		// Only allow *.py, *.pyc
		const char ext_py[] = "py";
		const char ext_pyc[] = "pyc";
		pfc::string8_fast ext = pfc::string_extension(si.filename.c_str());

		if (_strnicmp(ext, ext_py, _countof(ext_py)) == 0 || _strnicmp(ext, ext_pyc, _countof(ext_pyc)) == 0)
		{
			p_out.add_item(si);
		}
	} while (findfile->FindNext());
}
