#pragma once

#include "resource.h"

class CMainConf : public CDialogImpl<CMainConf>
	, public CWinDataExchange<CMainConf>
{
public:
	enum { IDD=IDD_DIALOG_CONF_MAIN };

	BEGIN_MSG_MAP(CMainConf)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_NCDESTROY(OnNcDestroy)
		MSG_WM_CLOSE(OnClose)
		COMMAND_HANDLER(IDC_BUTTON_PREF_PROVIDER, BN_CLICKED, OnBnClickedButtonPrefProvider)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnBnClickedOk)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnBnClickedCancel)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CMainConf)
		DDX_CONTROL_HANDLE(IDC_COMBO_PROXYTYPE, m_combo_proxytype)
		DDX_CONTROL_HANDLE(IDC_COMBO_PREF_PROVIDER, m_combo_providers)
	END_DDX_MAP()

	
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	LRESULT OnNcDestroy()
	{
		delete this;
		return 0;
	}
	void OnClose()
	{
		DestroyWindow();
	}
	LRESULT OnBnClickedOk(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedButtonPrefProvider(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	//
	bool SaveConfiguration();
	grabber::provider_ptr GetCurrentLyricProvider();

private:
	CComboBox m_combo_proxytype;
	CComboBox m_combo_providers;
};