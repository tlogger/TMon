#pragma once
#include "afxcmn.h"
#include "resource.h"

// CPaneTreeView ��ȭ �����Դϴ�.

class CFolderEdit : public CXTBrowseEdit
{
public:
	DECLARE_MESSAGE_MAP()
protected:	
	afx_msg void OnChangeDir();	
	CString m_sPath;

};

class CPaneTreeView : public CDialog
{
	DECLARE_DYNAMIC(CPaneTreeView)

public:
	CPaneTreeView(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CPaneTreeView();

	

public:
	CImageList m_ilResourceTree;
	CTreeCtrl m_ctrlTree;
	CFolderEdit m_ctrFolderEdit;

	CXTButton m_btRefresh;


	CXTPColorPicker  m_cpGraphColor;

public:
	void setTreeItem();
	void addTreeItem(CString path, HTREEITEM htree);


	

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_FILETREE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonRefresh();	
	afx_msg void OnSelGraphClr();
};
