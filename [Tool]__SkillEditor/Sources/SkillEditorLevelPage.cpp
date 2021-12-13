// SkillEditorLevelPage.cpp : implementation file
//

#include "pch.h"
#include "SkillEditor.h"
#include "SkillEditorLevelPage.h"
#include "GLOGIC.h"

#include "DxEffcharData.h"
#include "DxSkinPieceContainer.h"
#include "DxSimpleMeshMan.h"
#include "EtcFunction.h"

#include "SheetWithTab.h"

#include "GLStringTable.h"
#include ".\skilleditorlevelpage.h"
#include "SkillEditorTreePage.h"

// SkillEditorLevelPage dialog

IMPLEMENT_DYNAMIC(SkillEditorLevelPage, CPropertyPage)
SkillEditorLevelPage::SkillEditorLevelPage(LOGFONT logfont)
	: CPropertyPage(SkillEditorLevelPage::IDD)
	,m_pFont( NULL )
	,m_SkillEditorTreePage( logfont )
{
	m_bDlgInit = FALSE;
	m_pSkill = NULL;
	m_pDummySkill = new GLSKILL;

	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

SkillEditorLevelPage::~SkillEditorLevelPage()
{
	SAFE_DELETE ( m_pDummySkill );
	SAFE_DELETE ( m_pFont );
}

void SkillEditorLevelPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SkillEditorLevelPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_LVLMINUS, OnBnClickedButtonLvlminus)
	ON_BN_CLICKED(IDC_BUTTON_LVLADD, OnBnClickedButtonLvladd)
	ON_BN_CLICKED(IDC_BUTTON_TRANEFFECT, OnBnClickedButtonTraneffect)
	ON_BN_CLICKED(IDC_BUTTON_TRAN_M1, OnBnClickedButtonTranM1)
	ON_BN_CLICKED(IDC_BUTTON_TRAN_M2, OnBnClickedButtonTranM2)
	ON_BN_CLICKED(IDC_BUTTON_TRAN_M3, OnBnClickedButtonTranM3)
	ON_BN_CLICKED(IDC_BUTTON_TRAN_M4, OnBnClickedButtonTranM4)
	ON_BN_CLICKED(IDC_BUTTON_TRAN_M5, OnBnClickedButtonTranM5)
	ON_BN_CLICKED(IDC_BUTTON_TRAN_M6, OnBnClickedButtonTranM6)
	ON_BN_CLICKED(IDC_BUTTON_TRAN_M7, OnBnClickedButtonTranM7)
	ON_BN_CLICKED(IDC_BUTTON_TRAN_M8, OnBnClickedButtonTranM8)
	ON_BN_CLICKED(IDC_BUTTON_TRAN_W1, OnBnClickedButtonTranW1)
	ON_BN_CLICKED(IDC_BUTTON_TRAN_W2, OnBnClickedButtonTranW2)
	ON_BN_CLICKED(IDC_BUTTON_TRAN_W3, OnBnClickedButtonTranW3)
	ON_BN_CLICKED(IDC_BUTTON_TRAN_W4, OnBnClickedButtonTranW4)
	ON_BN_CLICKED(IDC_BUTTON_TRAN_W5, OnBnClickedButtonTranW5)
	ON_BN_CLICKED(IDC_BUTTON_TRAN_W6, OnBnClickedButtonTranW6)
	ON_BN_CLICKED(IDC_BUTTON_TRAN_W7, OnBnClickedButtonTranW7)
	ON_BN_CLICKED(IDC_BUTTON_TRAN_W8, OnBnClickedButtonTranW8)
	ON_BN_CLICKED(IDC_BUTTON_TRAN_CHF, OnBnClickedButtonTranChf)
END_MESSAGE_MAP()


// SkillEditorLevelPage message handlers
BOOL SkillEditorLevelPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );
	
	m_bDlgInit = TRUE;
	InitDefaultCtrls ();
	UpdateSkill( m_dwSkillLevel );

	return TRUE;  // return TRUE unless you set the focus to a control
}
void SkillEditorLevelPage::OnBnClickedButtonOk()
{
	if ( !InverseUpdateSkill( m_dwSkillLevel ) )
	{		
		return;
	}

	WORD MID = m_pDummySkill->m_sBASIC.sNATIVEID.wMainID;
	WORD SID = m_pDummySkill->m_sBASIC.sNATIVEID.wSubID;

	m_pSkill = NULL;
	m_SkillEditorTreePage.UpdateData ( MID,SID,m_pDummySkill );
	m_pSheetTab->ActiveSkillTreePage ();
}

