// tGraph.h : main header file for the tGraph application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CtGraphApp:
// See tGraph.cpp for the implementation of this class
//

class CtGraphApp : public CWinApp
{
public:
	CtGraphApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CtGraphApp theApp;