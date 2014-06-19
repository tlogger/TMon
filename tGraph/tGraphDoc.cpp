// tGraphDoc.cpp : implementation of the CtGraphDoc class
//

#include "stdafx.h"
#include "tGraph.h"

#include "tGraphDoc.h"
#include "Compat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CtGraphDoc

IMPLEMENT_DYNCREATE(CtGraphDoc, CDocument)

BEGIN_MESSAGE_MAP(CtGraphDoc, CDocument)
END_MESSAGE_MAP()


// CtGraphDoc construction/destruction

CtGraphDoc::CtGraphDoc()
{
	// TODO: add one-time construction code here

}

CtGraphDoc::~CtGraphDoc()
{
}

BOOL CtGraphDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CtGraphDoc serialization

void CtGraphDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CtGraphDoc diagnostics

#ifdef _DEBUG
void CtGraphDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CtGraphDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

void CtGraphDoc::SetTitle( LPCTSTR lpszTitle )
{
	
	CString str;
	str.Format("%s", PRG_VER);

	CDocument::SetTitle(str);
}
#endif //_DEBUG


// CtGraphDoc commands
