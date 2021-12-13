// DlgMobGenItem.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "QuestEdit.h"
#include "DlgMobGenItem.h"

#include "EtcFunction.h"
#include "GLItemMan.h"
#include "GLCrowData.h"
#include "DlgItem.h"
#include "DlgNPC.h"

// CDlgMobGenItem 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgMobGenItem, CDialog)
CDlgMobGenItem::CDlgMobGenItem(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMobGenItem::IDD, pParent)
{
}

CDlgMobGenItem::~CDlgMobGenItem()
{
}

void CDlgMobGenItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GEN_MOB, m_listboxGENMOB);
}


BEGIN_MESSAGE_MAP(CDlgMobGenItem, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_GENITEM, OnBnClickedButtonGenitem)
	ON_BN_CLICKED(IDC_BUTTON_GEN_MOB_ADD, OnBnClickedButtonGenMobAdd)
	ON_BN_CLICKED(IDC_BUTTON_GEN_MOB_DEL, OnBnClickedButtonGenMobDel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgMobGenItem 메시지 처리기입니다.

BOOL CDlgMobGenItem::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateDlgItem();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgMobGenItem::UpdateDlgItem ()
{
	CString strGENITEM_NAME;
	
	SITEM* pITEM = GLItemMan::GetInstance().GetItem ( m_sGENQITEM.sNID );	
	if ( pITEM )
	{
		strGENITEM_NAME.Format( "%d/%d %s", pITEM->sBasicOp.sNativeID.wMainID, pITEM->sBasicOp.sNativeID.wSubID,
								pITEM->GetName() );
	}



	SetWin_Text ( this, IDC_EDIT_GENITEM, strGENITEM_NAME );

	SetWin_Num_int ( this, IDC_EDIT_GEN_NUM, m_sGENQITEM.wNUM );
	SetWin_Num_float ( this, IDC_EDIT_GEN_RATE, m_sGENQITEM.fGEN_RATE );

	m_listboxGENMOB.ResetContent();
	DWQARRAY_ITER iter = m_sGENQITEM.vecGEN_MOB.begin();
	DWQARRAY_ITER iter_end = m_sGENQITEM.vecGEN_MOB.end();
	for ( ; iter!=iter_end; ++iter )
	{
		const SNATIVEID sMOBID((*iter));
		PCROWDATA pCROW = GLCrowDataMan::GetInstance().GetCrowData ( sMOBID );

		CString strNAME;
		strNAME.Format ( "[%d/%d] %s", sMOBID.wMainID, sMOBID.wSubID, pCROW->GetName() );
		int nIndex = m_listboxGENMOB.AddString ( strNAME );

		DWORD dwData = DWORD(iter-m_sGENQITEM.vecGEN_MOB.begin());
		m_listboxGENMOB.SetItemData ( nIndex, dwData );
	}
}

void CDlgMobGenItem::OnBnClickedButtonGenitem()
{
	CDlgItem sDlgItem;
	if ( IDOK == sDlgItem.DoModal () )
	{
		m_sGENQITEM.sNID = sDlgItem.m_nidITEM;
	}

	UpdateDlgItem();
}

void CDlgMobGenItem::OnBnClickedButtonGenMobAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDlgNPC cDlgMob;
	cDlgMob.m_bMob = true;

	if ( IDOK == cDlgMob.DoModal() )
	{
		if ( cDlgMob.m_nidNPC != SNATIVEID(false) )
			m_sGENQITEM.vecGEN_MOB.push_back ( cDlgMob.m_nidNPC.dwID );
	}

	UpdateDlgItem();
}

void CDlgMobGenItem::OnBnClickedButtonGenMobDel()
{
	int nIndex = m_listboxGENMOB.GetCurSel ();
	if ( LB_ERR == nIndex )		return;

	DWORD dwDATA = (DWORD) m_listboxGENMOB.GetItemData ( nIndex );

	m_sGENQITEM.GENMOB_ERASE ( dwDATA );
}

void CDlgMobGenItem::OnBnClickedOk()
{
	m_sGENQITEM.wNUM = GetWin_Num_int ( this, IDC_EDIT_GEN_NUM );
	m_sGENQITEM.fGEN_RATE = GetWin_Num_float ( this, IDC_EDIT_GEN_RATE );

	OnOK();
}

void CDlgMobGenItem::OnBnClickedCancel()
{
	OnCancel();
}
