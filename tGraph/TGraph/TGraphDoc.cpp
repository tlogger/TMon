// TGraphDoc.cpp : implementation of the CTGraphDoc class
//

#include "stdafx.h"
#include "TGraph.h"

#include "TGraphDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTGraphDoc

IMPLEMENT_DYNCREATE(CTGraphDoc, CDocument)

BEGIN_MESSAGE_MAP(CTGraphDoc, CDocument)
END_MESSAGE_MAP()


// CTGraphDoc construction/destruction

CTGraphDoc::CTGraphDoc()
{
	// TODO: add one-time construction code here

}

CTGraphDoc::~CTGraphDoc()
{
}

BOOL CTGraphDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CTGraphDoc serialization

void CTGraphDoc::Serialize(CArchive& ar)
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


// CTGraphDoc diagnostics

#ifdef _DEBUG
void CTGraphDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTGraphDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CTGraphDoc commands
