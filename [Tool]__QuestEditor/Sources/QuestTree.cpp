// QuestTree.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "QuestEdit.h"
#include "QuestTree.h"
#include "SheetWithTab.h"
#include "InputIDDlg.h"
#include ".\questtree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CQuestTree 대화 상자입니다.

IMPLEMENT_DYNAMIC(CQuestTree, CPropertyPage)
CQuestTree::CQuestTree( LOGFONT logfont )
	: CPropertyPage(CQuestTree::IDD),
	m_pFont ( NULL )
{
	m_bDlgInit = FALSE;
	m_pDummyQuestNode = new QUESTNODE;
	m_pDummyQuestNode->pQUEST = new GLQUEST;
	m_bDummyHasQuest = FALSE;

	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CQuestTree::~CQuestTree()
{
	SAFE_DELETE ( m_pDummyQuestNode->pQUEST );
	SAFE_DELETE ( m_pDummyQuestNode );	
	SAFE_DELETE ( m_pFont );
}

void CQuestTree::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ITEM, m_ctrlQuestTree);
}


BEGIN_MESSAGE_MAP(CQuestTree, CPropertyPage)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_ITEM, OnNMRclickTreeItem)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_ITEM, OnNMDblclkTreeItem)
END_MESSAGE_MAP()


// CQuestTree 메시지 처리기입니다.

BOOL CQuestTree::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// PropertyPage 폰트 강제 변경
	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_bDlgInit = TRUE;
	UpdateTree ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CQuestTree::OnNMRclickTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	POINT OriginPoint;
	GetCursorPos ( &OriginPoint );

	//	우클릭 강제 설정
	ForceSelectOnRightClick ( OriginPoint );

	//	메뉴 선택 & 선택 메뉴 실행
	RunSelectMenu ( GetSelectMenu ( OriginPoint ) );
}

void CQuestTree::RunSelectMenu ( int nSelect )
{
	HTREEITEM hItem = m_ctrlQuestTree.GetSelectedItem ();

	PQUESTNODE pQuestNode = NULL;
	if ( hItem )
	{
		pQuestNode = (PQUESTNODE)m_ctrlQuestTree.GetItemData ( hItem );
	}

	switch ( nSelect )
	{
	case ID_MENUITEM_QUEST_MODIFY:
		{
			if ( GetSelectItem () )
			{				
				m_pSheetTab->ActiveQuestPage ( QUESTTREE_PAGE, pQuestNode );
			}			
		}
		break;
	case ID_MENUITEM_QUESTSTEP_MODIFY:
		{
			if ( GetSelectItem () )
			{
				m_pSheetTab->ActiveQuestStepPage ( QUESTTREE_PAGE, pQuestNode );
			}			
		}
		break;
	case ID_MENUITEM_ADD:
		{
			AddQuest ();
		}
		break;
	case ID_MENUITEM_DEL:
		{
			if ( GetSelectItem () )
			{
				if ( MessageBox ( "Do you want to delete?", "Question", MB_YESNO ) == IDYES )
				{
					DelQuest();
				}
			}
		}
		break;
	case ID_MENUITEM_COPY:
		{
			CopyQuest ();
		}
		break;
	case ID_MENUITEM_PASTE_NEW:
		{
			PasteQuestNew ();
		}
		break;
	case ID_MENUITEM_PASTE_VALUE:
		{
			PasteQuestValue ();
		}
		break;
	};
}

