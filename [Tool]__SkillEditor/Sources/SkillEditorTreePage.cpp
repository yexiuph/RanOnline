// SkillEditorTreePage.cpp : implementation file
//

#include "pch.h"
#include <vector>
#include "SkillEditor.h"
#include "SheetWithTab.h"
#include "SkillEditorTreePage.h"
#include ".\skilleditortreepage.h"
#include "GLStringTable.h"


// SkillEditorTreePage dialog

IMPLEMENT_DYNAMIC(SkillEditorTreePage, CPropertyPage)
SkillEditorTreePage::SkillEditorTreePage(LOGFONT logfont)
	: CPropertyPage(SkillEditorTreePage::IDD),
	m_pFont( NULL )
{
	m_bDlgInit = FALSE;
	m_pDummySkill = new GLSKILL;	
	m_bDummyHasSkill = FALSE;

	
	
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

SkillEditorTreePage::~SkillEditorTreePage()
{
}

void SkillEditorTreePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_SKILL, m_ctrlSkillTree);
}


BEGIN_MESSAGE_MAP(SkillEditorTreePage, CPropertyPage)
	ON_NOTIFY(NM_CLICK, IDC_TREE_SKILL, OnClickLeft)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_SKILL, OnClickRight)
END_MESSAGE_MAP()


// SkillEditorTreePage message handlers
BOOL SkillEditorTreePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

	m_bDlgInit = TRUE;
	UpdateTree ();

	return TRUE;  // return TRUE unless you set the focus to a control
}
BOOL	SkillEditorTreePage::UpdateTree ()
{	
	if ( !m_bDlgInit )
	{
		return FALSE;
	}

	m_ctrlSkillTree.DeleteAllItems();

	for( DWORD i = 0 ; i < EMSKILLCLASS_NSIZE ; i ++ )
	{
		m_SkillTreeRoot[i] = m_ctrlSkillTree.InsertItem( COMMENT::SKILLCLASS[i].c_str()  );
	}
	for ( WORD MID=0; MID<EMSKILLCLASS_NSIZE; ++MID )
	for ( WORD SID=0; SID<GLSkillMan::MAX_CLASSSKILL; ++SID )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( MID, SID );
		if ( pSkill )
		{
			CString strSkill;
			strSkill = GetFormatName ( pSkill );

			m_SkillTree = m_ctrlSkillTree.InsertItem( strSkill.GetString(), m_SkillTreeRoot[MID] );
			m_ctrlSkillTree.SetItemData( m_SkillTree, (DWORD_PTR) pSkill );
		}
	}

	return TRUE;
}

const char* SkillEditorTreePage::GetFormatName ( PGLSKILL pSkill  )
{
	GASSERT ( pSkill );

	static	CString strSkill;
	if ( pSkill )
	{	
		strSkill.Format ( "[%03d/%03d][Ani %02d/%02d][%02d]  %s",
		pSkill->m_sBASIC.sNATIVEID.wMainID , pSkill->m_sBASIC.sNATIVEID.wSubID ,  pSkill->m_sEXT_DATA.emANIMTYPE ,pSkill->m_sEXT_DATA.emANISTYPE,pSkill->m_sBASIC.dwGRADE, pSkill->GetName() );
	}
	

	return strSkill.GetString ();
}

BOOL SkillEditorTreePage::UpdateSkill ( PGLSKILL pSkill, HTREEITEM hInsert )
{
	CString strSkill;
	strSkill = GetFormatName ( pSkill );
	m_SkillTree = m_ctrlSkillTree.InsertItem( strSkill.GetString(), hInsert );
	m_ctrlSkillTree.SetItemData( m_SkillTree, (DWORD_PTR) pSkill );

	return TRUE;
}
void SkillEditorTreePage::UpdateData ( WORD MID, WORD SID,PGLSKILL pSkill )
{
	HTREEITEM hSkill = m_ctrlSkillTree.GetSelectedItem ();
	if ( hSkill && GetSelectSkill () )
	{
		WORD MID = GetSelectSkill()->m_sBASIC.sNATIVEID.wMainID ;
		WORD SID = GetSelectSkill()->m_sBASIC.sNATIVEID.wSubID ;

		*(GetSelectSkill()) = *pSkill;

		GetSelectSkill()->m_sBASIC.sNATIVEID.wMainID = MID;
		GetSelectSkill()->m_sBASIC.sNATIVEID.wSubID = SID;

		UpdateData( GetSelectSkill() );
	}

}
void SkillEditorTreePage::UpdateData ( PGLSKILL pSkill )
{
	HTREEITEM hSkill = m_ctrlSkillTree.GetSelectedItem ();
	GASSERT ( hSkill );
	//PGLSKILL pSkill = (PGLSKILL)m_ctrlSkillTree.GetItemData ( hSkill );
	//GASSERT ( pSkill );
	

	m_ctrlSkillTree.SetItemData( hSkill, (DWORD_PTR) pSkill );

	m_ctrlSkillTree.SetItemText ( hSkill, GetFormatName ( pSkill  ) );
}
void SkillEditorTreePage::UpdateName ()
{
	HTREEITEM hSkill = m_ctrlSkillTree.GetSelectedItem ();
	GASSERT ( hSkill );
	PGLSKILL pSkill = (PGLSKILL)m_ctrlSkillTree.GetItemData ( hSkill );
	GASSERT ( pSkill );

	m_ctrlSkillTree.SetItemText ( hSkill, GetFormatName ( pSkill  ) );
}
void SkillEditorTreePage::OnClickLeft(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
}