void SkillEditorLevelPage::OnBnClickedButtonNext()
{
	if ( !InverseUpdateSkill( m_dwSkillLevel ) )
	{		
		return;
	}

	WORD MID = m_pDummySkill->m_sBASIC.sNATIVEID.wMainID;
	WORD SID = m_pDummySkill->m_sBASIC.sNATIVEID.wSubID;

	//m_SkillEditorTreePage.UpdateData ( MID,SID,m_pDummySkill );
	m_pSheetTab->ActiveSkillBasicPage ( m_CallPage,m_pDummySkill );
	m_pSkill = NULL;
}

void SkillEditorLevelPage::OnBnClickedButtonCancel()
{
	m_pSkill = NULL;
	m_pSheetTab->ActiveSkillTreePage ();
}

void SkillEditorLevelPage::InitAllSkill ()
{
	m_pSkill = NULL;
}

BOOL SkillEditorLevelPage::SetSkill ( PGLSKILL pSkill )
{
	if ( m_pSkill )
	{
		return FALSE;
	}

	if (pSkill)
	{
		m_pSkill = pSkill;
		m_pDummySkill = m_pSkill;
	}

	if ( m_bDlgInit ) 
	{
		InitDefaultCtrls ();
		UpdateSkill( m_dwSkillLevel );
	}

	return TRUE;
}
void SkillEditorLevelPage::InitDefaultCtrls ()
{
	CString strTemp;
	strTemp.Format( "Basic Attribute : %s" , COMMENT::SKILL_TYPES[m_pDummySkill->m_sAPPLY.emBASIC_TYPE].c_str() );
	SetWin_Text( this ,IDC_STATIC_BASIC , strTemp.GetString() );

	strTemp.Format( "State Trouble : %s" , COMMENT::BLOW[m_pDummySkill->m_sAPPLY.emSTATE_BLOW].c_str() );
	SetWin_Text( this ,IDC_STATIC_STT , strTemp.GetString() );
	if ( m_pDummySkill->m_sAPPLY.emSTATE_BLOW == EMBLOW_NONE )
	{
		SetWin_Enable( this ,IDC_EDIT_STTRATE  , FALSE );
		SetWin_Enable( this ,IDC_EDIT_STTVAR1  , FALSE );
		SetWin_Enable( this ,IDC_EDIT_STTVAR2  , FALSE );
	}
	else
	{
		SetWin_Enable( this ,IDC_EDIT_STTRATE  , TRUE );
		SetWin_Enable( this ,IDC_EDIT_STTVAR1  , TRUE );
		SetWin_Enable( this ,IDC_EDIT_STTVAR2  , TRUE );
	}

	SetWin_Text( this ,IDC_STATIC_TXTVAR1 , COMMENT::BLOW_VAR1[m_pDummySkill->m_sAPPLY.emSTATE_BLOW].c_str() );
	SetWin_Text( this ,IDC_STATIC_TXTVAR2 , COMMENT::BLOW_VAR2[m_pDummySkill->m_sAPPLY.emSTATE_BLOW].c_str() );

	strTemp.Format( "Side Effect : %s" , COMMENT::IMPACT_ADDON[m_pDummySkill->m_sAPPLY.emADDON].c_str() );
	SetWin_Text( this ,IDC_STATIC_SDE , strTemp.GetString() );
	if ( m_pDummySkill->m_sAPPLY.emADDON == EMIMPACTA_NONE )
	{
		SetWin_Enable( this ,IDC_EDIT_ADDONVAR  , FALSE );
	}
	else
	{
		SetWin_Enable( this ,IDC_EDIT_ADDONVAR  , TRUE );
	}

	strTemp.Format( "Special Ability : %s" , COMMENT::SPEC_ADDON[m_pDummySkill->m_sAPPLY.emSPEC].c_str() );
	SetWin_Text( this ,IDC_STATIC_SPA , strTemp.GetString() );

	if ( m_pDummySkill->m_sAPPLY.emSPEC == EMSPECA_NULL )
	{
		SetWin_Enable( this ,IDC_EDIT_SPVAR1  , FALSE );
		SetWin_Enable( this ,IDC_EDIT_SPVAR2  , FALSE );
	}
	else
	{
		SetWin_Enable( this ,IDC_EDIT_SPVAR1  , TRUE );
		SetWin_Enable( this ,IDC_EDIT_SPVAR2  , TRUE );
	}

	strTemp.Format( "%s" , COMMENT::SPEC_ADDON_VAR1[m_pDummySkill->m_sAPPLY.emSPEC].c_str() );
	SetWin_Text( this ,IDC_STATIC_SPVAR1 , strTemp.GetString() );

	strTemp.Format( "%s" , COMMENT::SPEC_ADDON_VAR2[m_pDummySkill->m_sAPPLY.emSPEC].c_str() );
	SetWin_Text( this ,IDC_STATIC_SPVAR2 , strTemp.GetString() );

	if ( m_pDummySkill->m_sAPPLY.emSPEC == EMSPECA_NONBLOW )
	{
		SetWin_Enable ( this, IDC_CHECK_TC_PARALYSIS, TRUE );
		SetWin_Enable ( this, IDC_CHECK_TC_FAINT, TRUE );
		SetWin_Enable ( this, IDC_CHECK_TC_STONE, TRUE );
		SetWin_Enable ( this, IDC_CHECK_TC_FLAME, TRUE );
		SetWin_Enable ( this, IDC_CHECK_TC_FREEZE, TRUE );
		SetWin_Enable ( this, IDC_CHECK_TC_MADNESS, TRUE );
		SetWin_Enable ( this, IDC_CHECK_TC_POISON, TRUE );
		SetWin_Enable ( this, IDC_CHECK_TC_CURSE, TRUE );
	}
	else
	{
		SetWin_Enable ( this, IDC_CHECK_TC_PARALYSIS, FALSE );
		SetWin_Enable ( this, IDC_CHECK_TC_FAINT, FALSE );
		SetWin_Enable ( this, IDC_CHECK_TC_STONE, FALSE );
		SetWin_Enable ( this, IDC_CHECK_TC_FLAME, FALSE );
		SetWin_Enable ( this, IDC_CHECK_TC_FREEZE, FALSE );
		SetWin_Enable ( this, IDC_CHECK_TC_MADNESS, FALSE );
		SetWin_Enable ( this, IDC_CHECK_TC_POISON, FALSE );
		SetWin_Enable ( this, IDC_CHECK_TC_CURSE, FALSE );
	}

	if ( m_pDummySkill->m_sAPPLY.emSPEC == EMSPECA_DEFENSE_SKILL_ACTIVE )
	{
		SetWin_Enable ( this, IDC_EDIT_AS_MID, TRUE );
		SetWin_Enable ( this, IDC_EDIT_AS_SID, TRUE );
		SetWin_Enable ( this, IDC_EDIT_AS_LVL, TRUE );
	}
	else
	{
		SetWin_Enable ( this, IDC_EDIT_AS_MID, FALSE );
		SetWin_Enable ( this, IDC_EDIT_AS_SID, FALSE );
		SetWin_Enable ( this, IDC_EDIT_AS_LVL, FALSE );
	}

	if ( m_pDummySkill->m_sSPECIAL_SKILL.emSSTYPE == SKILL::EMSSTYPE_TRANSFORM )
	{
		SetWin_Enable ( this, IDC_BUTTON_TRANEFFECT, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRANEFFECT, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRANEFFECTTIME, TRUE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M1, TRUE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M2, TRUE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M3, TRUE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M4, TRUE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M5, TRUE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M6, TRUE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M7, TRUE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M8, TRUE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W1, TRUE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W2, TRUE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W3, TRUE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W4, TRUE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W5, TRUE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W6, TRUE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W7, TRUE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W8, TRUE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_CHF, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M1, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M2, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M3, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M4, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M5, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M6, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M7, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M8, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W1, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W2, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W3, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W4, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W5, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W6, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W7, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W8, TRUE );

		SetWin_Enable ( this, IDC_EDIT_SUMMID, FALSE );
		SetWin_Enable ( this, IDC_EDIT_SUMSID, FALSE );
		SetWin_Enable ( this, IDC_EDIT_SUMTIME, FALSE );

		SetWin_Enable ( this, IDC_EDIT_TRAN_CHF, FALSE );
	}
	/*Summon skill
	else if ( m_pDummySkill->m_sSPECIAL_SKILL.emSSTYPE == SKILL::EMSSTYPE_SUMMON )
	{
		SetWin_Enable ( this, IDC_EDIT_SUMMID, TRUE );
		SetWin_Enable ( this, IDC_EDIT_SUMSID, TRUE );
		SetWin_Enable ( this, IDC_EDIT_SUMTIME, TRUE );


		SetWin_Enable ( this, IDC_BUTTON_TRANEFFECT, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRANEFFECT, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRANEFFECTTIME, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M1, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M2, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M3, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M4, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M5, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M6, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M7, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M8, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W1, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W2, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W3, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W4, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W5, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W6, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W7, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W8, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_CHF, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M1, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M2, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M3, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M4, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M5, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M6, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M7, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M8, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W1, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W2, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W3, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W4, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W5, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W6, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W7, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W8, FALSE );

		SetWin_Enable ( this, IDC_EDIT_TRAN_CHF, FALSE );
	}
	*/
	/* Transform
	else if ( m_pDummySkill->m_sSPECIAL_SKILL.emSSTYPE == SKILL::EMSSTYPE_TRANSCHF )
	{
		SetWin_Enable ( this, IDC_BUTTON_TRANEFFECT, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRANEFFECT, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRANEFFECTTIME, TRUE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M1, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M2, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M3, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M4, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M5, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M6, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M7, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M8, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W1, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W2, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W3, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W4, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W5, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W6, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W7, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W8, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_CHF, TRUE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M1, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M2, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M3, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M4, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M5, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M6, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M7, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M8, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W1, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W2, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W3, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W4, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W5, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W6, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W7, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W8, FALSE );

		SetWin_Enable ( this, IDC_EDIT_SUMMID, FALSE );
		SetWin_Enable ( this, IDC_EDIT_SUMSID, FALSE );
		SetWin_Enable ( this, IDC_EDIT_SUMTIME, FALSE );

		SetWin_Enable ( this, IDC_EDIT_TRAN_CHF, TRUE );
	}
	*/
	else
	{
		SetWin_Enable ( this, IDC_BUTTON_TRANEFFECT, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRANEFFECT, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRANEFFECTTIME, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M1, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M2, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M3, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M4, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M5, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M6, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M7, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_M8, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W1, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W2, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W3, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W4, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W5, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W6, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W7, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_W8, FALSE );
		SetWin_Enable ( this, IDC_BUTTON_TRAN_CHF, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M1, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M2, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M3, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M4, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M5, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M6, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M7, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_M8, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W1, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W2, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W3, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W4, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W5, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W6, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W7, FALSE );
		SetWin_Enable ( this, IDC_EDIT_TRAN_W8, FALSE );

		SetWin_Enable ( this, IDC_EDIT_SUMMID, FALSE );
		SetWin_Enable ( this, IDC_EDIT_SUMSID, FALSE );
		SetWin_Enable ( this, IDC_EDIT_SUMTIME, FALSE );

		SetWin_Enable ( this, IDC_EDIT_TRAN_CHF, FALSE );
	}


	m_dwSkillLevel = 0;
}
void SkillEditorLevelPage::OnBnClickedButtonLvlminus()
{
	InverseUpdateSkill( m_dwSkillLevel );
	if ( m_dwSkillLevel <= 0 ) return ;
	m_dwSkillLevel--;
	UpdateSkill( m_dwSkillLevel );
}

