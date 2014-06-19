#include "stdafx.h"
#include "LogGraph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


extern CConfig* config;

BEGIN_MESSAGE_MAP(CDialogRange, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
END_MESSAGE_MAP()

bool CDialogRange::getValue(CWnd* pParent, double& fLower, double& fUpper)
{
	int l = 0;
	int t = 0;
	int w = 300;
	int h = 140;
	int ch = 21;		// control's height
	int bw = 80;		// button width

	CString s = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, LoadCursor(NULL, IDC_ARROW), CBrush(::GetSysColor(COLOR_BTNFACE)));
	if (!CreateEx(WS_EX_DLGMODALFRAME, s, "Set Graph's Upper && Lower limit", WS_SYSMENU | WS_POPUP | WS_BORDER | WS_CAPTION, l, t, w, h, pParent->GetSafeHwnd(), NULL)) 
		return 0;

	CenterWindow();

	DWORD dwStaticStyles = WS_VISIBLE | WS_CHILD | SS_NOPREFIX | SS_RIGHT;
	DWORD dwEditStyles = WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL;
	DWORD dwButtonStyles = WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON;

	l = 6;
	t = 6;
	if (!m_cLowerLegend.Create("Lower Limit", dwStaticStyles, CRect(l,t,l+100,t+ch), this)) 
		return false;

	l += 120;
	s.Format("%.2f", fLower);
	if (!m_cLower.CreateEx(WS_EX_CLIENTEDGE, _T("EDIT"), s, dwEditStyles, CRect(l,t,l+100,t+ch), this, 1))
		return false;

	l = 6;
	t += ch + 10;
	if (!m_cUpperLegend.Create("Upper Limit", dwStaticStyles, CRect(l,t,l+100,t+ch), this)) 
		return false;

	l += 120;
	s.Format("%.2f", fUpper);
	if (!m_cUpper.CreateEx(WS_EX_CLIENTEDGE, _T("EDIT"), s, dwEditStyles, CRect(l,t,l+100,t+ch), this, 2))
		return false;

	l = w / 2 - (bw + 5);
	t += ch + 10;
	if (!m_cOk.Create(_T("OK"), dwButtonStyles, CRect(l, t, l+bw, t+ch), this, IDOK))
		return false;

	l = w / 2 + 5;
	if (!m_cCancel.Create(_T("Cancel"), dwButtonStyles, CRect(l, t, l+bw, t+ch), this, IDCANCEL))
		return false;

	CFont m_Font;
	m_Font.CreatePointFont(80, "MS Sans Serif");

	m_cLowerLegend.SetFont(&m_Font, false);
	m_cUpperLegend.SetFont(&m_Font, false);
	m_cLower.SetFont(&m_Font, false);
	m_cUpper.SetFont(&m_Font, false);
	m_cOk.SetFont(&m_Font, false);
	m_cCancel.SetFont(&m_Font, false);

	ShowWindow(SW_SHOW);

	pParent->EnableWindow(false);
	EnableWindow(true);

	// enter modal loop
	DWORD dwFlags = MLF_SHOWONIDLE;
	if (GetStyle() & DS_NOIDLEMSG)
		dwFlags |= MLF_NOIDLEMSG;

	m_cLower.SetFocus();

	int nRet = RunModalLoop(MLF_NOIDLEMSG);
	if (nRet == IDOK) {
		m_cLower.GetWindowText(s);
		fLower = atof(s);
		m_cUpper.GetWindowText(s);
		fUpper = atof(s);
	}

	if (m_hWnd != NULL)
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);

	pParent->SetFocus();
	if (GetParent() != NULL && ::GetActiveWindow() == m_hWnd)
		::SetActiveWindow(pParent->m_hWnd);

	if (::IsWindow(m_hWnd))
		DestroyWindow();

	pParent->EnableWindow(true);

	return true;
}

BOOL CLogGraph::m_bRegistered = CLogGraph::registerClass(); 


BEGIN_MESSAGE_MAP(CLogGraph, CWnd)
	//{{AFX_MSG_MAP(CLogGraph)
	ON_WM_PAINT()
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CLogGraph::CLogGraph()
{
	m_bCreated = false;
//	m_hQue  = 0;
	m_bNav = false;
	m_tLastNav = time(NULL);
	m_nScaleIndex = -1;
	m_nLastCursorPos = -1;
	m_aTmp = NULL;

	m_bDragSel = false;
	m_bLdown = false;

	m_tStart = time(NULL);
	m_tEnd = time(NULL);
	
	m_nHeaderHeight = 40;
	m_nScaleWidth  = 34;
	m_bAutoZoom = false;

	setZoom(1.0);
	setGrid(20, RGB(70, 70, 70));
	setHeader("Segoe UI", 13, RGB(200, 200, 200), RGB(120, 120, 120));
	setScale("Segoe UI", 13, RGB(200, 200, 0), RGB(100, 100, 100));
	setGraph("Segoe UI", 13, RGB(200, 200, 200), RGB(0, 0, 0));
	setScaleIndex(-1, -1);

	clearGraph();

	m_bLoad = false;
//	uMSH_MOUSEWHEEL = RegisterWindowMessage("MSWHEEL_ROLLMSG");
}

void CLogGraph::clearGraph()
{
	for (int i = 0; i < (int)m_aChannels.size(); i++)
		delete m_aChannels[i];
	m_aChannels.clear();
}

CLogGraph::~CLogGraph()
{
	if (m_aTmp)
		delete m_aTmp;

	clearGraph();

	m_fontHeader.DeleteObject();
	m_fontScale.DeleteObject();
	m_fontGraph.DeleteObject();
}

