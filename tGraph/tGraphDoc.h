// tGraphDoc.h : interface of the CtGraphDoc class
//


#pragma once


class CtGraphDoc : public CDocument
{
protected: // create from serialization only
	CtGraphDoc();
	DECLARE_DYNCREATE(CtGraphDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CtGraphDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	virtual void SetTitle(LPCTSTR lpszTitle);
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