void SkillEditorLevelPage::OnBnClickedButtonLvladd()
{
	InverseUpdateSkill( m_dwSkillLevel );
	m_dwSkillLevel++;
	if ( m_dwSkillLevel >= SKILL::MAX_LEVEL ) m_dwSkillLevel--;
	UpdateSkill( m_dwSkillLevel );
}

void SkillEditorLevelPage::UpdateSkill( DWORD	m_dwSkillLevel )
{
	SetDlgItemText( IDC_EDIT_SKILLLEVEL , COMMENT::SKILL_LEVEL[m_dwSkillLevel].c_str() );

	//Basic Attribute
	SetWin_Num_float ( this, IDC_EDIT_BDELAY, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].fDELAYTIME );
	SetWin_Num_float ( this, IDC_EDIT_BLIFE, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].fLIFE );
	SetWin_Num_int ( this, IDC_EDIT_BAPPLYRANGE, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wAPPLYRANGE );
	SetWin_Num_int ( this, IDC_EDIT_BAPPLYNUM, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wAPPLYNUM );
	SetWin_Num_int ( this, IDC_EDIT_BAPPLYANGLE, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wAPPLYANGLE );
	SetWin_Num_int ( this, IDC_EDIT_BPIERCENUM, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wPIERCENUM );
	SetWin_Num_int ( this, IDC_EDIT_BTARNUM, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wTARNUM);
	SetWin_Num_float ( this, IDC_EDIT_BBASICVAR, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].fBASIC_VAR );
	
	//consumption condition
	SetWin_Num_int ( this, IDC_EDIT_CARROW, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_ARROWNUM);
	SetWin_Num_int ( this, IDC_EDIT_CCHARM, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_CHARMNUM);
	SetWin_Num_int ( this, IDC_EDIT_CEXP, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_EXP);
	SetWin_Num_int ( this, IDC_EDIT_CHP, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_HP);
	SetWin_Num_int ( this, IDC_EDIT_CMP, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_MP);
	SetWin_Num_int ( this, IDC_EDIT_CSP, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_SP);
	SetWin_Num_int ( this, IDC_EDIT_CPHP, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_HP_PTY);
	SetWin_Num_int ( this, IDC_EDIT_CPMP, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_MP_PTY);
	SetWin_Num_int ( this, IDC_EDIT_CPSP, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_SP_PTY);
	//SetWin_Num_int ( this, IDC_EDIT_CCP, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_CP);
	//SetWin_Num_int ( this, IDC_EDIT_CBULLET, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_BULLET);
	//SetWin_Num_int ( this, IDC_EDIT_CEXTRA, m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_EXTRA);

	//request Stats
	SetWin_Num_int ( this, IDC_EDIT_RLEVEL, m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].dwLEVEL );
	SetWin_Num_int ( this, IDC_EDIT_RSKP, m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].dwSKP );
	SetWin_Num_int ( this, IDC_EDIT_RSKLEVEL, m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].dwSKILL_LVL );
	//SetWin_Num_int ( this, IDC_EDIT_RREBORN, m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].dwREBORN );
	SetWin_Num_int ( this, IDC_EDIT_RPOW, m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].sSTATS.wPow );
	SetWin_Num_int ( this, IDC_EDIT_RSPR, m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].sSTATS.wSpi );
	SetWin_Num_int ( this, IDC_EDIT_RINT, m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].sSTATS.wInt );
	SetWin_Num_int ( this, IDC_EDIT_RSTR, m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].sSTATS.wStr );
	SetWin_Num_int ( this, IDC_EDIT_RDEX, m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].sSTATS.wDex);
	SetWin_Num_int ( this, IDC_EDIT_RSTA, m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].sSTATS.wSta);

	//state blow
	SetWin_Num_float ( this, IDC_EDIT_STTRATE, m_pDummySkill->m_sAPPLY.sSTATE_BLOW[ m_dwSkillLevel ].fRATE );
	SetWin_Num_float ( this, IDC_EDIT_STTVAR1, m_pDummySkill->m_sAPPLY.sSTATE_BLOW[ m_dwSkillLevel ].fVAR1 );
	SetWin_Num_float ( this, IDC_EDIT_STTVAR2, m_pDummySkill->m_sAPPLY.sSTATE_BLOW[ m_dwSkillLevel ].fVAR2 );
	
	//side effect
	SetWin_Num_float ( this, IDC_EDIT_ADDONVAR, m_pDummySkill->m_sAPPLY.fADDON_VAR[ m_dwSkillLevel ] );
	
	//special ability
	SetWin_Num_float ( this, IDC_EDIT_SPVAR1, m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].fVAR1 );
	SetWin_Num_float ( this, IDC_EDIT_SPVAR2, m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].fVAR2 );

	//trouble cure
	SetWin_Check ( this, IDC_CHECK_TC_PARALYSIS, m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwFLAG&DIS_NUMB ) ;
	SetWin_Check ( this, IDC_CHECK_TC_FAINT, m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwFLAG&DIS_STUN );
	SetWin_Check ( this, IDC_CHECK_TC_STONE, m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwFLAG&DIS_STONE );
	SetWin_Check ( this, IDC_CHECK_TC_FLAME, m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwFLAG&DIS_BURN );
	SetWin_Check ( this, IDC_CHECK_TC_FREEZE, m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwFLAG&DIS_FROZEN );
	SetWin_Check ( this, IDC_CHECK_TC_MADNESS, m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwFLAG&DIS_MAD );
	SetWin_Check ( this, IDC_CHECK_TC_POISON, m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwFLAG&DIS_POISON );
	SetWin_Check ( this, IDC_CHECK_TC_CURSE, m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwFLAG&DIS_CURSE );

	//active skill
	SetWin_Num_int ( this, IDC_EDIT_AS_MID, m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwNativeID.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_AS_SID, m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwNativeID.wSubID );
	SetWin_Num_int ( this, IDC_EDIT_AS_LVL, m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwFLAG );

	//transformation
	SetWin_Text ( this, IDC_EDIT_TRANEFFECT, m_pDummySkill->m_sSPECIAL_SKILL.strEffectName.c_str() );
	SetWin_Num_int ( this, IDC_EDIT_TRANEFFECTTIME, m_pDummySkill->m_sSPECIAL_SKILL.dwRemainSecond );
	SetWin_Text ( this, IDC_EDIT_TRAN_M1, m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Man[0].c_str() );
	SetWin_Text ( this, IDC_EDIT_TRAN_M2, m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Man[1].c_str() );
	SetWin_Text ( this, IDC_EDIT_TRAN_M3, m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Man[2].c_str() );
	SetWin_Text ( this, IDC_EDIT_TRAN_M4, m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Man[3].c_str() );
	SetWin_Text ( this, IDC_EDIT_TRAN_M5, m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Man[4].c_str() );
	SetWin_Text ( this, IDC_EDIT_TRAN_M6, m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Man[5].c_str() );
	SetWin_Text ( this, IDC_EDIT_TRAN_M7, m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Man[6].c_str() );
	SetWin_Text ( this, IDC_EDIT_TRAN_M8, m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Man[7].c_str() );
	SetWin_Text ( this, IDC_EDIT_TRAN_W1, m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Woman[0].c_str() );
	SetWin_Text ( this, IDC_EDIT_TRAN_W2, m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Woman[1].c_str() );
	SetWin_Text ( this, IDC_EDIT_TRAN_W3, m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Woman[2].c_str() );
	SetWin_Text ( this, IDC_EDIT_TRAN_W4, m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Woman[3].c_str() );
	SetWin_Text ( this, IDC_EDIT_TRAN_W5, m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Woman[4].c_str() );
	SetWin_Text ( this, IDC_EDIT_TRAN_W6, m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Woman[5].c_str() );
	SetWin_Text ( this, IDC_EDIT_TRAN_W7, m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Woman[6].c_str() );
	SetWin_Text ( this, IDC_EDIT_TRAN_W8, m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Woman[7].c_str() );

	//summon
	//SetWin_Num_int ( this, IDC_EDIT_SUMMID, m_pDummySkill->m_sSPECIAL_SKILL.sSummonID.wMainID );
	//SetWin_Num_int ( this, IDC_EDIT_SUMSID, m_pDummySkill->m_sSPECIAL_SKILL.sSummonID.wSubID );
	//SetWin_Num_int ( this, IDC_EDIT_SUMTIME, m_pDummySkill->m_sSPECIAL_SKILL.dwSummonTime );

	//requirements (consume)
	//SetWin_Num_int ( this, IDC_EDIT_REQMONEY, m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].dwMoneyReq);
	//SetWin_Num_int ( this, IDC_EDIT_REQVOTEP, m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].dwVotePoint);
	//SetWin_Num_int ( this, IDC_EDIT_REQPREMP, m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].dwPremPoint);

	//transform chf
	//SetWin_Text ( this, IDC_EDIT_TRAN_CHF, m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Chf.c_str() );
}

