// ItemTreePage.cpp : implementation file
//

#include "pch.h"
#include "LevelEditor.h"
#include "ItemTreePage.h"
#include "SheetWithTab.h"

#include "EtcFunction.h"

#include "GLDefine.h"

#include "GLItem.h"
#include "GLItemMan.h"
#include ".\itemtreepage.h"

// CItemTreePage dialog

IMPLEMENT_DYNAMIC(CItemTreePage, CPropertyPage)
CItemTreePage::CItemTreePage()
	: CPropertyPage(CItemTreePage::IDD)
	, m_CalledPage ( -1 )
	, m_bDlgInit ( FALSE )
	, m_bNativeIDInit ( FALSE )
{
}

CItemTreePage::~CItemTreePage()
{
}

void CItemTreePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ITEM, m_ctrlItemTree);
}


BEGIN_MESSAGE_MAP(CItemTreePage, CPropertyPage)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_ITEM_OK, OnBnClickedButtonItemOk)
	ON_BN_CLICKED(IDC_BUTTON_ITEM_CANCEL, OnBnClickedButtonItemCancel)
END_MESSAGE_MAP()


// CItemTreePage message handlers
BOOL CItemTreePage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bDlgInit = TRUE;
	UpdateTreeItem();

	return TRUE;
}

void CItemTreePage::OnShowWindow ( BOOL bShow, UINT nStatus )
{
	if ( bShow )
		UpdateTreeItem();
}

void CItemTreePage::SetPropertyData ( int nCallPage, SNATIVEID NativeID )
{
	m_CalledPage = nCallPage;
	m_NativeID = NativeID;

	m_bNativeIDInit = TRUE;
}

BOOL CItemTreePage::UpdateTreeItem()
{
	if ( m_bDlgInit )
	{
		m_ctrlItemTree.DeleteAllItems();

		HTREEITEM hSelectItem = NULL;

		for ( WORD MID = 0; MID < GLItemMan::GetInstance().MAX_MID; ++MID )
		{
			HTREEITEM hMainItem = NULL;
			HTREEITEM hSubItem	= NULL;

			for ( WORD SID = 0; SID < GLItemMan::GetInstance().MAX_SID; ++SID )
			{
				SITEM* pItem = GLItemMan::GetInstance().GetItem ( MID, SID );

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

void CItemTreePage::OnBnClickedButtonItemOk()
{
	// TODO: Add your control notification handler code here
	HTREEITEM hSelectItem = m_ctrlItemTree.GetSelectedItem();
	SITEM* pItem = (SITEM*)m_ctrlItemTree.GetItemData ( hSelectItem );

	if ( m_CalledPage == REQUIREPAGE )
	{
		m_pSheetTab->m_RequirePage.SetReqItem ( pItem->sBasicOp.sNativeID );
	}

	m_pSheetTab->SetActivePage ( m_CalledPage );
	m_bNativeIDInit = FALSE;
}

void CItemTreePage::OnBnClickedButtonItemCancel()
{
	// TODO: Add your control notification handler code here
	m_pSheetTab->SetActivePage ( m_CalledPage );
	m_bNativeIDInit = FALSE;
}
