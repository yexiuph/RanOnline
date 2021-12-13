// CrowTree.cpp : implementation file
//

#include "pch.h"
#include "LevelEditor.h"
#include "CrowTree.h"
#include "SheetWithTab.h"

#include "EtcFunction.h"

#include "GLDefine.h"

#include "GLCrowData.h"

// CCrowTree dialog

IMPLEMENT_DYNAMIC(CCrowTree, CPropertyPage)
CCrowTree::CCrowTree()
	: CPropertyPage(CCrowTree::IDD)
	, m_CalledPage ( -1 )
	, m_bDlgInit ( FALSE )
	, m_bNativeIDInit ( FALSE )
{
}

CCrowTree::~CCrowTree()
{
}

void CCrowTree::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_CROW, m_ctrlItemTree);
}


BEGIN_MESSAGE_MAP(CCrowTree, CPropertyPage)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_CROW_OK, OnBnClickedButtonCrowOk)
	ON_BN_CLICKED(IDC_BUTTON_CROW_CANCEL, OnBnClickedButtonCrowCancel)
END_MESSAGE_MAP()


// CCrowTree message handlers
BOOL CCrowTree::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bDlgInit = TRUE;
	UpdateTreeItem();

	return TRUE;
}

void CCrowTree::OnShowWindow ( BOOL bShow, UINT nStatus )
{
	if ( bShow )
		UpdateTreeItem();
}

void CCrowTree::SetPropertyData ( int nCallPage, SNATIVEID NativeID )
{
	m_CalledPage = nCallPage;
	m_NativeID = NativeID;

	m_bNativeIDInit = TRUE;
}

BOOL CCrowTree::UpdateTreeItem()
{
	if ( m_bDlgInit )
	{
		m_ctrlItemTree.DeleteAllItems();

		HTREEITEM hSelectItem = NULL;

		for ( WORD MID = 0; MID < GLCrowDataMan::GetInstance().MAX_CROW_MID; ++MID )
		{
			HTREEITEM hMainItem = NULL;

			for ( WORD SID = 0; SID < GLCrowDataMan::GetInstance().MAX_CROW_SID; ++SID )
			{
				PCROWDATA pItem = GLCrowDataMan::GetInstance().GetCrowData ( MID, SID );

				if ( pItem )
				{
					if ( hMainItem )
					{
						const char* szName = pItem->GetName();
						HTREEITEM hSubItem = m_ctrlItemTree.InsertItem ( szName, hMainItem );
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

void CCrowTree::OnBnClickedButtonCrowOk()
{
	HTREEITEM hSelectItem = m_ctrlItemTree.GetSelectedItem();
	PCROWDATA pItem = (PCROWDATA)m_ctrlItemTree.GetItemData ( hSelectItem );

	if ( m_CalledPage == MOBSETPAGE )
	{
		m_pSheetTab->m_MobSetPage.SetNewID ( pItem->m_sBasic.sNativeID.wMainID, pItem->m_sBasic.sNativeID.wSubID );
	}

	m_pSheetTab->SetActivePage ( m_CalledPage );
	m_bNativeIDInit = FALSE;
}

void CCrowTree::OnBnClickedButtonCrowCancel()
{
	m_pSheetTab->SetActivePage ( m_CalledPage );
	m_bNativeIDInit = FALSE;
}
