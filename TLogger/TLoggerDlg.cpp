#include "stdafx.h"
#include "TLogger.h"
#include "TLoggerDlg.h"
#include "DialogConfigEquip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTLoggerDlg::CTLoggerDlg(CWnd* pParent /*=NULL*/) : CDialog(CTLoggerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hThread = 0;
	m_nSelectedOld = -1;
}

void CTLoggerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM_LIST, m_cList);
	DDX_Control(pDX, IDC_CUSTOM_TEMP, m_cTemp);
	DDX_Control(pDX, IDC_CUSTOM_LOG, m_cLog);
	DDX_Control(pDX, IDC_EDIT_SVRLOG, m_cServerLog);
}

// clear log, clear log all

BEGIN_MESSAGE_MAP(CTLoggerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_SYSCOMMAND()
	ON_COMMAND(IDR_RESTORE, OnRestore)

	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_LIST, &CTLoggerDlg::OnEquipEdit)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM_LIST, &CTLoggerDlg::OnEquipSelect)
	ON_NOTIFY(NM_RCLICK, IDC_CUSTOM_LIST, &CTLoggerDlg::OnEquipMenu)
	ON_NOTIFY(NM_DBLCLK, IDC_CUSTOM_LOG, &CTLoggerDlg::OnLogFileOpen)
	ON_NOTIFY(NM_RCLICK, IDC_CUSTOM_LOG, &CTLoggerDlg::OnLogFileMenu)
	
	ON_MESSAGE(CEquip::LOG_GSTECH_START, &CTLoggerDlg::OnGSTechStart)
	ON_MESSAGE(CEquip::LOG_GSTECH_END, &CTLoggerDlg::OnGSTechStop)

	ON_MESSAGE(CUDPServer::SVR_HANDLE_ERROR, &CTLoggerDlg::OnServerError)
	ON_MESSAGE(CUDPServer::SVR_UNKNOWN_IP_ERROR, &CTLoggerDlg::OnServerIpError)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_LOG, &CTLoggerDlg::OnClearLog)
END_MESSAGE_MAP()

BOOL CTLoggerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);         // Set big icon
	SetIcon(m_hIcon, FALSE);        // Set small icon

	if (!m_TrayIcon.Create(
		_T("Temp Logging Server"),	// Toolktip text
		this,						// Parent window
		IDR_MAINFRAME,				// Icon resource ID
		IDR_MENU_TRAY,				// Resource ID of popup menu
		IDR_RESTORE,				// Default menu item for popup menu
		false))						// True if default menu item is located by position
	{
		TRACE0("Failed to create tray icon\n");
		return -1;
	}
	//m_bMinimized = false;
	//m_TrayIcon.MaximizeFromTray(this);

	m_cList.SetFontHeader("Tahoma", 14);
	m_cList.SetFontText("Tahoma", 14);
	m_cList.FocusSubItems(FALSE);
	m_cList.SetMultipleSelection(FALSE);
	m_cList.GetReportHeader()->AllowColumnRemove(FALSE);
	m_cList.GetReportHeader()->AllowColumnSort(FALSE);
	m_cList.AllowEdit(FALSE);

	m_cList.ColSet(COL_EQUIPID,	"Equip. ID", 160, DT_LEFT);
	m_cList.ColSet(COL_COND,	"Log Type", 450, DT_LEFT);
	m_cList.ColSet(COL_STATE,	"Status", 60, DT_CENTER);
	m_cList.ColSet(COL_TEMP,	"Temp", 80, DT_RIGHT);
	m_cList.ColSet(COL_START,	"Log Start", 150, DT_CENTER);

	//	m_cDate.SetFormat("yy-MM-dd HH:mm:ss");
	updateList(true);

	m_cTemp.SetFontHeader("Tahoma", 14);
	m_cTemp.SetFontText("Tahoma", 14);
	m_cTemp.FocusSubItems(FALSE);
	m_cTemp.SetMultipleSelection(FALSE);
	m_cTemp.GetReportHeader()->AllowColumnRemove(FALSE);
	m_cTemp.GetReportHeader()->AllowColumnSort(FALSE);
	m_cTemp.AllowEdit(FALSE);
	m_cTemp.ColSet(0, "TimeStamp", 150, DT_CENTER);
	m_cTemp.ColSet(1, "Temp", 80, DT_RIGHT);
	m_cTemp.Populate();

	m_cLog.SetFontHeader("Tahoma", 14);
	m_cLog.SetFontText("Tahoma", 14);
	m_cLog.FocusSubItems(FALSE);
	m_cLog.SetMultipleSelection(FALSE);
	m_cLog.GetReportHeader()->AllowColumnRemove(FALSE);
	m_cLog.GetReportHeader()->AllowColumnSort(FALSE);
	m_cLog.AllowEdit(FALSE);
	m_cLog.ColSet(0, "TimeStamp", 150, DT_CENTER);
	m_cLog.ColSet(1, "Message", 500, DT_LEFT);

	m_cLog.Populate();

	bool bRet = m_svr.start(config.m_nPort, GetSafeHwnd());
	addServerLog(LOG_NORMAL, "Server Started %s", bRet ? "ok" : "fail");

