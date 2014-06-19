// TGraphView.cpp : implementation of the CTGraphView class
//

#include "stdafx.h"
#include "TGraph.h"

#include "TGraphDoc.h"
#include "TGraphView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTGraphView

IMPLEMENT_DYNCREATE(CTGraphView, CView)

BEGIN_MESSAGE_MAP(CTGraphView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CTGraphView construction/destruction

CTGraphView::CTGraphView()
{
	// TODO: add construction code here

}

CTGraphView::~CTGraphView()
{
}

BOOL CTGraphView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CTGraphView drawing

void CTGraphView::OnDraw(CDC* /*pDC*/)
{
	CTGraphDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CTGraphView printing

BOOL CTGraphView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTGraphView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CTGraphView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CTGraphView diagnostics

#ifdef _DEBUG
void CTGraphView::AssertValid() const
{
	CView::AssertValid();
}

void CTGraphView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTGraphDoc* CTGraphView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTGraphDoc)));
	return (CTGraphDoc*)m_pDocument;
}
#endif //_DEBUG


// CTGraphView message handlers
