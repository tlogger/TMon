
#pragma once

#include <windows.h>
#include <math.h>
#include <vector>
#include <algorithm>

#include "mmf.h"

using namespace std;

#define NAV_SHOW_TIME		10				// 내비게이션 버튼이나 스크롤 바를 눌렀을 때 화면에 해당 위치의 데이터를 보여주고 있을 시간

#define LG_CH_SHOW			WM_USER+16
#define LG_NAV_MSG			WM_USER+14
#define LG_NAV_START		WM_USER+17
#define LG_NAV_END			WM_USER+18

#ifndef NO_OUV
#define NO_OUV -999.0
#endif

#pragma pack(push, 1)

typedef struct temp_data {
	unsigned int time : 32; 
	short temp : 16;
}tdata;

#pragma pack(pop, 1)

class CDialogRange : public CWnd
{
public:
	CDialogRange() {}
	virtual ~CDialogRange() {}

public:
	bool getValue(CWnd* pParent, double& fLower, double& fUpper);

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (CWnd::PreTranslateMessage(pMsg))
			return TRUE;

		if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_CANCEL)) {
			EndModalLoop(IDCANCEL);
			return TRUE;
		}

		return PreTranslateInput(pMsg);
//		return CWnd::PreTranslateMessage(pMsg);
	}

	afx_msg BOOL OnEraseBkgnd(CDC* pDC)
	{
		CBrush brush(::GetSysColor(COLOR_BTNFACE));
		CRect r;
		pDC->GetClipBox(&r);
		pDC->FillRect(&r, &brush);
		return true;
	}

	afx_msg void OnClose() { EndModalLoop(IDCANCEL); }
	afx_msg void OnCancel() {EndModalLoop(IDCANCEL); }
	afx_msg void OnOK() {EndModalLoop(IDOK); }

	CEdit	m_cLower;
	CEdit	m_cUpper;
	CStatic m_cLowerLegend;
	CStatic m_cUpperLegend;
	CButton m_cOk;
	CButton m_cCancel;

	DECLARE_MESSAGE_MAP()
};

class CLogGraph : public CWnd 
{
public:
	CLogGraph();
	virtual ~CLogGraph();

	class Channel {
	public:
		~Channel() { pen.DeleteObject(); }
		CString		name;
		double		low, high;				// 최대, 최소값
		double		over, under;			// 경고 메시지용 최대, 최소값 (아직 사용안함)
		double		scale;					// short 값의 스케일 factor
		double		C;						// 계산 편의를 위한 상수
		double		v;						// 현재 값
		CPen		pen;					// 화면 출력용 펜 
		CString		fmt;					// 화면 출력용 포맷
		BOOL		show;					// 사용 여부 결정
		BOOL		showOU;					// over, under limit를 보여줄지 결정. over나 under가 -999.0이면 어차피 보여주지 않음.
		double		max;					// 최대값
		double		min;					// 최소값
		double		avg;					// 평균값
		int			pos;					// item position.
		COLORREF	 col;
	};
	
	vector<Channel*>	m_aChannels;		// 채널 정의
	
public:
	bool		m_bNav;						// 스크롤바나 nav 버튼을 눌러 화면을 이동했을 때 true가 됨. 10초 자동 리셋.
//	HND			m_hQue;						// data que handle
	
public:										//add
	CMMF		m_clMmf;
	bool		m_bLoad;

public:
	bool		setFile();
	tdata*		getTptr(int pos = 0);
	int			getLength();
	int			getLast();
	int			getStart() {return 0;}
	tdata*		getPtrLeftPos();

	char*		tformat(const char* fmt, time_t x);

	void		CopyToClipboard();

	void		changePen(COLORREF rgb);

public:
	bool		m_bDragSel;
	bool		m_bLdown;
	CPoint      m_ptStMouse;
	CPoint      m_ptEtMouse;
	tdata*		m_pDataSt;
	tdata*		m_pDataEt;

	CToolTipCtrl m_ctrTooltip;

	


private:
	int			m_nID;						// graph의 고유 id
	HWND		m_hWnd;						// notify할 window
	CRect		m_rectWnd;					// 할당된 window의 크기
	CRect		m_rectHeader;				// 헤더 영역
	CRect		m_rectScale;				// 스케일 표시 영역
	CRect		m_rectScale2;				// 두번째 스케일 표시 영역
	CRect		m_rectGraph;				// 그래프 영역
	CRect		m_rectHome;					// 버튼 rectangle
	CRect		m_rectPgUp;
	CRect		m_rectPgDn;
	CRect		m_rectEnd;
	CRect		m_rectZoom;

	double		m_fZoomFactor;				// 줌 배율. default=1
	bool		m_bAutoZoom;				// 기록이 늘어남에 따라 그래프 크기를 자동으로 단축시킴

private:
	time_t		m_tLastNav;					// 마지막으로 스크롤바나 nav 버튼을 누른 시각.
	int			m_nScaleIndex;				// 스케일 텍스트에 사용할 채널 번호 
	int			m_nScaleIndex2;				// 두번째 스케일 텍스트에 사용할 채널 번호
	time_t		m_tStart;					// 화면에 표시한 que의 시작 시간
	time_t		m_tEnd;						// 화면에 표시한 que의 마지막 시간
	time_t		m_tLast;					// 큐에 마지막 엔트리가 들어간 시간
	int			m_nCurrentPos;				// 현재 위치의 큐 인덱스

