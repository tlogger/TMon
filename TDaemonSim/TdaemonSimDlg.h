#pragma once

#include "UDPClient.h"
#include <vector>

using namespace std;

class CTdaemonSimDlg : public CDialogEx
{
public:
	CTdaemonSimDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	enum { IDD = IDD_TDAEMONSIM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButtonStart();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
public:

	bool	m_bStart;
	bool	m_bStable;
	CString m_sHost;
	int		m_nPort;

	CString m_sPrefix1;
	CString m_sPrefix2;
	CString m_sProfile;
	BOOL	m_bContinue;
	BOOL	m_bGSTECH;
	int		m_nPeriod;
	double	m_fTolerance;
	int		m_nDuration;
	double	m_fTr;
	double	m_fTf;
	double	m_fPV;
	size_t	m_nCurrentStep;
	vector<double> m_aStep;

	CUDPClient	m_cli;

	void	process();
	virtual void OnOK();
};