BOOL SkillEditorLevelPage::InverseUpdateSkill( DWORD	m_dwSkillLevel )
{
	SetDlgItemText( IDC_EDIT_SKILLLEVEL , COMMENT::SKILL_LEVEL[m_dwSkillLevel].c_str() );

	//Basic Attribute
	m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].fDELAYTIME = GetWin_Num_float ( this, IDC_EDIT_BDELAY );
	m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].fLIFE = GetWin_Num_float ( this, IDC_EDIT_BLIFE );
	m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wAPPLYRANGE =  GetWin_Num_int ( this, IDC_EDIT_BAPPLYRANGE );
	m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wAPPLYNUM =  GetWin_Num_int ( this, IDC_EDIT_BAPPLYNUM );
	m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wAPPLYANGLE =  GetWin_Num_int ( this, IDC_EDIT_BAPPLYANGLE );
	m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wPIERCENUM =  GetWin_Num_int ( this, IDC_EDIT_BPIERCENUM );
	m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wTARNUM =  GetWin_Num_int ( this, IDC_EDIT_BTARNUM );
	m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].fBASIC_VAR = GetWin_Num_float ( this, IDC_EDIT_BBASICVAR );

	//consumption condition
	m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_ARROWNUM =  GetWin_Num_int ( this, IDC_EDIT_CARROW );
	m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_CHARMNUM =  GetWin_Num_int ( this, IDC_EDIT_CCHARM );
	m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_EXP  =  GetWin_Num_int ( this, IDC_EDIT_CEXP );
	m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_HP  =  GetWin_Num_int ( this, IDC_EDIT_CHP ); 
	m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_MP  =  GetWin_Num_int ( this, IDC_EDIT_CMP ); 
	m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_SP  =  GetWin_Num_int ( this, IDC_EDIT_CSP ); 
	m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_HP_PTY = GetWin_Num_int ( this, IDC_EDIT_CPHP ); 
	m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_MP_PTY = GetWin_Num_int ( this, IDC_EDIT_CPMP ); 
	m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_SP_PTY = GetWin_Num_int ( this, IDC_EDIT_CPSP ); 
	//m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_CP = GetWin_Num_int ( this, IDC_EDIT_CCP ); 
	//m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_BULLET = GetWin_Num_int ( this, IDC_EDIT_CBULLET ); 
	//m_pDummySkill->m_sAPPLY.sDATA_LVL[ m_dwSkillLevel ].wUSE_EXTRA = GetWin_Num_int ( this, IDC_EDIT_CEXTRA ); 
	
	//request Stats
	m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].dwLEVEL = GetWin_Num_int ( this, IDC_EDIT_RLEVEL ); 
	m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].dwSKP = GetWin_Num_int ( this, IDC_EDIT_RSKP ); 
	m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].dwSKILL_LVL = GetWin_Num_int ( this, IDC_EDIT_RSKLEVEL ); 
	//m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].dwREBORN = GetWin_Num_int ( this, IDC_EDIT_RREBORN ); 
	m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].sSTATS.wPow = GetWin_Num_int ( this, IDC_EDIT_RPOW ); 
	m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].sSTATS.wSpi = GetWin_Num_int ( this, IDC_EDIT_RSPR ); 
	m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].sSTATS.wInt = GetWin_Num_int ( this, IDC_EDIT_RINT ); 
	m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].sSTATS.wStr = GetWin_Num_int ( this, IDC_EDIT_RSTR ); 
	m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].sSTATS.wDex = GetWin_Num_int ( this, IDC_EDIT_RDEX ); 
	m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].sSTATS.wSta = GetWin_Num_int ( this, IDC_EDIT_RSTA ); 

	//state blow
	m_pDummySkill->m_sAPPLY.sSTATE_BLOW[ m_dwSkillLevel ].fRATE = GetWin_Num_float ( this, IDC_EDIT_STTRATE );
	m_pDummySkill->m_sAPPLY.sSTATE_BLOW[ m_dwSkillLevel ].fVAR1 = GetWin_Num_float ( this, IDC_EDIT_STTVAR1 );
	m_pDummySkill->m_sAPPLY.sSTATE_BLOW[ m_dwSkillLevel ].fVAR2 = GetWin_Num_float ( this, IDC_EDIT_STTVAR2 );
	
	//side effect
	m_pDummySkill->m_sAPPLY.fADDON_VAR[ m_dwSkillLevel ] = GetWin_Num_float ( this, IDC_EDIT_ADDONVAR );
	
	//special ability
	m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].fVAR1 = GetWin_Num_float ( this, IDC_EDIT_SPVAR1 );
	m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].fVAR2 = GetWin_Num_float ( this, IDC_EDIT_SPVAR2 );

	if ( m_pDummySkill->m_sAPPLY.emSPEC == EMSPECA_NONBLOW )
	{
		//trouble cure
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_TC_PARALYSIS ), m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwFLAG, DIS_NUMB );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_TC_FAINT ), m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwFLAG, DIS_STUN );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_TC_STONE ), m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwFLAG, DIS_STONE );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_TC_FLAME ), m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwFLAG, DIS_BURN );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_TC_FREEZE ), m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwFLAG, DIS_FROZEN );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_TC_MADNESS ), m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwFLAG, DIS_MAD );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_TC_POISON ), m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwFLAG, DIS_POISON );
		SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_TC_CURSE ), m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwFLAG, DIS_CURSE );
	}
	else if ( m_pDummySkill->m_sAPPLY.emSPEC == EMSPECA_DEFENSE_SKILL_ACTIVE )
	{
		//active skill
		m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwNativeID.wMainID = GetWin_Num_int ( this, IDC_EDIT_AS_MID ); 
		m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwNativeID.wSubID = GetWin_Num_int ( this, IDC_EDIT_AS_SID ); 
		m_pDummySkill->m_sAPPLY.sSPEC[ m_dwSkillLevel ].dwFLAG = GetWin_Num_int ( this, IDC_EDIT_AS_LVL ); 
	}

	//transformation
	m_pDummySkill->m_sSPECIAL_SKILL.strEffectName = GetWin_Text ( this, IDC_EDIT_TRANEFFECT );
	m_pDummySkill->m_sSPECIAL_SKILL.dwRemainSecond = GetWin_Num_int ( this, IDC_EDIT_TRANEFFECTTIME ); 
	m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Man[0] = GetWin_Text ( this, IDC_EDIT_TRAN_M1 );
	m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Man[1] = GetWin_Text ( this, IDC_EDIT_TRAN_M2 );
	m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Man[2] = GetWin_Text ( this, IDC_EDIT_TRAN_M3 );
	m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Man[3] = GetWin_Text ( this, IDC_EDIT_TRAN_M4 );
	m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Man[4] = GetWin_Text ( this, IDC_EDIT_TRAN_M5 );
	m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Man[5] = GetWin_Text ( this, IDC_EDIT_TRAN_M6 );
	m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Man[6] = GetWin_Text ( this, IDC_EDIT_TRAN_M7 );
	m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Man[7] = GetWin_Text ( this, IDC_EDIT_TRAN_M8 );
	m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Woman[0] = GetWin_Text ( this, IDC_EDIT_TRAN_W1 );
	m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Woman[1] = GetWin_Text ( this, IDC_EDIT_TRAN_W2 );
	m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Woman[2] = GetWin_Text ( this, IDC_EDIT_TRAN_W3 );
	m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Woman[3] = GetWin_Text ( this, IDC_EDIT_TRAN_W4 );
	m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Woman[4] = GetWin_Text ( this, IDC_EDIT_TRAN_W5 );
	m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Woman[5] = GetWin_Text ( this, IDC_EDIT_TRAN_W6 );
	m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Woman[6] = GetWin_Text ( this, IDC_EDIT_TRAN_W7 );
	m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Woman[7] = GetWin_Text ( this, IDC_EDIT_TRAN_W8 );

	//summon
	//m_pDummySkill->m_sSPECIAL_SKILL.sSummonID.wMainID = GetWin_Num_int ( this, IDC_EDIT_SUMMID );
	//m_pDummySkill->m_sSPECIAL_SKILL.sSummonID.wSubID = GetWin_Num_int ( this, IDC_EDIT_SUMSID );
	//m_pDummySkill->m_sSPECIAL_SKILL.dwSummonTime = GetWin_Num_int ( this, IDC_EDIT_SUMTIME );

	//requirements (consume)
	//m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].dwMoneyReq = GetWin_Num_int ( this, IDC_EDIT_REQMONEY );
	//m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].dwVotePoint = GetWin_Num_int ( this, IDC_EDIT_REQVOTEP );
	//m_pDummySkill->m_sLEARN.sLVL_STEP[ m_dwSkillLevel ].dwPremPoint = GetWin_Num_int ( this, IDC_EDIT_REQPREMP );

	//transform chf
	//m_pDummySkill->m_sSPECIAL_SKILL.strTransform_Chf = GetWin_Text ( this, IDC_EDIT_TRAN_CHF );

	return TRUE;
}
void SkillEditorLevelPage::Loadegp( int nID )
{
	CString szFilter = "Effect File (*.egp)|*.egp|";
	
	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath ();

	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
	}
}
void SkillEditorLevelPage::Loadcps( int nID )
{
	CString szFilter = "Wearing File (*.cps,*.abl,*.vcf)|*.cps;*.abl;*.vcf|";
	
	CFileDialog dlg(TRUE,".cps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(SkillEditorLevelPage*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
	}
}
void SkillEditorLevelPage::Loadchf( int nID )
{
	CString szFilter = "File (*.chf,*.CHF )|*.chf;*.CHF|";
	
	CFileDialog dlg(TRUE,".chf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(SkillEditorLevelPage*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
	}
}
void SkillEditorLevelPage::OnBnClickedButtonTraneffect()
{
	Loadegp(IDC_EDIT_TRANEFFECT);
}

