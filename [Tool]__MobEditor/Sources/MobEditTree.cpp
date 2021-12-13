// MobEditTree.cpp : implementation file
//

#include "pch.h"
#include <vector>
#include "MobEdit.h"
#include "SheetWithTab.h"
#include "MobEditTree.h"
#include ".\mobedittree.h"
#include "GLStringTable.h"


// MobEditTree dialog

IMPLEMENT_DYNAMIC(MobEditTree, CPropertyPage)
MobEditTree::MobEditTree(LOGFONT logfont)
	: CPropertyPage(MobEditTree::IDD)
	,m_pFont( NULL )
{
	m_bDlgInit = FALSE;
	m_pDummyCrow = new SCROWDATA;	
	m_bDummyHasCrow = FALSE;

	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

MobEditTree::~MobEditTree()
{
}

void MobEditTree::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_CROW, m_ctrlCrowTree);
}


BEGIN_MESSAGE_MAP(MobEditTree, CPropertyPage)
	ON_NOTIFY(NM_CLICK, IDC_TREE_CROW, OnClickLeft)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_CROW, OnClickRight)
END_MESSAGE_MAP()


// MobEditTree message handlers
BOOL MobEditTree::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );
	m_bDlgInit = TRUE;
	UpdateTree ();

	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL	MobEditTree::UpdateTree ()
{	
	if ( !m_bDlgInit )
	{
		return FALSE;
	}

	m_ctrlCrowTree.DeleteAllItems();
	m_TreeRoot = m_ctrlCrowTree.InsertItem ( "Root Node" );

	for ( int MID = 0; MID < GLCrowDataMan::MAX_CROW_MID; MID++ )
	{
		HTREEITEM hMainCrow= NULL;
		HTREEITEM hSubCrow = NULL;

		for ( int SID = 0; SID < GLCrowDataMan::MAX_CROW_SID; SID++ )
		{        	

			PCROWDATA pCrow = GLCrowDataMan::GetInstance().GetCrowData ( MID, SID );
			if ( pCrow )
			{
				CString strCrow;
				strCrow = GetFormatName ( pCrow );

				if ( !hMainCrow )
				{
					hMainCrow =m_ctrlCrowTree.InsertItem ( strCrow, m_TreeRoot );
					m_ctrlCrowTree.SetItemData ( hMainCrow, (DWORD_PTR)pCrow );
				}
				else
				{
					hSubCrow = m_ctrlCrowTree.InsertItem ( strCrow, hMainCrow );
					m_ctrlCrowTree.SetItemData ( hSubCrow, (DWORD_PTR)pCrow );
				}
			}
		}
	}

	return TRUE;
}
const char* MobEditTree::GetFormatName ( PCROWDATA pCrow  )
{
	GASSERT ( pCrow );
	static	CString strCrow;
	if ( pCrow )
	{	
		strCrow.Format ( "[%03d/%03d][LVL:%d]  %s",
		pCrow->m_sBasic.sNativeID.wMainID ,pCrow->m_sBasic.sNativeID.wSubID , pCrow->m_sBasic.m_wLevel, pCrow->GetName() );
	}
	return strCrow.GetString ();
}
BOOL MobEditTree::UpdateCrow ( PCROWDATA pCrow, HTREEITEM hMainCrow )
{
	CString strCrow;
	strCrow = GetFormatName ( pCrow );

	if ( !hMainCrow )
	{
		hMainCrow = m_ctrlCrowTree.InsertItem ( strCrow, m_TreeRoot );
		m_ctrlCrowTree.SetItemData ( hMainCrow, (DWORD_PTR)pCrow );
	}
	else
	{
		hSubCrow = m_ctrlCrowTree.InsertItem ( strCrow, hMainCrow );
		m_ctrlCrowTree.SetItemData ( hSubCrow, (DWORD_PTR)pCrow );
	}
	return TRUE;
}
void MobEditTree::OnClickLeft(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
}