void SkillEditorTreePage::OnClickRight(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;	
	POINT OriginPoint;
	GetCursorPos ( &OriginPoint );
	ForceSelectOnRightClick ( OriginPoint );
	RunSelectMenu ( GetSelectMenu ( OriginPoint ) );
}
void SkillEditorTreePage::ForceSelectOnRightClick ( POINT pt )
{		
	m_ctrlSkillTree.ScreenToClient( &pt );
	UINT uFlag = 0;
	PGLSKILL pSkillWork = NULL;
	HTREEITEM hSkill = m_ctrlSkillTree.HitTest ( CPoint(pt), &uFlag );

    SetSelectSkill ( NULL );	
	if ( hSkill )
	{
		m_ctrlSkillTree.SelectItem ( hSkill );
		if ( uFlag & TVHT_ONITEMLABEL )
		{			
			SetSelectSkill ( (PGLSKILL)m_ctrlSkillTree.GetItemData ( hSkill ) );
		}
	}
}
void SkillEditorTreePage::SetSelectSkill ( PGLSKILL pSkill )
{
	m_pSkill = pSkill;
}

PGLSKILL SkillEditorTreePage::GetSelectSkill ()
{
	return m_pSkill;
}
int	 SkillEditorTreePage::GetSelectMenu ( POINT pt )
{
	int pMenuID = 0;
	CMenu Menu;
	int SelectionMade = -1;

	VERIFY ( Menu.LoadMenu ( IDR_MENU1 ) );
	CMenu* pPopup = Menu.GetSubMenu ( pMenuID );
	ASSERT ( pPopup != NULL );	

	SelectionMade = pPopup->TrackPopupMenu (
		TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
		pt.x, pt.y, this );

	pPopup->DestroyMenu ();
	return SelectionMade;
}
void SkillEditorTreePage::RunSelectMenu ( int nSelect )
{
	HTREEITEM hSkill = m_ctrlSkillTree.GetSelectedItem ();
	PGLSKILL pSkill = NULL;
	if ( hSkill )
	{
		pSkill = (PGLSKILL)m_ctrlSkillTree.GetItemData ( hSkill );
	}

	switch ( nSelect )
	{
	case ID_MENU_SKILLDATAEDIT:
		{
			if ( GetSelectSkill () )
			{				
				m_pSheetTab->ActiveSkillBasicPage ( SKILLBASICPAGE, pSkill );
			}			
		}
		break;
	case ID_MENU_SKILLLEVELEDIT:
		{
			if ( GetSelectSkill () )
			{
				m_pSheetTab->ActiveSkillLevelPage ( SKILLLEVELPAGE, pSkill );
			}			
		}
		break;
	case ID_MENU_ADD:
		{
			AddSkill ();
		}
		break;
	case ID_MENU_DELETE:
		{
			if ( GetSelectSkill () )
			{
				CString strTemp;
				strTemp.Format ( "Do you want to delete? [%03d/%03d] %s"
					,GetSelectSkill()->m_sBASIC.sNATIVEID.wMainID
					,GetSelectSkill()->m_sBASIC.sNATIVEID.wSubID
					,GetSelectSkill()->GetName() );

				if ( MessageBox ( strTemp, "Question", MB_YESNO ) == IDYES )
				{
					DelSkill();
				}
			}
		}
		break;
	case ID_MENU_COPY:
		{
			CopySkill();
		}
		break;
	case ID_MENU_PASTENEW:
		{
			PasteSkillNew();
		}
		break;
	case ID_MENU_PASTEVALUE:
		{
			PasteSkillValue ();
		}
		break;
	case ID_MENU_JDEV:
		{
			MessageBox ( "If you Found Bugs, you can contact me Yahoo ID: mac14_89@yahoo.com", "Hi!", MB_OK );
		}
		break;
	};
}
WORD SkillEditorTreePage::GetRootID()
{
	HTREEITEM handle = m_ctrlSkillTree.GetSelectedItem();

	std::string strTemp("");
	strTemp = m_ctrlSkillTree.GetItemText(handle);

	for (WORD i=0; i<EMSKILLCLASS_NSIZE; ++i )
	{
		if ( strcmp( strTemp.c_str() , COMMENT::SKILLCLASS[i].c_str() ) == 0 ) return i;
	}

	return 0;
}

