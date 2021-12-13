// DlgNpcTalk.cpp : ���� �����Դϴ�.
//

#include "pch.h"
#include "NpcAction.h"
#include "DlgNpcTalk.h"
#include ".\dlgnpctalk.h"
#include "NpcDialogue.h"


// CDlgNpcTalk ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgNpcTalk, CDialog)
CDlgNpcTalk::CDlgNpcTalk( CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNpcTalk::IDD, pParent)
	, m_dwNpcID( UINT_MAX )
	, m_pNpcDialogueSet ( NULL ) 
{
}

CDlgNpcTalk::CDlgNpcTalk( CNpcDialogueSet* pNpcDialogueSet, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNpcTalk::IDD, pParent)
	, m_dwNpcID( UINT_MAX )
	, m_pNpcDialogueSet ( pNpcDialogueSet ) 
{
}

CDlgNpcTalk::~CDlgNpcTalk()
{
}

void CDlgNpcTalk::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NPCTALK_LIST, m_listNpcTalk);
}


BEGIN_MESSAGE_MAP(CDlgNpcTalk, CDialog)
END_MESSAGE_MAP()


// CDlgNpcTalk �޽��� ó�����Դϴ�.

BOOL CDlgNpcTalk::OnInitDialog()
{
	CDialog::OnInitDialog();

	if ( m_pNpcDialogueSet )
	{
		CNpcDialogueSet::DIALOGMAP* pMap = m_pNpcDialogueSet->GetDialogueMap ();
	
		CNpcDialogueSet::DIALOGMAP_IT iter = pMap->begin();
		CNpcDialogueSet::DIALOGMAP_IT iter_end = pMap->end();		

        CString strTemp;

		for ( int nIndex = 0; iter!=iter_end; ++iter, nIndex++ )
		{
			CNpcDialogue* pDialogue = (*iter).second;

			DWORD dwIndex = pDialogue->FindNonCondition();
			if ( dwIndex==UINT_MAX )	dwIndex = 0;

			CNpcDialogueCase* pDlgCase = pDialogue->GetDlgCase(dwIndex);
			if ( !pDlgCase )
			{
				
				pMap->erase( iter++ );
				--nIndex;
				continue;			
			}

			strTemp.Format ( "[%3d] %s", pDialogue->GetDlgNID(), pDlgCase->GetBasicTalk().c_str() );
			
			int nItem = m_listNpcTalk.AddString( strTemp );
			m_listNpcTalk.SetItemData ( nItem, pDialogue->GetDlgNID() );
		}

	}

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgNpcTalk::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	int nIndex = m_listNpcTalk.GetCurSel();		

	if ( nIndex > 0 || nIndex < m_listNpcTalk.GetCount() )
	{
		m_dwNpcID = m_listNpcTalk.GetItemData( nIndex );
	}

	CDialog::OnOK();
}

void CDlgNpcTalk::OnCancel()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CDialog::OnCancel();
}
