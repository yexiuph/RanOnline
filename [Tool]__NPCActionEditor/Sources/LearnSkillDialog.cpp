// LearnSkillDialog.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "NpcAction.h"
#include "LearnSkillDialog.h"
#include "GLSkill.h"
#include ".\learnskilldialog.h"
#include "EtcFunction.h"


// CLearnSkillDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLearnSkillDialog, CDialog)
CLearnSkillDialog::CLearnSkillDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CLearnSkillDialog::IDD, pParent)
{
	m_bConfirm = FALSE;
}

CLearnSkillDialog::~CLearnSkillDialog()
{
}

void CLearnSkillDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLearnSkillDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnBnClickedButtonFind)
END_MESSAGE_MAP()


// CLearnSkillDialog 메시지 처리기입니다.

void CLearnSkillDialog::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bConfirm = TRUE;

	CComboBox* pComboBox = (CComboBox*) GetDlgItem ( IDC_COMBO_SKILL_LEVEL );
	int nIndex = pComboBox->GetCurSel ();
	if ( nIndex==LB_ERR )	return;

	m_Condition.wLevel = (WORD) pComboBox->GetItemData ( nIndex );

	CListBox * pListBox = (CListBox *) GetDlgItem ( IDC_LIST_SKILL );
	nIndex = pListBox->GetCurSel ();
	if ( nIndex==LB_ERR )	return;

	m_Condition.dwNID = (DWORD) pListBox->GetItemData ( nIndex );

	OnOK ();
}

SSkillCondition	CLearnSkillDialog::GetSkillCondition ()
{
	assert ( m_bConfirm && "쓰레기값 넘어갑니다." );

	return m_Condition;
}

void CLearnSkillDialog::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	OnCancel ();
}

BOOL CLearnSkillDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bConfirm = FALSE;

	//	현재 밖으로 드러난 아이템들 모두 로드하기
	CString strTemp;

	CComboBox* pComboBox = (CComboBox*) GetDlgItem ( IDC_COMBO_SKILL_LEVEL );
	for ( int i = 1; i <= SKILL::MAX_LEVEL; i++ )
	{
		strTemp.Format ( "%d", i );
		int nIndex = pComboBox->AddString ( strTemp );
		pComboBox->SetItemData ( nIndex, i );
	}
	pComboBox->SetCurSel ( 0 );

	CListBox * pListBox = (CListBox *) GetDlgItem ( IDC_LIST_SKILL );
	for ( WORD i=0; i<EMSKILLCLASS_NSIZE; ++i )
	for ( WORD j=0; j<GLSkillMan::MAX_CLASSSKILL; ++j )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( i, j );
		if ( !pSkill )	continue;

		CString strTemp;
		strTemp.Format ( "[%03d/%03d] %s", pSkill->m_sBASIC.sNATIVEID.wMainID,
			pSkill->m_sBASIC.sNATIVEID.wSubID, pSkill->GetName() );
		
		int nIndex = pListBox->AddString ( strTemp.GetString() );
		pListBox->SetItemData ( nIndex, pSkill->m_sBASIC.sNATIVEID.dwID );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLearnSkillDialog::OnBnClickedButtonFind()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nMainID, nSubID;

	nMainID = GetWin_Num_int( this, IDC_EDIT_FINDMID );
	nSubID = GetWin_Num_int( this, IDC_EDIT_FINDSID );

	CListBox* pListBox = (CListBox*) GetDlgItem ( IDC_LIST_SKILL );
	int nSize = pListBox->GetCount();

	SNATIVEID sNativeID;

	for ( int i = 0; i < nSize; ++i )
	{
		sNativeID = (DWORD) pListBox->GetItemData( i );

		if ( nMainID == sNativeID.wMainID && nSubID == sNativeID.wSubID )
		{
			pListBox->SetCurSel( i );
			return;
		}
	}
}
