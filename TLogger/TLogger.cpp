#include "stdafx.h"
#include "TLogger.h"
#include "TLoggerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTLoggerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CTLoggerApp::CTLoggerApp()
{
}

CTLoggerApp theApp;
CConfig config;
HANDLE hMutex;

BOOL CTLoggerApp::InitInstance()
{
	hMutex = CreateMutex(NULL, TRUE, "TLOGGER_MUTEX");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL, "Another instance of " PRG_NAME " is currently running on this system\r\nYou can't run " PRG_NAME " multiple times. Program will quit", "TLogger", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// ShellExecute 실행을 준비
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	SetRegistryKey(APPID);

	CString s = m_pszHelpFilePath;
	int pos = s.ReverseFind('\\');
	s = s.Left(pos);

	if (!config.load(PRG_CONFIG_FILE)) {
		MessageBox(NULL, "cant load " PRG_CONFIG_FILE, NULL, MB_OK | MB_ICONERROR);
		return FALSE;
	}

	/*
	config.m_sExePath = s;
	
	if (!config.load()) {
		s.Format("Can't load '%s' in current directory. Program will be stopped.", PRG_CONFIG_FILE);
		MessageBox(NULL, s, "Configuration Load Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	*/
	CTLoggerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	return FALSE;
}


int CTLoggerApp::ExitInstance()
{
	ReleaseMutex(hMutex);

	return CWinApp::ExitInstance();
}
