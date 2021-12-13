// DlgQuest.cpp : ���� �����Դϴ�.
//

#include "pch.h"
#include "NpcAction.h"
#include "DlgQuest.h"

#include "GLQUEST.h"
#include "GLQuestMan.h"
#include ".\dlgquest.h"
#include "EtcFunction.h"


// CDlgQuest ��ȭ �����Դϴ�.

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


// CDlgQuest �޽��� ó�����Դϴ�.
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
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgQuest::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
