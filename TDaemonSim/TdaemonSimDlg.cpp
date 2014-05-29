#include "stdafx.h"
#include "TdaemonSim.h"
#include "TdaemonSimDlg.h"
#include "afxdialogex.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const char* PRG = "TDaemonSim";

// TODO gstech start, end로 테스트

CTdaemonSimDlg::CTdaemonSimDlg(CWnd* pParent /*=NULL*/) : CDialogEx(CTdaemonSimDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_sHost = theApp.GetProfileString(PRG, "m_sHost", "127.0.0.1");
	m_nPort	= theApp.GetProfileInt(PRG, "m_nPort", 1000);

	m_bGSTECH = theApp.GetProfileInt(PRG, "m_bGSTECH", 0);
	m_sPrefix1 = theApp.GetProfileString(PRG, "m_sPrefix1", "TA");
	m_sPrefix2 = theApp.GetProfileString(PRG, "m_sPrefix2", "TB");
	m_sProfile = theApp.GetProfileString(PRG, "m_sProfile", "85,125,65,45,25");
	m_bContinue = theApp.GetProfileInt(PRG, "m_bContinue", 1);
	m_nPeriod = theApp.GetProfileInt(PRG, "m_nPeriod", 1000);
	m_fTolerance = theApp.GetProfileInt(PRG, "m_fTolerance", 500) / 1000.0;
	m_nDuration = theApp.GetProfileInt(PRG, "m_nDuration", 5);
	m_fTr = theApp.GetProfileInt(PRG, "m_nTr", 1000) / 1000.0;
	m_fTf = theApp.GetProfileInt(PRG, "m_nTf", 1000) / 1000.0;

	m_bStart = false;
	m_fPV = 25.0;
	m_nCurrentStep = 0;
}

BOOL CTdaemonSimDlg::DestroyWindow()
{
	UpdateData(TRUE);

	theApp.WriteProfileString(PRG, "m_sHost", m_sHost);
	theApp.WriteProfileInt(PRG, "m_nPort", m_nPort);

	theApp.WriteProfileInt(PRG, "m_bGSTECH", m_bGSTECH);
	theApp.WriteProfileString(PRG, "m_sPrefix1", m_sPrefix1);
	theApp.WriteProfileString(PRG, "m_sPrefix2", m_sPrefix2);
	theApp.WriteProfileString(PRG, "m_sProfile", m_sProfile);
	theApp.WriteProfileInt(PRG, "m_bContinue", m_bContinue);
	theApp.WriteProfileInt(PRG, "m_nPeriod", m_nPeriod);
	theApp.WriteProfileInt(PRG, "m_nDuration", m_nDuration);
	theApp.WriteProfileInt(PRG, "m_nPort", m_nPort);
	theApp.WriteProfileInt(PRG, "m_fTolerance", int(m_fTolerance*1000.0));
	theApp.WriteProfileInt(PRG, "m_fTr", int(m_fTr*1000.0));
	theApp.WriteProfileInt(PRG, "m_fTf", int(m_fTf*1000.0));

	return CDialogEx::DestroyWindow();
}

void CTdaemonSimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_HOST, m_sHost);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDX_Text(pDX, IDC_EDIT_PREFIX1, m_sPrefix1);
	DDX_Text(pDX, IDC_EDIT_PREFIX2, m_sPrefix2);
	DDX_Text(pDX, IDC_EDIT_PROFILE, m_sProfile);
	DDX_Text(pDX, IDC_EDIT_PERIOD, m_nPeriod);
	DDX_Text(pDX, IDC_EDIT_TR, m_fTr);
	DDX_Text(pDX, IDC_EDIT_TF, m_fTf);
	DDX_Check(pDX, IDC_CHECK_CONT, m_bContinue);
	DDX_Check(pDX, IDC_CHECK_GSTECH, m_bGSTECH);
	DDX_Text(pDX, IDC_EDIT_TOLERANCE, m_fTolerance);
	DDX_Text(pDX, IDC_EDIT_DURATION, m_nDuration);
}

BEGIN_MESSAGE_MAP(CTdaemonSimDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CTdaemonSimDlg::OnBnClickedButtonStart)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CTdaemonSimDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CTdaemonSimDlg::OnPaint()
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

HCURSOR CTdaemonSimDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTdaemonSimDlg::OnBnClickedButtonStart()
{
	CString s;

	if (m_bStart) {
		KillTimer(13840);
		Sleep(m_nPeriod*1.2);
		TRACE("client stop\n");
		m_cli.stop();
		m_bStart = !m_bStart;
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("Start");

		if (m_bGSTECH) {
			s.Format("%s789.0F\r", m_sPrefix1);	// end signal
			m_cli.send(s);
		}
	}
	else {
		UpdateData(TRUE);
		TRACE("client run\n");
		m_nCurrentStep = 0;
		m_fPV = 25.0;
		m_cli.start(m_sHost, m_nPort);
		m_bStart = !m_bStart;
		GetDlgItem(IDC_BUTTON_START)->SetWindowText("Stop");

		if (m_bGSTECH) {
			s.Format("%s456.0F\r", m_sPrefix1);	// start signal
			m_cli.send(s);
		}

		CString tok;
		int pos = 0;
		m_aStep.clear();
		for (tok = m_sProfile.Tokenize(",", pos); tok != ""; tok = m_sProfile.Tokenize(",", pos)) 
			m_aStep.push_back(atof(tok));

		SetTimer(13840, m_nPeriod, NULL);
	}
}

void CTdaemonSimDlg::process()
{
	double SP = m_aStep[m_nCurrentStep];
	CString s;
	static time_t st;

	if (abs(m_fPV - SP) < 1.0) {		// 온도가 맞음
		if (m_bStable) {
			if (time(NULL) > st + m_nDuration) {	// duration까지 stable 구간 유지
				m_nCurrentStep++;
				if (m_nCurrentStep >= m_aStep.size()) {	
					if (m_bContinue) {				// 처음부터 다시 시작

						if (m_bGSTECH) {
							s.Format("%s789.0F\r", m_sPrefix1);	// end signal
							m_cli.send(s);

							m_fPV = 25.0;
							s.Format("%s456.0F\r", m_sPrefix1);	// start signal
							m_cli.send(s);
						}

						m_nCurrentStep = 0;
						return;
					}
					else {
						OnBnClickedButtonStart();	// click stop
					}
				}
			}
		}
		else {					// stable 구간 시작
			m_bStable = true;
			st = time(NULL);
		}
	}
	else if (m_fPV > SP) {			// 현재 온도가 설정 온도보다 큼. 온도 내림
		m_fPV -= m_fTf;
		m_bStable = false;
	}
	else if (m_fPV < SP) {			// 현재 온도가 설정 온도보다 작음. 온도 올림
		m_fPV += m_fTr;
		m_bStable = false;
	}

	// 온도에 약간의 변화를 줌.
	double tol = (rand() % 100)/ 100.0 * m_fTolerance - (m_fTolerance / 2.0);

	s.Format("%s%05.1fF\r", m_sPrefix1, m_fPV + tol);
	m_cli.send(s);

}

void CTdaemonSimDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent) {
		process();
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CTdaemonSimDlg::OnOK()
{
//	CDialogEx::OnOK();
}
