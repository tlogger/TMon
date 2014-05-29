#pragma once

#define XLEGEND_SZ 50
#define TOP_SZ		40

class CtestView : public CView
{
protected: // create from serialization only
	CtestView();
	DECLARE_DYNCREATE(CtestView)

public:
	CtestDoc* GetDocument() const;
	CRect		m_rectWnd;
	CRect		m_rectHeader;
	CRect		m_rectScale;
	CRect		m_rectGraph;

	CRect		m_rectHome;					// 버튼 rectangle
	CRect		m_rectPgUp;
	CRect		m_rectPgDn;
	CRect		m_rectEnd;
	CRect		m_rectZoom;

	double		m_fHigh;
	double		m_fLow;
	
	CFont		m_fontHeader;				// 헤더에 표시할 텍스트의 폰트
	CFont		m_fontScale;
	CFont		m_fontGraph;
	COLORREF	m_crGrid;
	COLORREF	m_crHeader;
	COLORREF	m_crHeaderBack;
	COLORREF	m_crScale;
	COLORREF	m_crScaleBack;
	COLORREF	m_crGraph;
	COLORREF	m_crBack;
	int			m_nGridSize;

public:
	void		setHeader(const char* font, int sz, COLORREF col, COLORREF back);
	void		setScale(const char* font, int sz, COLORREF col, COLORREF back);
	void		setGraph(const char* font, int sz, COLORREF col, COLORREF back);
	void		drawScale(CDC& dc);
	void		drawGraph(CDC& dc);
	void		drawHeader(CDC& dc);

public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

public:
	virtual ~CtestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // debug version in testView.cpp
inline CtestDoc* CtestView::GetDocument() const
   { return reinterpret_cast<CtestDoc*>(m_pDocument); }
#endif
