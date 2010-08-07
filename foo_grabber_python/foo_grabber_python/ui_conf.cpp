#include "stdafx.h"
#include "ui_conf.h"

using namespace pfc::stringcvt;


LRESULT CConfDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
	DoDataExchange();

	// Center the dialog on the screen
	CenterWindow();

	// Set listview style
	m_list_scripts.SetExtendedListViewStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	// Insert Columns
	m_list_scripts.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 120);
	m_list_scripts.InsertColumn(1, _T("Version"), LVCFMT_LEFT, 80);
	m_list_scripts.InsertColumn(2, _T("Author"), LVCFMT_LEFT, 120);
	LoadInfo();

	// Custom Info
	uSetDlgItemText(m_hWnd, IDC_EDIT_CUSTOM_LIBPATH, m_site.get_config().extra_libpath.c_str());
	uSetDlgItemText(m_hWnd, IDC_EDIT_CUSTOM_SCRIPTPATH, m_site.get_config().extra_scriptpath.c_str());

	return TRUE;
}

LRESULT CConfDlg::OnButtonRefreshBnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	SaveConf();

	if (m_site.refresh())
	{
		LoadInfo();
	}
	else
	{
		console::info("Python Interpreter is now locked, please try again later.");
	}

	return 0;
}

void CConfDlg::LoadInfo()
{
	m_list_scripts.DeleteAllItems();

	config_info & m_scripts = m_site.get_config();

	for (t_size i = 0; i < m_scripts.get_count(); i++)
	{
		m_list_scripts.AddItem(i, 0, string_os_from_utf8_fast(m_scripts[i].name.c_str()));
		m_list_scripts.AddItem(i, 1, string_os_from_utf8_fast(m_scripts[i].version.c_str()));
		m_list_scripts.AddItem(i, 2, string_os_from_utf8_fast(m_scripts[i].author.c_str()));
	}
}

LRESULT CConfDlg::OnListScriptsNMRClick(LPNMHDR pnmh)
{
	LPNMITEMACTIVATE pnmia = (LPNMITEMACTIVATE)pnmh;

	if (pnmia->iItem >= m_site.get_config().get_count())
		return 0;

	script_info & info_ref = m_site.get_config()[pnmia->iItem];
	CMenu menu;
	POINT pt;
	int cmd;

	GetCursorPos(&pt);

	menu.LoadMenu(IDR_MENU_CONTEXT);
	menu.EnableMenuItem(ID_POPUP_DESCRIPTION, info_ref.description.empty() ? MF_GRAYED : MF_ENABLED);
	menu.EnableMenuItem(ID_POPUP_WEBSITE, info_ref.url.empty() ? MF_GRAYED : MF_ENABLED);

	cmd = menu.GetSubMenu(0).TrackPopupMenu(TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
		pt.x, pt.y, m_hWnd);

	// Do menu
	switch (cmd)
	{
	case ID_POPUP_DESCRIPTION:
		popup_message::g_show(info_ref.description.c_str(), info_ref.name.c_str());
		break;

	case ID_POPUP_WEBSITE:
		uShellExecute(m_hWnd, "open", info_ref.url.c_str(), NULL, NULL, SW_MAXIMIZE);
		break;
	}

	return 0;
}

void CConfDlg::SaveConf()
{
	m_site.get_config().extra_libpath = uGetDlgItemText(m_hWnd, IDC_EDIT_CUSTOM_LIBPATH).ptr();
	m_site.get_config().extra_scriptpath = uGetDlgItemText(m_hWnd, IDC_EDIT_CUSTOM_SCRIPTPATH).ptr();
}