void SkillEditorLevelPage::OnBnClickedButtonTranM1()
{
	Loadcps(IDC_EDIT_TRAN_M1);
}

void SkillEditorLevelPage::OnBnClickedButtonTranM2()
{
	Loadcps(IDC_EDIT_TRAN_M2);
}

void SkillEditorLevelPage::OnBnClickedButtonTranM3()
{
	Loadcps(IDC_EDIT_TRAN_M3);
}

void SkillEditorLevelPage::OnBnClickedButtonTranM4()
{
	Loadcps(IDC_EDIT_TRAN_M4);
}

void SkillEditorLevelPage::OnBnClickedButtonTranM5()
{
	Loadcps(IDC_EDIT_TRAN_M5);
}

void SkillEditorLevelPage::OnBnClickedButtonTranM6()
{
	Loadcps(IDC_EDIT_TRAN_M6);
}

void SkillEditorLevelPage::OnBnClickedButtonTranM7()
{
	Loadcps(IDC_EDIT_TRAN_M7);
}

void SkillEditorLevelPage::OnBnClickedButtonTranM8()
{
	Loadcps(IDC_EDIT_TRAN_M8);
}

void SkillEditorLevelPage::OnBnClickedButtonTranW1()
{
	Loadcps(IDC_EDIT_TRAN_W1);
}

