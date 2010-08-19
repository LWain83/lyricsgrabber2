#include "stdafx.h"
#include "ui_tagger.h"
#include "tag_writer.h"

#include <string>


#ifdef UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

const TCHAR CTaggerDialog::status_failed[] = _T("Failed");
const TCHAR CTaggerDialog::status_success[] = _T("Success");

LRESULT CTaggerDialog::OnButtonUpdateBnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	static_api_ptr_t<metadb_io_v2>()->update_info_async(m_handles, 
		new service_impl_t<tag_writter>(m_handles, m_values, m_skipped), 
		core_api::get_main_window(), metadb_io_v2::op_flag_delay_ui, NULL);
	DestroyWindow();
	return 0;
}

LRESULT CTaggerDialog::OnListFilesLvnKeydown(LPNMHDR pnmh)
{
	LPNMLVKEYDOWN pnkd = (LPNMLVKEYDOWN)pnmh;
	WORD key = pnkd->wVKey;

	// key DEL
	if (key == VK_DELETE)
	{
		DeleteSelected();
		return 0;
	}

	// forward shortcuts
	service_ptr_t<keyboard_shortcut_manager> ksm;
	keyboard_shortcut_manager::g_get(ksm);
	metadb_handle_list data;

	GetSelection(data);
	ksm->on_keydown_auto_context(data, key, contextmenu_item::caller_active_playlist_selection);
	return 0;
}

BOOL CTaggerDialog::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
	// Init DDX
	DoDataExchange();

	// Set caption
	uSetWindowText(m_hWnd, m_name);

	// Init the CDialogResize code
	DlgResize_Init();

	m_splitter.SubclassWindow(GetDlgItem(IDC_SPLITTER));
	m_splitter.SetSplitterPanes(m_files.m_hWnd, m_lyric.m_hWnd);

	m_files.SetExtendedListViewStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	// Add Columns
	for (int i = 0; i < COL_COUNT; i++)
	{
		m_files.InsertColumn(i, m_columns[i].title, LVCFMT_LEFT, m_columns[i].width);
	}

	static_api_ptr_t<titleformat_compiler> compiler;
	service_ptr_t<titleformat_object> tfo_ar, tfo_ti;

	// Get ARTIST
	compiler->compile_safe(tfo_ar, "[%artist%]");
	// Get TITLE
	compiler->compile_safe(tfo_ti, "$if2(%title%,%filename_ext%)");

	// Add Items
	for (t_size i = 0; i < m_handles.get_count(); ++i)
	{
		const metadb_handle_ptr & p = m_handles[i];
		tstring artist, title, status, lyrics;

		{
			pfc::string8_fast ar, ti;
			pfc::stringcvt::string_wide_from_utf8 conv;

			p->format_title(NULL, ar, tfo_ar, NULL);
			p->format_title(NULL, ti, tfo_ti, NULL);

			conv.convert(ar);
			artist = conv;
			conv.convert(ti);
			title = conv;
		}

		status = (m_values[i] && *m_values[i]) ? status_success : status_failed;
		//lyrics = pfc::stringcvt::string_wide_from_utf8(m_values[i].get_ptr());

		m_files.AddItem(i, COL_ARTIST, artist.c_str());
		m_files.AddItem(i, COL_TITLE, title.c_str());
		m_files.AddItem(i, COL_STATUS, status.c_str());
		//m_files.AddItem(i, COL_LYRICS, lyrics.c_str());
		m_files.SetItemData(i, i);
	}

	return TRUE; // set focus to default control
}

void CTaggerDialog::DeleteSelected()
{
	int idx;

	while (-1 != (idx = m_files.GetNextItem(-1, LVNI_SELECTED)))
	{
		t_size i = (t_size)m_files.GetItemData(idx);

		if (i >= m_handles.get_count())
			break;

		m_skipped.add_item(m_handles[i].get_ptr());
		m_files.DeleteItem(idx);
	}
}

LRESULT CTaggerDialog::OnButtonCloseBnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	DestroyWindow();
	return 0;
}

bool CTaggerDialog::GetSelection(metadb_handle_list & p_out)
{
	int idx = -1;

	while (-1 != (idx = m_files.GetNextItem(idx, LVNI_SELECTED)))
	{
		t_size i = (t_size)m_files.GetItemData(idx);

		if (i >= m_handles.get_count())
			break;

		p_out.add_item(m_handles[i]);
	}

	return p_out.get_count() > 0;
}

LRESULT CTaggerDialog::OnListFilesLvnItemchanged(LPNMHDR pnmh)
{
	LPNMLISTVIEW pnmv = (LPNMLISTVIEW)pnmh;

	if (pnmv->uChanged != LVIF_STATE)
		return 0;

	if (!(pnmv->uNewState & (LVIS_FOCUSED | LVIS_SELECTED)))
		return 0;

	int item = pnmv->iItem;

	// Set Preview
	t_size n = m_files.GetItemData(item);

	if (n >= m_values.get_count())
		return 0;

	uSetDlgItemText(m_hWnd, IDC_EDIT_LYRIC, m_values[n].get_ptr());
	return 0;
}

