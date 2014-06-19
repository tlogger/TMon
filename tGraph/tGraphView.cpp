// tGraphView.cpp : implementation of the CtGraphView class
//

#include "stdafx.h"
#include "tGraph.h"

#include "tGraphDoc.h"
#include "tGraphView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CConfig* config;
// CtGraphView

IMPLEMENT_DYNCREATE(CtGraphView, CView)

BEGIN_MESSAGE_MAP(CtGraphView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CtGraphView construction/destruction

CtGraphView::CtGraphView()
{
	// TODO: add construction code here

}

CtGraphView::~CtGraphView()
{
}

BOOL CtGraphView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CtGraphView drawing

void CtGraphView::OnDraw(CDC* /*pDC*/)
{
	CtGraphDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CtGraphView printing

BOOL CtGraphView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CtGraphView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CtGraphView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CtGraphView diagnostics

#ifdef _DEBUG
void CtGraphView::AssertValid() const
{
	CView::AssertValid();
}

void CtGraphView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CtGraphDoc* CtGraphView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CtGraphDoc)));
	return (CtGraphDoc*)m_pDocument;
}
#endif //_DEBUG


// CtGraphView message handlers


int CtGraphView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
//	m_wndTreeView.CreateEx(0, NULL, "File List" , WS_POPUP| WS_VISIBLE, CRect(0,0,0,0), this, NULL );
	m_wndTreeView.Create(IDD_DIALOG_FILETREE, this);
	config->g_pView = this;


	m_wndVGraph.create(1, this);
	m_wndVGraph.setZoom(1.0);
	m_wndVGraph.setGraphType(0);
	m_wndVGraph.setScaleIndex(0,-1);
	m_wndVGraph.setMsgReceiver(GetSafeHwnd());

	m_wndVGraph.add("temp", config->g_fMinTemp, config->g_fMaxTemp, 0, 0, 100.0, 0, "%5.0f¡É", true, config->g_colGraph); 

	

	return 0;
}

void CtGraphView::makeFile()
{
	CFile cf;
	CString name = config->g_sFilePath+ "\\test.dat";
	cf.Open(name, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
	time_t t = time(NULL);
	tdata td;
	for (size_t i = 0; i < 86400*100; i++ )
	{

		td.time = t++;
		td.temp = (short)(double(i%100)*100.0);
		cf.Write(&td, sizeof(tdata));
	}
	cf.Close();
}

void CtGraphView::selectFile()
{
	m_wndVGraph.setFile();
	m_wndVGraph.redraw();

//	makeFile();
	

	
}

void CtGraphView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (m_wndVGraph.GetSafeHwnd())
		m_wndVGraph.MoveWindow(0, 0, cx, cy);
}


BOOL CtGraphView::PreTranslateMessage(MSG* pMsg)
{
	
	if ( pMsg->message == WM_KEYDOWN)
	{
		BOOL bTemp = pMsg->wParam == 'C' || pMsg->wParam == 'c' ;
		if ( bTemp && GetAsyncKeyState(VK_CONTROL) )
		{
			m_wndVGraph.CopyToClipboard();
		}
	}
	return CView::PreTranslateMessage(pMsg);
}