void MobEditTree::OnClickRight(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;	
	POINT OriginPoint;
	GetCursorPos ( &OriginPoint );
	ForceSelectOnRightClick ( OriginPoint );
	RunSelectMenu ( GetSelectMenu ( OriginPoint ) );
}
void MobEditTree::ForceSelectOnRightClick ( POINT pt )
{		
	m_ctrlCrowTree.ScreenToClient( &pt );
	UINT uFlag = 0;
	PCROWDATA pCrowWork = NULL;
	HTREEITEM hCrow = m_ctrlCrowTree.HitTest ( CPoint(pt), &uFlag );

    SetSelectCrow ( NULL );	
	if ( hCrow )
	{
		m_ctrlCrowTree.SelectItem ( hCrow );
		if ( uFlag & TVHT_ONITEMLABEL )
		{			
			SetSelectCrow ( (PCROWDATA)m_ctrlCrowTree.GetItemData ( hCrow ) );
		}
	}
}
void MobEditTree::SetSelectCrow ( PCROWDATA pCrow )
{
	m_pCrow = pCrow;
}

PCROWDATA MobEditTree::GetSelectCrow ()
{
	return m_pCrow;
}
int	 MobEditTree::GetSelectMenu ( POINT pt )
{
	int pMenuID = 0;
	CMenu Menu;
	int SelectionMade = -1;

	VERIFY ( Menu.LoadMenu ( IDR_MENU_CROW ) );
	CMenu* pPopup = Menu.GetSubMenu ( pMenuID );
	ASSERT ( pPopup != NULL );	

	SelectionMade = pPopup->TrackPopupMenu (
		TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
		pt.x, pt.y, this );

	pPopup->DestroyMenu ();
	return SelectionMade;
}
void MobEditTree::RunSelectMenu ( int nSelect )
{
	HTREEITEM hCrow = m_ctrlCrowTree.GetSelectedItem ();
	PCROWDATA pCrow = NULL;
	if ( hCrow )
	{
		pCrow = (PCROWDATA)m_ctrlCrowTree.GetItemData ( hCrow );
	}

	switch ( nSelect )
	{
	case ID_MENU_CROWDATAEDIT:
		{
			if ( GetSelectCrow () )
			{				
				m_pSheetTab->ActiveCrowDataPage ( CROWDATAPAGE, pCrow );
			}			
		}
		break;
	case ID_MENU_CROWATTACKEDIT:
		{
			if ( GetSelectCrow () )
			{
				m_pSheetTab->ActiveCrowAttackPage ( CROWATTACKPAGE, pCrow );
			}			
		}
		break;
	case ID_MENU_ADD:
		{
			AddCrow ();
		}
		break;
	case ID_MENU_DELETE:
		{
			if ( GetSelectCrow () )
			{
				CString strTemp;
				strTemp.Format ( "Do you want to delete? [%03d/%03d] %s"
					,GetSelectCrow()->m_sBasic.sNativeID.wMainID
					,GetSelectCrow()->m_sBasic.sNativeID.wSubID
					,GetSelectCrow()->GetName() );

				if ( MessageBox ( strTemp, "Question", MB_YESNO ) == IDYES )
				{
					DelCrow();
				}
			}
		}
		break;
	case ID_MENU_COPY:
		{
			CopyCrow();
		}
		break;
	case ID_MENU_PASTENEW:
		{
			PasteCrowNew();
		}
		break;
	case ID_MENU_PASTEVALUE:
		{
			PasteCrowValue ();
		}
		break;
	case ID_MENU_JDEV:
		{
			MessageBox ( "If you Found Bugs, you can contact me Yahoo ID: mAc14_89@yahoo.com", "Hi!", MB_OK );
		}
		break;
	};
}


BOOL	MobEditTree::AddCrow ()
{
	HTREEITEM hMainCrow = m_ctrlCrowTree.GetSelectedItem ();
	if ( hMainCrow && GetSelectCrow () )
	{		
		WORD MID = GetSelectCrow()->m_sBasic.sNativeID.wMainID ;
		WORD SID = GLCrowDataMan::GetInstance().FindFreeCrowDataSID( MID );
		if ( SID == 65535 )
		{
			return FALSE;
		}

		PCROWDATA pCrow;
		pCrow = new SCROWDATA;
		pCrow->m_sBasic.sNativeID.wMainID = MID;
		pCrow->m_sBasic.sNativeID.wSubID = SID;

		GLCrowDataMan::GetInstance().InsertCrowData ( MID, SID, pCrow );
		PCROWDATA pCROW =  GLCrowDataMan::GetInstance().GetCrowData ( MID, SID );

		HTREEITEM hParentCrow = m_ctrlCrowTree.GetParentItem ( hMainCrow );
		if ( hParentCrow != m_TreeRoot )
		{
			UpdateCrow ( pCROW, hParentCrow );
		}
		else
		{
			UpdateCrow ( pCROW, hMainCrow );
		}

		return TRUE;
	}
	else
	{		
		WORD MID = GLCrowDataMan::GetInstance().FindFreeCrowDataMID();
		if ( MID == 65535 )
		{
			return FALSE;
		}
		WORD SID = GLCrowDataMan::GetInstance().FindFreeCrowDataSID( MID );
		if ( SID == 65535 )
		{
			return FALSE;
		}

		PCROWDATA pCrow;
		pCrow = new SCROWDATA;
		pCrow->m_sBasic.sNativeID.wMainID = MID;
		pCrow->m_sBasic.sNativeID.wSubID = SID;

		GLCrowDataMan::GetInstance().InsertCrowData ( MID, SID, pCrow );
		PCROWDATA pCROW =  GLCrowDataMan::GetInstance().GetCrowData ( MID, SID );

		UpdateCrow ( pCROW, NULL );

		return TRUE;
	}
	return FALSE;
}

