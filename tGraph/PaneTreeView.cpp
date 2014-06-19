// PaneTreeView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "tGraph.h"
#include "PaneTreeView.h"
#include "afxdialogex.h"

extern CConfig* config;

// CPaneTreeView 대화 상자입니다.

BEGIN_MESSAGE_MAP(CFolderEdit, CXTBrowseEdit)
	ON_CONTROL_REFLECT(EN_CHANGE, OnChangeDir)
END_MESSAGE_MAP()

void CFolderEdit::OnChangeDir()
{
	this->GetWindowText(config->g_sFilePath);
	if (config->g_pTreeView != NULL)
		config->g_pTreeView->setTreeItem();
}



IMPLEMENT_DYNAMIC(CPaneTreeView, CDialog)

CPaneTreeView::CPaneTreeView(CWnd* pParent /*=NULL*/)
	: CDialog(CPaneTreeView::IDD, pParent)	
{

}

CPaneTreeView::~CPaneTreeView()
{
}

void CPaneTreeView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_FILE, m_ctrlTree);	
	DDX_Control(pDX, IDC_BUTTON_COLOR, m_cpGraphColor);
	DDX_Control(pDX, IDC_BUTTON_REFRESH, m_btRefresh);
}


BEGIN_MESSAGE_MAP(CPaneTreeView, CDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FILE, OnSelchangedTree)	
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CPaneTreeView::OnBnClickedButtonRefresh)
	ON_CPN_XTP_SELENDOK(IDC_BUTTON_COLOR, OnSelGraphClr)
	
	
END_MESSAGE_MAP()


// CPaneTreeView 메시지 처리기입니다.


void CPaneTreeView::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_ctrlTree.GetSafeHwnd() != NULL)
		m_ctrlTree.MoveWindow(0,45,cx,cy);
	if (m_ctrFolderEdit.GetSafeHwnd() != NULL)
		m_ctrFolderEdit.MoveWindow(0,0,cx-20,20);
	if (GetDlgItem(IDC_BUTTON_REFRESH)->GetSafeHwnd() != NULL)
		GetDlgItem(IDC_BUTTON_REFRESH)->MoveWindow(0,23,cx-100, 23);
	if (GetDlgItem(IDC_BUTTON_COLOR)->GetSafeHwnd() != NULL)
		GetDlgItem(IDC_BUTTON_COLOR)->MoveWindow(cx-100, 24, 100, 24);

}


int CPaneTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	config->g_pTreeView = this;

	return 0;
}

void CPaneTreeView::addTreeItem(CString path, HTREEITEM hroot)
{
	CFileFind f;
	CString name;
	BOOL bNext = f.FindFile(path + _T("*.*"));
	while(bNext) {
		bNext = f.FindNextFile();
		if (f.IsDots())
			continue;

		if (f.IsDirectory()) {
			name = f.GetFileName();
			HTREEITEM hitem = m_ctrlTree.InsertItem(name, 2, 2, hroot);
			name = path +"\\" +name + "\\";
			addTreeItem(name , hitem);

		}
		else {
			CString ext = f.GetFileName().Right(4);
			if (ext.CompareNoCase(_T(".dat")) == 0) {
				name = f.GetFileName();
				m_ctrlTree.InsertItem(name, 4, 4, hroot);
			}else if (ext.CompareNoCase(_T(".bin")) == 0 ){
				name = f.GetFileName();
				m_ctrlTree.InsertItem(name, 5, 5, hroot);
			}
		}
	}
}

void CPaneTreeView::setTreeItem()
{
	m_ctrlTree.DeleteAllItems();

	CFileFind finder;
	bool bfd = finder.FindFile(config->g_sFilePath);
	if (!bfd)return;

	
	CString path = config->g_sFilePath + "\\";
	CString root = path.Mid(path.ReverseFind('\\')+1, path.GetLength());
	HTREEITEM htree = m_ctrlTree.InsertItem(root, 1, 1, NULL);
//	m_ctrlTree.SetItemText(htree, "root");
	addTreeItem(path, htree);
	m_ctrlTree.Expand(htree, TVE_EXPAND);

}

BOOL CPaneTreeView::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (!m_ilResourceTree.Create(16, 16, ILC_MASK|ILC_COLOR24, 0, 1))
		return -1;

	CBitmap bmp;
	bmp.LoadBitmap(IDB_BITMAP_TREE);

	m_ilResourceTree.Add(&bmp, CXTPImageManager::GetBitmapMaskColor(bmp, CPoint(0, 0)));
	m_ctrlTree.SetImageList(&m_ilResourceTree, TVSIL_NORMAL);
	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	
	UpdateData(FALSE);
	setTreeItem();

	m_ctrFolderEdit.Create(WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this , 0);// = new CXTBrowseEdit();
	m_ctrFolderEdit.Initialize(this, BES_XT_CHOOSEDIR);
	m_ctrFolderEdit.SetWindowText(config->g_sFilePath);
	m_ctrFolderEdit.ShowWindow(SW_SHOW);

	m_cpGraphColor.SetColor(config->g_colGraph);

	m_btRefresh.SetBitmap(CSize(24, 24), IDB_BITMAP_REFRESH);
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CPaneTreeView::OnSelchangedTree( NMHDR* pNMHDR, LRESULT* pResult )
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	CString strCurItem = "";
	CString strItem = "", strParent = "";
	HTREEITEM hItemParent, hItemCur;
	hItemCur = pNMTreeView->itemNew.hItem;    //현재 트리 핸들
	
	if (m_ctrlTree.GetChildItem(hItemCur))
		return;
	
	strCurItem = m_ctrlTree.GetItemText(hItemCur);   //현재 트리 스트링
	int cnt= 0; //무한루프 방지
	while(true){
		if(++cnt > 6){  //Level지정
			AfxMessageBox("TAG Level 초과");
			break;
		}
		hItemParent = m_ctrlTree.GetParentItem(hItemCur); //부모 아이템  
		strParent = m_ctrlTree.GetItemText(hItemParent); //부모 텍스트
		if(hItemParent == NULL)  
			break;
		strCurItem = strParent + "\\" + strCurItem;  
		hItemCur = hItemParent;  
	}
	config->g_sFileName = strCurItem;
	config->g_pView->selectFile();

	UpdateData(FALSE);
	*pResult = 0;

}


void CPaneTreeView::OnBnClickedButtonRefresh()
{
	setTreeItem();
}

void CPaneTreeView::OnSelGraphClr()
{
	config->g_colGraph = m_cpGraphColor.GetColor();	
	config->g_pView->m_wndVGraph.changePen(config->g_colGraph);
	config->g_pView->m_wndVGraph.redraw();

}