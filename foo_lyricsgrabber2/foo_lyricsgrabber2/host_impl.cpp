#include "stdafx.h"
#include "host_impl.h"
#include "debug.h"


// {37A9829D-3264-43c7-9764-7C7AA167E457}
FOOGUIDDECL const GUID host_impl::guid_config = 
{ 0x37a9829d, 0x3264, 0x43c7, { 0x97, 0x64, 0x7c, 0x7a, 0xa1, 0x67, 0xe4, 0x57 } };

bool host_impl::m_inited = false;

// {284ECEF0-AF6E-41e5-BE9F-87870B9128C5}
static const GUID guid_lyrics_grabber = 
{ 0x284ecef0, 0xaf6e, 0x41e5, { 0xbe, 0x9f, 0x87, 0x87, 0xb, 0x91, 0x28, 0xc5 } };
static cfg_lyrics_grabber g_cfg_lyrics_grabber(guid_lyrics_grabber);

static initquit_factory_t<initquit_impl> g_initquit;
static service_factory_single_t<host_impl> g_host;

grabber::provider_ptr_list host_impl::provider_ptr_list;

void initquit_impl::on_init()
{
	host_impl::g_load_providers();
}

void initquit_impl::on_quit()
{
	host_impl::g_save_providers();
}

const grabber::config_item & host_impl::get_global_config() const
{
	return g_get_config_ref();
}

grabber::provider_ptr_list & host_impl::g_get_provider_list()
{
	return provider_ptr_list;
}

grabber::config_item & host_impl::g_get_config_ref()
{
	return g_cfg_lyrics_grabber.val();
}

void host_impl::g_load_providers()
{
	if (m_inited)
		return;

	service_enum_t<grabber::provider_base> e;
	grabber::provider_ptr ptr;

	service_ptr_t<file> file_io;
	abort_callback_dummy abort;
	pfc::map_t<pfc::string8_fast, t_filesize, pfc::comparator_strcmp> pos_map;
	pfc::string8_fast path = file_path_display(core_api::get_profile_path());

	path.fix_dir_separator('\\');
	path += "lyrics_grabber_provider.cfg";
	provider_ptr_list.remove_all();

	try
	{
		GUID guid;

		filesystem::g_open_read(file_io, path, abort);
		file_io->seek(0, abort);
		file_io->read_lendian_t(guid, abort);

		if (guid == guid_config)
		{
			t_size count;

			file_io->read_lendian_t(count, abort);

			for (t_size i = 0; i < count; i++)
			{
				pfc::string8_fast name;
				t_size size;

				file_io->read_string(name, abort);
				pos_map.find_or_add(name) = file_io->get_position(abort);
				file_io->read_lendian_t(size, abort);
				file_io->seek_ex(size, file::seek_from_current, abort);
			}
		}
	}
	catch (exception_io &)
	{
	}

	if (e.first(ptr)) do
	{
		pfc::string8_fast name;
		t_filesize pos = 0;
		t_size size = 0;

		host_impl::g_get_provider_list().add_item(ptr);

		if (file_io.is_valid() && pos_map.get_count() > 0)
		{
			ptr->get_provider_name(name);
			file_io->seek(0, abort);
			
			if (pos_map.query(name, pos))
			{
				file_io->seek(pos, abort);
				file_io->read_lendian_t(size, abort);
			}
		}

		ptr->set_config(file_io.get_ptr(), size, abort);
	} while (e.next(ptr));

	host_impl::g_get_provider_list().sort();

	m_inited = true;
}

void host_impl::g_save_providers()
{
	service_ptr_t<file> file_io;
	abort_callback_dummy abort;
	pfc::string8_fast path = file_path_display(core_api::get_profile_path());

	path.fix_dir_separator('\\');
	path += "lyrics_grabber_provider.cfg";

	try
	{
		filesystem::g_open_write_new(file_io, path, abort);
		file_io->seek(0, abort);
		file_io->write_lendian_t(guid_config, abort);
		file_io->write_lendian_t(provider_ptr_list.get_count(), abort);

		for (t_size i = 0; i < provider_ptr_list.get_count(); i++)
		{
			grabber::provider_ptr ptr = provider_ptr_list[i];
			pfc::string8_fast name;
			t_size size;
			stream_writer_buffer_simple stream_buffer;

			ptr->get_provider_name(name);
			file_io->write_string(name, abort);
			ptr->get_config(&stream_buffer, abort);
			size = stream_buffer.m_buffer.get_size();
			file_io->write_lendian_t(size, abort);
			file_io->write(stream_buffer.m_buffer.get_ptr(), stream_buffer.m_buffer.get_size(), abort);
		}
	}
	catch (exception_io &)
	{
	}
}