CTaggerDialog::CTaggerDialog(const pfc::string_base & p_name, metadb_handle_list_cref p_handles, const pfc::string_list_impl & p_values) 
: m_handles(p_handles)
, m_values(p_values)
{
	const t_columns default_columns_data[] = 
	{
		{_T("Artist"), 120, true},
		{_T("Title"), 140, true},
		{_T("Status"), 70, true},
		//{_T("Lyrics"), 220, true},
	};

	for (int i = 0; i < COL_COUNT; i++)
	{
		m_columns[i].title = default_columns_data[i].title;
		m_columns[i].width = default_columns_data[i].width;
		m_columns[i].checked = default_columns_data[i].checked;
	}

	// Prevent unexpected behavior
	t_size handle_count = m_handles.get_count();
	t_size val_count = m_values.get_count();

	if (handle_count > val_count)
	{
		for (t_size i = val_count; i < handle_count; i++)
		{
			m_values.add_item("");
		}
	}

	// Title
	m_name = "Lyrics Grabber Querying Result";
	m_name << " (" << p_name << ")";
}

LRESULT CTaggerDialog::OnListFilesNMRClick(LPNMHDR pnmh)
{
	service_ptr_t<contextmenu_manager> cm;
	metadb_handle_list handles;

	// Generate context menu as foobar2000 context
	GetSelection(handles);

	if (handles.get_count() > 0)
	{
		contextmenu_manager::g_create(cm);
		cm->win32_run_menu_context(m_hWnd, handles);
	}

	return 0;
}

DWORD CSortListViewCtrlColor::OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	const int buffer_len = 10;
	LPNMLVCUSTOMDRAW plvcd = (LPNMLVCUSTOMDRAW)lpNMCustomDraw;
	int id = plvcd->nmcd.dwItemSpec;
	TCHAR buffer[buffer_len] = {0};

	GetItemText(id, CTaggerDialog::COL_STATUS, buffer, buffer_len);

	if (_tcsnicmp(buffer, CTaggerDialog::status_success, _countof(CTaggerDialog::status_success)) == 0)
	{
		plvcd->clrText = GetSysColor(COLOR_BTNTEXT);
	}
	else
	{
		plvcd->clrText = GetSysColor(COLOR_GRAYTEXT);
	}

	return CDRF_DODEFAULT;
}

LRESULT CTaggerDialog::OnSizing(UINT Edge, LPRECT lpRect)
{
	CTaggerDialog * pT = static_cast<CTaggerDialog *>(this);
	ATLASSERT(::IsWindow(pT->m_hWnd));

	DWORD dwStyle = pT->GetStyle();

	// Cleanup in case of multiple initialization
	// block: first check for the gripper control, destroy it if needed
	{
		ATL::CWindow wndGripper = pT->GetDlgItem(ATL_IDW_STATUS_BAR);
		if(wndGripper.IsWindow() && m_arrData.GetSize() > 0 && (m_arrData[0].m_dwResizeFlags & _DLSZ_GRIPPER) != 0)
			wndGripper.DestroyWindow();
	}
	// clear out everything else
	m_arrData.RemoveAll();

	// Get initial dialog client size
	RECT rectDlg = { 0 };
	pT->GetClientRect(&rectDlg);
	m_sizeDialog.cx = rectDlg.right;
	m_sizeDialog.cy = rectDlg.bottom;

	// Walk the map and initialize data
	const _AtlDlgResizeMap* pMap = pT->GetDlgResizeMap();
	ATLASSERT(pMap != NULL);
	int nGroupStart = -1;
	for(int nCount = 1; !(pMap->m_nCtlID == -1 && pMap->m_dwResizeFlags == 0); nCount++, pMap++)
	{
		if(pMap->m_nCtlID == -1)
		{
			switch(pMap->m_dwResizeFlags)
			{
			case _DLSZ_BEGIN_GROUP:
				ATLASSERT(nGroupStart == -1);
				nGroupStart = m_arrData.GetSize();
				break;
			case _DLSZ_END_GROUP:
				{
					ATLASSERT(nGroupStart != -1);
					int nGroupCount = m_arrData.GetSize() - nGroupStart;
					m_arrData[nGroupStart].SetGroupCount(nGroupCount);
					nGroupStart = -1;
				}
				break;
			default:
				ATLASSERT(FALSE && _T("Invalid DLGRESIZE Map Entry"));
				break;
			}
		}
		else
		{
			// this ID conflicts with the default gripper one
			ATLASSERT(m_bGripper ? (pMap->m_nCtlID != ATL_IDW_STATUS_BAR) : TRUE);

			ATL::CWindow ctl = pT->GetDlgItem(pMap->m_nCtlID);
			ATLASSERT(ctl.IsWindow());
			RECT rectCtl = { 0 };
			ctl.GetWindowRect(&rectCtl);
			::MapWindowPoints(NULL, pT->m_hWnd, (LPPOINT)&rectCtl, 2);

			DWORD dwGroupFlag = (nGroupStart != -1 && m_arrData.GetSize() == nGroupStart) ? _DLSZ_BEGIN_GROUP : 0;
			_AtlDlgResizeData data = { pMap->m_nCtlID, pMap->m_dwResizeFlags | dwGroupFlag, { rectCtl.left, rectCtl.top, rectCtl.right, rectCtl.bottom } };
			m_arrData.Add(data);
		}
	}
	ATLASSERT((nGroupStart == -1) && _T("No End Group Entry in the DLGRESIZE Map"));

	return TRUE;//process the message
}
