// DlgCustomItem.cpp : ���� �����Դϴ�.
//

#include "pch.h"
#include "QuestEdit.h"
#include "DlgCustomItem.h"

#include "EtcFunction.h"
#include "GLItemMan.h"


// CDlgCustomItem ��ȭ �����Դϴ�.

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


// CDlgCustomItem �޽��� ó�����Դϴ�.


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
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnCancel();
}
