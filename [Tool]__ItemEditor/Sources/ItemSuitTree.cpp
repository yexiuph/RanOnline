// AllItemView.cpp : ���� �����Դϴ�.
//

#include "pch.h"
#include <vector>
#include "ItemEdit.h"
#include "SheetWithTab.h"
#include "ItemSuitTree.h"
#include ".\itemsuittree.h"

// CItemSuitTree ��ȭ �����Դϴ�.

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CItemSuitTree, CPropertyPage)
CItemSuitTree::CItemSuitTree( LOGFONT logfont )
	: CPropertyPage(CItemSuitTree::IDD),
	m_pFont( NULL )
{
	m_bDlgInit = FALSE;
	m_pDummyItemNode = new CItemNode;	
	m_bDummyHasItem = FALSE;

	
	
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CItemSuitTree::~CItemSuitTree()
{
	SAFE_DELETE ( m_pDummyItemNode );
	SAFE_DELETE ( m_pFont );
}

void CItemSuitTree::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ITEM, m_ctrlItemTree);
}


BEGIN_MESSAGE_MAP(CItemSuitTree, CPropertyPage)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_ITEM, OnNMRclickTreeItem)
	ON_NOTIFY(NM_CLICK, IDC_TREE_ITEM, OnNMClickTreeItem)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_ITEM, OnNMDblclkTreeItem)
END_MESSAGE_MAP()


// CItemSuitTree �޽��� ó�����Դϴ�.

BOOL CItemSuitTree::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_bDlgInit = TRUE;
	UpdateTree ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CItemSuitTree::OnNMRclickTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;	

	POINT OriginPoint;
	GetCursorPos ( &OriginPoint );

	//	��Ŭ�� ���� ����
	ForceSelectOnRightClick ( OriginPoint );

	//	�޴� ���� & ���� �޴� ����
	RunSelectMenu ( GetSelectMenu ( OriginPoint ) );
}

void CItemSuitTree::RunSelectMenu ( int nSelect )
{
	HTREEITEM hItem = m_ctrlItemTree.GetSelectedItem ();

	PITEMNODE pItemNode = NULL;
	if ( hItem )
	{
		pItemNode = (PITEMNODE)m_ctrlItemTree.GetItemData ( hItem );
	}

	switch ( nSelect )
	{
	case ID_MENUITEM_PARENTMODIFY:
		{
			if ( GetSelectItem () )
			{				
				m_pSheetTab->ActiveItemParentPage ( ITEMSUITTREEPAGE, &pItemNode->m_sItem );
			}			
		}
		break;
	case ID_MENUITEM_SUITMODIFY:
		{
			if ( GetSelectItem () )
			{
				m_pSheetTab->ActiveItemSuitPage ( ITEMSUITTREEPAGE, &pItemNode->m_sItem );
			}			
		}
		break;
	case ID_MENUITEM_ADD:
		{
			AddItem ();
		}
		break;
	case ID_MENUITEM_DEL:
		{
			if ( GetSelectItem () )
			{
				if ( MessageBox ( "Do you want to delete?", "Question", MB_YESNO ) == IDYES )
				{
					DelItem();
				}
			}
		}
		break;
	case ID_MENUITEM_COPY:
		{
			CopyItem ();
		}
		break;
	case ID_MENUITEM_PASTE_NEW:
		{
			PasteItemNew ();
		}
		break;
	case ID_MENUITEM_PASTE_VALUE:
		{
			PasteItemValue ();
		}
		break;
	};
}

int	 CItemSuitTree::GetSelectMenu ( POINT pt )
{
	//	<--	���� �۵� �޴� ����
	int pMenuID = 0;
	CMenu Menu;

	int SelectionMade = -1;
	//switch ( m_SelectMenu )
	//{
	//case 0:
	//	{
		VERIFY ( Menu.LoadMenu ( IDR_MENU_ITEMSUIT_POPUP ) );

		CMenu* pPopup = Menu.GetSubMenu ( pMenuID );
		ASSERT ( pPopup != NULL );	

		SetReadMode( pPopup );

		SelectionMade = pPopup->TrackPopupMenu (
			TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
			pt.x, pt.y, this );

		pPopup->DestroyMenu ();
	//	}
	//	break;
	//}
	//	-->	

	return SelectionMade;
}

