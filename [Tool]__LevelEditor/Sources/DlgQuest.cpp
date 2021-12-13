// DlgQuest.cpp : implementation file
//

#include "pch.h"
#include <map>

#include "LevelEditor.h"
#include "DlgQuest.h"

#include "EtcFunction.h"

#include "GLQuest.h"
#include "GLQuestMan.h"

// CDlgQuest dialog

IMPLEMENT_DYNAMIC(CDlgQuest, CDialog)
CDlgQuest::CDlgQuest(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgQuest::IDD, pParent)
	, m_dwQuestID ( -1 )
{
}

CDlgQuest::~CDlgQuest()
{
}

void CDlgQuest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_QUEST, m_listQuest);
}


BEGIN_MESSAGE_MAP(CDlgQuest, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_QUEST_OK, OnBnClickedButtonQuestOk)
	ON_BN_CLICKED(IDC_BUTTON_QUEST_CANCEL, OnBnClickedButtonQuestCancel)
END_MESSAGE_MAP()


// CDlgQuest message handlers
BOOL CDlgQuest::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_listQuest.ResetContent();

	GLQuestMan::MAPQUEST& m_mapQuestMap = GLQuestMan::GetInstance().GetMap();
	GLQuestMan::MAPQUEST_ITER iter_pos;

	DWORD dwQuestID = 0;
	GLQUEST* pQUEST = NULL;
	CString strTemp;

	for ( iter_pos = m_mapQuestMap.begin(); iter_pos != m_mapQuestMap.end(); iter_pos++ )
	{
		dwQuestID = (DWORD) iter_pos->first;	
		pQUEST = GLQuestMan::GetInstance().Find ( dwQuestID );

		if ( pQUEST ) 
		{
			strTemp.Format ( "[%d] %s" , dwQuestID, pQUEST->GetTITLE() );
			int nIndex = m_listQuest.AddString ( strTemp );
			m_listQuest.SetItemData ( nIndex, (DWORD_PTR)pQUEST->m_sNID.dwID );
		}
	}

	return TRUE;
}

void CDlgQuest::OnBnClickedButtonQuestOk()
{
	int nIndex = m_listQuest.GetCurSel ();

	if ( nIndex != -1 )
	{
		m_dwQuestID = (DWORD)m_listQuest.GetItemData ( nIndex );
	}
}

void CDlgQuest::OnBnClickedButtonQuestCancel()
{
	m_dwQuestID = -1;
}