BOOL CLogGraph::registerClass()
{
	// Static function to automatically register this class 

	WNDCLASS wc;
	
	ZeroMemory(&wc, sizeof(WNDCLASS));
    wc.lpfnWndProc   = ::DefWindowProc;
    wc.hInstance     = (HINSTANCE)::GetModuleHandle(NULL);        
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);                
    wc.lpszClassName = _T("LogGraph");	
    return AfxRegisterClass(&wc);
}

void CLogGraph::calcRect()
{
	m_rectHeader = m_rectWnd;
	m_rectHeader.bottom = m_rectHeader.top + m_nHeaderHeight;

	CRect r = m_rectWnd;
	r.top = r.top + m_nHeaderHeight;

	m_rectScale = r;
	if (m_nScaleIndex == -1)
		m_rectScale.right = m_rectScale.left;
	else 
		m_rectScale.right = m_rectScale.left + m_nScaleWidth;
	
	m_rectScale2 = r;
	if (m_nScaleIndex2 == -1) 
		m_rectScale2.left = m_rectScale2.right;
	else
		m_rectScale2.left = m_rectScale2.right - m_nScaleWidth;
	
	m_rectGraph = r;
	m_rectGraph.left = m_rectScale.right;
	m_rectGraph.right = m_rectScale2.left;

	if (m_aTmp != NULL) {
		delete m_aTmp;
		m_aTmp = NULL;
	}
	// TODO MAX_AD 변경
#define MAX_AD 128

	int sz = (m_rectGraph.Width() + 1 ) * MAX_AD * sizeof(short) + 8;
	if (sz < 0 )
		return;
	
	m_aTmp = new short[sz];
}

// TODO view에 attach 가능하도록

bool CLogGraph::attach(UINT id, CWnd* pParent)
{
	CStatic  wndStatic;		
	CRect    r;

	if (pParent == NULL || !wndStatic.SubclassDlgItem(id, pParent))
		return false;

	wndStatic.GetWindowRect(&r);
	pParent->ScreenToClient(&r);
		
	if ( !CreateEx( wndStatic.GetExStyle(), 
		            NULL, NULL, WS_CHILD | WS_VISIBLE | wndStatic.GetStyle(),
		            r, pParent, id, NULL))
	{		
		wndStatic.DestroyWindow();
		return false;
	}
	
	wndStatic.DestroyWindow();

	EnableScrollBarCtrl(SB_HORZ, TRUE);
	GetClientRect(&m_rectWnd);
	calcRect();
	SetTimer(11839, 1000, NULL);

	m_bCreated = true;
	return true;
}

bool CLogGraph::create(UINT id, CWnd* pParent)
{
	Create("LogGraph", "LogGraph", WS_CHILD | WS_VISIBLE ,
		CRect(0,0,0,0) , pParent, 0);
// 	if ( !CreateEx(0, NULL, NULL, WS_CHILD | WS_VISIBLE ,
// 		CRect(0,0,0,0) , pParent, id, NULL))
// 	{		
// 		
// 		return false;
// 	}
	m_nID = id;
	EnableScrollBarCtrl(SB_HORZ, TRUE);
	GetClientRect(&m_rectWnd);
	calcRect();
	SetTimer(11839, 1000, NULL);

	m_ctrTooltip.Create(this, TTS_ALWAYSTIP|TTS_NOPREFIX);
	m_ctrTooltip.SetDelayTime(50);
	m_ctrTooltip.Activate(TRUE);


	m_bCreated = true;
	return true;
}

void CLogGraph::setHeader(const char* font, int sz, COLORREF col, COLORREF back)
{ 
	m_fontHeader.DeleteObject();
	m_fontHeader.CreateFont(sz, 0, 0, 0, FW_NORMAL, false, false, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, font);
	m_crHeader = col; 
	m_crHeaderBack = back;
}

void CLogGraph::setScale(const char* font, int sz, COLORREF col, COLORREF back)
{ 
	m_fontScale.DeleteObject();
	m_fontScale.CreateFont(sz, 0, 0, 0, FW_NORMAL, false, false, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, font);
	m_crScale = col; 
	m_crScaleBack = back;
}

void CLogGraph::setGraph(const char* font, int sz, COLORREF col, COLORREF back)
{ 
	m_fontGraph.DeleteObject();
	m_fontGraph.CreateFont(sz, 0, 0, 0, FW_NORMAL, false, false, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, font);
	m_crGraph = col; 
	m_crBack = back;
}

void CLogGraph::setZoomFit()
{
	double l = getLast();//lib->mque_reclast(m_hQue);
	double fz = l / m_rectGraph.Width();
	m_nCurrentPos = 0;

//	TRACE("%s : cpos %d\n", __FUNCTION__, m_nCurrentPos);
	if (fz < 1.0) fz = 1.0;
	setZoom(fz); 
}

int CLogGraph::getWndQueLastPos()
{
	int en = m_nCurrentPos + getWndQueLength();
//	if (en > lib->mque_reclast(m_hQue))
//		en = lib->mque_reclast(m_hQue);
	if (en > getLast())
		en = getLast();
	return en;
}

void CLogGraph::goLast()
{
	int en = getWndQueLastPos();
// 	if (lib->mque_reclast(m_hQue) > en)
// 		en = lib->mque_reclast(m_hQue);
	if (getLast() > en)
		en = getLast();
	m_nCurrentPos = en - getWndQueLength();
	
 	if (m_nCurrentPos < getLast())
 		m_nCurrentPos = getLast();

	TRACE("goLast: current=%d, last=%d\n", m_nCurrentPos, en);
	setScrollBar();
}

