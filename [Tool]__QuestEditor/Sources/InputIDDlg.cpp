// InputIDDlg.cpp : ���� �����Դϴ�.
//

#include "pch.h"
#include "QuestEdit.h"
#include "InputIDDlg.h"
#include ".\inputiddlg.h"
#include "GLQuestMan.h"
#include "GLQuest.h"


// CInputIDDlg ��ȭ �����Դϴ�.

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


// CInputIDDlg �޽��� ó�����Դϴ�.

void CInputIDDlg::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	m_nNewID = GLQuestMan::GetInstance().MakeNewQNID();

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
