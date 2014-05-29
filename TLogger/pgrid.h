#pragma once

#include "FontEx.h"

class CPropertyGridItemBrowse : public CXTPPropertyGridItem
{
public:
	CPropertyGridItemBrowse(const char* caption, CString* sBind) : CXTPPropertyGridItem(caption, *sBind, sBind) {}
	virtual ~CPropertyGridItemBrowse() {}

	void OnInplaceButtonDown(CXTPPropertyGridInplaceButton* pButton)
	{
		LPCTSTR pDir = GetValue();
		CXTBrowseDialog d;
		d.SetOwner(GetGrid()->GetSafeHwnd());
		d.SetTitle((TCHAR*)pDir);
		d.SetSelPath((TCHAR*)pDir);
		d.SetOptions(BIF_VALIDATE | BIF_USENEWUI);
		if (d.DoModal() == IDOK) {
			SetValue(d.GetSelPath());
		}
	}
};

class CPropertyGridItemBrowseSavePath : public CXTPPropertyGridItem
{
public:
	CPropertyGridItemBrowseSavePath(const char* caption, CString* sBind) : CXTPPropertyGridItem(caption, *sBind, sBind) {}
	virtual ~CPropertyGridItemBrowseSavePath() {}

	void OnInplaceButtonDown(CXTPPropertyGridInplaceButton* pButton)
	{
		LPCTSTR pFile = GetValue();
		CFileDialog d(FALSE, "*.log", pFile, 0, "Log Files (*.log)|*.log||", GetGrid());
		if (d.DoModal() == IDOK) {
			SetValue(d.GetPathName());
		}
	}
};

class CPropertyGridItemFilePath : public CXTPPropertyGridItem
{
public:
	CPropertyGridItemFilePath(const char* caption, CString* sBind, const char *ext, const char* filespec) : 
	  CXTPPropertyGridItem(caption, *sBind, sBind) 
	{
		sExt = ext;
		sFileSpec = filespec;
	}
	virtual ~CPropertyGridItemFilePath() {}

	CString sExt;
	CString sFileSpec;

	void OnInplaceButtonDown(CXTPPropertyGridInplaceButton* pButton)
	{
		LPCTSTR pFile = GetValue();
		CFileDialog d(TRUE, sExt, pFile, 0, sFileSpec, GetGrid());
		if (d.DoModal() == IDOK) {
			SetValue(d.GetPathName());
		}
	}
};

class CPropertyGridItemIP : public CXTPPropertyGridItem
{
	class CInplaceEditIPAddress : public CXTPPropertyGridInplaceEdit
	{
	public:
		BOOL Create(LPCTSTR /*lpszClassName*/, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
		{		
			if (!CWnd::Create(WC_IPADDRESS, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
				return FALSE;

			ModifyStyle(WS_BORDER, 0);
			ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);		
			return TRUE;
		}
	};

public:
	CPropertyGridItemIP::CPropertyGridItemIP(CString caption, CString* sBind) : CXTPPropertyGridItem(caption, *sBind, sBind) {}

protected:
	virtual CXTPPropertyGridInplaceEdit& GetInplaceEdit() { return m_wndEdit; }

private:
	CInplaceEditIPAddress m_wndEdit;

};

class CPropertyGridItemFontEx : public CXTPPropertyGridItemFont
{
public:
	CPropertyGridItemFontEx(const char* caption, CString* fontset) : CXTPPropertyGridItemFont(caption, lf)
	{
		fontstr = fontset;
		fex = *fontstr;
		CXTPPropertyGridItemFont::SetColor(fex.getColor());
		SetFont(*(fex.getLogFont()));
	}

	CFontEx		fex;
	LOGFONT		lf;
	CString*	fontstr;

	CString FontToString(const LOGFONT& lfValue) {
		fex = (LOGFONT&)lfValue;
		*fontstr = fex.getString();
		return *fontstr;
	}

	void OnInplaceButtonDown()
	{
		fex = GetValue();
		SetFont(*fex.getLogFont());
		SetColor(fex.getColor());

		CXTPPropertyGridItemFont::OnInplaceButtonDown(NULL);
		COLORREF x = GetColor();
		fex.setColor(x);
		*fontstr = fex.getString();
		SetValue(*fontstr);
	}
};

class CPropGrid : public CXTPPropertyGrid
{
public:
	CFont f;

	BOOL setup(int nId)
	{
		CRect r;
		GetDlgItem(nId)->GetWindowRect(&r);
		ScreenToClient( &r );
		return Create(r, this, nId );
	}

	void expandAll(bool bExpand = true)
	{
		CXTPPropertyGridItems* pItems = GetCategories();
		for (int i = 0; i < pItems->GetCount(); i++)
			if (bExpand)
				pItems->GetAt(i)->Expand();
			else
				pItems->GetAt(i)->Collapse();
	}

	inline CXTPPropertyGridItem* addFont(CXTPPropertyGridItem* pGroup, const char* sCaption, CString* sBindFont, const char* desc = NULL)
	{
		CPropertyGridItemFontEx* pItem = (CPropertyGridItemFontEx*)pGroup->AddChildItem(
			new CPropertyGridItemFontEx(sCaption, sBindFont));
		pItem->SetDescription(desc);
		pItem->SetID((UINT)-2);
		return pItem;
	}

	inline CXTPPropertyGridItem* addDir(CXTPPropertyGridItem* pGroup, const char* sCaption, CString* sBindPath, int nID = -1, const char* desc = NULL)
	{
		CPropertyGridItemBrowse* pItem = (CPropertyGridItemBrowse*)pGroup->AddChildItem(new CPropertyGridItemBrowse(sCaption, sBindPath));
		pItem->SetDescription(desc);
		pItem->SetID(nID);
		pItem->SetFlags(xtpGridItemHasExpandButton);
		return pItem;
	}

