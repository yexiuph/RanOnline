// DlgQuest.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "NpcAction.h"
#include "DlgQuest.h"

#include "GLQUEST.h"
#include "GLQuestMan.h"
#include ".\dlgquest.h"
#include "EtcFunction.h"


// CDlgQuest 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgQuest, CDialog)
CDlgQuest::CDlgQuest(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgQuest::IDD, pParent),
	m_dwQuestID(UINT_MAX)
{
}

CDlgQuest::~CDlgQuest()
{
}

void CDlgQuest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listQuest);
}


BEGIN_MESSAGE_MAP(CDlgQuest, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(ID_BUTTON_FIND, OnBnClickedButtonFind)
END_MESSAGE_MAP()


// CDlgQuest 메시지 처리기입니다.
BOOL CDlgQuest::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_listQuest.ResetContent();

	GLQuestMan::MAPQUEST & mapQuest = GLQuestMan::GetInstance().GetMap ();
	GLQuestMan::MAPQUEST_ITER iter = mapQuest.begin();
	GLQuestMan::MAPQUEST_ITER iter_end = mapQuest.end();
	for ( ; iter!=iter_end; ++iter )
	{
		DWORD dwQUESTID = (*iter).first;
		GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );;
		if ( !pQUEST )	continue;

		CString strNAME;
		strNAME.Format ( "[%05d] %s", dwQUESTID, pQUEST->GetTITLE() );

		int nIndex = m_listQuest.AddString ( strNAME );
		m_listQuest.SetItemData ( nIndex, pQUEST->m_sNID.dwID );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgQuest::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_listQuest.GetCurSel ();
	if ( nIndex == LB_ERR )		return;

	DWORD dwData = (DWORD) m_listQuest.GetItemData ( nIndex );
	m_dwQuestID = dwData;

	OnOK();
}

void CDlgQuest::OnBnClickedCancel()
{
	m_dwQuestID = UINT_MAX;

	OnCancel();
}

void CDlgQuest::OnBnClickedButtonFind()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nID;

	nID = GetWin_Num_int( this, IDC_EDIT_FINDID );

	int nSize = m_listQuest.GetCount();

	DWORD dwQuestID;

	for ( int i = 0; i < nSize; ++i )
	{
		dwQuestID = (DWORD) m_listQuest.GetItemData( i );

		if ( nID == dwQuestID )
		{
			m_listQuest.SetCurSel( i );
			return;
		}
	}
}
