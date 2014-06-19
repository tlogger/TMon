
#pragma once

#include <windows.h>
#include <math.h>
#include <vector>
#include <algorithm>

#include "mmf.h"

using namespace std;

#define NAV_SHOW_TIME		10				// ������̼� ��ư�̳� ��ũ�� �ٸ� ������ �� ȭ�鿡 �ش� ��ġ�� �����͸� �����ְ� ���� �ð�

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
		double		low, high;				// �ִ�, �ּҰ�
		double		over, under;			// ��� �޽����� �ִ�, �ּҰ� (���� ������)
		double		scale;					// short ���� ������ factor
		double		C;						// ��� ���Ǹ� ���� ���
		double		v;						// ���� ��
		CPen		pen;					// ȭ�� ��¿� �� 
		CString		fmt;					// ȭ�� ��¿� ����
		BOOL		show;					// ��� ���� ����
		BOOL		showOU;					// over, under limit�� �������� ����. over�� under�� -999.0�̸� ������ �������� ����.
		double		max;					// �ִ밪
		double		min;					// �ּҰ�
		double		avg;					// ��հ�
		int			pos;					// item position.
		COLORREF	 col;
	};
	
	vector<Channel*>	m_aChannels;		// ä�� ����
	
public:
	bool		m_bNav;						// ��ũ�ѹٳ� nav ��ư�� ���� ȭ���� �̵����� �� true�� ��. 10�� �ڵ� ����.
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
	int			m_nID;						// graph�� ���� id
	HWND		m_hWnd;						// notify�� window
	CRect		m_rectWnd;					// �Ҵ�� window�� ũ��
	CRect		m_rectHeader;				// ��� ����
	CRect		m_rectScale;				// ������ ǥ�� ����
	CRect		m_rectScale2;				// �ι�° ������ ǥ�� ����
	CRect		m_rectGraph;				// �׷��� ����
	CRect		m_rectHome;					// ��ư rectangle
	CRect		m_rectPgUp;
	CRect		m_rectPgDn;
	CRect		m_rectEnd;
	CRect		m_rectZoom;

	double		m_fZoomFactor;				// �� ����. default=1
	bool		m_bAutoZoom;				// ����� �þ�� ���� �׷��� ũ�⸦ �ڵ����� �����Ŵ

private:
	time_t		m_tLastNav;					// ���������� ��ũ�ѹٳ� nav ��ư�� ���� �ð�.
	int			m_nScaleIndex;				// ������ �ؽ�Ʈ�� ����� ä�� ��ȣ 
	int			m_nScaleIndex2;				// �ι�° ������ �ؽ�Ʈ�� ����� ä�� ��ȣ
	time_t		m_tStart;					// ȭ�鿡 ǥ���� que�� ���� �ð�
	time_t		m_tEnd;						// ȭ�鿡 ǥ���� que�� ������ �ð�
	time_t		m_tLast;					// ť�� ������ ��Ʈ���� �� �ð�
	int			m_nCurrentPos;				// ���� ��ġ�� ť �ε���

	int			m_nHeaderHeight;			// ����� ����
	int			m_nScaleWidth;				// ������ ǥ�ñ��� ��
	int			m_nGraphType;				// �׷��� ����. 0=line, 1=stack
	int			m_nLastCursorPos;			// mouse move �� �� ������ ��ġ��
	CFont		m_fontHeader;				// ����� ǥ���� �ؽ�Ʈ�� ��Ʈ
	CFont		m_fontScale;
	CFont		m_fontGraph;

	COLORREF	m_crBack;					// �� �׶���
	COLORREF	m_crHeader;
	COLORREF	m_crHeaderBack;				
	COLORREF	m_crScale;
	COLORREF	m_crScaleBack;
	COLORREF	m_crGraph;
	COLORREF	m_crGrid;					// grid color
	int			m_nGridSize;				// granulity of grid
	
	bool		m_bCreated;					// ��Ʈ���� ��������� true
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
	CDC			m_dcBack;					// ȭ�� ���۸���
	CBitmap		m_bmBack, *m_pOldBitmap;	// ȭ�� ���۸���
	
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

