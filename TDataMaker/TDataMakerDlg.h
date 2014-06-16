#pragma once

#include <vector>
#include "afxcmn.h"

using namespace std;

#pragma pack(push, 1)

typedef struct LogFileRecordStruct 
{
	DWORD		t;				// win32 time_t는 8byte. 이중 4byte만 사용
	short		temp;			// 온도는 /100한 값

	LogFileRecordStruct() {  t = (DWORD)time(NULL); temp = 0; }
	LogFileRecordStruct(time_t tt, double x) { t = (DWORD)tt; temp = short(x * 100.0); }

} logfile_rec_t;

#pragma pack(pop, 1)

class CTDataMakerDlg : public CDialogEx
{
public:
	CTDataMakerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	enum { IDD = IDD_TDAEMONSIM_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
	virtual void OnOK();

	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButtonStart();

	DECLARE_MESSAGE_MAP()

public:

	CString		m_sDir;
	CString		m_sProfile;
	double		m_fTolerance;
	int			m_nDuration;
	double		m_fTr;
	double		m_fTf;
	int			m_nSamples;
	BOOL		m_bCSV;
	CProgressCtrl m_cProgress;

	double		m_fPV;
	size_t		m_nCurrentStep;
	vector<double> m_aStep;
	bool		m_bStart;
	bool		m_bStable;

	HANDLE		m_hThread;
	CFile		m_fileBIN;
	CStdioFile	m_fileCSV;
	CString		m_fnBIN;
	CString		m_fnCSV;

	void		run();
	bool		writeRec(DWORD st);

	static DWORD WINAPI threadProc(void* LPARAM) { 
		((CTDataMakerDlg*)LPARAM)->run(); 
		return 1; 
	}
};
