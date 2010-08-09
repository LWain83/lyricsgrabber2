#include "stdafx.h"
#include "ui_main_conf.h"
#include "helper.h"
#include "host_impl.h"


BOOL CMainConf::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
	// Do DDX first
	DoDataExchange();

	// Init data
	// Lyric field name
	grabber::config_item & conf = host_impl::g_get_config_ref();
	uSetDlgItemText(m_hWnd, IDC_EDIT_LYRIC_FIELD, conf.lyric_filed_name);
	uSendDlgItemMessage(IDC_CHECK_SKIP_PRESENT, BM_SETCHECK, conf.skip_exist ? BST_CHECKED : BST_UNCHECKED);
	uSendDlgItemMessage(IDC_CHECK_QUIET, BM_SETCHECK, conf.quiet_mode ? BST_CHECKED : BST_UNCHECKED);
	
	// Auth
	uSendDlgItemMessage(IDC_CHECK_NEED_AUTH, BM_SETCHECK, conf.proxy_need_auth ? BST_CHECKED : BST_UNCHECKED);
	uSetDlgItemText(m_hWnd, IDC_EDIT_PROXY_HOST_PORT, conf.proxy_hostport);
	uSetDlgItemText(m_hWnd, IDC_EDIT_PROXY_USERNAME, conf.proxy_username);
	uSetDlgItemText(m_hWnd, IDC_EDIT_PROXY_PASSWORD, conf.proxy_password);
	
	// Proxy types
	const struct  {t_uint32 key; TCHAR * val;} proxy_types [] = 
	{
		{grabber::PROXY_NONE, _T("(none)")},
		{grabber::PROXY_HTTP, _T("http")},
		{grabber::PROXY_SOCKS4, _T("socks4")},
		{grabber::PROXY_SOCKS5, _T("socks5")},
		{infinite, NULL},
	};

	for (int i = 0; ; ++i)
	{
		if (proxy_types[i].key == infinite)
			break;

		m_combo_proxytype.AddString(proxy_types[i].val);
	}

	m_combo_proxytype.SetCurSel(conf.proxy_type);

	// enumerate provider services
	for (t_size i = 0; i < host_impl::g_get_provider_list().get_count(); ++i)
	{
		grabber::provider_ptr ptr = host_impl::g_get_provider_list()[i];

		if (ptr.is_empty())
			continue;

		pfc::string8_fast name;

		ptr->get_provider_name(name);
		m_combo_providers.AddString(pfc::stringcvt::string_os_from_utf8(name));
	}

	m_combo_providers.SetCurSel(0);

	return 0;
}

LRESULT CMainConf::OnBnClickedOk(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (SaveConfiguration())
		DestroyWindow();

	return 0;
}

LRESULT CMainConf::OnBnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DestroyWindow();
	return 0;
}

bool CMainConf::SaveConfiguration()
{
	// Check
	pfc::string8_fast tmp;

	uGetDlgItemText(m_hWnd, IDC_EDIT_LYRIC_FIELD, tmp);
	tmp = string_trim(tmp);

	if (tmp.is_empty())
		return false;

	// Save Configuration
	// Lyric Field Name
	grabber::config_item & cfg = host_impl::g_get_config_ref();
	cfg.lyric_filed_name = tmp;
	cfg.skip_exist = uSendDlgItemMessage(IDC_CHECK_SKIP_PRESENT, BM_GETCHECK) == BST_CHECKED;
	cfg.quiet_mode = uSendDlgItemMessage(IDC_CHECK_QUIET, BM_GETCHECK) == BST_CHECKED;

	// Proxy
	uGetDlgItemText(m_hWnd, IDC_EDIT_PROXY_HOST_PORT, cfg.proxy_hostport);
	uGetDlgItemText(m_hWnd, IDC_EDIT_PROXY_USERNAME, cfg.proxy_username);
	uGetDlgItemText(m_hWnd, IDC_EDIT_PROXY_PASSWORD, cfg.proxy_password);
	cfg.proxy_type = m_combo_proxytype.GetCurSel();
	cfg.proxy_need_auth = BST_CHECKED == uSendDlgItemMessage(IDC_CHECK_NEED_AUTH, BM_GETCHECK);

	return true;
}

LRESULT CMainConf::OnBnClickedButtonPrefProvider(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CMenu menu;
	grabber::provider_ptr ptr = GetCurrentLyricProvider();

	if (ptr.is_empty())
		return 0;

	menu.LoadMenu(IDR_MENU_MAIN_CONF_DROPDOWN);

	// Get Button pos info
	RECT rect_button;
	::GetWindowRect(hWndCtl, &rect_button);

	pfc::string8 description;
	pfc::string8 url;

	menu.EnableMenuItem(ID_POPUP_SETTING, ptr->have_config_popup() ? MF_ENABLED : MF_GRAYED);
	menu.EnableMenuItem(ID_POPUP_ABOUT, ptr->get_provider_description(description) ? MF_ENABLED : MF_GRAYED);
	menu.EnableMenuItem(ID_POPUP_WEBSITE, ptr->get_provider_url(url) ? MF_ENABLED : MF_GRAYED);

	int cmd = menu.GetSubMenu(0).TrackPopupMenu(TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
		rect_button.left, rect_button.bottom, m_hWnd);

	// Do menu
	switch (cmd)
	{
	case ID_POPUP_SETTING:
		ptr->show_config_popup(m_hWnd);
		break;

	case ID_POPUP_ABOUT:
		{
			pfc::string8 name;

			ptr->get_provider_name(name);
			popup_message::g_show(description, name);
		}
		break;

	case ID_POPUP_WEBSITE:
		ptr->get_provider_url(url);
		uShellExecute(m_hWnd, "open", url, NULL, NULL, SW_MAXIMIZE);
		break;
	}

	return 0;
}

grabber::provider_ptr CMainConf::GetCurrentLyricProvider()
{
	int id = m_combo_providers.GetCurSel();

	if (id < 0)
		return NULL;

	return host_impl::g_get_provider_list()[id];
}