	inline CXTPPropertyGridItem* addFilePath(CXTPPropertyGridItem* pGroup, const char* sCaption, CString* sBindPath, const char* ext, const char* filespec, int nID = -1, const char* desc = NULL)
	{
		CPropertyGridItemFilePath* pItem = (CPropertyGridItemFilePath*)pGroup->AddChildItem(
			new CPropertyGridItemFilePath(sCaption, sBindPath, ext, filespec));
		pItem->SetDescription(desc);
		pItem->SetID(nID);
		pItem->SetFlags(xtpGridItemHasExpandButton);
		return pItem;
	}

	inline CXTPPropertyGridItem* addSavePath(CXTPPropertyGridItem* pGroup, const char* sCaption, CString* sBindPath, int nID = -1, const char* desc = NULL)
	{
		CPropertyGridItemBrowseSavePath* pItem = (CPropertyGridItemBrowseSavePath*)pGroup->AddChildItem(new CPropertyGridItemBrowseSavePath(sCaption, sBindPath));
		pItem->SetDescription(desc);
		pItem->SetID(nID);
		pItem->SetFlags(xtpGridItemHasExpandButton);
		return pItem;
	}

	inline CXTPPropertyGridItem* addBool(CXTPPropertyGridItem* pGroup, const char* sCaption, BOOL* bBind, int nID = -1, const char* desc = NULL)
	{
		CXTPPropertyGridItemBool* pItem = (CXTPPropertyGridItemBool*)pGroup->AddChildItem(new CXTPPropertyGridItemBool(sCaption, *bBind, bBind));

		if (nID != -1) {
			pItem->SetCheckBoxStyle(true);
			pItem->SetID(nID);
		}
		pItem->SetDescription(desc);
		return pItem;
	}

	inline CXTPPropertyGridItem* addInt(CXTPPropertyGridItem* pGroup, const char* sCaption, int* nBind, const char* desc = NULL)
	{	
		CXTPPropertyGridItemNumber* pItem = (CXTPPropertyGridItemNumber*)pGroup->AddChildItem(new CXTPPropertyGridItemNumber(sCaption, *nBind, (long*)nBind));
		pItem->SetDescription(desc);
		return pItem;
	}

	inline CXTPPropertyGridItem* addIP(CXTPPropertyGridItem* pGroup, const char* sCaption, CString* sBind, const char* desc = NULL, int nID = 0)
	{
		CPropertyGridItemIP* pItem = (CPropertyGridItemIP*)pGroup->AddChildItem(new CPropertyGridItemIP(sCaption, sBind));
		pItem->SetDescription(desc);
		pItem->SetID(nID);
		return pItem;
	}

	inline CXTPPropertyGridItem* addString(CXTPPropertyGridItem* pGroup, const char* sCaption, CString* sBind, const char* desc = NULL, int nID = 0)
	{
		CXTPPropertyGridItem* pItem = pGroup->AddChildItem(new CXTPPropertyGridItem(sCaption, *sBind, sBind));
		pItem->SetDescription(desc);
		pItem->SetID(nID);
		return pItem;
	}

	inline CXTPPropertyGridItem* addDate(CXTPPropertyGridItem* pGroup, const char* sCaption, COleDateTime* sBind, const char* desc = NULL, int nID = 0)
	{
		CXTPPropertyGridItem* pItem = pGroup->AddChildItem(new CXTPPropertyGridItemDate(sCaption, *sBind));
		pItem->SetDescription(desc);
		pItem->SetID(nID);
		return pItem;
	}

	inline CXTPPropertyGridItem* addTime(CXTPPropertyGridItem* pGroup, const char* sCaption, COleDateTime* sBind, const char* desc = NULL, int nID = 0)
	{

	}

	inline CXTPPropertyGridItem* addColor(CXTPPropertyGridItem* pGroup, const char* sCaption, COLORREF* color, const char* desc = NULL)
	{
		CXTPPropertyGridItemColor* pItem = (CXTPPropertyGridItemColor*)pGroup->AddChildItem(new CXTPPropertyGridItemColor(sCaption, *color, color));
		pItem->SetDescription(desc);
		return pItem;
	}

	inline CXTPPropertyGridItem* addCombo(CXTPPropertyGridItem* pGroup, const char* sCaption, CString* sBind, CStringArray& sa, int nID = -1, const char* desc = NULL)
	{
		CXTPPropertyGridItem* pItem =  pGroup->AddChildItem(new CXTPPropertyGridItem(sCaption, *sBind, sBind));
		CXTPPropertyGridItemConstraints* pList = pItem->GetConstraints();
		for (int i = 0; i < sa.GetCount(); i++)	
			pList->AddConstraint(sa[i]);
		pItem->SetFlags(xtpGridItemHasComboButton); // | xtpGridItemHasEdit);
		pItem->SetDescription(desc);
		if (nID != -1)
			pItem->SetID(nID);
		return pItem;
	}

	inline CXTPPropertyGridItem* addCombo(CXTPPropertyGridItem* pGroup, const char* sCaption, CString* sBind, const char* list[], int nID = -1, const char* desc = NULL)
	{
		CStringArray sa;
		for (int i = 0; list[i][0] != '\0'; i++)
			sa.Add(list[i]);
		return addCombo(pGroup, sCaption, sBind, sa, nID, desc);
	}

};
