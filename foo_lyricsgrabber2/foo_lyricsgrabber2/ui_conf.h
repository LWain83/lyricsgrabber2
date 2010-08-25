#pragma once

#include "py_site.h"
#include "resource.h"


class CConfDlg : public CDialogImpl<CConfDlg>
	, public CWinDataExchange<CConfDlg>
{
private:
	CListViewCtrl m_list_scripts;
	py_site & m_site;

public:
	enum { IDD = IDD_CONF };

	BEGIN_MSG_MAP(CConfDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_NCDESTROY(OnNcDestroy)
		MSG_WM_CLOSE(OnClose)
		COMMAND_ID_HANDLER_EX(IDOK, OnOK)
		COMMAND_HANDLER_EX(IDC_BUTTON_REFRESH, BN_CLICKED, OnButtonRefreshBnClicked)
		NOTIFY_HANDLER_EX(IDC_LIST_SCRIPTS, NM_RCLICK, OnListScriptsNMRClick)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CConfDlg)
		DDX_CONTROL_HANDLE(IDC_LIST_SCRIPTS, m_list_scripts)
	END_DDX_MAP()


	CConfDlg(py_site & p_site) : m_site(p_site)
	{
	}

	void LoadInfo();

	LRESULT OnInitDialog(HWND wndFocus, LPARAM lInitParam);

	LRESULT OnNcDestroy()
	{
		delete this;
		return 0;
	}

	void OnClose()
	{
		CloseDialog();
	}

	LRESULT OnOK(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		SaveConf();
		CloseDialog();
		return 0;
	}

	void SaveConf();

	void CloseDialog()
	{
		DestroyWindow();
	}

	LRESULT OnButtonRefreshBnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	LRESULT OnListScriptsNMRClick(LPNMHDR pnmh);
};