#ifdef USE_THREAD
	DWORD nThreadId;
	m_hThread = CreateThread(NULL, 0, &CTLoggerDlg::threadProc, this, NORMAL_PRIORITY_CLASS, &nThreadId);
	if (m_hThread == INVALID_HANDLE_VALUE)
		addServerLog(LOG_ERROR, "Can't create thread. program cannot work properly");
#else
	SetTimer(17338, 100, NULL);
#endif

	return TRUE;  
}

void CTLoggerDlg::OnCancel()
{
	// 키 입력 잘못되어 실행 중단되는 것 막기
	// CDialog::OnCancel();
}

void CTLoggerDlg::OnOK()
{
	// 키 입력 잘못되어 실행 중단되는 것 막기
	// CDialog::OnOK();
}

void CTLoggerDlg::OnRestore()
{
	m_bMinimized = !m_bMinimized;

	if (m_bMinimized)
	{
		m_TrayIcon.MinimizeToTray(this);
	}
	else
	{
		m_TrayIcon.MaximizeFromTray(this);
	}
}

void CTLoggerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_MINIMIZE  ) {
		m_TrayIcon.MinimizeToTray(this);
		m_bMinimized = true;
	}

	CDialog::OnSysCommand(nID, lParam);
}

HCURSOR CTLoggerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTLoggerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); 

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

void CTLoggerDlg::OnDestroy()
{
	cleanup();
	CDialog::OnDestroy();
}