bool CLogGraph::openGraph(const char* fn, int len, int maxLog, bool bCreate)
{
	//m_hQue = lib->mque_create(fn, len, maxLog, bCreate);
	bool ret = setFile();
	if (ret) {
		// 처음 오픈했을 때는 m_nCurrentPos를 마지막으로 이동한다.
		goLast();
	}
	if (m_aTmp) 
		calcRect();

	return ret;
}

void CLogGraph::setZoom(double zf)
{
	m_fZoomFactor = zf;

	// zoom factor가 바뀌면 m_nCurrentPos 다시 계산
	
	int mqlast = getLast();
	if (mqlast < getWndQueLastPos()) 
		m_nCurrentPos = mqlast - getWndQueLength();
	if (m_nCurrentPos < 0)
		m_nCurrentPos = 0;
//	TRACE("%s : cpos %d\n", __FUNCTION__, m_nCurrentPos);
	m_bDragSel = false;
	setScrollBar();
}

void CLogGraph::setScrollBar()
{
	// scrollbar setup

	SCROLLINFO info;
	
	info.cbSize = sizeof(SCROLLINFO);
	info.nTrackPos = 0;

	info.fMask = SIF_ALL;
	info.nMin = 0;
	info.nMax = getLast();
	if (!m_bLoad)
		info.nPage = 0;
	else
		info.nPage = getWndQueLength();
	
	info.nPos = m_nCurrentPos;
	//TRACE("%s : cpos %d\n", __FUNCTION__, m_nCurrentPos);
	SetScrollInfo(SB_HORZ, &info);
	if (m_bCreated)
		ShowScrollBar(SB_HORZ, (info.nMax > (int)info.nPage));
	
}

void CLogGraph::showOverUnder(int idx, bool bShow)
{
	if (idx >= 0 && idx < (int)m_aChannels.size()) 
		m_aChannels[idx]->showOU = bShow;
}

bool CLogGraph::add(const char* name, double low, double high, double over, double under, double scale, int nPos, const char* fmt, bool bShow, COLORREF color)
{
//	if (!m_hQue)
//		return false;
	Channel* pCh = new Channel;

	pCh->showOU = false;
	pCh->low = low;
	pCh->high = high;
	pCh->over = over;
	pCh->under = under;
	pCh->name = name;
	pCh->fmt = fmt;
	pCh->scale = scale;
	pCh->pen.CreatePen(PS_SOLID, 1, color);	
	pCh->C = 0.0;
	pCh->show  = bShow;
	pCh->pos = nPos;
	pCh->col = color;
	m_aChannels.push_back(pCh);
	return true;
}

void CLogGraph::changePen(COLORREF rgb)
{
	Channel* pCh = m_aChannels[m_nScaleIndex];
	pCh->pen.DeleteObject();
	pCh->pen.CreatePen(PS_SOLID, 1, rgb);
	pCh->col = rgb;
	
}

void CLogGraph::removeAll()
{
	for (int i = 0; i < (int)m_aChannels.size(); i++)
		delete m_aChannels[i];
	m_aChannels.clear();
}

int CLogGraph::getMaxChannel()
{
	int n = m_aChannels.size();
//	if (lib->mque_reclen(m_hQue) < n)
//		n = lib->mque_reclen(m_hQue);
	return n;
}

void CLogGraph::clearStat()
{
	int n = getMaxChannel();
	for (int i = 0; i < n; i++) {
		Channel* pCh = m_aChannels[i];
		pCh->min = pCh->high;
		pCh->max = pCh->low;
		pCh->avg = (pCh->high - pCh->low) / 2.0;	// TODO 계산 방법 다시 
	}
}

void CLogGraph::calcStat(short* x, bool stat)
{
	int n = getMaxChannel();
	for (int i = 0; i < n; i++) {
		Channel* pCh = m_aChannels[i];
		pCh->v = double(x[i]) / pCh->scale;
		if (stat) {
			pCh->min = min(pCh->min, pCh->v);
			pCh->max = max(pCh->max, pCh->v);
			pCh->avg = (pCh->avg + pCh->v) / 2.0;
		}
	}
}

bool CLogGraph::push(int t, short* x)
{
//	if (!m_hQue)
//		return false;

//	int len = m_aChannels.size();

//	bool ret = lib->mque_putn(m_hQue, t, x) != 0;
//	calcStat(x, true);
//	if (m_bAutoZoom)
//		setZoomFit();

// 	if (!m_bNav) {
// 		//redraw();
// 		sendMsg(x);
// 	}
	// TODO file 기록 (flush)
//	return ret;
	return true;
}

bool CLogGraph::push(int t, int* x)
{
	int n = getMaxChannel();
	for (int i = 0; i < n; i++)
		m_aTmp[i] = (short)x[i];
	return push(t, m_aTmp);
}

bool CLogGraph::push(int t, vector<int> x)
{
	int n = getMaxChannel();
	if (n > (int)x.size())
		n = x.size();
	for (int i = 0; i < n; i++)
		m_aTmp[i] = (short)x[i];
	return push(t, m_aTmp);
}

void CLogGraph::setNavMode2(bool bNav)
{
	m_bNav = bNav;
	m_tLastNav = time(NULL);
}

void CLogGraph::setNavMode(bool bNav)
{
	m_bNav = bNav;
	if (m_bNav) {
		sendMsg(LG_NAV_START);
		m_tLastNav = time(NULL);
	}
	else {
		sendMsg(LG_NAV_END);
	}
}

void CLogGraph::sendMsg(int kind, int pos)
{
	if (!m_hWnd)
		return;

	::PostMessage(m_hWnd, kind, pos, m_nID);
}

