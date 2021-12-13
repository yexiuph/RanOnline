// InputIDDlg.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "QuestEdit.h"
#include "InputIDDlg.h"
#include ".\inputiddlg.h"
#include "GLQuestMan.h"
#include "GLQuest.h"


// CInputIDDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CInputIDDlg, CDialog)
CInputIDDlg::CInputIDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputIDDlg::IDD, pParent)
	, m_nNewID(0)
{
}

CInputIDDlg::~CInputIDDlg()
{
}

void CInputIDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NEW_ID, m_nNewID);
	DDV_MinMaxUInt(pDX, m_nNewID, 0, 9999);
}


BEGIN_MESSAGE_MAP(CInputIDDlg, CDialog)
END_MESSAGE_MAP()


// CInputIDDlg 메시지 처리기입니다.

void CInputIDDlg::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	UpdateData( TRUE );

	GLQUEST* pQuest = GLQuestMan::GetInstance().Find( m_nNewID );
	
	if ( pQuest )
	{
		AfxMessageBox ( "No Free ID", MB_OK );
		return;
	}
	
	CDialog::OnOK();
}

BOOL CInputIDDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_nNewID = GLQuestMan::GetInstance().MakeNewQNID();

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