BOOL	CItemSuitTree::UpdateTree ()
{	
	if ( !m_bDlgInit )
	{
		return FALSE;
	}

	CleanAllItem ();

	for ( int MID = 0; MID < GLItemMan::MAX_MID; MID++ )
	{
		HTREEITEM hMainItem= NULL;
		HTREEITEM hSubItem = NULL;

		for ( int SID = 0; SID < GLItemMan::MAX_SID; SID++ )
		{        		
			SITEM* pItem = GLItemMan::GetInstance().GetItem ( MID, SID );

			if ( pItem )
			{
				CString strItem = GetFormatName ( pItem );
				if ( !hMainItem )
				{
					hMainItem = m_ctrlItemTree.InsertItem ( strItem, m_TreeRoot );
					m_ctrlItemTree.SetItemData ( hMainItem, (DWORD_PTR)pItem );

					m_TreeItemList[MID][SID] = hMainItem;
				}
				else
				{
					hSubItem = m_ctrlItemTree.InsertItem ( strItem, hMainItem );
					m_ctrlItemTree.SetItemData ( hSubItem, (DWORD_PTR)pItem );

					m_TreeItemList[MID][SID] = hSubItem;
				}				
			}
		}
	}

	return TRUE;
}



void CItemSuitTree::CleanAllItem ()
{
	if ( !m_bDlgInit )
	{
		return ;
	}

	m_ctrlItemTree.DeleteAllItems ();
	m_TreeRoot = m_ctrlItemTree.InsertItem ( "Root Node" );
}

BOOL	CItemSuitTree::CopyItem ()
{
	HTREEITEM hMainItem = m_ctrlItemTree.GetSelectedItem ();
	if ( hMainItem && hMainItem != m_TreeRoot )
	{
		*m_pDummyItemNode = *((PITEMNODE)m_ctrlItemTree.GetItemData ( hMainItem ));
		m_bDummyHasItem = TRUE;
		return TRUE;
	}

	return FALSE;
}

BOOL	CItemSuitTree::PasteItemNew ()
{
	if ( !m_bDummyHasItem )
	{
		return FALSE;
	}

	HTREEITEM hMainItem = m_ctrlItemTree.GetSelectedItem ();
	if ( hMainItem && GetSelectItem () )	//	����� �߰���
	{
		WORD MID = GetSelectItem()->m_sItem.sBasicOp.sNativeID.wMainID;
		WORD SID = GLItemMan::GetInstance().FindFreeSID ( MID );
		if ( SID == ITEMID_NOTFOUND )
		{
			return FALSE;
		}

		CItemNode sItemNode;
		sItemNode = *m_pDummyItemNode;
		sItemNode.m_sItem.sBasicOp.sNativeID.wMainID= MID;
		sItemNode.m_sItem.sBasicOp.sNativeID.wSubID = SID;

		GLItemMan::GetInstance().InsertItem ( MID, SID, &sItemNode );
		SITEM *pITEM = GLItemMan::GetInstance().GetItem ( MID, SID );

		HTREEITEM hParentItem = m_ctrlItemTree.GetParentItem ( hMainItem );
		if ( hParentItem != m_TreeRoot )
		{
			UpdateItem (pITEM, hParentItem );
		}
		else
		{
			UpdateItem ( pITEM, hMainItem );
		}

		return TRUE;
	}
	else	//	�������� �߰���
	{
		WORD MID = GLItemMan::GetInstance().FindFreeMID ();
		if ( MID == ITEMID_NOTFOUND )
		{
			return FALSE;
		}
		WORD SID = GLItemMan::GetInstance().FindFreeSID ( MID );
		if ( SID == ITEMID_NOTFOUND )
		{
			return FALSE;
		}
		
		CItemNode sItemNode;
		sItemNode = *m_pDummyItemNode;
		sItemNode.m_sItem.sBasicOp.sNativeID.wMainID = MID;
		sItemNode.m_sItem.sBasicOp.sNativeID.wSubID = SID;

		GLItemMan::GetInstance().InsertItem ( MID, SID, &sItemNode );
		SITEM *pITEM = GLItemMan::GetInstance().GetItem ( MID, SID );

		UpdateItem ( pITEM, NULL );
		
		return TRUE;
	}
}