int CLogGraph::find(const char* name)
{
	int len = m_aChannels.size();
	for (int i = 0; i < len; i++) {
		if (m_aChannels[i]->name.CompareNoCase(name) == 0)
			return i;
	}
	return -1;
}

void CLogGraph::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask = SIF_ALL;
	GetScrollInfo(SB_HORZ, &info);

	int minpos, maxpos;
	GetScrollRange(SB_HORZ, &minpos, &maxpos);
	maxpos = GetScrollLimit(SB_HORZ);
	int curpos = GetScrollPos(SB_HORZ);
//	TRACE("%s : cpos %d npos %x max %d, nsb %d\n", __FUNCTION__, curpos, nPos, maxpos, nSBCode);
	switch(nSBCode) {
		case SB_LEFT: curpos = minpos; break;
		case SB_RIGHT: curpos = maxpos; break;
		case SB_ENDSCROLL: break;
		case SB_LINELEFT: 
// 			if (curpos - 100 > minpos) 
// 				curpos -= 100; 
// 			else
// 				curpos = 0;
			if (curpos > minpos)
				curpos = max(minpos, curpos - (int)info.nPage);
			break;
		case SB_LINERIGHT: 
//			if (curpos + 100 < maxpos) 
//				curpos += 100; 
			if (curpos < maxpos)
				curpos = min(maxpos, curpos + (int)info.nPage);
			break;
		case SB_PAGELEFT:
			{
//				SCROLLINFO info;
//				GetScrollInfo(SB_HORZ, &info, SIF_ALL);
				if (curpos > minpos)
					curpos = max(minpos, curpos - (int)info.nPage);
			}
			break;
		case SB_PAGERIGHT:
			{
	//			SCROLLINFO info;
	//			GetScrollInfo(SB_HORZ, &info, SIF_ALL);
				if (curpos < maxpos)
					curpos = min(maxpos, curpos + (int)info.nPage);
			}
			break;
		case SB_THUMBPOSITION: curpos = info.nTrackPos; break;
		case SB_THUMBTRACK: curpos = info.nTrackPos; break;
	}
	m_bDragSel = false;
	if (curpos < 0)
		curpos = 0;
	SetScrollPos(SB_HORZ, curpos);
	m_nCurrentPos = curpos;
//	TRACE("cpos %d \n", curpos);
	redraw();

	setNavMode(true);
	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}



void CLogGraph::OnMouseMove(UINT nFlags, CPoint point)
{
	if (hitRect(point, m_rectGraph) && m_bNav) {
		CDC* pDC = GetDC();
		if (pDC) {
			int oldRop = pDC->GetROP2();
			pDC->SetROP2(R2_XORPEN);

			CPen pen(PS_DOT, 1, RGB(255, 255, 255));
			CPen* pOldPen = pDC->SelectObject(&pen);

			if (m_nLastCursorPos != -1) {
				pDC->MoveTo(m_rectGraph.left + m_nLastCursorPos, m_rectGraph.top);
				pDC->LineTo(m_rectGraph.left + m_nLastCursorPos, m_rectGraph.bottom);
			}

			m_nLastCursorPos = (point.x - m_rectGraph.left);
			pDC->MoveTo(m_rectGraph.left + m_nLastCursorPos, m_rectGraph.top);
			pDC->LineTo(m_rectGraph.left + m_nLastCursorPos, m_rectGraph.bottom);
			pDC->SelectObject(pOldPen);
			pDC->SetROP2(oldRop);

			if (m_bNav) {
				int pos = m_nCurrentPos + int(m_fZoomFactor * m_nLastCursorPos);
//				lib->mque_getn(m_hQue, pos, m_aTmp);
//				calcStat(m_aTmp, false);
				sendMsg(LG_NAV_MSG, pos);
				m_tLastNav = time(NULL);
			}
			ReleaseDC(pDC);
		}
	}

	if (m_bLdown)
	{
		m_bDragSel = true;
		m_ptEtMouse = point;
		redraw();
	}

	if (m_bLoad){
		tdata *tp = getTptr(m_nCurrentPos); //getPtrLeftPos();

		int x = point.x - m_rectGraph.left;	
		if (x < 0) goto NOMATCH;
		int last = getLength(); 
		int pos = m_nCurrentPos + point.x * (int)m_fZoomFactor - m_rectGraph.left;
		if (pos > last){
			goto NOMATCH;		
		}

		tp = tp+ x*((int)m_fZoomFactor);


		Channel* pCh = m_aChannels[0];
		int ey = m_rectGraph.top + m_rectGraph.Height();
		int ry = ey - int(pCh->C * (int(tp->temp) / pCh->scale - pCh->low));

	//	TRACE("%s %d\n", __FUNCTION__ , tp->temp);
	
		CString strTemp;
		CString stime = tformat("%Y-%m-%d %H:%M:%S", tp->time);
		strTemp.Format("%s\t  %d℃",stime, tp->temp/100 );
		//        strTemp.Format("%5ld",lUnitSize);
		m_ctrTooltip.Activate(TRUE);
		m_ctrTooltip.AddTool(this,strTemp, CRect(point.x-1,point.y-1,point.x+1,point.y+1), TRUE);    // skin[i].rc    각 버튼들의 Rect...
		m_ctrTooltip.Update();
	}
NOMATCH:	
//	TRACE("%s : cpos %d\n", __FUNCTION__, m_nCurrentPos);
	CWnd::OnMouseMove(nFlags, point);
}

bool CLogGraph::hitRect(CPoint pt, CRect r)
{
	return (pt.x > r.left && pt.x < r.right && pt.y > r.top && pt.y < r.bottom);
}

