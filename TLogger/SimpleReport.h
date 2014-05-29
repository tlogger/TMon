#pragma once

class CSimpleReport : public CXTPReportControl  
{
public:
	CSimpleReport();
	virtual ~CSimpleReport() {}

	CFont m_fontHeader;
	CFont m_fontText;

	// 기본 컬러 세팅

	void SetColorBack(COLORREF x) { GetPaintManager()->m_clrControlBack = x; }
	void SetColorHighLight(COLORREF text, COLORREF back) {  GetPaintManager()->m_clrHighlightText = text;  GetPaintManager()->m_clrHighlight = back; }
	void SetFontText(const char* fontname, int sz);
	void SetFontHeader(const char* fontname, int sz);

	// 컬럼 세팅 

	CXTPReportColumn* ColSet(int order, const char* s, int width, int align = DT_CENTER);
	
	void ColHide(CXTPReportColumn* pCol);
	void ColShow(CXTPReportColumn* pCol);

	void ColHide(int cno)	{ ColHide(GetColumns()->GetAt(cno)); }
	void ColShow(int cno)	{ ColShow(GetColumns()->GetAt(cno)); }
	void ColSetComboData(CXTPReportColumn* p, CStringArray& s);
	void ColSetComboData(CXTPReportColumn* p, const char** s);

	// 레코드 세팅

	inline CXTPReportRecord* RowAdd() { return AddRecord(newRec()); }
	inline CXTPReportRecordItem* ColAdd(CXTPReportRecord* p, const char* s) { return (p) ? p->AddItem(new CXTPReportRecordItemText(s)) : NULL; }
	inline CXTPReportRecordItem* ColAdd(CXTPReportRecord* p, double f, const char* fmt) { return (p) ? p->AddItem(new CXTPReportRecordItemNumber(f, fmt)) : NULL; }
	inline CXTPReportRecordItem* ColAdd(CXTPReportRecord* p, int f, const char* fmt) { return (p) ? p->AddItem(new CXTPReportRecordItemNumber((double)f, fmt)) : NULL; }

	// 특정 레코드의 컬럼 변경

	void ColSetColor(int r, int c, COLORREF fore, COLORREF back);
	void ColSetBold(int r, int c, BOOL x = TRUE);
	void ColReplace(int r, int c, const char* s);
	void ColReplace(int r, int c, int n);
	void ColReplace(int r, int c, double n);

	// 특정 레코드의 컬럼값 읽기

	int ColGetData(CXTPReportRecord* p, int colpos);
	double ColGetF(CXTPReportRecord* p, int colpos);
	int ColGetI(CXTPReportRecord* p, int colpos);
	const char* ColGetS(CXTPReportRecord* p, int colpos);

	inline double ColGetF(int recno, int colpos) { return ColGetF(GetRecords()->GetAt(recno), colpos); }
	inline int ColGetI(int recno, int colpos) { return ColGetI(GetRecords()->GetAt(recno), colpos); }
	inline const char* ColGetS(int recno, int colpos) { return ColGetS(GetRecords()->GetAt(recno), colpos); }

	inline int ColGetData(CXTPReportRow* pRow, int colpos) { return ColGetData(pRow->GetRecord(), colpos); }
	inline double ColGetF(CXTPReportRow* pRow, int colpos) { return ColGetF(pRow->GetRecord(), colpos); }
	inline int ColGetI(CXTPReportRow* pRow, int colpos) { return ColGetI(pRow->GetRecord(), colpos); }
	inline const char* ColGetS(CXTPReportRow* pRow, int colpos) { return ColGetS(pRow->GetRecord(), colpos); }

	// 레코드의 조작

	void RowSetColor(CXTPReportRecord* pRec, COLORREF fore, COLORREF back);
	void RowSetBold(CXTPReportRecord* pRec, BOOL x = TRUE);
	inline void RowSetColor(int rno, COLORREF fore, COLORREF back) { RowSetColor(GetRecords()->GetAt(rno), fore, back); }
	inline void RowSetBold(int rno, BOOL x = TRUE) { RowSetBold(GetRecords()->GetAt(rno), x); }
	inline void RowRemoveAll() { GetRecords()->RemoveAll(); }

	int RowGetFocused();
	void RowSetFocused(CXTPReportRecord* pRec);
	void RowSetFocused(int nRow);
	void RowSetSelected(int nRow);
	void RowDown( bool (*copyFunc)(CSimpleReport*, CXTPReportRecord*, CXTPReportRecord*) );
	void RowUp( bool (*copyFunc)(CSimpleReport*, CXTPReportRecord*, CXTPReportRecord*) );
	void RowDownMulti();
	void RowUpMulti();
	void RowInsert(CXTPReportRecord* x, bool bAdd = false);
	void RowDelete();

	// report control의 상태 저장/불러오기

	void LayoutSave(const char* id, const char* tag);
	void LayoutLoad(const char* id, const char* tag);

	// copy,cut,paste context menu 처리

public:
	enum {
		DO_ADD = 1,
		DO_COPY = 2,
		DO_CUT = 4,
		DO_PASTE = 8,
		DO_ALL = 0xfff
	};

	BOOL m_bAdd;			// paste할 때 add할 것인지 insert할 것인지
	void Paste();

	bool doCutPaste(CPoint pt, int op);

	CXTPReportRecord* (*callback_newrec)();
	bool (*callback_add)(CSimpleReport* pCtrl, CXTPReportRecord* pRec);
	bool (*callback_del)(CSimpleReport* pCtrl, CXTPReportRecord* pRec);
	bool (*callback_copy)(CSimpleReport* pCtrl, CXTPReportRecord* pSource, CXTPReportRecord* pTarget);

	CXTPReportRecord* newRec();
	void setCallbackNewRec(CXTPReportRecord* (*newrec)()) { callback_newrec = newrec; }
	void setCallback(bool (*add)(CSimpleReport*, CXTPReportRecord*), bool (*del)(CSimpleReport*, CXTPReportRecord*), bool (*copy)(CSimpleReport*, CXTPReportRecord*, CXTPReportRecord*))
	{
		callback_add = add;
		callback_del = del;
		callback_copy = copy;
	}
public:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

class CSimpleReportView : public CXTPReportView
{
public:
	CSimpleReportView();           // protected constructor used by dynamic creation
	virtual ~CSimpleReportView();
	DECLARE_DYNCREATE(CSimpleReportView)

public:

	virtual void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult); 
	virtual void OnValueChanged(NMHDR * pNotifyStruct, LRESULT *pResult);
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	DECLARE_MESSAGE_MAP()
};

