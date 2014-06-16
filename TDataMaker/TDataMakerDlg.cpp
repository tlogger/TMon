#include "stdafx.h"
#include "TDataMaker.h"
#include "TDataMakerDlg.h"
#include "afxdialogex.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTDataMakerDlg::CTDataMakerDlg(CWnd* pParent /*=NULL*/) : CDialogEx(CTDataMakerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_sDir = theApp.GetProfileString(APPID, "m_sDir", ".\\");
	m_sProfile = theApp.GetProfileString(APPID, "m_sProfile", "85,125,65,45,25");
	m_fTolerance = theApp.GetProfileInt(APPID, "m_fTolerance", 300) / 1000.0;
	m_nDuration = theApp.GetProfileInt(APPID, "m_nDuration", 3600);
	m_fTr = theApp.GetProfileInt(APPID, "m_fTr", 100) / 1000.0;
	m_fTf = theApp.GetProfileInt(APPID, "m_fTf", 100) / 1000.0;
	m_nSamples = theApp.GetProfileInt(APPID, "m_nSamples", 604800);	// 7days
	m_bCSV = theApp.GetProfileInt(APPID, "m_bCSV", 0);

	m_bStart = false;
	m_fPV = 25.0;
	m_nCurrentStep = 0;
}

BOOL CTDataMakerDlg::DestroyWindow()
{
	UpdateData(TRUE);

	theApp.WriteProfileString(APPID, "m_sDir", m_sDir);
	theApp.WriteProfileString(APPID, "m_sProfile", m_sProfile);
	theApp.WriteProfileInt(APPID, "m_fTolerance", int(m_fTolerance*1000.0));
	theApp.WriteProfileInt(APPID, "m_nDuration", m_nDuration);
	theApp.WriteProfileInt(APPID, "m_fTr", int(m_fTr*1000.0));
	theApp.WriteProfileInt(APPID, "m_fTf", int(m_fTf*1000.0));
	theApp.WriteProfileInt(APPID, "m_nSamples", m_nSamples);
	theApp.WriteProfileInt(APPID, "m_bCSV", m_bCSV);

	return CDialogEx::DestroyWindow();
}

void CTDataMakerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DIR, m_sDir);
	DDX_Text(pDX, IDC_EDIT_PROFILE, m_sProfile);
	DDX_Text(pDX, IDC_EDIT_TR, m_fTr);
	DDX_Text(pDX, IDC_EDIT_TF, m_fTf);
	DDX_Text(pDX, IDC_EDIT_TOLERANCE, m_fTolerance);
	DDX_Text(pDX, IDC_EDIT_DURATION, m_nDuration);
	DDX_Text(pDX, IDC_EDIT_SAMPLES, m_nSamples);
	DDX_Control(pDX, IDC_PROGRESS1, m_cProgress);
	DDX_Check(pDX, IDC_CHECK_CSV, m_bCSV);
}

BEGIN_MESSAGE_MAP(CTDataMakerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CTDataMakerDlg::OnBnClickedButtonStart)
END_MESSAGE_MAP()

BOOL CTDataMakerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(PRG_VER);

	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CTDataMakerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CTDataMakerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTDataMakerDlg::OnOK()
{
	//	CDialogEx::OnOK();
}