void CLogGraph::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMenu menu;

	
	
	if (hitRect(point, m_rectHome)) {
		m_nCurrentPos = 0; 
		redraw();
		setNavMode(true);
	}
	else if (hitRect(point, m_rectPgUp)) {
		if (m_nCurrentPos - getWndQueLength() < 0) 
			m_nCurrentPos = 0;
		else 
			m_nCurrentPos -= getWndQueLength();

		redraw();
		setNavMode(true);
	}
	else if (hitRect(point, m_rectPgDn)) {
		int mqlast = getLast();
		if (m_nCurrentPos + getWndQueLength() >= mqlast) {
			m_nCurrentPos = mqlast - getWndQueLength();
			if (m_nCurrentPos < 0)
				m_nCurrentPos = 0;
		}
		else
			m_nCurrentPos += getWndQueLength();
		redraw();
		setNavMode(true);
	}
	else if (hitRect(point, m_rectEnd)) {
		m_nCurrentPos = getLast() - getWndQueLength();
		if (m_nCurrentPos < 0)
			m_nCurrentPos = 0;
		redraw();
		setNavMode(false);
		sendMsg(LG_NAV_END);
	}
	else if (hitRect(point, m_rectZoom)) {
		if (!menu.CreatePopupMenu())
			return;
		menu.AppendMenu(MF_STRING, 1, "Zoom Fit");
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, 2, "Zoom 1:1");
		menu.AppendMenu(MF_STRING, 3, "Zoom 2:1");
		menu.AppendMenu(MF_STRING, 4, "Zoom 4:1");
		menu.AppendMenu(MF_STRING, 5, "Zoom 10:1");
		menu.AppendMenu(MF_STRING, 6, "Zoom 60:1");
		menu.AppendMenu(MF_STRING, 7, "Zoom 3600:1");
		ClientToScreen(&point);
		int n = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, point.x, point.y, this);

		menu.DestroyMenu();
		switch (n) {
			case 2: setZoom(1.0); redraw(); break;
			case 3: setZoom(2.0); redraw(); break;
			case 4: setZoom(4.0); redraw(); break;
			case 5: setZoom(10.0); redraw(); break;
			case 6: setZoom(60.0); redraw(); break;
			case 7: setZoom(3600.0); redraw(); break;
			case 1: setZoomFit(); redraw(); break;
				break;
		}
	//	if (!m_bNav)
	//		goLast();
//		setNavMode();
	}
	else if (hitRect(point, m_rectHeader)) {
		TRACE("header click\n");
	}
	else if (hitRect(point, m_rectScale)) {
		TRACE("scale click\n");
	}else {
		m_bLdown = true;
		m_ptStMouse = point;
	}

//	TRACE("%s : cpos %d\n", __FUNCTION__, m_nCurrentPos);
	CWnd::OnLButtonDown(nFlags, point);
}

void CLogGraph::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLdown = false;
	if (m_ptStMouse == point){
		m_bDragSel = false;
		redraw();
	}
	



	CWnd::OnLButtonUp(nFlags, point);
}

void CLogGraph::OnRButtonDown(UINT nCode, CPoint point)
{
	if (hitRect(point, m_rectGraph)) {
	
//		Channel* pCh;
		CMenu menu;
	
		if (!menu.CreatePopupMenu())
			return;
		
		/*
		menu.AppendMenu(MF_STRING, 3, "Mark Begin");
		menu.AppendMenu(MF_STRING, 4, "Mark End");
		menu.AppendMenu(MF_STRING, 5, "Mark Clear");
		*/
		menu.AppendMenu(MF_STRING, 3, "Lower/Upper Limit");
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, 4, "Copy Graph");
		
// 		for (int i = 0; i < (int)m_aChannels.size(); i++) {
// 			pCh = m_aChannels[i];
// 			if (!pCh->show) 
// 				menu.AppendMenu(MF_STRING | MF_UNCHECKED, 100+i, pCh->name);
// 			else
// 				menu.AppendMenu(MF_STRING | MF_CHECKED, 100+i, pCh->name);
// 		}

		ClientToScreen(&point);
		int n = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, point.x, point.y, this);

		switch (n) {
			case 3:
				{
					if (m_nScaleIndex >= 0 && m_nScaleIndex < (int)m_aChannels.size()) {
						CDialogRange dlg;
						dlg.getValue(this, m_aChannels[m_nScaleIndex]->high, m_aChannels[m_nScaleIndex]->low);
						for (int i = 0; i<(int)m_aChannels.size(); i++){
							m_aChannels[i]->high = m_aChannels[m_nScaleIndex]->high;
							m_aChannels[i]->low = m_aChannels[m_nScaleIndex]->low;
						}

						if (m_nID == 1) //volt graph
						{
							config->g_fMaxTemp =  m_aChannels[m_nScaleIndex]->high;
							config->g_fMinTemp =  m_aChannels[m_nScaleIndex]->low;
							redraw();
						}
//						config->saveConfig();
					}
				}
				break;
			case 4:
				CopyToClipboard();
				break;
			default:
						
				{
					UINT state = menu.GetMenuState(n, MF_BYCOMMAND);
//					TRACE("%d -> %04x [%04x]\n", n-100, state, MF_CHECKED);
					if (state & MF_CHECKED) 
					{
						hide(n-100);
						::PostMessage(m_hWnd, LG_CH_SHOW, (DWORD)(n-100), 0);
					}else
					{
						show(n-100);
						::PostMessage(m_hWnd, LG_CH_SHOW, (DWORD)(n-100), 1);
					}
					redraw();
				}
			}
		menu.DestroyMenu();
	}
	CWnd::OnRButtonDown(nCode, point);
}

