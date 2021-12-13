// HaveItemDialog.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "NpcAction.h"
#include "HaveItemDialog.h"
#include "haveitemdialog.h"
#include "GLItemMan.h"
#include "GLStringTable.h"
#include "FindItem.h"
#include "EtcFunction.h"

// CHaveItemDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CHaveItemDialog, CDialog)
CHaveItemDialog::CHaveItemDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CHaveItemDialog::IDD, pParent),
	m_dwItemID(UINT_MAX)
{
}

CHaveItemDialog::~CHaveItemDialog()
{
}

void CHaveItemDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHaveItemDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnBnClickedButtonFind)
END_MESSAGE_MAP()


// CHaveItemDialog 메시지 처리기입니다.

void CHaveItemDialog::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_dwItemID = UINT_MAX;
	OnCancel ();
}

void CHaveItemDialog::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bConfirm = TRUE;
	CListBox* pListBox = (CListBox*) GetDlgItem ( IDC_LIST_ITEM );
	int nIndex = pListBox->GetCurSel ();
	if ( nIndex==LB_ERR )	return;

	m_dwItemID = (DWORD) pListBox->GetItemData ( nIndex );

	OnOK ();
}

void CHaveItemDialog::OnBnClickedButtonFind()
{	
	int nMainID, nSubID;

	nMainID = GetWin_Num_int( this, IDC_EDIT_FINDMID );
	nSubID = GetWin_Num_int( this, IDC_EDIT_FINDSID );

	CListBox* pListBox = (CListBox*) GetDlgItem ( IDC_LIST_ITEM );
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

BOOL CHaveItemDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_bConfirm = FALSE;	

	//	현재 밖으로 드러난 아이템들 모두 로드하기
	CListBox* pListBox = (CListBox*) GetDlgItem ( IDC_LIST_ITEM );

	for ( WORD i=0; i<GLItemMan::MAX_MID; ++i )
	for ( WORD j=0; j<GLItemMan::MAX_SID; ++j )
	{
		SITEM *pItem = GLItemMan::GetInstance().GetItem ( i, j );
		if ( !pItem )	continue;

		CString strTemp;
		strTemp.Format ( "[%03d/%03d] %s", pItem->sBasicOp.sNativeID.wMainID,
			pItem->sBasicOp.sNativeID.wSubID, pItem->GetName() );
		
		int nIndex = pListBox->AddString ( strTemp.GetString() );
		pListBox->SetItemData ( nIndex, pItem->sBasicOp.sNativeID.dwID );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

DWORD CHaveItemDialog::GetItemID ()
{
	assert ( m_bConfirm && "쓰레기 값이 넘어갑니다." );

	return m_dwItemID;
}
