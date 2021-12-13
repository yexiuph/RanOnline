// DlgNPC.cpp : ���� �����Դϴ�.
//

#include "pch.h"
#include "QuestEdit.h"
#include "DlgNPC.h"

//#include <strstream>
#include "GLCrowData.h"
#include ".\dlgnpc.h"


// CDlgNPC ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgNPC, CDialog)
CDlgNPC::CDlgNPC(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNPC::IDD, pParent),
	m_bMob(false),
	m_nidNPC(false)
{
}

CDlgNPC::~CDlgNPC()
{
}

void CDlgNPC::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_NPC, m_listNPC);
}


BEGIN_MESSAGE_MAP(CDlgNPC, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgNPC �޽��� ó�����Դϴ�.

BOOL CDlgNPC::OnInitDialog()
{
	CDialog::OnInitDialog();

	TCHAR szName[128] = {0};

	for ( WORD i=0; i<GLCrowDataMan::MAX_CROW_MID; ++i )
	for ( WORD j=0; j<GLCrowDataMan::MAX_CROW_SID; ++j )
	{
		PCROWDATA pDATA = GLCrowDataMan::GetInstance().GetCrowData ( i, j );
		if ( !pDATA )	continue;

		if ( m_bMob && pDATA->m_emCrow!=CROW_MOB )	continue;
		if ( !m_bMob && pDATA->m_emCrow==CROW_MOB )	continue;

		//std::strstream strName;
		//strName << "[" << i << "/" << j << "] ";
		//strName << pDATA->GetName() << std::ends;

		_snprintf_s( szName, 128, "[%u/%u] %s", i, j, pDATA->GetName() );

		int nIndex = m_listNPC.AddString ( szName );

		m_listNPC.SetItemData ( nIndex, pDATA->sNativeID.dwID );

		//strName.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgNPC::OnBnClickedOk()
{
	int nIndex = m_listNPC.GetCurSel();
	if ( LB_ERR==nIndex )
	{
		MessageBox ( "The item was not selected.", "Caution", MB_OK );
		return;
	}

	m_nidNPC.dwID = (DWORD) m_listNPC.GetItemData ( nIndex );

	OnOK();
}

void CDlgNPC::OnBnClickedCancel()
{
	m_nidNPC = SNATIVEID(false);

	OnCancel();
}
