#include "stdafx.h"
#include "TLogger.h"
#include "DialogConfigEquip.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CDialogConfigEquip, CDialogEx)

CDialogConfigEquip::CDialogConfigEquip(CWnd* pParent /*=NULL*/) : CDialogEx(CDialogConfigEquip::IDD, pParent)
{
}

CDialogConfigEquip::~CDialogConfigEquip()
{
}

void CDialogConfigEquip::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CONFIG, m_cGrid);
}

BEGIN_MESSAGE_MAP(CDialogConfigEquip, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
	ON_BN_CLICKED(IDOK, &CDialogConfigEquip::OnOk)
END_MESSAGE_MAP()

BOOL CDialogConfigEquip::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//m_cGrid.ShowToolBar();
	m_cGrid.SetTheme(xtpGridThemeNativeWinXP);
	m_cGrid.ShowHelp(true);	

	CXTPPropertyGridItem* pItem = m_cGrid.AddCategory("Logger Setup");
	int n = 0;
	m_pItems[n++] = m_cGrid.addString(pItem, "Equipment ID", &m_pEquip->m_sID, "장비 ID는 파일 이름으로 사용가능한 문자로만 구성해야 합니다", ITEM_EQUIP_ID);
	m_pItems[n++] = m_cGrid.addCombo(pItem, "Logger Type", &m_pEquip->m_sLoggerType, m_pEquip->m_aLoggerType, ITEM_LOGGER_TYPE);
	m_pItems[n++] = m_cGrid.addIP(pItem, "IP Address", &m_pEquip->m_sIP, "Logger의 IP Address");
	m_pItems[n++] = m_cGrid.addString(pItem, "Prefix", &m_pEquip->m_sPrefix, "Temp log의 prefix data");
	m_pItems[n++] = m_cGrid.addInt(pItem, "Log Interval", &m_pEquip->m_nInterval, "로그 저장 간격");

	// 5
	m_pItems[n++] = m_cGrid.addCombo(pItem, "Log Method", &m_pEquip->m_sLogMethod, m_pEquip->m_aLogMethod, ITEM_LOG_METHOD);
	m_pItems[n++] = m_cGrid.addString(pItem, "Tolerance", &m_pEquip->m_sTolerance, "Log Method가 Detect일 때 Start Condition 및 End Condition에서 지정한 온도에 +- 이 값을 기준 조건으로 판정", ITEM_TOLERANCE);
	m_pItems[n++] = m_cGrid.addString(pItem, "Start Condition", &m_pEquip->m_sStartCond, "Log Method가 Detect일 때 이 조건을 만족하면 로그 시작. 예: '>25,5s' 25도 이상이 5초 이상 유지되면 로그 시작", ITEM_SCOND);
	m_pItems[n++] = m_cGrid.addString(pItem, "End Condition", &m_pEquip->m_sEndCond, "Log Method가 Detect일 때 이 조건을 만족하면 로그 중단. 예: '<26,1m': 26도 이하로 1분 이상 유지되면 로그 중단", ITEM_ECOND);
	m_pItems[n++] = m_cGrid.addString(pItem, "Start Time", &m_pEquip->m_sStartTime, "Log Method가 Periodic일 때 이 시각부터 로그 기록 시작", ITEM_STIME);
	m_pItems[n++] = m_cGrid.addString(pItem, "End Time", &m_pEquip->m_sEndTime, "Log Method가 Periodic일 때 이 시각에 도달하면 로그 기록 중단", ITEM_ETIME);

	m_cGrid.expandAll(true);
	redrawGrid();
	return TRUE;  
}

BOOL CDialogConfigEquip::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(r);
	COLORREF crStart = XTPColorManager()->GetColor(COLOR_BTNFACE);
	COLORREF crEnd = XTPColorManager()->GetColor(COLOR_BTNSHADOW);

	XTPPaintManager()->GradientFill(pDC, r, crStart, crEnd, FALSE);
	return TRUE;

//	return CDialogEx::OnEraseBkgnd(pDC);
}

void CDialogConfigEquip::redrawGrid()
{
	bool bGSTECH = (m_pItems[1]->GetValue() == m_pEquip->m_aLoggerType[0]);
	bool bPeriodic = (m_pItems[5]->GetValue() == m_pEquip->m_aLogMethod[0]);

	if (bGSTECH) {
		for (size_t i = 5; i < 11; i++)
			m_pItems[i]->SetReadOnly(TRUE);
	}
	else {
		m_pItems[5]->SetReadOnly(FALSE);
		m_pItems[6]->SetReadOnly(bPeriodic);
		m_pItems[7]->SetReadOnly(bPeriodic);
		m_pItems[8]->SetReadOnly(bPeriodic);
		m_pItems[9]->SetReadOnly(!bPeriodic);
		m_pItems[10]->SetReadOnly(!bPeriodic);
	}
}

LRESULT CDialogConfigEquip::OnGridNotify(WPARAM wParam, LPARAM lParam)
{
	static CString old;
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;

	if (wParam == XTP_PGN_SELECTION_CHANGED && pItem != NULL) {
		old = pItem->GetValue();
	}
	else if (wParam == XTP_PGN_ITEMVALUE_CHANGED && pItem != NULL) {
		
		int nId = pItem->GetID();
		CString s = pItem->GetValue();

		if (nId == ITEM_EQUIP_ID) {
			config.convertValidFilename(s, false);
			pItem->SetValue(s);
		}
		else if (nId == ITEM_TOLERANCE) {
			s.Format("%5.2f", atof(s));
			pItem->SetValue(s);
		}
		else if (nId == ITEM_SCOND || nId == ITEM_ECOND) {
			CEquip::cond_t cond;
			cond = m_pEquip->s2cond(pItem->GetValue());
			s = m_pEquip->cond2s(cond);
			pItem->SetValue(s);
		}
		else if (nId == ITEM_STIME || nId == ITEM_ETIME) {
			time_t t;
			config.s2t(pItem->GetValue(), t);
			if (t > 0) 
				pItem->SetValue(config.t2s(t));
			else
				pItem->SetValue(old);
		}

		redrawGrid();

		m_cGrid.Refresh();
	}
	return 0;
}

void CDialogConfigEquip::OnOk()
{
	m_pEquip->convert();
	CDialogEx::OnOK();
}
