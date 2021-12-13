// DlgCustomItem.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "QuestEdit.h"
#include "DlgCustomItem.h"

#include "EtcFunction.h"
#include "GLItemMan.h"


// CDlgCustomItem 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgCustomItem, CDialog)
CDlgCustomItem::CDlgCustomItem(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCustomItem::IDD, pParent)
{
}

CDlgCustomItem::~CDlgCustomItem()
{
}

void CDlgCustomItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgCustomItem, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgCustomItem 메시지 처리기입니다.


BOOL CDlgCustomItem::OnInitDialog()
{
	CDialog::OnInitDialog();

	SITEM* pITEM = GLItemMan::GetInstance().GetItem ( m_sITEMCUSTOM.sNativeID );
	if ( !pITEM )	return TRUE;

	SetWin_Num_int ( this, IDC_EDIT_MID, m_sITEMCUSTOM.sNativeID.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_SID, m_sITEMCUSTOM.sNativeID.wSubID );
	SetWin_Text ( this, IDC_EDIT_NAME, pITEM->GetName() );

	SetWin_Num_int ( this, IDC_EDIT_DAMAGE, m_sITEMCUSTOM.cDAMAGE );
	SetWin_Num_int ( this, IDC_EDIT_DEFENSE, m_sITEMCUSTOM.cDEFENSE );

	SetWin_Num_int ( this, IDC_EDIT_ELECTRIC, m_sITEMCUSTOM.cRESIST_ELEC );
	SetWin_Num_int ( this, IDC_EDIT_FIRE, m_sITEMCUSTOM.cRESIST_FIRE );
	SetWin_Num_int ( this, IDC_EDIT_ICE, m_sITEMCUSTOM.cRESIST_ICE );
	SetWin_Num_int ( this, IDC_EDIT_POISON, m_sITEMCUSTOM.cRESIST_POISON );
	SetWin_Num_int ( this, IDC_EDIT_SPIRIT, m_sITEMCUSTOM.cRESIST_SPIRIT );

	SetWin_Num_int ( this, IDC_EDIT_TURNNUM, m_sITEMCUSTOM.wTurnNum );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgCustomItem::OnBnClickedOk()
{
	m_sITEMCUSTOM.cDAMAGE = GetWin_Num_int ( this, IDC_EDIT_DAMAGE );
	m_sITEMCUSTOM.cDEFENSE = GetWin_Num_int ( this, IDC_EDIT_DEFENSE );

	m_sITEMCUSTOM.cRESIST_ELEC = GetWin_Num_int ( this, IDC_EDIT_ELECTRIC );
	m_sITEMCUSTOM.cRESIST_FIRE = GetWin_Num_int ( this, IDC_EDIT_FIRE );
	m_sITEMCUSTOM.cRESIST_ICE = GetWin_Num_int ( this, IDC_EDIT_ICE );
	m_sITEMCUSTOM.cRESIST_POISON = GetWin_Num_int ( this, IDC_EDIT_POISON );
	m_sITEMCUSTOM.cRESIST_SPIRIT = GetWin_Num_int ( this, IDC_EDIT_SPIRIT );

	m_sITEMCUSTOM.wTurnNum = GetWin_Num_int ( this, IDC_EDIT_TURNNUM );


	OnOK();
}

void CDlgCustomItem::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnCancel();
}