BOOL	SkillEditorTreePage::AddSkill ()
{
	HTREEITEM hSkill = m_ctrlSkillTree.GetSelectedItem ();
	if ( hSkill && GetSelectSkill () )
	{		
		WORD MID = GetSelectSkill()->m_sBASIC.sNATIVEID.wMainID ;
		WORD SID = GLSkillMan::GetInstance().FindFreeSkillIndex( MID );
		if ( SID == 65535 )
		{
			return FALSE;
		}

		PGLSKILL pSkill;
		pSkill = new GLSKILL;
		pSkill->m_sBASIC.sNATIVEID.wMainID = MID;
		pSkill->m_sBASIC.sNATIVEID.wSubID = SID;

		GLSkillMan::GetInstance().SetData( MID, SID, pSkill );
		PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData ( MID, SID );

		if (  pSKILL )
		{
			HTREEITEM hParentSkill = m_ctrlSkillTree.GetParentItem ( hSkill );
			UpdateSkill ( pSKILL, hParentSkill );
		}

		return TRUE;
	}
	else
	{
		WORD MID = GetRootID();
		if ( MID == 65535 )
		{
			return FALSE;
		}
		WORD SID = GLSkillMan::GetInstance().FindFreeSkillIndex( MID );
		if ( SID == 65535 )
		{
			return FALSE;
		}

		PGLSKILL pSkill;
		pSkill = new GLSKILL;
		pSkill->m_sBASIC.sNATIVEID.wMainID = MID;
		pSkill->m_sBASIC.sNATIVEID.wSubID = SID;

		GLSkillMan::GetInstance().SetData( MID, SID, pSkill );
		PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData ( MID, SID );

		if (  pSKILL )
		{
			//HTREEITEM hParentSkill = m_ctrlSkillTree.GetParentItem ( hSkill );
			UpdateSkill ( pSKILL, hSkill );
		}

		return TRUE;
	}
	return FALSE;
}

BOOL SkillEditorTreePage::DelSkill ()
{
	if ( GetSelectSkill() )
	{
		WORD MID = GetSelectSkill()->m_sBASIC.sNATIVEID.wMainID ;
		WORD SID = GetSelectSkill()->m_sBASIC.sNATIVEID.wSubID;

		std::string DelName = GetSelectSkill()->m_sBASIC.szNAME;
		std::string DelDesc = GetSelectSkill()->m_sEXT_DATA.strCOMMENTS.c_str();
		GLStringTable::GetInstance().DeleteString( DelName , DelDesc, GLStringTable::SKILL );
		GLSkillMan::GetInstance().DeleteSkill ( MID,SID);
		m_ctrlSkillTree.DeleteItem ( m_ctrlSkillTree.GetSelectedItem () );

		return TRUE;	
	}
	return FALSE;
}
BOOL	SkillEditorTreePage::CopySkill ()
{
	HTREEITEM hSkill = m_ctrlSkillTree.GetSelectedItem ();
	if ( hSkill  )
	{
		*m_pDummySkill = *((PGLSKILL)m_ctrlSkillTree.GetItemData ( hSkill ));

		strTempName = GetSelectSkill()->GetName();
		strTempDesc = GetSelectSkill()->GetDesc();
		m_bDummyHasSkill = TRUE;
		return TRUE;
	}

	return FALSE;
}

