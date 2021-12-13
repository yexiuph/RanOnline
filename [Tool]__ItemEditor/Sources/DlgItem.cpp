// DlgItem.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ItemEdit.h"
#include "DlgItem.h"
#include "GLItemMan.h"
#include "GLCrowData.h"
#include "EtcFunction.h"
#include "InputDataDlg.h"

// CDlgItem 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgItem, CDialog)
CDlgItem::CDlgItem(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgItem::IDD, pParent)
	, m_dwITEM(UINT_MAX)
	, m_fRate(0)
	, m_dwNum(1)
	, m_bRateEnable(false)
	, m_bNumEnable(false)
	, m_bUseMonster(false)
	, m_wMonsterMid(0xFFFF)
	, m_wMonsterSid(0xFFFF)
{
}

CDlgItem::~CDlgItem()
{
}

void CDlgItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ITEM, m_listITEM);
	DDX_Control(pDX, IDC_LIST_ITEM2, m_listMonster);
	DDX_Text(pDX, IDC_EDIT_PERCENT, m_fRate);
	DDX_Control(pDX, IDC_EDIT_PERCENT, m_Edit_Percent);
}


BEGIN_MESSAGE_MAP(CDlgItem, CDialog)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_BN_CLICKED(IDOK, OnCancel)
	ON_EN_SETFOCUS(IDC_EDIT_PERCENT, OnEnSetfocusEditPercent)
END_MESSAGE_MAP()


// CDlgItem 메시지 처리기입니다.
BOOL CDlgItem::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( m_bUseMonster == FALSE )
	{
		for ( int MID = 0; MID < GLItemMan::MAX_MID; MID++ )
		for ( int SID = 0; SID < GLItemMan::MAX_SID; SID++ )
		{
			SITEM* pItem = GLItemMan::GetInstance().GetItem ( MID, SID );
			if ( !pItem )	continue;
			
			CString strNAME;
			strNAME.Format ( "[%03d/%03d] %s", MID, SID, pItem->GetName() );

			int nIndex = m_listITEM.AddString ( strNAME );
			m_listITEM.SetItemData ( nIndex, pItem->sBasicOp.sNativeID.dwID );
		}
		m_listMonster.ShowWindow( SW_HIDE );
	}else{
		for ( int MID = 0; MID < GLCrowDataMan::MAX_CROW_MID; MID++ )
		for ( int SID = 0; SID < GLCrowDataMan::MAX_CROW_SID; SID++ )
		{
			PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( MID, SID );
			if ( !pCrowData )	continue;

			CString strNAME;
			strNAME.Format ( "[%03d/%03d] %s", MID, SID, pCrowData->GetName() );

			int nIndex = m_listMonster.AddString ( strNAME );
			m_listMonster.SetItemData ( nIndex, pCrowData->sNativeID.dwID );
		}
		m_listITEM.ShowWindow( SW_HIDE );
	}

	m_Edit_Percent.EnableWindow(m_bRateEnable);
	
	SetWin_Enable ( this, IDC_EDIT_NUM, m_bNumEnable );
	if ( m_bNumEnable )
	{
		SetWin_Num_int ( this, IDC_EDIT_NUM, m_dwNum );
	}

	m_SelMap.clear();

	return TRUE;
}

void CDlgItem::OnOk()
{

	if( m_bUseMonster == TRUE ) 
	{
		CString strTemp = GetWin_Text( this, IDC_EDIT_PERCENT );
		m_fRate = (float)atof(strTemp.GetString());
		if( m_fRate == 0.0f ) return;

		DWORD nIndex = m_listMonster.GetCurSel();
		if ( nIndex == -1 ) 
		{
			CDialog::OnCancel(); 
			return;
		}

		DWORD dwData = (DWORD) m_listMonster.GetItemData ( nIndex );
		PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( dwData );
        m_wMonsterMid = pCrowData->sNativeID.wMainID;
		m_wMonsterSid = pCrowData->sNativeID.wSubID;


		CDialog::OnOK();
		return;
	}
	int nCnt = m_listITEM.GetSelCount ();
	if ( nCnt <= 0 ) return;

	int* pSelect = new int[nCnt];
	memset( pSelect, -1, sizeof ( int ) * nCnt );

	m_listITEM.GetSelItems( nCnt, pSelect );
	
	CInputDataDlg dlg;

	for ( int i = 0; i < nCnt; ++i ) 
	{
		DWORD dwData = (DWORD) m_listITEM.GetItemData ( pSelect[i] );
		SITEM* pItem= GLItemMan::GetInstance().GetItem ( dwData );

		dlg.m_dwNum = 1;
		dlg.m_fRate = 0.0f;
		dlg.m_bNum = m_bNumEnable;
		dlg.m_strItemName = pItem->GetName();
		if ( dlg.DoModal() == IDOK )
		{
			m_SelMap.insert( pair<DWORD,DWORD>(dwData, dlg.m_dwNum) );
		}
	}

	delete[] pSelect;

	CDialog::OnOK();
}

void CDlgItem::OnCancel()
{
	m_dwITEM = UINT_MAX;
	
	CDialog::OnCancel();
}

// 추가 : 2004.9.6 By 경대
//
void CDlgItem::OnEnSetfocusEditPercent()
{
	m_Edit_Percent.SetWindowText("");
}