void CLogGraph::OnSizing(UINT fwSide, LPRECT pRect) 
{
	if (GetParent()->GetStyle() & ~WS_CLIPCHILDREN)
	{
		// Eliminate flickering 
		m_bStylesModified = true;
		GetParent()->ModifyStyle(0, WS_CLIPCHILDREN);		
	}

	ModifyStyle(0, WS_CLIPSIBLINGS);

	CWnd::OnSizing(fwSide, pRect);		
	GetClientRect(&m_rectWnd);
	calcRect();
	Invalidate(FALSE);	
}

void CLogGraph::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	GetClientRect(&m_rectWnd);
	calcRect();
	Invalidate(FALSE);
}

void CLogGraph::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 11839) {
		if (m_bNav && time(NULL) > m_tLastNav + NAV_SHOW_TIME) {
//			goLast();
//			redraw();
//			setNavMode(false);
		}
	}
	CWnd::OnTimer(nIDEvent);
}

BOOL CLogGraph::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void CLogGraph::OnPaint() 
{
	CPaintDC paintDc(this);
	CDC& dc = FBufferInit(paintDc);

	dc.IntersectClipRect(&m_rectWnd);		// ensure draw only in client area

	dc.SetWindowOrg(0, 0);
	dc.SetViewportOrg(0, 0);
	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt(1, 1);
	dc.SetViewportExt(1, 1);

//	dc.FillSolidRect(&m_rectGraph, m_crBack);
	XTPPaintManager()->GradientFill(&dc, m_rectGraph, RGB(60,0,0), RGB(0,0,60), FALSE);
	dc.SetBkMode(TRANSPARENT);

	// 그리기 시작 
	if (m_bLoad){
		drawHeader(dc);
		drawGraph(dc);
	}
	FBufferRelease(paintDc);

	if (m_bStylesModified){			// reset style if resized
		GetParent()->ModifyStyle(WS_CLIPCHILDREN, 0);
		ModifyStyle(WS_CLIPSIBLINGS, 0);
		m_bStylesModified = false;
	}
	m_nLastCursorPos = -1;
}

void CLogGraph::drawScale(CDC& dc, int nIndex, CRect rect, int nAlign)
{
	XTPPaintManager()->GradientFill(&dc, rect, m_crScaleBack, RGB(0,0,0), FALSE);

	double high, low, step, v; 
	CString fmt;

	if (nIndex < 0 || nIndex > (int)m_aChannels.size()) {
		high = 100.0;
		low = 0.0;
		fmt = " %.0f%% ";
	}
	else {
		high = m_aChannels[nIndex]->high;
		low = m_aChannels[nIndex]->low;
		fmt = m_aChannels[nIndex]->fmt;
	}
	step = (high - low) / double(m_nGridSize);
	v = high;

	// draw grid & scale text

	CPen* pOldPen, pen;
	CFont* pOldFont;
	pen.CreatePen(PS_SOLID, 1, m_crGrid);
	pOldPen = dc.SelectObject(&pen);
	pOldFont = dc.SelectObject(&m_fontScale);
	dc.SetTextColor(m_crScale);

	double h = (double)m_rectGraph.Height() / (double)m_nGridSize;
	double y = m_rectGraph.top; 
	for (int i = 0; i < m_nGridSize; i++) {
		dc.MoveTo(m_rectGraph.left, (int)y);
		dc.LineTo(m_rectGraph.right, (int)y);

		CRect r = rect;
		r.top = int(y);
		r.bottom = int(y + 12);
		CString s;
		s.Format(fmt, v);
		dc.DrawText(s, &r, nAlign);
		y += h;
		v -= step;
	}
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldFont);
	pen.DeleteObject();
}

char* CLogGraph::tformat(const char* fmt, time_t x)
{
	time_t zz = (time_t)x;
	struct tm *tp = localtime(&zz);
	static char dbuf[50];

	if (fmt[0] == '@')
		strftime(dbuf, sizeof(dbuf), "%Y%m%dT%H%M%SZ", tp);
	else if (fmt[0] == '#')
		strftime(dbuf, sizeof(dbuf), "%Y-%m-%d %H:%M:%S", tp);
	else
		strftime(dbuf, sizeof(dbuf), fmt, tp);
	return dbuf;
}