BOOL MobEditTree::DelCrow ()
{
	if ( GetSelectCrow() )
	{
		WORD MID = GetSelectCrow()->m_sBasic.sNativeID.wMainID ;
		WORD SID = GetSelectCrow()->m_sBasic.sNativeID.wSubID;

		if ( SID == 0 )	
		{
			for ( int i = 0; i < GLCrowDataMan::MAX_CROW_SID; i++ )
			{
				GLStringTable::GetInstance().DeleteString( MID, i, GLStringTable::CROW );
				GLStringTable::GetInstance().DeleteString( MID , i, GLStringTable::SKILL );
				GLCrowDataMan::GetInstance().DeleteCrowData ( MID,i );
			}
		}
		else
		{
			GLStringTable::GetInstance().DeleteString( MID , SID, GLStringTable::CROW );
			GLCrowDataMan::GetInstance().DeleteCrowData ( MID,SID);
		}
		m_ctrlCrowTree.DeleteItem ( m_ctrlCrowTree.GetSelectedItem () );

		return TRUE;	
	}
	return FALSE;
}

BOOL	MobEditTree::CopyCrow ()
{
	HTREEITEM hCrow = m_ctrlCrowTree.GetSelectedItem ();
	if ( hCrow  )
	{
		PCROWDATA m_pCrowCopy;
		m_pCrowCopy = (PCROWDATA)m_ctrlCrowTree.GetItemData ( hCrow );
		m_pDummyCrow = new SCROWDATA;
		memcpy( m_pDummyCrow , m_pCrowCopy , sizeof(SCROWDATA) );
		strTempName = GetSelectCrow()->GetName();
		m_bDummyHasCrow = TRUE;
		return TRUE;
	}

	return FALSE;
}

BOOL	MobEditTree::PasteCrowNew ()
{
	if ( !m_bDummyHasCrow )
	{
		return FALSE;
	}

	HTREEITEM hMainCrow = m_ctrlCrowTree.GetSelectedItem ();
	if ( hMainCrow && GetSelectCrow () )
	{		
		WORD MID = GetSelectCrow()->m_sBasic.sNativeID.wMainID ;
		WORD SID = GLCrowDataMan::GetInstance().FindFreeCrowDataSID( MID );
		if ( SID == 65535 )
		{
			return FALSE;
		}

		PCROWDATA pCrow;
		pCrow = new SCROWDATA;
		memcpy( pCrow , m_pDummyCrow , sizeof(SCROWDATA) );
		pCrow->m_sBasic.sNativeID.wMainID = MID;
		pCrow->m_sBasic.sNativeID.wSubID = SID;

		CString strTemp;
		strTemp.Format( "CN_%03d_%03d" , MID , SID);
		strcpy_s( pCrow->m_sBasic.m_szName ,strTemp);

		GLCrowDataMan::GetInstance().InsertCrowData ( MID, SID, pCrow );
		strTempNameKey = pCrow->m_sBasic.m_szName;
		GLStringTable::GetInstance().DeleteString ( MID, SID, GLStringTable::CROW );
		GLStringTable::GetInstance().InsertString ( strTempNameKey, strTempName, GLStringTable::CROW );
		PCROWDATA pCROW =  GLCrowDataMan::GetInstance().GetCrowData ( MID, SID );

		HTREEITEM hParentCrow = m_ctrlCrowTree.GetParentItem ( hMainCrow );
		if ( hParentCrow != m_TreeRoot )
		{
			UpdateCrow ( pCROW, hParentCrow );
		}
		else
		{
			UpdateCrow ( pCROW, hMainCrow );
		}

		return TRUE;
	}
	else
	{		
		WORD MID = GLCrowDataMan::GetInstance().FindFreeCrowDataMID();
		if ( MID == 65535 )
		{
			return FALSE;
		}
		WORD SID = GLCrowDataMan::GetInstance().FindFreeCrowDataSID( MID );
		if ( SID == 65535 )
		{
			return FALSE;
		}

		PCROWDATA pCrow;
		pCrow = new SCROWDATA;
		memcpy( pCrow, m_pDummyCrow, sizeof(SCROWDATA) );
		pCrow->m_sBasic.sNativeID.wMainID = MID;
		pCrow->m_sBasic.sNativeID.wSubID = SID;

		CString strTemp;
		strTemp.Format( "CN_%03d_%03d" , MID , SID);
		strcpy_s( pCrow->m_sBasic.m_szName ,strTemp);

		GLCrowDataMan::GetInstance().InsertCrowData ( MID, SID, pCrow );
		strTempNameKey = pCrow->m_sBasic.m_szName;
		GLStringTable::GetInstance().DeleteString ( MID, SID, GLStringTable::CROW );
		GLStringTable::GetInstance().InsertString ( strTempNameKey, strTempName, GLStringTable::CROW );
		PCROWDATA pCROW =  GLCrowDataMan::GetInstance().GetCrowData ( MID, SID );

		UpdateCrow ( pCROW, NULL );

		return TRUE;
	}
	return FALSE;
}