BOOL	CItemSuitTree::PasteItemValue ()
{
	if ( !m_bDummyHasItem )
	{
		return FALSE;
	}

	HTREEITEM hMainItem = m_ctrlItemTree.GetSelectedItem ();
	if ( hMainItem && GetSelectItem () )
	{		
		WORD MID = GetSelectItem()->m_sItem.sBasicOp.sNativeID.wMainID;
		WORD SID = GetSelectItem()->m_sItem.sBasicOp.sNativeID.wSubID;

		*(GetSelectItem()) = *m_pDummyItemNode;

		GetSelectItem()->m_sItem.sBasicOp.sNativeID.wMainID= MID;
		GetSelectItem()->m_sItem.sBasicOp.sNativeID.wSubID = SID;

		m_ctrlItemTree.SetItemText ( hMainItem, GetFormatName ( &GetSelectItem()->m_sItem ) );
		
		return TRUE;
	}

	return FALSE;
}

BOOL CItemSuitTree::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	switch ( pMsg->message )
	{
	case WM_KEYDOWN:
		{
			if ( pMsg->wParam == VK_RETURN )
			{
				HTREEITEM hItem = m_ctrlItemTree.GetSelectedItem ();
				if ( !hItem ) return TRUE;

				PITEMNODE pItemNode = (PITEMNODE)m_ctrlItemTree.GetItemData ( hItem );
				if ( !pItemNode )
				{
					return TRUE;
				}

				m_pSheetTab->ActiveItemParentPage ( ITEMSUITTREEPAGE, &pItemNode->m_sItem );
			}

			if ( ::GetKeyState ( VK_CONTROL ) < 0 )
			{
				if ( pMsg->wParam == 'C' )
				{
					RunSelectMenu ( ID_MENUITEM_COPY );
				}
			}

			if ( ::GetKeyState ( VK_CONTROL ) < 0 )
			{
				if ( pMsg->wParam == 'V' )
				{
					HTREEITEM hItem = m_ctrlItemTree.GetSelectedItem ();
					PITEMNODE pItemNode = (PITEMNODE)m_ctrlItemTree.GetItemData ( hItem );
					SetSelectItem ( pItemNode );

					RunSelectMenu ( ID_MENUITEM_PASTE_NEW );
				}
			}

			if ( ::GetKeyState ( VK_CONTROL ) < 0 )
			{
				if ( pMsg->wParam == 'B' )
				{
					HTREEITEM hItem = m_ctrlItemTree.GetSelectedItem ();
					PITEMNODE pItemNode = (PITEMNODE)m_ctrlItemTree.GetItemData ( hItem );
					SetSelectItem ( pItemNode );

					RunSelectMenu ( ID_MENUITEM_PASTE_VALUE );
				}
			}

		}
		break;
	}

	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CItemSuitTree::SetSelectItem ( PITEMNODE pItemNode )
{
	m_pItemNode = pItemNode;
}

PITEMNODE CItemSuitTree::GetSelectItem ()
{
	return m_pItemNode;
}
void CItemSuitTree::OnNMClickTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}


void CItemSuitTree::SetReadMode( CMenu* pMenu )
{
//  �б����� ����϶��� �����Ѵ�. 
//  ���ҽ� �߰��� ���� ��� 
#ifdef READTOOL_PARAM

	pMenu->EnableMenuItem ( ID_MENUITEM_ADD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem ( ID_MENUITEM_DEL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem ( ID_MENUITEM_COPY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem ( ID_MENUITEM_PASTE_NEW, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem ( ID_MENUITEM_PASTE_VALUE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );

#endif
}
void CItemSuitTree::OnNMDblclkTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	HTREEITEM	hItem = m_ctrlItemTree.GetSelectedItem ();
	if (hItem && hItem != m_TreeRoot)
	{
		PITEMNODE pItemNode = (PITEMNODE)m_ctrlItemTree.GetItemData(hItem);
		m_pSheetTab->ActiveItemParentPage(ITEMSUITTREEPAGE, &pItemNode->m_sItem);
	}
}
