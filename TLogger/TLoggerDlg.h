#pragma once

#include "UDPServer.h"
#include "SimpleReport.h"

#define USE_THREAD

class CTLoggerDlg : public CDialog
{
public:
	CTLoggerDlg(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_UDP_TEST_DIALOG };

	enum {
		COL_EQUIPID = 0,
		COL_COND,
		COL_STATE,
		COL_TEMP,
		COL_START
	};

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnRestore();

	afx_msg void OnClearLog();
	virtual void OnEquipEdit(NMHDR* pNMHDR, LRESULT* pResult); 
	virtual void OnEquipSelect(NMHDR* pNMHDR, LRESULT* pResult); 
	virtual void OnEquipMenu(NMHDR* pNMHDR, LRESULT* pResult); 
	virtual void OnLogFileOpen(NMHDR* pNMHDR, LRESULT* pResult); 
	virtual void OnLogFileMenu(NMHDR* pNMHDR, LRESULT* pResult); 

	LRESULT OnGSTechStart(WPARAM wparam, LPARAM lparam);
	LRESULT OnGSTechStop(WPARAM wparam, LPARAM lparam);

	LRESULT OnServerError(WPARAM wparam, LPARAM lparam);
	LRESULT OnServerIpError(WPARAM wparam, LPARAM lparam);

protected:
	HICON m_hIcon;
	bool        m_bMinimized;
	CXTPTrayIcon m_TrayIcon;

	DECLARE_MESSAGE_MAP()

public:
	CUDPServer		m_svr;
	CSimpleReport	m_cList;
	CSimpleReport	m_cTemp;
	CSimpleReport	m_cLog;
	CEdit			m_cServerLog;
	int				m_nSelectedOld;
	HANDLE			m_hThread;
	CStdioFile		m_fileServerLog;
	CString			m_sServerLogFilename;
	CString			m_sTemp;				// HACK: thread에서 stack에 할당하면 깨져서 클래스 변수로 만듬.

public:
	CEquip*			getEquip(CXTPReportRecord* pRec);
	void			updateList(bool bFirst);
	CString			getCondition(CEquip* p);
	void			setState(CXTPReportRecord* pRec, const char* msg, int e);
	void			selectLog();
	void			addServerLog(int e, const char* fmt, ...);
	void			addLog(CEquip* p, int e, const char* fmt, ...);
	void			addTemp(CEquip* p);
	void			cleanup();
	void			run();
	void			process();

	static DWORD WINAPI threadProc(void* LPARAM) { 
		((CTLoggerDlg*)LPARAM)->run(); 
		return 1; 
	}
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