void CTDataMakerDlg::OnBnClickedButtonStart()
{
	CString s;

	if (m_bStart) {
		m_bStart = !m_bStart;
		TerminateThread(m_hThread, 0);
		m_fileBIN.Close();
		if (m_bCSV)
			m_fileCSV.Close();
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("Start");
	}
	else {
		UpdateData(TRUE);
		m_nCurrentStep = 0;
		m_fPV = 25.0;

		CString tok;
		int pos = 0;
		m_aStep.clear();
		for (tok = m_sProfile.Tokenize(",", pos); tok != ""; tok = m_sProfile.Tokenize(",", pos)) 
			m_aStep.push_back(atof(tok));

		m_cProgress.SetRange32(0, m_nSamples);

		char dt[20];
		time_t st = time(NULL);
		struct tm tmp;

		localtime_s(&tmp, &st);
		strftime(dt, sizeof(dt), "%Y-%m-%d_%H_%M_%S", &tmp);
		m_fnBIN.Format("%s\\%s.dat", m_sDir, dt);
		m_fnCSV.Format("%s\\%s.csv", m_sDir, dt);

		try {
			CString s;
			if (!m_fileBIN.Open(m_fnBIN, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone)) {
				s.Format("can't open binary file '%s'", m_fnBIN);
				MessageBox(s, "File Open Error", MB_OK | MB_ICONERROR);
				return;
			}

			if (m_bCSV) {
				if (!m_fileCSV.Open(m_fnCSV, CFile::modeCreate | CFile::typeText | CFile::modeWrite | CFile::shareDenyNone)) {
					s.Format("can't open CSV file '%s'", m_fnBIN);
					MessageBox(s, "File Open Error", MB_OK | MB_ICONERROR);
					return;
				}
				m_fileCSV.WriteString("DateTime,Temp\n");
			}

			DWORD m_nThreadId; 
			m_hThread = CreateThread(NULL, NULL, threadProc, this, NULL, &m_nThreadId);
			if (m_hThread == INVALID_HANDLE_VALUE) {
				m_fileBIN.Close();
				m_fileCSV.Close();
				return;
			}
		}
		catch(...) {
			if (m_fileBIN.m_hFile != INVALID_HANDLE_VALUE)
				m_fileBIN.Close();

			if (m_bCSV && m_fileCSV.m_hFile != INVALID_HANDLE_VALUE)
				m_fileCSV.Close();
		}

		m_bStart = !m_bStart;
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("Stop");
	}
}

void CTDataMakerDlg::run()
{
	double SP = m_aStep[m_nCurrentStep];
	CString s;
	time_t st = time(NULL);

	try {
		for (int i = 0; i < m_nSamples; i++) {
			if (i % 100 == 0) {
				m_cProgress.SetPos(i);
				TRACE("%s: %d\n", __FUNCTION__, i);
			}

			if (abs(m_fPV - SP) < 1.0) {		// 온도가 맞음

				for (int j = 0; i < m_nSamples && j < m_nDuration; j++, i++) {
					if (i % 100 == 0) {
						m_cProgress.SetPos(i);
						TRACE("%s: s %d\n", __FUNCTION__, i);
					}					
					if (!writeRec((DWORD)st))
						throw 1;
					st++;
				}

				if (++m_nCurrentStep >= m_aStep.size())
					m_nCurrentStep = 0;
				SP = m_aStep[m_nCurrentStep];

				continue;
			}
			else if (m_fPV > SP) {			// 현재 온도가 설정 온도보다 큼. 온도 내림
				m_fPV -= m_fTf;
			}
			else if (m_fPV < SP) {			// 현재 온도가 설정 온도보다 작음. 온도 올림
				m_fPV += m_fTr;
			}
		
			if (!writeRec((DWORD)st))
				throw 1;

			st++;
		}
	}
	catch(int x) {
		if (x == 1) {
			TRACE("%s: write error\n", __FUNCTION__);
		}
	}

	m_fileBIN.Flush();
	m_fileBIN.Close();
	if (m_bCSV) {
		m_fileCSV.Flush();
		m_fileCSV.Close();
	}

	m_bStart = !m_bStart;
	GetDlgItem(IDC_BUTTON_START)->SetWindowText("Start");
}

bool CTDataMakerDlg::writeRec(DWORD st)
{
	double tol;
	logfile_rec_t rec;

	tol = m_fPV + (rand() % 100)/ 100.0 * m_fTolerance - (m_fTolerance / 2.0);
	rec.t = st;
	rec.temp = short(tol * 100.0);
	try {
		m_fileBIN.Write(&rec, sizeof(rec));

		if (m_bCSV) {
			char dt[20];
			struct tm tmp;
			time_t t = st;
			localtime_s(&tmp, &t);
			strftime(dt, sizeof(dt), "%Y-%m-%d %H:%M:%S", &tmp);
			CString s;
			s.Format("%s,%.2f\n", dt, double(rec.temp) / 100.0);
			m_fileCSV.WriteString(s);
		}
	}
	catch (...) {
		return false;
	}
	return true;

}