void CTLoggerDlg::OnClose()
{
#ifdef _DEBUG
	m_svr.stop();
	addServerLog(LOG_NORMAL, "server gracefully closed...");
	CDialog::OnOK();
#else
	CString s;
	s.Format("Are you sure to quit the Temp Logger Server?\r\n");
	if (MessageBox(s, "Quit", MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK) {
		m_svr.stop();
		addServerLog(LOG_NORMAL, "server gracefully closed...");
		CDialog::OnOK();
	}
#endif
}

void CTLoggerDlg::updateList(bool bFirst)
{
	if (bFirst) {
		m_cList.RowRemoveAll();

		CXTPReportRecord* pRec;
		CXTPReportRecordItem* pItem;
		for (size_t i = 0; i < config.m_aEquip.size(); i++) {
			CEquip* p = config.m_aEquip[i];
			pRec = m_cList.RowAdd();

			pItem = m_cList.ColAdd(pRec, p->m_sID);	// 0
			pItem->SetItemData((DWORD)p);
			p->m_hWnd = GetSafeHwnd();
			p->m_nIndex = i;
			m_cList.ColAdd(pRec, getCondition(p));	// 1
			m_cList.ColAdd(pRec, "Stop");			// 2
			m_cList.ColAdd(pRec, 0.0, "%.1f ℃");	// 3
			m_cList.ColAdd(pRec, "2014-05-10 10:11:11"); // log start (3)
		}
	}
	else {
		CXTPReportRows* pRows = m_cList.GetRows();
		CXTPReportRecord* pRec;
		for (size_t i = 0; i < config.m_aEquip.size(); i++) {
			CEquip* p = config.m_aEquip[i];
			pRec = pRows->GetAt(i)->GetRecord();
			pRec->GetItem(COL_EQUIPID)->SetCaption(p->m_sID);
			pRec->GetItem(COL_COND)->SetCaption(getCondition(p));
		}			
	}

	m_cList.Populate();
}

CEquip* CTLoggerDlg::getEquip(CXTPReportRecord* pRec)
{
	return (pRec == NULL) ? NULL : (CEquip*)pRec->GetItem(COL_EQUIPID)->GetItemData();
}

void CTLoggerDlg::OnEquipEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	XTP_NM_REPORTRECORDITEM* pItemNotify = (XTP_NM_REPORTRECORDITEM*) pNMHDR;

	if (pItemNotify == NULL)
		return;
	if (pItemNotify->pRow == NULL)
		return;

	CXTPReportRecord* pRec = pItemNotify->pRow->GetRecord();
	CXTPReportRecordItem* pItem = pRec->GetItem(COL_STATE);
	CString s = pItem->GetCaption(NULL);
	if (pItem->GetCaption(NULL) == "Run")
		return;

	CDialogConfigEquip d;
	d.m_pEquip = getEquip(pRec);
	if (d.DoModal() == IDOK) {
		config.save();
		updateList(false);
	}

	*pResult = 0;
}

void CTLoggerDlg::OnEquipSelect(NMHDR* pNMHDR, LRESULT* pResult) 
{
	XTP_NM_REPORTRECORDITEM* pItemNotify = (XTP_NM_REPORTRECORDITEM*) pNMHDR;

	if (pItemNotify == NULL)
		return;
	if (pItemNotify->pRow == NULL)
		return;

	int n = pItemNotify->pRow->GetRecord()->GetIndex();

	if (n != m_nSelectedOld) {
		selectLog();
		m_nSelectedOld = n;
	}
}

void CTLoggerDlg::OnEquipMenu(NMHDR* pNMHDR, LRESULT* pResult) 
{
	XTP_NM_REPORTRECORDITEM* pItemNotify = (XTP_NM_REPORTRECORDITEM*) pNMHDR;

	if (pItemNotify == NULL)
		return;
	if (pItemNotify->pRow == NULL)
		return;
	CXTPReportRecord* pRec = pItemNotify->pRow->GetRecord();
	if (pRec == NULL)
		return;

	CEquip* p = getEquip(pRec);
	CMenu menu;
	if (!menu.CreatePopupMenu())
		return;

	if (!p->m_bLogging)
		menu.AppendMenu(MF_STRING, 1, "Edit Equipment");
	if (p->m_bLogging)
		menu.AppendMenu(MF_STRING, 2, "Force Stop");
	if (!p->m_bLogging)
		menu.AppendMenu(MF_STRING, 3, "Force Start");
	if (p->m_bLock)
		menu.AppendMenu(MF_STRING, 4, "UnLock");

	CPoint pt = pItemNotify->pt;
	int n = menu.TrackPopupMenu(TPM_CENTERALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, pt.x, pt.y, this);
	menu.DestroyMenu();
	switch (n) {
	case 1:
		{
			CDialogConfigEquip d;
			d.m_pEquip = p;
			if (d.DoModal() == IDOK) {
				config.save();
				updateList(false);
			}
		}
		break;
	case 2:
		p->stop(true);
		break;
	case 3:
		p->start();
		break;
	case 4:
		p->setLock(false);
		setState(pRec, "Stop", LOG_NORMAL);
		break;

	}
}

