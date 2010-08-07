#include "stdafx.h"
#include "py_class_wrapper.h"

using namespace boost::python;


void py_metadb_handle_ptr_wrapper::g_register()
{
	class_<py_metadb_handle_ptr_wrapper>("metadb_handle_ptr", init<metadb_handle_ptr>())
		.def("Format", &format)
		;
}

std::string py_metadb_handle_ptr_wrapper::format(const char * spec)
{
	static_api_ptr_t<titleformat_compiler> tc;
	service_ptr_t<titleformat_object> to;
	pfc::string8_fast str;

	tc->compile_safe_ex(to, spec);
	m_handle->format_title(NULL, str, to, NULL);
	return str.get_ptr();
}

void py_metadb_handle_list_wrapper::g_register()
{
	class_<py_metadb_handle_list_wrapper, boost::noncopyable>("metadb_handle_list", no_init)
		.def("__iter__", &__iter__, return_self<>())
		.def("next", &next)
		;
}

py_metadb_handle_ptr_wrapper py_metadb_handle_list_wrapper::next()
{
	if (m_it == m_itend)
	{
		PyErr_SetObject(PyExc_StopIteration, Py_None);
		throw error_already_set();
	}
	else
	{
		return m_handles[m_it++];
	}
}

void py_process_status_wrapper::advance()
{
	if (m_state == m_count)
		return;

	metadb_handle_ptr & ptr = m_handles[m_state];
	pfc::string8_fast path = ptr->get_path();

	if (ptr->get_subsong_index() > 0)
	{
		path << " /index:" << ptr->get_subsong_index();
	}

	m_status.set_progress(m_state + 1, m_count);
	m_status.set_item_path(path);
	m_state++;

	// 'Cause We all don't wanto flood server
	const DWORD magic_sleep[] = {300, 700, 1200, 1800, 2500, 3300, 4200, 5200, 5500};
	const t_size magic_items[] = {0, 60, 140, 270, 390, 500, 620, 880, 1000, infinite};

	for (int i = 0; i < _countof(magic_items); i++)
	{
		if (magic_items[i] < m_state && m_state <= magic_items[i])
		{
			Sleep(magic_sleep[i]);
			break;
		}
	}
}

void py_abort_callback_wrapper::g_register()
{
	class_<py_abort_callback_wrapper, boost::noncopyable>("abort_callback", no_init)
		.def("Aborting", &is_aborting)
		;
}


void py_process_status_wrapper::g_register()
{
	class_<py_process_status_wrapper, boost::noncopyable>("threaded_process_status", no_init)
		.def("Advance", &advance)
		;
}

std::string py_provider_base_wrapper::GetName()
{
	return this->get_override("GetName")();
}

std::string py_provider_base_wrapper::GetVersion()
{
	if (override fn = this->get_override("GetVersion"))
		return fn();

	return py_provider_base::GetVersion();
}

std::string py_provider_base_wrapper::GetAuhtor()
{
	if (override fn = this->get_override("GetAuhtor"))
		return fn();

	return py_provider_base::GetAuhtor();
}

std::string py_provider_base_wrapper::GetDescription()
{
	if (override fn = this->get_override("GetDescription"))
		return fn();

	return py_provider_base::GetDescription();
}

std::string py_provider_base_wrapper::GetURL()
{
	if (override fn = this->get_override("GetURL"))
		return fn();

	return py_provider_base::GetURL();
}

list py_provider_base_wrapper::Query(py_metadb_handle_list_wrapper & p_meta_wrapper, py_process_status_wrapper & p_status_wrapper, py_abort_callback_wrapper & p_abort_wrapper)
{
	return this->get_override("Query")(p_meta_wrapper, p_status_wrapper, p_abort_wrapper);
}

std::string py_provider_base_wrapper::GetVersion_Default()
{
	return this->py_provider_base::GetVersion();
}

std::string py_provider_base_wrapper::GetAuthor_Default()
{
	return this->py_provider_base::GetAuhtor();
}

std::string py_provider_base_wrapper::GetDescription_Default()
{
	return this->py_provider_base::GetDescription();
}

std::string py_provider_base_wrapper::GetURL_Default()
{
	return this->py_provider_base::GetURL();
}

void py_provider_base_wrapper::g_register()
{
	typedef py_provider_base base;
	typedef py_provider_base_wrapper wrapped;

	class_<py_provider_base_wrapper, boost::noncopyable>("LyricProviderBase")
		.def("GetName", pure_virtual(&base::GetName))
		.def("Query", pure_virtual(&base::Query))
		.def("GetVersion", &base::GetVersion, &wrapped::GetVersion_Default)
		.def("GetAuthor", &base::GetAuhtor, &wrapped::GetAuthor_Default)
		.def("GetDescription", &base::GetDescription, &wrapped::GetDescription_Default)
		.def("GetURL", &base::GetURL, &wrapped::GetURL_Default)
		;
}
