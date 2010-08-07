#pragma once

const char my_title[] = "Lyrics Grabber";

inline void popup_message_error(const char * str) throw()
{
	popup_message::g_show(str, my_title, popup_message::icon_error);
}

inline void console_error(const char * str) throw()
{
	console::printf("ERROR: %s:%s", my_title, str);
}