void CTLoggerDlg::OnLogFileOpen(NMHDR* pNMHDR, LRESULT* pResult) 
{
	XTP_NM_REPORTRECORDITEM* pItemNotify = (XTP_NM_REPORTRECORDITEM*) pNMHDR;
	if (pItemNotify == NULL)
		return;

	if (pItemNotify->pRow == NULL)
		return;
	CXTPReportRecord* pRec = pItemNotify->pRow->GetRecord();
	CString s = pRec->GetItem(1)->GetCaption(NULL);

	int sp = 0, ep;
	sp = s.Find('\'', sp);
	ep = s.Find('\'', sp+1);

	if (sp == -1 || ep == -1)
		return;

	s = s.Mid(sp+1, ep-sp-5);	// exclude file extension
	s += ".csv";

	ShellExecute(NULL, "edit", s, NULL, NULL, SW_SHOWNORMAL);
}

void CTLoggerDlg::OnLogFileMenu(NMHDR* pNMHDR, LRESULT* pResult)
{
	XTP_NM_REPORTRECORDITEM* pItemNotify = (XTP_NM_REPORTRECORDITEM*) pNMHDR;
	if (pItemNotify == NULL)
		return;

	if (pItemNotify->pRow == NULL)
		return;
	CXTPReportRecord* pRec = pItemNotify->pRow->GetRecord();
	CString s = pRec->GetItem(1)->GetCaption(NULL);

	int sp = 0, ep;
	sp = s.Find('\'', sp);
	ep = s.Find('\'', sp+1);

	if (sp == -1 || ep == -1)
		return;

	s = s.Mid(sp+1, ep-sp-5);	// exclude file extension

	CMenu menu;
	if (!menu.CreatePopupMenu())
		return;

	menu.AppendMenu(MF_STRING, 1, "Open with TGraph");
	menu.AppendMenu(MF_STRING, 2, "Open with Excel");
	menu.AppendMenu(MF_STRING, 3, "Open with Notepad");

	CPoint pt = pItemNotify->pt;
	int n = menu.TrackPopupMenu(TPM_CENTERALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, pt.x, pt.y, this);
	menu.DestroyMenu();
	switch (n) {
		case 1: 
			s += ".dat"; 
			ShellExecute(NULL, "open", s, NULL, NULL, 0); 
			break;
		case 2: 
			s += ".csv"; 
			ShellExecute(NULL, "open", s, "excel.exe", NULL, 0); 
			break;
		case 3: 
			s += ".csv"; 
			ShellExecute(NULL, "edit", s, "notepad.exe", NULL, SW_SHOWNORMAL); 
			break;
	}
}

CString CTLoggerDlg::getCondition(CEquip* p)
{
	CString s;
	if (p->m_nLoggerType == 0)
		s.Format("%s: Interval=%d", p->m_sLoggerType, p->m_nInterval);
	else {
		if (p->m_nLogMethod == 0)		// periodic
			s.Format("%s: Interval=%d, %s ~ %s", p->m_sLogMethod, p->m_nInterval, p->m_sStartTime, p->m_sEndTime);
		else
			s.Format("%s: Interval=%d, Tolerance=%.1f, Start[%s], End[%s]", p->m_sLogMethod, p->m_nInterval, p->m_fTolerance, p->m_sStartCond, p->m_sEndCond);
	}
	return s;
}

LRESULT CTLoggerDlg::OnServerError(WPARAM wparam, LPARAM lparam)
{
	addServerLog(LOG_ERROR, "Server Error: %s, server stopped.", m_svr.m_sError);
	m_svr.stop();

	// server restart
	bool bRet = m_svr.start(config.m_nPort, GetSafeHwnd());
	addServerLog(LOG_NORMAL, "Server Started %s", bRet ? "ok" : "fail");
	return 0;
}

LRESULT CTLoggerDlg::OnServerIpError(WPARAM wparam, LPARAM lparam)
{
	struct sockaddr_in sa;
	sa.sin_addr.s_addr = (unsigned int)wparam;
	CString ip = inet_ntoa(sa.sin_addr);

	addServerLog(LOG_WARN, "Unknown Server (%s): temp=%5.2f", ip, lparam / 100.0);
	return 0;
}

