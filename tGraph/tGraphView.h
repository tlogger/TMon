// tGraphView.h : interface of the CtGraphView class
//
#pragma once


#include "tGraphDoc.h"
#include "PaneTreeView.h"
#include "LogGraph.h"

class CtGraphView : public CView
{
public: // create from serialization only
	CtGraphView();
	DECLARE_DYNCREATE(CtGraphView)

// Attributes
public:
	CtGraphDoc* GetDocument() const;
	CPaneTreeView m_wndTreeView;
// Operations
public:

	CLogGraph m_wndVGraph;
	void selectFile();

	void makeFile();

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CtGraphView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#ifndef _DEBUG  // debug version in tGraphView.cpp
inline CtGraphDoc* CtGraphView::GetDocument() const
   { return reinterpret_cast<CtGraphDoc*>(m_pDocument); }
#endif