void CLogGraph::drawHeader(CDC& dc)
{
	XTPPaintManager()->GradientFill(&dc, m_rectHeader,m_crHeaderBack, RGB(0,0,0), FALSE);

	CPen* pOldPen, pen;
	CFont* pOldFont;
	CString fmt, s;
	int i;

	if (m_nScaleIndex != -1)
		drawScale(dc, m_nScaleIndex, m_rectScale, DT_RIGHT);

	if (m_nScaleIndex2 != -1)
		drawScale(dc, m_nScaleIndex2, m_rectScale2, DT_LEFT);

	// draw Header (로그 시작점, 로그 끝점 표시)

	CString tmp;

	pOldFont = dc.SelectObject(&m_fontHeader);
	dc.SetTextColor(m_crHeader);

	if (!m_bLoad)return;
	//short* a = new short [lib->mque_reclen(m_hQue)]; //////////////
	tdata* tp = getTptr(m_nCurrentPos);
	// read date
//	lib->mque_getn(m_hQue, m_nCurrentPos+1, a);
	m_tStart = tp->time;
	s = tformat("%Y-%m-%d\r\n%H:%M:%S", m_tStart);
	tmp.Format("%s\r\n%d", s, m_nCurrentPos);
	dc.DrawText(tmp, &m_rectHeader, DT_LEFT);

//	lib->mque_getn(m_hQue, getWndQueLastPos()-1, a);
	tp = getTptr(getWndQueLastPos()-1);
//	m_tEnd = lib->mque_time(m_hQue);
//	s = tformat("%Y-%m-%d\r\n%H:%M:%S", (int)m_tEnd);
	tmp.Format("%s\r\n%d", s, getWndQueLastPos());
	dc.DrawText(tmp, &m_rectHeader, DT_RIGHT);
//	delete a;

	// draw Header (nav button, zoom)
	
	int nav[4][20] = {		// 20x20 짜리 nav button
		{ 1,1,  4,1,  4,8,  18,1,  18,19,  4,12,  4,19,  1,19,  1,1,  0,0 },		// home
		{ 1,10,  10,1,  10,8,  19,1,  19,19,  10,12,  10,19,  1,10,  0,0 },			// pgup
		{ 1,1,   10,8,  10,1,  19,10,  10,19,  10,12,   1,19,  1,1,  0,0 },			// pgdn
		{ 1,1,  16,8,  16,1,  19,1,  19,19,  16,19,  16,12,  1,19,  1,1,  0,0 }		// end
	};

	CRect r = m_rectHeader;
	int w = 20;				// button width & height
	int iw = w - 4;		// icon size

	CBrush brush;
	CRgn rgn;
	CPoint pt[20];

	brush.CreateSolidBrush(RGB(255, 100, 100));
	pen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	pOldPen = dc.SelectObject(&pen);
	r.left = r.Width() / 2 - (w * 4) / 2;
	r.right = r.left + w * 4;
	r.bottom = r.top + w;
	int x = r.left;
	for (i = 0; i < 4; i++) {
		CRect rect = r;
		rect.left = x + 1;
		rect.right = x + w - 1;
		rect.top++;
		rect.bottom--;
		switch (i) {
			case 0 : m_rectHome = rect; break;
			case 1 : m_rectPgUp = rect; break;
			case 2 : m_rectPgDn = rect; break;
			case 3 : m_rectEnd = rect; break;
		}

		XTPPaintManager()->GradientFill(&dc, &rect, RGB(100,100,100), RGB(0,0,0), FALSE);
		int j;
		for (j = 0; nav[i][j] !=0; j+=2) {
			pt[j/2].x = int(double(nav[i][j]) * iw / 20.0) + x + 2;
			pt[j/2].y = int(double(nav[i][j+1]) * iw / 20.0) + r.top + 2;
		}
		VERIFY(rgn.CreatePolygonRgn(pt, j/2, WINDING));
		dc.FillRgn(&rgn, &brush);
		rgn.DeleteObject();
		x += w;
	}
	dc.SelectObject(pOldPen);

	r.top = r.top + w;
	r.DeflateRect(2, 2);
	r.bottom = m_rectHeader.bottom;
	m_rectZoom = r;

	XTPPaintManager()->GradientFill(&dc, &r, RGB(100,50,50), RGB(0,0,0), FALSE);
	s.Format("Zoom %.0f:1", m_fZoomFactor);
	dc.DrawText(s, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	dc.SelectObject(pOldFont);
}

void CLogGraph::drawGraph(CDC& dc)
{
	dc.IntersectClipRect(&m_rectGraph);		// ensure draw only in client area

	CPen* pOldPen;
	CFont * pOldFont;
	int n = getMaxChannel();
	int h = m_rectGraph.Height();
	int w = m_rectGraph.Width();
	int x = m_rectGraph.left;
	int ey = m_rectGraph.top + h;
	int mqlast = getLast();
	int reclen = getLength();
	int idx;
	int qpos = 0;
	int dlast;

	tdata* tp;

	for (idx = 0; idx < n; idx++) {
		Channel* pCh = m_aChannels[idx];
		pCh->C = double(h) / (pCh->high - pCh->low);
	}

	int st = m_nCurrentPos;
//	if (!m_bNav) {
//		st = mqlast - getWndQueLength();
//		if (st < 0)
//			st = 0;
//	}
//	lib->mque_gets(m_hQue, st);
	tp = getTptr(st);
//	TRACE("st %d\n", st);
//	TRACE("st=%d, end=%d mqlast=%d\n", st, st + int (m_fZoomFactor * w), mqlast);

	dlast = w;
	
	for (int i = 0; i < w; i++) {
		qpos = st + int(m_fZoomFactor * i);
		if (qpos > mqlast) {
			dlast = i;
			break;
		}
	//	lib->mque_getn(m_hQue, qpos, m_aTmp + i * reclen);
//		tp = getTptr(qpos);
//		TRACE("%d : ti=%04x, te=%04x %d\n", qpos, tp->time, tp->temp , sizeof(tdata));
	}
//	TRACE("dlast %d \n", dlast);
//	tp = getTptr(0);

	if (m_nGraphType == 1) {		// stack graph

		for (idx = 0; idx < n; idx++) {
			Channel* pCh = m_aChannels[idx];
			if (!pCh->show)
				continue;
		
			pOldPen = dc.SelectObject(&pCh->pen);
			x = m_rectGraph.left;
			//short* data = m_aTmp + pCh->pos;
			for (int i = 0; i < dlast ; i++) {
				dc.MoveTo(x, ey - int(pCh->C * (int(tp->temp) / pCh->scale - pCh->low)));
				dc.LineTo(x, ey);
				x++;
				tp++;
				//data += reclen;
			}
			dc.SelectObject(pOldPen);
		}
	}
	else {
		
		for (idx = 0; idx < n; idx++) {
			Channel* pCh = m_aChannels[idx];
			if (!pCh->show)
				continue;
//			dc.SetTextColor(config->g_colGraph);
			dc.SetTextColor(pCh->col);
			pOldFont = dc.SelectObject(&m_fontGraph);
			pOldPen = dc.SelectObject(&pCh->pen);
			x = m_rectGraph.left;
			//short* data = m_aTmp + pCh->pos;
			
			dc.MoveTo(x, ey - int(pCh->C * (int(tp->temp) / pCh->scale - pCh->low)));

			int rx=0;
			int ry=0;
			
			int sx, ex;

			if (m_ptStMouse.x > m_ptEtMouse.x){
				sx = m_ptEtMouse.x;
				ex = m_ptStMouse.x;
			}else{
				sx = m_ptStMouse.x;
				ex = m_ptEtMouse.x;
			}

			for (int i = 0; i < dlast-1; i++) {
				rx =x+1;
				ry = ey - int(pCh->C * (int(tp->temp) / pCh->scale - pCh->low));
				//dc.LineTo(x, ey - int(pCh->C * (int(tp->temp) / pCh->scale - pCh->low)));
			

				if (m_bDragSel &&  x >= sx && x <= ex){
						//dc.AlphaBlend(x, ry-1,rx, ry+1, 
//						&MemDC,0,0,10, 10, bf);
						if (x == sx)m_pDataSt = tp;
						if (x == ex)m_pDataEt = tp;
						dc.MoveTo(x, ey - int(pCh->C * (int(tp->temp) / pCh->scale - pCh->low)));
						dc.LineTo(x, ey);
					
				}else 				
					dc.LineTo(x, ey - int(pCh->C * (int(tp->temp) / pCh->scale - pCh->low)));

				x++;
				
				tp = tp+(1 * (int)m_fZoomFactor);
				//tp++;
		//		if (i%10 == 0)
		//			TRACE("%d : ti=%04x, te=%d \n", i, tp->time, tp->temp/100  );
			//	data += reclen;
			}
			ry -= 5;
		//	CRect tr = CRect(rx, ry, rx+25, ry+15 );
		//	dc.DrawText(pCh->name, tr, DT_LEFT);
			dc.SelectObject(pOldPen);
			dc.SelectObject(pOldFont);
		}
	}

//	TRACE("ok\n");

	// draw over, under
/*
	CPen pen;

	pen.CreatePen(PS_DOT, 1, RGB(255, 0, 0));
	for (idx = 0; idx < n; idx++) {
		Channel* pCh = m_aChannels[idx];
		if (!pCh->showOU)
			continue;

		pOldPen = dc.SelectObject(&pen);
		if (pCh->over != NO_OUV) {
			int y = ey - int(pCh->C * pCh->over);
			dc.MoveTo(m_rectGraph.left, y);
			dc.LineTo(m_rectGraph.right, y);
		}
		if (pCh->under != NO_OUV) {
			int y = ey - int(pCh->C * pCh->under);
			dc.MoveTo(m_rectGraph.left, y);
			dc.LineTo(m_rectGraph.right, y);
		}
		dc.SelectObject(pOldPen);
	}
	pen.DeleteObject();
	*/
}

void CLogGraph::setQuePos(int idx, int nPos)
{
	if ((size_t)idx<0 || (size_t)idx>m_aChannels.size())
		return;
	m_aChannels[idx]->pos = nPos;
}


bool CLogGraph::setFile()
{
	m_clMmf.close();
	CString fn = config->g_sFilePath + config->g_sFileName;
	m_bLoad = m_clMmf.open(fn);

	
	m_nCurrentPos = 0;
	if (!m_bLoad) 
		TRACE("%s: file '%s' open error\n", __FUNCTION__, fn);

	return m_bLoad;
}

tdata* CLogGraph::getTptr(int pos)
{
	int idx = pos*6;
	return (tdata*)m_clMmf.getPtr(idx);
}

int	CLogGraph::getLength()
{
	return m_clMmf.getLength()/6;
}

int CLogGraph::getLast()
{
	return (getLength()-1);
}

tdata* CLogGraph::getPtrLeftPos()
{
	int st = m_nCurrentPos;
// 	if (!m_bNav) {
// 		st = getLast() - getWndQueLength();
// 		if (st < 0)
// 			st = 0;
// 	}
	//	lib->mque_gets(m_hQue, st);
	tdata* tp = getTptr(st);
	return tp;
}





BOOL CLogGraph::PreTranslateMessage(MSG* pMsg)
{

	m_ctrTooltip.RelayEvent(pMsg);  
	return CWnd::PreTranslateMessage(pMsg);
}

void CLogGraph::CopyToClipboard()
{
	if (!m_bDragSel)return;

	if ( !OpenClipboard() )
	{
		AfxMessageBox( "Cannot open the Clipboard" );
		return;
	} 
	if( !EmptyClipboard() )
	{
		AfxMessageBox( "Cannot empty the Clipboard" );
		return;
	}
	int size = m_pDataEt - m_pDataSt;
	tdata* p = m_pDataSt;
	CString strsum = "";
	CString str;
	for (int i = 0; i<size; i++)
	{
		str.Format("%s, %d\r\n", tformat("%Y-%m-%d %H:%M:%S", p->time), p->temp/100);
		strsum += str;
		p++;
	}

	HGLOBAL hGlobal = GlobalAlloc( GHND | GMEM_SHARE, (strsum.GetLength() + 1) *sizeof(TCHAR) );
	PSTR pGlobal = (PSTR)GlobalLock( hGlobal );
	lstrcpy( pGlobal, TEXT( strsum ) );
	GlobalUnlock( hGlobal ); 



	if ( ::SetClipboardData( CF_TEXT, hGlobal  ) == NULL )
	{
		AfxMessageBox( "Unable to set Clipboard data" );
		CloseClipboard();
		return;
	}

	CloseClipboard(); 


}