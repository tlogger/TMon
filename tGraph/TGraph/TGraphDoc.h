// TGraphDoc.h : interface of the CTGraphDoc class
//


#pragma once


class CTGraphDoc : public CDocument
{
protected: // create from serialization only
	CTGraphDoc();
	DECLARE_DYNCREATE(CTGraphDoc)

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
	virtual ~CTGraphDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