void CTLoggerDlg::addServerLog(int e, const char* fmt, ...)
{
	CString s;
	if (m_fileServerLog.m_hFile == INVALID_HANDLE_VALUE) {
		m_sServerLogFilename.Format("%s\\tlogger.log", config.m_sLogPath);
		if (!m_fileServerLog.Open(m_sServerLogFilename, CFile::modeNoTruncate | CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone)) {
			s.Format("Can't open server log file '%s", m_sServerLogFilename);
			MessageBox(s, "Critical Error", MB_OK);
			CDialog::OnCancel();
		}

		CString log;
		ULONGLONG maxsz = 64*1024;
		ULONGLONG x = m_fileServerLog.GetLength();
		bool bReopen = false;
		if (x > maxsz) {
			while (m_fileServerLog.ReadString(s) && (x - m_fileServerLog.GetPosition()) > 64 * 1024)
				;
			bReopen = true;
		}

		while (m_fileServerLog.ReadString(s)) {
			log += s;
			log += "\n";
		}

		if (bReopen) {
			m_fileServerLog.Close();
			if (!m_fileServerLog.Open(m_sServerLogFilename, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone)) {
				s.Format("Can't open server log file '%s", m_sServerLogFilename);
				MessageBox(s, "Critical Error", MB_OK);
				CDialog::OnCancel();
			}
			m_fileServerLog.WriteString(log);
		}

		log.Replace("\n", "\r\n");
		int n = m_cServerLog.GetWindowTextLength();
		m_cServerLog.SetSel(n, n);
		m_cServerLog.ReplaceSel(log);
	}

	va_list	ap;
	char buf[2048];

	va_start(ap, fmt);
	vsprintf_s(buf, sizeof buf, fmt, ap);
	va_end(ap);

	s.Format("%s [%d]: %s", config.t2s(time(NULL)), e, buf);
	CString x = s + "\n";

	m_fileServerLog.WriteString(x);
	m_fileServerLog.Flush();
	OutputDebugString(x);

	x = s + "\r\n";
	int n = m_cServerLog.GetWindowTextLength();
	m_cServerLog.SetSel(n, n, TRUE);
	m_cServerLog.ReplaceSel(x);

	// TODO 로그의 마지막으로 자동 이동. 왜 아래줄이 안될까?
	n = m_cServerLog.GetWindowTextLength();
	m_cServerLog.SetFocus();
	m_cServerLog.SetSel(n, -1, TRUE);
	
}

void CTLoggerDlg::addLog(CEquip* p, int e, const char* fmt, ...)
{
	// 현재 focus 된 row와 일치하지 않으면 log 추가 하지 않음.
	CXTPReportRow* pRow = m_cList.GetFocusedRow();
	if (pRow == NULL)
		return;
	
	if (p != getEquip((pRow->GetRecord())))
		return;

	va_list	ap;
	char buf[2048];
	CString s;

	va_start(ap, fmt);
	vsprintf_s(buf, sizeof buf, fmt, ap);
	va_end(ap);

	size_t n = m_cLog.GetRecords()->GetCount();
	if (n > config.m_nLogMax)
		m_cLog.GetRecords()->RemoveAt(n-1);

	CXTPReportRecord* pRec = new CXTPReportRecord;
	s.Format("%s", buf);

	m_cLog.ColAdd(pRec, config.t2s(time(NULL)));
	m_cLog.ColAdd(pRec, s);

	m_cLog.GetRecords()->InsertAt(0, pRec);
	if (e == LOG_WARN)
		m_cLog.RowSetColor(pRec, RGB(0, 0, 0), RGB(255, 255, 0));
	else if (e == LOG_ERROR)
		m_cLog.RowSetColor(pRec, RGB(255, 255, 255), RGB(255, 0, 0));

	m_cLog.Populate();
	m_cLog.RowSetFocused(0);
}