	int			m_nHeaderHeight;			// 헤더의 높이
	int			m_nScaleWidth;				// 스케일 표시기의 폭
	int			m_nGraphType;				// 그래프 형태. 0=line, 1=stack
	int			m_nLastCursorPos;			// mouse move 할 때 마지막 위치값
	CFont		m_fontHeader;				// 헤더에 표시할 텍스트의 폰트
	CFont		m_fontScale;
	CFont		m_fontGraph;

	COLORREF	m_crBack;					// 백 그라운드
	COLORREF	m_crHeader;
	COLORREF	m_crHeaderBack;				
	COLORREF	m_crScale;
	COLORREF	m_crScaleBack;
	COLORREF	m_crGraph;
	COLORREF	m_crGrid;					// grid color
	int			m_nGridSize;				// granulity of grid
	
	bool		m_bCreated;					// 컨트롤이 만들어지면 true
	bool		m_bStylesModified;
	static BOOL	m_bRegistered;
	short*		m_aTmp;

private:
	static BOOL	registerClass();
	void		calcRect();
	bool		hitRect(CPoint pt, CRect r);		
	inline int	getWndQueLength()	{ return int(m_fZoomFactor * m_rectGraph.Width() + 0.5); }
	inline int	getWndQueLastPos();
	void		setScrollBar();
	void		sendMsg(int kind, int pos = 0);
	void		calcStat(short* x, bool stat = true);
	int			getMaxChannel();

public:

	bool	openGraph(const char* fn, int len, int maxLog, bool bCreate = false);
	void	clearGraph();
	bool	attach(UINT id, CWnd* pParent);
	bool	create(UINT id, CWnd* pParent);
	bool	isCreated() { return m_bCreated; }
	void	clearStat();
	void	goLast();
	void	setNavMode(bool bNav = true);
	void	setNavMode2(bool bNav = true);
	void	setScaleIndex(int idx, int idx2){ m_nScaleIndex = idx; m_nScaleIndex2 = idx2;}
	void	setGrid(int sz, COLORREF gc) 	{ m_nGridSize = sz; m_crGrid = gc; }
	void	setHeader(const char* font, int sz, COLORREF col, COLORREF back);
	void	setScale(const char* font, int sz, COLORREF col, COLORREF back);
	void	setGraph(const char* font, int sz, COLORREF col, COLORREF back);
	void	setGraphType(int n)				{ m_nGraphType = n; }
	void	setZoom(double zf);
	void	setZoomFit();
	void	setPos(int pos) { m_nCurrentPos = pos; setScrollBar(); }
	void	setMsgReceiver(HWND h)		{ m_hWnd = h; }
	void	setID(int n) { m_nID = n; }
	int		getID()		 { return m_nID;}
	bool	add(const char* name, double low, double high, double over, double under, double scale, int nPos, const char* fmt = "%5.2fV ", bool bShow = true, COLORREF col = RGB(255, 255, 0));
	bool	push(int t, short* x);
	bool	push(int t, int* x);
	bool	push(int t, vector<int> x);
	void	show(const char* name)		{ int n = find(name); if (n != -1) show(n); }
	void	show(int idx)				{ if (idx >= 0 && idx < (int)m_aChannels.size()) 
							m_aChannels[idx]->show = true; }
	void	showOverUnder(int idx, bool bShow);
	void	hide(const char* name)		{ int n = find(name); if (n != -1) show(n); }
	void	hide(int idx)				{ if (idx >= 0 && idx < (int)m_aChannels.size()) m_aChannels[idx]->show = false; }
	bool	getShow(int idx)			{ if (idx >= 0 && idx < (int)m_aChannels.size()) return (m_aChannels[idx]->show == TRUE);	}
	int		find(const char* name);
	
	void	drawHeader(CDC& dc);
	void	drawScale(CDC& dc, int nIndex, CRect rect, int nAlign);
	void	drawGraph(CDC& dc);
	void	redraw()					{ setScrollBar(); Invalidate(); }
	void	removeAll();
//	void	setQue(HND h)				{ m_hQue = h; }

	void	setQuePos(int idx, int nPos);

	

private:
	CDC			m_dcBack;					// 화면 버퍼링용
	CBitmap		m_bmBack, *m_pOldBitmap;	// 화면 버퍼링용
	
	CDC& FBufferInit(CPaintDC &dc)
	{
		CRect r;
		GetClientRect(&r);
		m_dcBack.CreateCompatibleDC(&dc);
		m_bmBack.CreateCompatibleBitmap(&dc, r.Width(), r.Height());

		m_pOldBitmap = m_dcBack.SelectObject(&m_bmBack);
		return m_dcBack;
	}

	void FBufferRelease(CPaintDC &dc)
	{
		CRect r;
		GetClientRect(&r);
		dc.BitBlt(r.left, r.top, r.Width(), r.Height(), &m_dcBack, r.left, r.top, SRCCOPY);
		dc.SelectObject(m_pOldBitmap);
		m_bmBack.DeleteObject();
		m_dcBack.DeleteDC();
	}

protected:
	//{{AFX_MSG(CLogGraph)
	afx_msg void OnPaint();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LONG OnWheel(UINT a, LONG b);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