BOOL	MobEditTree::PasteCrowValue ()
{
	if ( !m_bDummyHasCrow )
	{
		return FALSE;
	}

	HTREEITEM hCrow = m_ctrlCrowTree.GetSelectedItem ();
	if ( hCrow && GetSelectCrow () )
	{
		WORD MID = GetSelectCrow()->m_sBasic.sNativeID.wMainID ;
		WORD SID = GetSelectCrow()->m_sBasic.sNativeID.wSubID ;
		memcpy( GetSelectCrow() , m_pDummyCrow , sizeof(SCROWDATA) );
		GetSelectCrow()->m_sBasic.sNativeID.wMainID = MID;
		GetSelectCrow()->m_sBasic.sNativeID.wSubID = SID;

		CString strTemp;
		strTemp.Format( "CN_%03d_%03d" , MID , SID);
		strcpy_s( GetSelectCrow()->m_sBasic.m_szName ,strTemp);

		UpdateData( GetSelectCrow() );

		strTempNameKey = GetSelectCrow()->m_sBasic.m_szName;
		GLStringTable::GetInstance().DeleteString ( MID, SID, GLStringTable::CROW );
		GLStringTable::GetInstance().InsertString ( strTempNameKey, strTempName, GLStringTable::CROW );
	
		return TRUE;
	}
	return FALSE;
}

void MobEditTree::UpdateData ( PCROWDATA pCrow )
{
	HTREEITEM hCrow = m_ctrlCrowTree.GetSelectedItem ();
	GASSERT ( hCrow );
	m_ctrlCrowTree.SetItemData( hCrow, (DWORD_PTR) pCrow );
	m_ctrlCrowTree.SetItemText ( hCrow, GetFormatName ( pCrow  ) );
}


void MobEditTree::UpdateData ( WORD MID, WORD SID,PCROWDATA pCrow )
{
	HTREEITEM hCrow = m_ctrlCrowTree.GetSelectedItem ();
	if ( hCrow && GetSelectCrow () )
	{
		WORD MID = GetSelectCrow()->m_sBasic.sNativeID.wMainID ;
		WORD SID = GetSelectCrow()->m_sBasic.sNativeID.wSubID ;
		memcpy( GetSelectCrow() , pCrow , sizeof(SCROWDATA) );
		GetSelectCrow()->m_sBasic.sNativeID.wMainID = MID;
		GetSelectCrow()->m_sBasic.sNativeID.wSubID = SID;
		UpdateData( GetSelectCrow() );
	}
}

void MobEditTree::UpdateName ()
{
	HTREEITEM hCrow = m_ctrlCrowTree.GetSelectedItem ();
	GASSERT ( hCrow );
	PCROWDATA pCrow = (PCROWDATA)m_ctrlCrowTree.GetItemData ( hCrow );
	GASSERT ( pCrow );
	m_ctrlCrowTree.SetItemText ( hCrow, GetFormatName ( pCrow  ) );
}