void SkillEditorLevelPage::OnBnClickedButtonTranW2()
{
	Loadcps(IDC_EDIT_TRAN_W2);
}

void SkillEditorLevelPage::OnBnClickedButtonTranW3()
{
	Loadcps(IDC_EDIT_TRAN_W3);
}

void SkillEditorLevelPage::OnBnClickedButtonTranW4()
{
	Loadcps(IDC_EDIT_TRAN_W4);
}

void SkillEditorLevelPage::OnBnClickedButtonTranW5()
{
	Loadcps(IDC_EDIT_TRAN_W5);
}

void SkillEditorLevelPage::OnBnClickedButtonTranW6()
{
	Loadcps(IDC_EDIT_TRAN_W6);
}

void SkillEditorLevelPage::OnBnClickedButtonTranW7()
{
	Loadcps(IDC_EDIT_TRAN_W7);
}

void SkillEditorLevelPage::OnBnClickedButtonTranW8()
{
	Loadcps(IDC_EDIT_TRAN_W8);
}
void SkillEditorLevelPage::ClearData()
{
	m_pDummySkill = NULL;
	m_pSkill = NULL;
}

void SkillEditorLevelPage::OnBnClickedButtonTranChf()
{
	Loadchf(IDC_EDIT_TRAN_CHF);
}
