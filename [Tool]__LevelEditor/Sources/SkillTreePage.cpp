// SkillTreePage.cpp : implementation file
//

#include "pch.h"
#include "LevelEditor.h"
#include "SkillTreePage.h"
#include "SheetWithTab.h"

#include "EtcFunction.h"

#include "GLDefine.h"

#include "GLCharDefine.h"
#include "GLSkill.h"
#include ".\skilltreepage.h"

// CSkillTreePage dialog

IMPLEMENT_DYNAMIC(CSkillTreePage, CPropertyPage)
CSkillTreePage::CSkillTreePage()
	: CPropertyPage(CSkillTreePage::IDD)
	, m_CalledPage ( -1 )
	, m_bDlgInit ( FALSE )
	, m_bNativeIDInit ( FALSE )
{
}

CSkillTreePage::~CSkillTreePage()
{
}

void CSkillTreePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_SKILL, m_ctrlItemTree);
}


BEGIN_MESSAGE_MAP(CSkillTreePage, CPropertyPage)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_SKILL_OK, OnBnClickedButtonSkillOk)
	ON_BN_CLICKED(IDC_BUTTON_SKILL_CANCEL, OnBnClickedButtonSkillCancel)
END_MESSAGE_MAP()


// CSkillTreePage message handlers
BOOL CSkillTreePage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bDlgInit = TRUE;
	UpdateTreeItem();

	return TRUE;
}

void CSkillTreePage::OnShowWindow ( BOOL bShow, UINT nStatus )
{
	if ( bShow )
		UpdateTreeItem();
}

void CSkillTreePage::SetPropertyData ( int nCallPage, SNATIVEID NativeID )
{
	m_CalledPage = nCallPage;
	m_NativeID = NativeID;

	m_bNativeIDInit = TRUE;
}

BOOL CSkillTreePage::UpdateTreeItem()
{
	if ( m_bDlgInit )
	{
		m_ctrlItemTree.DeleteAllItems();

		HTREEITEM hSelectItem = NULL;

		for ( WORD MID = 0; MID < EMSKILLCLASS_NSIZE; ++MID )
		{
			HTREEITEM hMainItem = NULL;
			HTREEITEM hSubItem	= NULL;

			for ( WORD SID = 0; SID < GLSkillMan::MAX_CLASSSKILL; ++SID )
			{
				PGLSKILL pItem = GLSkillMan::GetInstance().GetData ( MID, SID );

				if ( pItem )
				{
					if ( hMainItem )
					{
						const char* szName = pItem->GetName();
						hSubItem = m_ctrlItemTree.InsertItem ( szName, hMainItem );
						m_ctrlItemTree.SetItemData ( hSubItem, (DWORD_PTR)pItem );

						if ( m_bNativeIDInit && m_NativeID.wMainID == MID && m_NativeID.wSubID == SID )
						{
							hSelectItem = hSubItem;
						}
					}
					else
					{
						const char* szName = pItem->GetName();
						hMainItem = m_ctrlItemTree.InsertItem ( szName );
						m_ctrlItemTree.SetItemData ( hMainItem, (DWORD_PTR)pItem );

						if ( m_bNativeIDInit && m_NativeID.wMainID == MID && m_NativeID.wSubID == SID )
						{
							hSelectItem = hMainItem;
						}
					}
				}
			}
		}
		
		m_ctrlItemTree.SelectItem ( hSelectItem );

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CSkillTreePage::OnBnClickedButtonSkillOk()
{
	// TODO: Add your control notification handler code here
	HTREEITEM hSelectItem = m_ctrlItemTree.GetSelectedItem();
	PGLSKILL pItem = (PGLSKILL)m_ctrlItemTree.GetItemData ( hSelectItem );

	if ( m_CalledPage == REQUIREPAGE )
	{
		m_pSheetTab->m_RequirePage.SetReqSkill ( pItem->m_sBASIC.sNATIVEID );
	}

	m_pSheetTab->SetActivePage ( m_CalledPage );
	m_bNativeIDInit = FALSE;
}

void CSkillTreePage::OnBnClickedButtonSkillCancel()
{
	// TODO: Add your control notification handler code here
	m_pSheetTab->SetActivePage ( m_CalledPage );
	m_bNativeIDInit = FALSE;
}