void CTLoggerDlg::addTemp(CEquip* p)
{
	// 현재 focus된 row와 일치하지 않으면 log 업데이트 하지 않음.
	CXTPReportRow* pRow = m_cList.GetFocusedRow();
	if (pRow == NULL)
		return;

	if (p != getEquip((pRow->GetRecord())))
		return;

	size_t n = m_cTemp.GetRecords()->GetCount();
	if (n > config.m_nLogTempMax)
		m_cTemp.GetRecords()->RemoveAt(n-1);

	CXTPReportRecord* pRec = new CXTPReportRecord;
	m_cTemp.ColAdd(pRec, config.t2s(p->m_tLastLogged));
	m_cTemp.ColAdd(pRec, p->m_fPV, "%5.2f ℃");
	m_cTemp.GetRecords()->InsertAt(0, pRec);

	m_cTemp.Populate();
	m_cTemp.RowSetFocused(0);
}

void CTLoggerDlg::selectLog()
{
	CXTPReportRow* pRow = m_cList.GetFocusedRow();
	if (pRow == NULL)
		return;

	CEquip* p = getEquip(pRow->GetRecord());

	m_cTemp.RowRemoveAll();
	m_cLog.RowRemoveAll();
	
	// setup temp log
	for (size_t i = 0; i < p->m_aTempLog.size(); i++) {
		CXTPReportRecord* pRec = new CXTPReportRecord;
		m_cTemp.ColAdd(pRec, config.t2s(p->m_aTempLog[i].t));
		m_cTemp.ColAdd(pRec, p->m_aTempLog[i].temp / 100.0, "%5.2f ℃");
		m_cTemp.GetRecords()->InsertAt(0, pRec);
	}
	m_cTemp.Populate();
	m_cTemp.RowSetFocused(0);

	// setup log
	for (size_t i = 0; i < p->m_aLog.size(); i++) {
		CXTPReportRecord* pRec = new CXTPReportRecord; 

		m_cLog.ColAdd(pRec, config.t2s(p->m_aLog[i].t));
		m_cLog.ColAdd(pRec, p->m_aLog[i].msg);

		m_cLog.GetRecords()->InsertAt(0, pRec);
		if (p->m_aLog[i].e == LOG_WARN)
			m_cLog.RowSetColor(pRec, RGB(0, 0, 0), RGB(255, 255, 0));
		else if (p->m_aLog[i].e == LOG_ERROR)
			m_cLog.RowSetColor(pRec, RGB(255, 255, 255), RGB(255, 0, 0));
	}
	m_cLog.Populate();
	m_cLog.RowSetFocused(0);
}

void CTLoggerDlg::cleanup()
{
	for (size_t i = 0; i < config.m_aEquip.size(); i++) {
		CEquip* p = config.m_aEquip[i];
		p->stop();
	}
#ifdef USE_THREAD
	if (m_hThread != 0 && !TerminateThread(m_hThread, 0)) {
		TRACE("can't terminate thread\n");
	}
#else	
	KillTimer(17338);

#endif
}