BOOL	SkillEditorTreePage::PasteSkillNew ()
{
	if ( !m_bDummyHasSkill )
	{
		return FALSE;
	}

	HTREEITEM hSkill = m_ctrlSkillTree.GetSelectedItem ();
	if ( hSkill && GetSelectSkill () )	
	{
		WORD MID = GetSelectSkill()->m_sBASIC.sNATIVEID.wMainID ;
		WORD SID = GLSkillMan::GetInstance().FindFreeSkillIndex( MID );
		if ( SID == 65535 )
		{
			return FALSE;
		}

		PGLSKILL pSkill;
		pSkill = new GLSKILL;

		
		*pSkill = *m_pDummySkill;
		
		pSkill->m_sBASIC.sNATIVEID.wMainID = MID;
		pSkill->m_sBASIC.sNATIVEID.wSubID = SID;

		CString strTemp;
		strTemp.Format( "SN_%03d_%03d" , MID , SID);
		strcpy_s( pSkill->m_sBASIC.szNAME ,strTemp);
		strTemp.Format( "SD_%03d_%03d" , MID , SID);
		pSkill->m_sEXT_DATA.strCOMMENTS = strTemp.GetString();

		GLSkillMan::GetInstance().SetData( MID, SID, pSkill );


		strTempNameKey = pSkill->m_sBASIC.szNAME;
		strTempDescKey = pSkill->m_sEXT_DATA.strCOMMENTS.c_str();

		GLStringTable::GetInstance().DeleteString ( strTempNameKey, strTempDescKey, GLStringTable::SKILL );
		GLStringTable::GetInstance().InsertString ( strTempNameKey, strTempName, GLStringTable::SKILL );
		GLStringTable::GetInstance().InsertString ( strTempDescKey, strTempDesc, GLStringTable::SKILL );

		PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData ( MID, SID );

		if ( pSKILL )
		{
			HTREEITEM hParentSkill = m_ctrlSkillTree.GetParentItem ( hSkill );
			UpdateSkill ( pSKILL, hParentSkill );
		}

		return TRUE;
	}
	else
	{
		WORD MID = GetRootID() ;
		WORD SID = GLSkillMan::GetInstance().FindFreeSkillIndex( MID );
		if ( SID == 65535 )
		{
			return FALSE;
		}

		PGLSKILL pSkill;
		pSkill = new GLSKILL;

		*pSkill = *m_pDummySkill;
		
		pSkill->m_sBASIC.sNATIVEID.wMainID = MID;
		pSkill->m_sBASIC.sNATIVEID.wSubID = SID;

		CString strTemp;
		strTemp.Format( "SN_%03d_%03d" , MID , SID);
		strcpy_s( pSkill->m_sBASIC.szNAME ,strTemp);
		strTemp.Format( "SD_%03d_%03d" , MID , SID);
		pSkill->m_sEXT_DATA.strCOMMENTS = strTemp.GetString();

		GLSkillMan::GetInstance().SetData( MID, SID, pSkill );


		strTempNameKey = pSkill->m_sBASIC.szNAME;
		strTempDescKey = pSkill->m_sEXT_DATA.strCOMMENTS.c_str();

		GLStringTable::GetInstance().DeleteString ( strTempNameKey, strTempDescKey, GLStringTable::SKILL );
		GLStringTable::GetInstance().InsertString ( strTempNameKey, strTempName, GLStringTable::SKILL );
		GLStringTable::GetInstance().InsertString ( strTempDescKey, strTempDesc, GLStringTable::SKILL );

		PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData ( MID, SID );

		if ( pSKILL )
		{
			//HTREEITEM hParentSkill = m_ctrlSkillTree.GetParentItem ( hSkill );
			UpdateSkill ( pSKILL, hSkill );
		}

		return TRUE;
	}
}
BOOL	SkillEditorTreePage::PasteSkillValue ()
{
	if ( !m_bDummyHasSkill )
	{
		return FALSE;
	}

	HTREEITEM hSkill = m_ctrlSkillTree.GetSelectedItem ();
	if ( hSkill && GetSelectSkill () )
	{
		WORD MID = GetSelectSkill()->m_sBASIC.sNATIVEID.wMainID ;
		WORD SID = GetSelectSkill()->m_sBASIC.sNATIVEID.wSubID ;

		*(GetSelectSkill()) = *m_pDummySkill;

		GetSelectSkill()->m_sBASIC.sNATIVEID.wMainID = MID;
		GetSelectSkill()->m_sBASIC.sNATIVEID.wSubID = SID;

		CString strTemp;
		strTemp.Format( "SN_%03d_%03d" , MID , SID);
		strcpy_s( GetSelectSkill()->m_sBASIC.szNAME ,strTemp);
		strTemp.Format( "SD_%03d_%03d" , MID , SID);
		GetSelectSkill()->m_sEXT_DATA.strCOMMENTS = strTemp.GetString();

		UpdateData( GetSelectSkill() );

		strTempNameKey = GetSelectSkill()->m_sBASIC.szNAME;
		strTempDescKey = GetSelectSkill()->m_sEXT_DATA.strCOMMENTS.c_str();
		GLStringTable::GetInstance().DeleteString ( strTempNameKey, strTempDescKey, GLStringTable::SKILL );
		GLStringTable::GetInstance().InsertString ( strTempNameKey, strTempName, GLStringTable::SKILL );
		GLStringTable::GetInstance().InsertString ( strTempDescKey, strTempDesc, GLStringTable::SKILL );

		return TRUE;
	}
	return FALSE;
}

void	SkillEditorTreePage::ClearData ()
{
	m_ctrlSkillTree.DeleteAllItems();
	m_pSkill = NULL;
	m_pDummySkill = NULL;
}