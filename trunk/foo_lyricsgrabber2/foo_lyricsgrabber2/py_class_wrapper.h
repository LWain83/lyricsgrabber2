#pragma once

#include "helper.h"


// stdout redirector
class py_stdout_redirector
{
public:
	void write(const char * str)
	{
		console::info(string_trim_right(str));
	}
};

class py_stderr_receiver
{
public:
	void write(const char * str)
	{
		m_buffer.add_string(str);
	}

	static inline pfc::string8_fast & g_get_buffer()
	{
		return m_buffer;
	}

	static inline void g_reset()
	{
		m_buffer.reset();
	}

	static inline void g_print_error()
	{
		PyErr_Print();
		console::error(g_get_buffer());
		g_reset();
		PyErr_Clear();
	}

	static inline void g_show_error()
	{
		PyErr_Print();
		popup_message::g_complain("foo_grabber_python", g_get_buffer());
		g_reset();
		PyErr_Clear();
	}

private:
	static pfc::string8_fast m_buffer;
};
FOOGUIDDECL pfc::string8_fast py_stderr_receiver::m_buffer;

// Wrapper fro metadb_handle_ptr
class py_metadb_handle_ptr_wrapper
{
public:
	static void g_register();

	py_metadb_handle_ptr_wrapper(metadb_handle_ptr p_handle) : m_handle(p_handle)
	{
	}

	std::string format(const char * spec);

private:
	metadb_handle_ptr m_handle;
};

// Wrapper for metadb_handle_list
class py_metadb_handle_list_wrapper
{
public:
	static void g_register();

	py_metadb_handle_list_wrapper(metadb_handle_list_cref p_handles) : m_handles(p_handles)
	{
		m_it = 0;
		m_itend = m_handles.get_count();
	}

	void __iter__()
	{
	}

	py_metadb_handle_ptr_wrapper next();

private:
	t_size m_it;
	t_size m_itend;
	metadb_handle_list_cref m_handles;
};

// Wrapper for threaded_process_status
class py_process_status_wrapper
{
public:
	static void g_register();

	py_process_status_wrapper(threaded_process_status & p_status, metadb_handle_list_cref p_handles) 
		: m_status(p_status)
		, m_handles(p_handles)
		, m_state(0)
	{
		m_count = m_handles.get_count();
	}

	// Progress + 1
	void advance();

private:
	t_size m_state, m_count;
	threaded_process_status & m_status;
	metadb_handle_list_cref m_handles;
};

class py_abort_callback_wrapper
{
public:
	static void g_register();

	py_abort_callback_wrapper(abort_callback & p_abort) : m_abort(p_abort)
	{
	}

	bool is_aborting()
	{
		return m_abort.is_aborting();
	}

private:
	abort_callback & m_abort;
};

struct py_provider_base
{
	virtual std::string GetName() = 0;
	virtual std::string GetVersion() { return "N/A"; }
	virtual std::string GetAuhtor() { return "Anonymous"; }
	virtual std::string GetDescription() { return ""; }
	virtual std::string GetURL() { return ""; }
	virtual boost::python::list Query(py_metadb_handle_list_wrapper & p_meta_wrapper, py_process_status_wrapper & p_status_wrapper, 
		py_abort_callback_wrapper & p_abort_wrapper) = 0;
};

struct py_provider_base_wrapper : py_provider_base, boost::python::wrapper<py_provider_base>
{
	static void g_register();

	std::string GetName();
	std::string GetVersion();
	std::string GetVersion_Default();
	std::string GetAuhtor();
	std::string GetAuthor_Default();
	std::string GetDescription();
	std::string GetDescription_Default();
	std::string GetURL();
	std::string GetURL_Default();
	boost::python::list Query(py_metadb_handle_list_wrapper & p_meta_wrapper, py_process_status_wrapper & p_status_wrapper, 
		py_abort_callback_wrapper & p_abort_wrapper);
};