int	 CQuestTree::GetSelectMenu ( POINT pt )
{
	//	<--	실제 작동 메뉴 따기
	int pMenuID = 0;
	CMenu Menu;

	int SelectionMade = -1;
	//switch ( m_SelectMenu )
	//{
	//case 0:
	//	{
		VERIFY ( Menu.LoadMenu ( IDR_MENU_QUEST_POPUP ) );

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

BOOL	CQuestTree::UpdateTree ()
{	
	if ( !m_bDlgInit )
	{
		return FALSE;
	}

	CleanAllQuest ();

	GLQuestMan::MAPQUEST& m_mapQuestMap = GLQuestMan::GetInstance().GetMap();

	GLQuestMan::MAPQUEST_ITER iter_pos;
	DWORD dwQuestID = 0;
	PQUESTNODE pQuestNode = NULL;
	CString strItem;

	for ( iter_pos = m_mapQuestMap.begin(); iter_pos != m_mapQuestMap.end(); iter_pos++ )
	{
		HTREEITEM hMainItem= NULL;
		
		dwQuestID = (DWORD) iter_pos->first;	
		pQuestNode = (PQUESTNODE) &iter_pos->second;

        if ( pQuestNode ) 
		{	
			strItem = GetFormatName ( pQuestNode );
			hMainItem = m_ctrlQuestTree.InsertItem ( strItem, m_TreeRoot );
			m_ctrlQuestTree.SetItemData ( hMainItem, (DWORD_PTR)pQuestNode );
		}
	}

	return TRUE;

}

void CQuestTree::CleanAllQuest ()
{
	if ( !m_bDlgInit )
	{
		return ;
	}

	m_ctrlQuestTree.DeleteAllItems ();
	m_TreeRoot = m_ctrlQuestTree.InsertItem ( "Root Node" );
}

BOOL	CQuestTree::CopyQuest ()
{
	HTREEITEM hMainItem = m_ctrlQuestTree.GetSelectedItem ();
	if ( hMainItem && hMainItem != m_TreeRoot )
	{
		PQUESTNODE pQuestNode = (PQUESTNODE)m_ctrlQuestTree.GetItemData ( hMainItem );
		*(m_pDummyQuestNode->pQUEST) = *(pQuestNode->pQUEST);
		m_pDummyQuestNode->strFILE = pQuestNode->strFILE;
		m_bDummyHasQuest = TRUE;
		return TRUE;
	}

	return FALSE;
}

void CQuestTree::SetSelectItem ( PQUESTNODE pQuestNode )
{
	m_pQuestNode = pQuestNode;
}

PQUESTNODE CQuestTree::GetSelectItem ()
{
	return m_pQuestNode;
}


BOOL CQuestTree::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	switch ( pMsg->message )
	{
	case WM_KEYDOWN:
		{
			if ( pMsg->wParam == VK_RETURN )
			{
				HTREEITEM hItem = m_ctrlQuestTree.GetSelectedItem ();
				if ( !hItem ) return TRUE;

				PQUESTNODE pQuestNode = (PQUESTNODE)m_ctrlQuestTree.GetItemData ( hItem );
				if ( !pQuestNode )
				{
					return TRUE;
				}

				m_pSheetTab->ActiveQuestPage ( QUESTTREE_PAGE, pQuestNode );
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
					HTREEITEM hItem = m_ctrlQuestTree.GetSelectedItem ();
					PQUESTNODE pQuestNode = (PQUESTNODE)m_ctrlQuestTree.GetItemData ( hItem );
					SetSelectItem ( pQuestNode );

					RunSelectMenu ( ID_MENUITEM_PASTE_NEW );
				}
			}

			if ( ::GetKeyState ( VK_CONTROL ) < 0 )
			{
				if ( pMsg->wParam == 'B' )
				{
					HTREEITEM hItem = m_ctrlQuestTree.GetSelectedItem ();
					PQUESTNODE pQuestNode = (PQUESTNODE)m_ctrlQuestTree.GetItemData ( hItem );
					SetSelectItem ( pQuestNode );

					RunSelectMenu ( ID_MENUITEM_PASTE_VALUE );
				}
			}

		}
		break;
	}

	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CQuestTree::SetReadMode( CMenu* pMenu )
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

	pMenu->EnableMenuItem ( ID_MENUITEM_ADD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem ( ID_MENUITEM_DEL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem ( ID_MENUITEM_COPY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem ( ID_MENUITEM_PASTE_NEW, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem ( ID_MENUITEM_PASTE_VALUE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );

#endif
}



void CQuestTree::OnNMDblclkTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	HTREEITEM hItem = m_ctrlQuestTree.GetSelectedItem ();

	PQUESTNODE pQuestNode = (PQUESTNODE)m_ctrlQuestTree.GetItemData ( hItem );
	m_pSheetTab->ActiveQuestPage ( QUESTTREE_PAGE, pQuestNode );
}
