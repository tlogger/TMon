// TGraph.h : main header file for the TGraph application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CTGraphApp:
// See TGraph.cpp for the implementation of this class
//

class CTGraphApp : public CWinApp
{
public:
	CTGraphApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CTGraphApp theApp;