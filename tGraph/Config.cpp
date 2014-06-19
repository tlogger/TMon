#include "StdAfx.h"
#include "Config.h"

#define SET_FILE "tgraph.set"

CConfig::CConfig(void)
{
	g_pView = NULL;
	g_pTreeView = NULL;
	g_sFilePath = "D:\\work\\TMon";
	g_sFileName = "";
	g_fMaxTemp = 100;
	g_fMinTemp = -20;
	g_colGraph = RGB(255,200,100);

	loadConfig();
}


CConfig::~CConfig(void)
{
	saveConfig();
}

void CConfig::loadConfig()
{
	CString fname = SET_FILE;
	FILE *f;
	fopen_s(&f, fname, "r");	
	if (f== NULL)return;

	char buf[1024];
	fgets(buf, 1024, f);
	CString s, token;
	s.Format("%s", buf);
	int pos = 0;
	int i = 0;

	for (token = s.Tokenize(",", pos); pos != -1; token = s.Tokenize(",", pos), i++) {
		if (i == 0){
			g_sFilePath = token;
			if (g_sFilePath == "")
				g_sFilePath = "d:";
		}
		else if( i == 1)
			g_fMaxTemp = atoi(token);
		else if( i == 2)
			g_fMinTemp = atoi(token);
		else if( i == 3)
			g_colGraph = rgbtostr(token);
	}
	fclose(f);

}

void CConfig::saveConfig()
{
	CString fname = SET_FILE;
	FILE *f;
	fopen_s(&f, fname, "w");
	if (f == NULL)return;

	CString str;
	str.Format("%s,%.0f,%.0f,%s", g_sFilePath, g_fMaxTemp, g_fMinTemp, strtoRgb(g_colGraph));
	fputs(str, f);
	fclose(f);

}

COLORREF CConfig::rgbtostr(CString str)
{
	if (str.Find(';') == -1)
		return RGB(255,255,255);

	int pos = 0;
	int r1, r2, r3;
	r1 = atoi(str.Tokenize(";", pos));
	r2 = atoi(str.Tokenize(";", pos));
	r3 = atoi(str.Tokenize(";", pos));
	return RGB(r1, r2, r3);
}

CString CConfig::strtoRgb(COLORREF rgb)
{
	int r = rgb >> 16 & 0xff;
	int g = rgb >> 8 & 0xff;
	int b = rgb & 0xff;
	CString str;
	str.Format("%d;%d;%d", b, g, r);
	return str;
}