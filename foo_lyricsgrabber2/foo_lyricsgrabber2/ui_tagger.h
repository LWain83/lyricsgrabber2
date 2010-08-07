#pragma once

#include "resource.h"
#include "splitctrl.h"


class my_string_list_impl : public pfc::string_list_const
{
public:
	t_size get_count() const {return m_data.get_size();}
	void get_item_ex(const char* & p_out, t_size n) const {p_out = m_data[n];}

	inline const char * operator[] (t_size n) const {return m_data[n];}
	inline pfc::string8 & operator[] (t_size n) {return m_data[n];}

	void add_item(const char * p_string) {
		t_size idx = m_data.get_size();
		m_data.set_size(idx + 1);
		m_data[idx] = p_string;
	}

	void add_items(const pfc::string_list_const & p_source) {_append(p_source);}

	void remove_all() 
	{
		m_data.set_size(0);
	}

	inline my_string_list_impl() {}
	inline my_string_list_impl(const my_string_list_impl & p_source) {_copy(p_source);}
	inline my_string_list_impl(const pfc::string_list_const & p_source) {_copy(p_source);}
	inline const my_string_list_impl & operator=(const my_string_list_impl & p_source) {_copy(p_source);return *this;}
	inline const my_string_list_impl & operator=(const pfc::string_list_const & p_source) {_copy(p_source);return *this;}
	inline const my_string_list_impl & operator+=(const my_string_list_impl & p_source) {_append(p_source);return *this;}
	inline const my_string_list_impl & operator+=(const pfc::string_list_const & p_source) {_append(p_source);return *this;}

private:
	void _append(const pfc::string_list_const & p_source) {
		const t_size toadd = p_source.get_count(), base = m_data.get_size();
		m_data.set_size(base+toadd);
		for(t_size n=0;n<toadd;n++) m_data[base+n] = p_source[n];
	}

	void _copy(const pfc::string_list_const & p_source) {
		const t_size newcount = p_source.get_count();
		m_data.set_size(newcount);
		for(t_size n=0;n<newcount;n++) m_data[n] = p_source[n];
	}

	pfc::array_t<pfc::string8,pfc::alloc_fast> m_data;
};

// Forward declaration
class CTaggerDialog;

class CSortListViewCtrlColor
	: public CSortListViewCtrlImpl<CSortListViewCtrlColor>
	, public CCustomDraw<CSortListViewCtrlColor>                
{
public:
	BEGIN_MSG_MAP(CSortListViewCtrlColor)
		CHAIN_MSG_MAP(CSortListViewCtrlImpl<CSortListViewCtrlColor>)
		CHAIN_MSG_MAP(CCustomDraw<CSortListViewCtrlColor>)
	END_MSG_MAP()

	DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
	{        
		return CDRF_NOTIFYITEMDRAW;
	}

	DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw);
};

class CTaggerDialog 
	: public CDialogImpl<CTaggerDialog>
	, public CDialogResize<CTaggerDialog>
	, public CWinDataExchange<CTaggerDialog>
{
public:
	enum 
	{
		COL_ARTIST = 0,
		COL_TITLE,
		COL_STATUS,
		//COL_LYRICS,
		COL_COUNT,
	};

	static const TCHAR status_failed[];
	static const TCHAR status_success[];

private:
	struct t_columns
	{
		TCHAR * title;
		t_uint width;
		bool checked;
	};

	t_columns m_columns[COL_COUNT];
	metadb_handle_list m_handles;
	my_string_list_impl m_values;
	pfc::list_t<const metadb_handle *> m_skipped;
	pfc::string8 m_name;

	CSortListViewCtrlColor m_files;
	CEdit m_lyric;
	CVertSplitterCtrl m_splitter;

public:
	enum { IDD = IDD_DIALOG_TAGGER };

	BEGIN_MSG_MAP(CTaggerDialog)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_NCDESTROY(OnNcDestroy)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZING(OnSizing)
		NOTIFY_HANDLER(IDC_LIST_FILES, NM_CUSTOMDRAW, m_files.OnCustomDraw)
		NOTIFY_HANDLER_EX(IDC_LIST_FILES, LVN_KEYDOWN, OnListFilesLvnKeydown)
		NOTIFY_HANDLER_EX(IDC_LIST_FILES, LVN_ITEMCHANGED, OnListFilesLvnItemchanged)
		NOTIFY_HANDLER_EX(IDC_LIST_FILES, NM_RCLICK, OnListFilesNMRClick)
		COMMAND_HANDLER_EX(IDC_BUTTON_CLOSE, BN_CLICKED, OnButtonCloseBnClicked)
		COMMAND_HANDLER_EX(IDC_BUTTON_UPDATE, BN_CLICKED, OnButtonUpdateBnClicked)
		CHAIN_MSG_MAP(CDialogResize<CTaggerDialog>)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CTaggerDialog)
		DDX_CONTROL(IDC_LIST_FILES, m_files)
		DDX_CONTROL_HANDLE(IDC_EDIT_LYRIC, m_lyric)
	END_DDX_MAP()

	BEGIN_DLGRESIZE_MAP(CTaggerDialog)
		DLGRESIZE_CONTROL(IDC_BUTTON_UPDATE, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_BUTTON_CLOSE, DLSZ_MOVE_X | DLSZ_MOVE_Y)  
		DLGRESIZE_CONTROL(IDC_LIST_FILES, DLSZ_SIZE_Y | DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_EDIT_LYRIC, DLSZ_SIZE_Y | DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_SPLITTER, DLSZ_MOVE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()


	CTaggerDialog(const pfc::string_base & p_name, metadb_handle_list_cref p_handles, const pfc::string_list_impl & p_values);
	virtual ~CTaggerDialog() {}

	void DeleteSelected();
	bool GetSelection(metadb_handle_list & p_out);

	//
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

	LRESULT OnButtonCloseBnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	LRESULT OnButtonUpdateBnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	LRESULT OnListFilesLvnKeydown(LPNMHDR pnmh);
	LRESULT OnListFilesLvnItemchanged(LPNMHDR pnmh);
	LRESULT OnListFilesNMRClick(LPNMHDR pnmh);
	LRESULT OnSizing(UINT Edge, LPRECT lpRect);
};
