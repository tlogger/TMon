#include "stdafx.h"
#include "test.h"
#include <math.h>

#include "testDoc.h"
#include "testView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CtestView, CView)

BEGIN_MESSAGE_MAP(CtestView, CView)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()

CtestView::CtestView()
{
	setHeader("Segoe UI", 14, RGB(200, 200, 200), RGB(120, 120, 120));
	setScale("Segoe UI", 14, RGB(200, 200, 0), RGB(100, 100, 100));
	setGraph("Segoe UI", 14, RGB(200, 200, 200), RGB(0, 0, 0));
	m_crGrid = RGB(100, 100, 100);
	m_crHeader = RGB(255, 255, 255);
	m_crHeaderBack = RGB(80, 80, 80);
	m_crScale = RGB(255, 255, 255);
	m_crScaleBack = RGB(80, 80, 80);
	m_crGraph = RGB(255, 255, 255);
	m_crBack = RGB(0, 0, 0);

	m_fHigh = 150;
	m_fLow = -40;
	m_nGridSize = int((m_fHigh - m_fLow) / 10.0);
}

CtestView::~CtestView()
{
}

BOOL CtestView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

void CtestView::OnDraw(CDC* /*pDC*/)
{
	CtestDoc* pDoc = GetDocument();
	/*
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	*/
}

BOOL CtestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CtestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CtestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

#ifdef _DEBUG
void CtestView::AssertValid() const
{
	CView::AssertValid();
}

void CtestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CtestDoc* CtestView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CtestDoc)));
	return (CtestDoc*)m_pDocument;
}
#endif //_DEBUG


void CtestView::setHeader(const char* font, int sz, COLORREF col, COLORREF back)
{ 
	m_fontHeader.DeleteObject();
	m_fontHeader.CreateFont(sz, 0, 0, 0, FW_NORMAL, false, false, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, font);
	m_crHeader = col; 
	m_crHeaderBack = back;
}

void CtestView::setScale(const char* font, int sz, COLORREF col, COLORREF back)
{ 
	m_fontScale.DeleteObject();
	m_fontScale.CreateFont(sz, 0, 0, 0, FW_NORMAL, false, false, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, font);
	m_crScale = col; 
	m_crScaleBack = back;
}

void CtestView::setGraph(const char* font, int sz, COLORREF col, COLORREF back)
{ 
	m_fontGraph.DeleteObject();
	m_fontGraph.CreateFont(sz, 0, 0, 0, FW_NORMAL, false, false, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, font);
	m_crGraph = col; 
	m_crBack = back;
}

void CtestView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	dc.SetWindowOrg(0, 0);
	dc.SetViewportOrg(0, 0);
	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt(1, 1);
	dc.SetViewportExt(1, 1);

	dc.SetBkMode(TRANSPARENT);
	dc.FillSolidRect(&m_rectWnd, RGB(0,0,0));
	dc.FillSolidRect(&m_rectGraph, m_crBack);

	XTPPaintManager()->GradientFill(&dc, m_rectGraph,RGB(150,0,0), RGB(0,0,80), FALSE);

	drawScale(dc);
	drawHeader(dc);
	drawGraph(dc);
}

void CtestView::drawGraph(CDC& dc)
{
	CPen pen, *open;
	CString s;
	CRect r;

	// draw graph

	pen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	open = dc.SelectObject(&pen);

	double ys = ((double)m_rectGraph.Height()) / (m_fHigh - m_fLow);
	double yo = m_rectGraph.bottom;

	struct {
		int tp;
		double temp;
		double du;		// duration or rise ratio
	} bstep[] = {
//		{ 0,  25.0, 5.00 },
		{ 1,  85.0, 1.0 },	
		{ 0,  85.0, 10.0 },

		{ 1, 125.0, 1.0 },
		{ 0, 125.0, 10.0 },
		{ -1, 65.0, 1.0 },
		{ 0,  65.0, 10.0 },

		{ 1, 125.0, 1.0 },
		{ 0, 125.0, 10.0 },
		{ -1, 45.0, 1.0 },
		{ 0,  45.0, 10.0 },

		{ 1, 125.0, 1.0 },
		{ 0, 125.0, 10.0 },
		{ -1, 45.0, 1.0 },
		{ 0,  45.0, 10.0 },

		{ -1, -1.0, -1.0 }
	};

#define PY(t) (m_rectGraph.bottom - int((t - m_fLow) * ys))
#define VT(t) (t + (rand() % 20 - 10) * 0.1)
	int x = m_rectGraph.left;
	double t = 26.0;

	dc.MoveTo(m_rectGraph.left, PY(t));

	for (int i = 0; bstep[i].temp != -1.0; i++) {

		if (bstep[i].tp == 0) {
			t = bstep[i].temp;
			int w = int(m_rectGraph.Width() * (bstep[i].du/100.0));
			for (int j = 0; j < w; j++) {
				dc.LineTo(x, PY(VT(t)));
				x++;
			}
		}
		else if (bstep[i].tp == 1) {
			while (t < bstep[i].temp) {
				t += bstep[i].du;
				dc.LineTo(x, PY(VT(t)));
				x++;
			}
		}
		else if (bstep[i].tp == -1) {
			while (t > bstep[i].temp) {
				t -= bstep[i].du;
				dc.LineTo(x, PY(VT(t)));
				x++;
			}
		}
	}
	dc.SelectObject(open);
	pen.DeleteObject();
}

void CtestView::drawScale(CDC& dc)
{
	XTPPaintManager()->GradientFill(&dc, m_rectScale, m_crScaleBack, RGB(0,0,0), FALSE);

	double step = 10.0, v; 
	CString fmt = " %.0f ";

	step = 10.0;
	v = m_fHigh;

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
		CRect r = m_rectScale;
		r.top = int(y);
		r.bottom = int(y + 12);
		CString s;
		s.Format(fmt, v);
		dc.DrawText(s, &r, DT_RIGHT);
		y += h;
		v -= step;
	}
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldFont);
	pen.DeleteObject();
}

void CtestView::drawHeader(CDC& dc)
{
	XTPPaintManager()->GradientFill(&dc, m_rectHeader,m_crHeaderBack, RGB(0,0,0), FALSE);

	CPen* pOldPen, pen;
	CFont* pOldFont;
	CString fmt, s;
	int i;

	CString tmp;

	pOldFont = dc.SelectObject(&m_fontHeader);
	dc.SetTextColor(m_crHeader);

	tmp.Format("2014-05-10 10:10:22\r\n2");
	dc.DrawText(tmp, &m_rectHeader, DT_LEFT);

	tmp.Format("2015-05-22 08:09:11\r\n2374");
	dc.DrawText(tmp, &m_rectHeader, DT_RIGHT);

	// draw Header (nav button, zoom)
	
	int nav[4][20] = {		// 20x20 Â¥¸® nav button
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
	s.Format("Zoom %.0f:1", 10.0);
	dc.DrawText(s, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	dc.SelectObject(pOldFont);
}

void CtestView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	GetClientRect(&m_rectWnd);

	m_rectHeader = m_rectWnd;
	m_rectScale = m_rectWnd;
	m_rectGraph = m_rectWnd;

	m_rectHeader.bottom = TOP_SZ;

	m_rectScale.top = TOP_SZ;
	m_rectGraph.top = TOP_SZ;

	m_rectScale.right = m_rectScale.left + XLEGEND_SZ;
	m_rectGraph.right = m_rectScale.right + m_rectGraph.Width() - XLEGEND_SZ;
	m_rectGraph.left += XLEGEND_SZ;
}
