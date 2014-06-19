#pragma once

#include "tGraphView.h"
#include "PaneTreeView.h"
class CConfig
{
public:
	CConfig(void);
	~CConfig(void);
	
	CString g_sFilePath;
	CString g_sFileName;

	CtGraphView* g_pView;
	CPaneTreeView* g_pTreeView;
	
	void loadConfig();
	void saveConfig();

	float g_fMaxTemp;
	float g_fMinTemp;

	COLORREF		g_colGraph;

	COLORREF rgbtostr(CString str);
	CString strtoRgb(COLORREF rgb);
};