void CTLoggerDlg::process()
{
	static time_t st = time(NULL);

	CXTPReportRecord* pRec;
	CXTPReportRow* pRow;
	CEquip* p;

	for (int i = 0; i < m_cList.GetRows()->GetCount(); i++) {

		pRow = m_cList.GetRows()->GetAt(i);
		if (pRow == NULL)
			continue;

		pRec = pRow->GetRecord();
		p = getEquip(pRec);
		int ret = p->run();

		// update temp
		m_sTemp.Format("%5.2f ℃", p->m_fPV);
		pRec->GetItem(COL_TEMP)->SetCaption(m_sTemp);

		switch (ret) {
		case CEquip::LOG_STARTED:
			addLog(p, LOG_NORMAL, "'%s' log started...", p->m_sBinFilename);
			addTemp(p);
			pRec->GetItem(COL_START)->SetCaption(config.t2s(p->m_tLogStart));
			setState(pRec, "Run", LOG_NORMAL);
			break;
		case CEquip::LOG_ENDED:
			addTemp(p);
			addLog(p, LOG_NORMAL, "'%s' log stopped...'", p->m_sBinFilename);
			setState(pRec, "Stop", LOG_NORMAL);
			break;
		case CEquip::LOG_LOCKED:
			setState(pRec, "Lock", LOG_WARN);
			break;
		case CEquip::LOG_PROGRESS:
			addTemp(p);
			break;
		case CEquip::LOG_PEND:
			if (p->m_bLogging)
				setState(pRec, "Run", LOG_NORMAL);
			else
				setState(pRec, "Stop", LOG_NORMAL);
			break;
		case LOG_ERROR:
			if (!p->m_bErrorLogged) {
				addLog(p, LOG_ERROR, p->m_sLastMsg);
				setState(pRec, "Error", LOG_ERROR);
				p->m_bErrorLogged = true;
			}
			break;
		}
	}

	if (time(NULL) > st) {
		st = time(NULL);

		m_cList.Populate();

		pRow = m_cList.GetFocusedRow();
		if (pRow != NULL) {
			p = getEquip(pRow->GetRecord());

			if (p->m_bLogging) 
				m_sTemp.Format("%s", config.d2s(int(time(NULL) - p->m_tLogStart)));
			else
				m_sTemp.Format("        ");
			GetDlgItem(IDC_STATIC_ELLAPSED)->SetWindowText(m_sTemp);

			m_sTemp = config.t2s(p->m_tLastUpdate);
			GetDlgItem(IDC_STATIC_LASTUPDATE)->SetWindowText(m_sTemp);
			m_cList.Populate();

			m_sTemp.Format("%d", p->m_nLogCount);
			GetDlgItem(IDC_STATIC_LOGCOUNT)->SetWindowText(m_sTemp);
		}
	}
}

void CTLoggerDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 17338) 
		process();
	CDialog::OnTimer(nIDEvent);
}

void CTLoggerDlg::run()
{
	while (1) {
		process();
		Sleep(10);
	}
}

void CTLoggerDlg::setState(CXTPReportRecord* pRec, const char* msg, int e)
{
	pRec->GetItem(COL_STATE)->SetCaption(msg);
	if (e == LOG_NORMAL)
		m_cList.RowSetColor(pRec, RGB(0, 0, 0), RGB(255, 255, 255));
	else if (e == LOG_WARN)
		m_cList.RowSetColor(pRec, RGB(0, 0, 0), RGB(255, 255, 0));
	else if (e == LOG_ERROR)
		m_cList.RowSetColor(pRec, RGB(255, 255, 255), RGB(255, 0, 0));
}

LRESULT CTLoggerDlg::OnGSTechStart(WPARAM wparam, LPARAM lparam)
{
	CXTPReportRecord* pRec = m_cList.GetRecords()->GetAt(wparam);
	CEquip* p = config.m_aEquip[wparam];

	addLog(p, LOG_NORMAL, "'%s' log started...", p->m_sBinFilename);
	pRec->GetItem(COL_START)->SetCaption(config.t2s(p->m_tLogStart));
	setState(pRec, "Run", LOG_NORMAL);
	return 0;
}

LRESULT CTLoggerDlg::OnGSTechStop(WPARAM wparam, LPARAM lparam)
{
	CXTPReportRecord* pRec = m_cList.GetRecords()->GetAt(wparam);
	CEquip* p = config.m_aEquip[wparam];

	addLog(p, LOG_NORMAL, "'%s' log stopped...'", p->m_sBinFilename);
	setState(pRec, "Stop", LOG_NORMAL);
	return 0;
}

void CTLoggerDlg::OnClearLog()
{
	CXTPReportRow* pRow = m_cList.GetFocusedRow();
	if (pRow == NULL)
		return;

	CEquip* p = getEquip((pRow->GetRecord()));
	p->clearLogFile();

	m_cLog.RowRemoveAll();
	m_cTemp.RowRemoveAll();
	m_cLog.Populate();
	m_cTemp.Populate();
}

