#pragma once

class CFontEx
{
public:
	CFontEx()					{ init(); }
	CFontEx(LOGFONT* pLF)		{ memcpy(&lf, pLF, sizeof lf); }
	CFontEx(const char* fstr)	{ init(); parse(fstr); }
	virtual ~CFontEx()			{}

protected:
	LOGFONT		lf;
	COLORREF	color;
	CString		str;

	void init()
	{
		color = RGB(0, 0, 0);
		CFont f;
		f.CreateFont(11, 0, 0, 0, FW_DEMIBOLD, false, false, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
		f.GetLogFont(&lf);
		f.DeleteObject();
	}

public:
	void operator=(LOGFONT& f)			{ memcpy(&lf, &f, sizeof(LOGFONT)); }
	void operator=(const char* fstr)	{ parse(fstr); }
	operator LOGFONT&()					{ return lf; }
	operator LOGFONT*()					{ return &lf; }
	inline LOGFONT* getLogFont()		{ return &lf; }

	void setColor(COLORREF x)			{ color = x; }
	COLORREF getColor()					{ return color; }

	const char* getString()
	{
		CWindowDC dc(AfxGetApp()->GetMainWnd());
		int PPI = GetDeviceCaps(dc, LOGPIXELSY);

		str = "";
		str.AppendFormat("%s; ", lf.lfFaceName);
		str.AppendFormat("%dpt; ", abs(MulDiv(lf.lfHeight, 72, PPI)));
		if (lf.lfItalic)
			str.AppendFormat("italic; ");
		if (lf.lfWeight >= 700)
			str.AppendFormat("bold; ");
		if (lf.lfUnderline)
			str.AppendFormat("underline; ");

		str.AppendFormat("#%02X%02X%02X; ", GetRValue(color), GetGValue(color), GetBValue(color));
		return str;
	}

	LOGFONT* parse(const char* fstr)
	{
		CString token, s = fstr;
		int pos = 0;
		lf.lfWeight = 0;
		lf.lfItalic = 0;
		lf.lfUnderline = 0;

		for (token = s.Tokenize(";", pos); pos != -1; token = s.Tokenize(";", pos)) {
			token = token.Trim();
			if (token.GetLength() <= 0)
				continue;
			if (token.CompareNoCase("bold") == 0) {
				lf.lfWeight = 700;
			}
			else if (token.CompareNoCase("italic") == 0) {
				lf.lfItalic = 1;
			}
			else if (token.CompareNoCase("underline") == 0) {
				lf.lfUnderline = 1;
			}
			else if (token.Right(2).Compare("pt") == 0) {
				CWindowDC dc(AfxGetApp()->GetMainWnd());
				int PPI = GetDeviceCaps(dc, LOGPIXELSY);
				lf.lfHeight = MulDiv(atoi(token), PPI, 72);
			}
			else if (token[0] == '#') {
				int r, g, b;
				sscanf_s(token, "#%02X%02X%02X", &r, &g, &b);
				color = RGB(r, g, b);
			}
			else {
				strcpy_s(lf.lfFaceName, sizeof lf.lfFaceName, token);
			}
		}
		return &lf;
	}
};