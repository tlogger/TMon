#pragma once

#include "pgrid.h"

class CDialogConfigEquip : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogConfigEquip)

public:
	CDialogConfigEquip(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogConfigEquip();

	enum { IDD = IDD_DIALOG_CONFIG_EQUIP };

	enum {
		ITEM_EQUIP_ID = 100,
		ITEM_LOGGER_TYPE,
		ITEM_LOG_METHOD,
		ITEM_TOLERANCE,
		ITEM_SCOND,
		ITEM_ECOND,
		ITEM_STIME,
		ITEM_ETIME
	};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnGridNotify(WPARAM, LPARAM);
	afx_msg void OnOk();

	DECLARE_MESSAGE_MAP()
public:

	CPropGrid	m_cGrid;
	CEquip*		m_pEquip;

	CXTPPropertyGridItem*	m_pItems[40];

	void redrawGrid();
};
