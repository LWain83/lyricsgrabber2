#pragma once

#include "stdafx.h"

#include "resource.h"

class CMainConfPref : public CDialogImpl<CMainConfPref>
	, public CWinDataExchange<CMainConfPref>
	, public preferences_page_instance
{
public:
	enum { IDD=IDD_DIALOG_CONF_MAIN_PREF };

	CMainConfPref(preferences_page_callback::ptr callback) : m_callback(callback) {}

	t_uint32 get_state();
	void apply();
	void reset();

	BEGIN_MSG_MAP(CMainConfPref)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER_EX(IDC_EDIT_LYRIC_FIELD_PREF, EN_CHANGE, OnEditChange)
		COMMAND_HANDLER_EX(IDC_EDIT_PROXY_HOST_PORT_PREF, EN_CHANGE, OnEditChange)
		COMMAND_HANDLER_EX(IDC_EDIT_PROXY_PASSWORD_PREF, EN_CHANGE, OnEditChange)
		COMMAND_HANDLER_EX(IDC_EDIT_PROXY_USERNAME_PREF, EN_CHANGE, OnEditChange)
		COMMAND_HANDLER_EX(IDC_CHECK_NEED_AUTH_PREF, BN_CLICKED, OnEditChange)
		COMMAND_HANDLER_EX(IDC_CHECK_QUIET_PREF, BN_CLICKED, OnEditChange)
		COMMAND_HANDLER_EX(IDC_CHECK_SKIP_PRESENT_PREF, BN_CLICKED, OnEditChange)
		COMMAND_HANDLER_EX(IDC_COMBO_PREF_PROVIDER_PREF, CBN_SELCHANGE, OnEditChange)
		COMMAND_HANDLER_EX(IDC_COMBO_PROXYTYPE_PREF, CBN_SELCHANGE, OnEditChange)
		COMMAND_HANDLER(IDC_BUTTON_PREF_PROVIDER_PREF, BN_CLICKED, OnBnClickedButtonPrefProvider)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CMainConfPref)
		DDX_CONTROL_HANDLE(IDC_COMBO_PROXYTYPE_PREF, m_combo_proxytype)
		DDX_CONTROL_HANDLE(IDC_COMBO_PREF_PROVIDER_PREF, m_combo_providers)
	END_DDX_MAP()

	
	//LRESULT OnNcDestroy()
	//{
	//	delete this;
	//	return 0;
	//}

	LRESULT OnBnClickedButtonPrefProvider(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	//
	bool SaveConfiguration();
	grabber::provider_ptr GetCurrentLyricProvider();

private:
	CComboBox m_combo_proxytype;
	CComboBox m_combo_providers;

	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	void OnEditChange(UINT, int, CWindow);
	bool HasChanged();
	void OnChanged();

	const preferences_page_callback::ptr m_callback;
};

class preferences_page_config : public preferences_page_impl<CMainConfPref> {
public:
	const char * get_name() {return "Lyrics Grabber 2";}
	GUID get_guid() {
		// 85228A00-6D5A-4BAA-AF62-A5EA8D30DC06
		static const GUID guid = { 0x85228A00, 0x6D5A, 0x4BAA, { 0xAF, 0x62, 0xA5, 0xEA, 0x8D, 0x30, 0xDC, 0x06 } };
		return guid;
	}
	GUID get_parent_guid() {return guid_tools;}
};