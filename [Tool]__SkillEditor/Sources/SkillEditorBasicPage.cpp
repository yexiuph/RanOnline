// SkillEditorBasicPage.cpp : implementation file
//

#include "pch.h"
#include "SkillEditor.h"
#include "SkillEditorBasicPage.h"
#include "GLOGIC.h"

#include "DxEffcharData.h"
#include "DxSkinPieceContainer.h"
#include "DxSimpleMeshMan.h"
#include "EtcFunction.h"

#include "SheetWithTab.h"

#include "GLStringTable.h"
#include ".\skilleditorbasicpage.h"
#include "SkillEditorTreePage.h"

// SkillEditorBasicPage dialog

IMPLEMENT_DYNAMIC(SkillEditorBasicPage, CPropertyPage)
SkillEditorBasicPage::SkillEditorBasicPage(LOGFONT logfont)
	: CPropertyPage(SkillEditorBasicPage::IDD)
	,m_pFont( NULL )
	,m_SkillEditorTreePage( logfont )
	// m_SkillEditorLevelPage( logfont )
{
	m_bDlgInit = FALSE;
	m_pSkill = NULL;
	m_pDummySkill = new GLSKILL;

	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

SkillEditorBasicPage::~SkillEditorBasicPage()
{
	SAFE_DELETE ( m_pDummySkill );
	SAFE_DELETE ( m_pFont );
}

void SkillEditorBasicPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SkillEditorBasicPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_ELEMINUS, OnBnClickedButtonEleminus)
	ON_BN_CLICKED(IDC_BUTTON_ELEADD, OnBnClickedButtonEleadd)
	ON_CBN_SELCHANGE(IDC_COMBO_EMANIMID, OnCbnSelchangeComboEmanimid)
	ON_CBN_SELCHANGE(IDC_COMBO_EMANISID, OnCbnSelchangeComboEmanisid)
	ON_CBN_SELCHANGE(IDC_COMBO_EMBASICTYPE, OnCbnSelchangeComboEmbasictype)
	ON_CBN_SELCHANGE(IDC_COMBO_EMELEMENT, OnCbnSelchangeComboEmelement)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_ICON, OnBnClickedButtonIcon)
	ON_BN_CLICKED(IDC_BUTTON_TAR01, OnBnClickedButtonTar01)
	ON_BN_CLICKED(IDC_BUTTON_TAR02, OnBnClickedButtonTar02)
	ON_BN_CLICKED(IDC_BUTTON_SELF01, OnBnClickedButtonSelf01)
	ON_BN_CLICKED(IDC_BUTTON_SELF02, OnBnClickedButtonSelf02)
	ON_BN_CLICKED(IDC_BUTTON_TAR, OnBnClickedButtonTar)
	ON_BN_CLICKED(IDC_BUTTON_TARGETBODY3, OnBnClickedButtonTargetbody3)
	ON_BN_CLICKED(IDC_BUTTON_SELFBODY, OnBnClickedButtonSelfbody)
	ON_BN_CLICKED(IDC_BUTTON_TARGETBODY1, OnBnClickedButtonTargetbody1)
	ON_BN_CLICKED(IDC_BUTTON_TARGETBODY2, OnBnClickedButtonTargetbody2)
END_MESSAGE_MAP()


// SkillEditorBasicPage message handlers
BOOL SkillEditorBasicPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );
	
	m_bDlgInit = TRUE;
	InitDefaultCtrls ();
	UpdateSkill ();

	return TRUE;  // return TRUE unless you set the focus to a control
}
void SkillEditorBasicPage::OnBnClickedButtonOk()
{
	if ( !InverseUpdateSkill() )
	{		
		return;
	}

	WORD MID = m_pDummySkill->m_sBASIC.sNATIVEID.wMainID;
	WORD SID = m_pDummySkill->m_sBASIC.sNATIVEID.wSubID;

	GLStringTable::GetInstance().DeleteString ( strKeyName, strName, GLStringTable::SKILL );
	GLStringTable::GetInstance().InsertString ( strKeyName, strName, GLStringTable::SKILL );
	GLStringTable::GetInstance().InsertString ( strKeyDesc, strDesc, GLStringTable::SKILL );

	m_pSkill = NULL;
	m_SkillEditorTreePage.UpdateData ( MID,SID,m_pDummySkill );
	m_pSheetTab->ActiveSkillTreePage ();
}
void SkillEditorBasicPage::OnBnClickedButtonNext()
{
	if ( !InverseUpdateSkill() )
	{		
		return;
	}

	WORD MID = m_pDummySkill->m_sBASIC.sNATIVEID.wMainID;
	WORD SID = m_pDummySkill->m_sBASIC.sNATIVEID.wSubID;

	GLStringTable::GetInstance().DeleteString ( strKeyName, strName, GLStringTable::SKILL );
	GLStringTable::GetInstance().InsertString ( strKeyName, strName, GLStringTable::SKILL );
	GLStringTable::GetInstance().InsertString ( strKeyDesc, strDesc, GLStringTable::SKILL );

	//m_SkillEditorTreePage.UpdateData ( MID,SID,m_pDummySkill );
	m_pSheetTab->ActiveSkillLevelPage ( m_CallPage,m_pDummySkill );
	m_pSkill = NULL;
}

void SkillEditorBasicPage::OnBnClickedButtonCancel()
{
	m_pSkill = NULL;
	m_pSheetTab->ActiveSkillTreePage ();
}

void SkillEditorBasicPage::InitAllSkill ()
{
	m_pSkill = NULL;
}

BOOL SkillEditorBasicPage::SetSkill ( PGLSKILL pSkill )
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
		UpdateSkill ();		
	}

	return TRUE;
}
void SkillEditorBasicPage::InitDefaultCtrls ()
{
	//GetDlgItem ( IDC_BUTTON_PREV )->EnableWindow ( FALSE );

	SetWin_Combo_Init ( this, IDC_COMBO_EMROLE, COMMENT::SKILL_ROLE, SKILL::EMROLE_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_EMAPPLY, COMMENT::SKILL_APPLY, SKILL::EMAPPLY_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_EMIMPACTTAR, COMMENT::IMPACT_TAR, TAR_SIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_EMIMPACTREALM, COMMENT::IMPACT_REALM, REALM_SIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_EMIMPACTSIDE, COMMENT::IMPACT_SIDE, SIDE_SIZE );

	SetWin_Combo_Init ( this, IDC_COMBO_EEMUSELITEM, COMMENT::ITEMATTACK, ITEMATT_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_EEMUSERITEM, COMMENT::ITEMATTACK, ITEMATT_NSIZE );

	SetWin_Combo_Init ( this, IDC_COMBO_EMBASICTYPE, COMMENT::SKILL_TYPES, SKILL::FOR_TYPE_SIZE);
	SetWin_Combo_Init ( this, IDC_COMBO_EMELEMENT, COMMENT::ELEMENT, EMELEMENT_MAXNUM2);
	SetWin_Combo_Init ( this, IDC_COMBO_EMSTATEBLOW, COMMENT::BLOW, EMBLOW_SIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_EMADDON, COMMENT::IMPACT_ADDON, EIMPACTA_SIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_EMSPEC, COMMENT::SPEC_ADDON, EMSPECA_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_EMSSTYPE, COMMENT::SPECIAL_SKILL_TYPE, SKILL::EMSSTYPE_NSIZE );
	
	SetWin_Combo_Init ( this, IDC_COMBO_TAR01, COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_TAR02, COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_SELF01, COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_SELF02, COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_TAR, COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_SELFBODY, COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_TARGETBODY1, COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_TARGETBODY2, COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );

	SetWin_Combo_Init ( this, IDC_COMBO_TARP01, COMMENT::SKILL_EFFPOS, SKILL::EMPOS_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_TARP02, COMMENT::SKILL_EFFPOS, SKILL::EMPOS_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_SELFP01, COMMENT::SKILL_EFFPOS, SKILL::EMPOS_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_SELFP02, COMMENT::SKILL_EFFPOS, SKILL::EMPOS_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_TARPA, COMMENT::SKILL_EFFPOS, SKILL::EMPOS_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_TARPB, COMMENT::SKILL_EFFPOS, SKILL::EMPOS_NSIZE );

	SetWin_Combo_Init ( this, IDC_COMBO_EMANIMID, COMMENT::ANI_MAINTYPE, AN_TYPE_SIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_EMANISID, COMMENT::ANI_SUBTYPE_00, AN_SUB_00_SIZE );

	SetWin_Combo_Init ( this, IDC_COMBO_EMBRIGHT, COMMENT::BRIGHT, BRIGHT_SIZE );

	m_dwEffectType = 0;

}
void SkillEditorBasicPage::UpdateSkill ()
{
	CString strTemp("");
	//sbasic
	SetWin_Num_int ( this, IDC_EDIT_SMID, m_pDummySkill->m_sBASIC.sNATIVEID.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_SSID, m_pDummySkill->m_sBASIC.sNATIVEID.wSubID );

	SetWin_Check ( this, IDC_CHECK_GM, m_pDummySkill->m_sBASIC.bLearnView );

	SetWin_Text ( this, IDC_EDIT_SNAME, m_pDummySkill->m_sBASIC.szNAME );

	SetWin_Text ( this, IDC_EDIT_ENAME, m_pDummySkill->GetName() );
	SetWin_Text ( this, IDC_EDIT_EDESC, m_pDummySkill->GetDesc() );

	SetWin_Num_int ( this, IDC_EDIT_BGRADE, m_pDummySkill->m_sBASIC.dwGRADE );
	SetWin_Num_int ( this, IDC_EDIT_BMASTERLEVEL, m_pDummySkill->m_sBASIC.dwMAXLEVEL );

	SetWin_Combo_Sel ( this, IDC_COMBO_EMROLE, (int)m_pDummySkill->m_sBASIC.emROLE );
	SetWin_Combo_Sel ( this, IDC_COMBO_EMAPPLY, (int)m_pDummySkill->m_sBASIC.emAPPLY );
	SetWin_Combo_Sel ( this, IDC_COMBO_EMIMPACTTAR, (int)m_pDummySkill->m_sBASIC.emIMPACT_TAR );
	SetWin_Combo_Sel ( this, IDC_COMBO_EMIMPACTREALM, (int)m_pDummySkill->m_sBASIC.emIMPACT_REALM );
	SetWin_Combo_Sel ( this, IDC_COMBO_EMIMPACTSIDE, (int)m_pDummySkill->m_sBASIC.emIMPACT_SIDE );

	SetWin_Num_int ( this, IDC_EDIT_BRANGE, m_pDummySkill->m_sBASIC.wTARRANGE );

	SetWin_Combo_Sel ( this, IDC_COMBO_EEMUSELITEM, (int)m_pDummySkill->m_sBASIC.emUSE_LITEM );
	SetWin_Combo_Sel ( this, IDC_COMBO_EEMUSERITEM, (int)m_pDummySkill->m_sBASIC.emUSE_RITEM );
	//
	//flag
	SetWin_Combo_Sel ( this, IDC_COMBO_EMBASICTYPE, (int)m_pDummySkill->m_sAPPLY.emBASIC_TYPE );
	SetWin_Combo_Sel ( this, IDC_COMBO_EMELEMENT, (int)m_pDummySkill->m_sAPPLY.emELEMENT );
	SetWin_Combo_Sel ( this, IDC_COMBO_EMSTATEBLOW, (int)m_pDummySkill->m_sAPPLY.emSTATE_BLOW );
	SetWin_Combo_Sel ( this, IDC_COMBO_EMADDON, (int)m_pDummySkill->m_sAPPLY.emADDON );
	SetWin_Combo_Sel ( this, IDC_COMBO_EMSPEC, (int)m_pDummySkill->m_sAPPLY.emSPEC );
	SetWin_Combo_Sel ( this, IDC_COMBO_EMSSTYPE, (int)m_pDummySkill->m_sSPECIAL_SKILL.emSSTYPE );

	SetWin_Combo_Sel ( this, IDC_COMBO_EMBRIGHT, (int)m_pDummySkill->m_sLEARN.emBRIGHT );
	SetWin_Num_int ( this, IDC_EDIT_REQSKILLMID, m_pDummySkill->m_sLEARN.sSKILL.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_REQSKILLSID, m_pDummySkill->m_sLEARN.sSKILL.wSubID );

	SetWin_Check ( this, IDC_CHECK_CURE_PARALYSIS, m_pDummySkill->m_sAPPLY.dwCUREFLAG&DIS_NUMB );
	SetWin_Check ( this, IDC_CHECK_CURE_FAINT, m_pDummySkill->m_sAPPLY.dwCUREFLAG&DIS_STUN  );
	SetWin_Check ( this, IDC_CHECK_CURE_STONE, m_pDummySkill->m_sAPPLY.dwCUREFLAG&DIS_STONE );
	SetWin_Check ( this, IDC_CHECK_CURE_FLAME, m_pDummySkill->m_sAPPLY.dwCUREFLAG&DIS_BURN );
	SetWin_Check ( this, IDC_CHECK_CURE_FREEZE, m_pDummySkill->m_sAPPLY.dwCUREFLAG&DIS_FROZEN );
	SetWin_Check ( this, IDC_CHECK_CURE_MADNESS, m_pDummySkill->m_sAPPLY.dwCUREFLAG&DIS_MAD  );
	SetWin_Check ( this, IDC_CHECK_CURE_POISON, m_pDummySkill->m_sAPPLY.dwCUREFLAG&DIS_POISON );
	SetWin_Check ( this, IDC_CHECK_CURE_CURSE, m_pDummySkill->m_sAPPLY.dwCUREFLAG&DIS_CURSE );

	UpdateSkillSExtData ( m_dwEffectType );
	OnCbnSelchangeComboEmanimid();
	OnCbnSelchangeComboEmanisid();
	OnCbnSelchangeComboEmbasictype();
	OnCbnSelchangeComboEmelement();
}
BOOL SkillEditorBasicPage::InverseUpdateSkill()
{
	m_pDummySkill->m_sBASIC.bLearnView = GetWin_Check ( this, IDC_CHECK_GM );

	//m_pDummySkill->m_sBASIC.szNAME =  GetWin_Text ( this, IDC_EDIT_SNAME );

	strKeyName = GetWin_Text ( this, IDC_EDIT_SNAME ).GetString();
	strName	= GetWin_Text ( this, IDC_EDIT_ENAME ).GetString();

	strKeyDesc = GetWin_Text ( this, IDC_EDIT_SDESC ).GetString();
	CString strComment = GetWin_Text ( this, IDC_EDIT_EDESC );
	if ( strComment.GetLength()+1 < ITEM_SZCOMMENT ) strDesc = strComment.GetString();
	
	m_pDummySkill->m_sBASIC.dwGRADE =  GetWin_Num_int ( this, IDC_EDIT_BGRADE );
	m_pDummySkill->m_sBASIC.dwMAXLEVEL =  GetWin_Num_int ( this, IDC_EDIT_BMASTERLEVEL );

	m_pDummySkill->m_sBASIC.emROLE = (SKILL::EMROLE) GetWin_Combo_Sel ( this, IDC_COMBO_EMROLE );
	m_pDummySkill->m_sBASIC.emAPPLY = (SKILL::EMAPPLY) GetWin_Combo_Sel ( this, IDC_COMBO_EMAPPLY );
	m_pDummySkill->m_sBASIC.emIMPACT_TAR = (EMIMPACT_TAR) GetWin_Combo_Sel ( this, IDC_COMBO_EMIMPACTTAR );
	m_pDummySkill->m_sBASIC.emIMPACT_REALM = (EMIMPACT_REALM) GetWin_Combo_Sel ( this, IDC_COMBO_EMIMPACTREALM );
	m_pDummySkill->m_sBASIC.emIMPACT_SIDE = (EMIMPACT_SIDE) GetWin_Combo_Sel ( this, IDC_COMBO_EMIMPACTSIDE );
	
	m_pDummySkill->m_sBASIC.wTARRANGE = GetWin_Num_int ( this, IDC_EDIT_BRANGE );

	m_pDummySkill->m_sBASIC.emUSE_LITEM = (GLITEM_ATT) GetWin_Combo_Sel ( this, IDC_COMBO_EEMUSELITEM );
	m_pDummySkill->m_sBASIC.emUSE_RITEM = (GLITEM_ATT) GetWin_Combo_Sel ( this, IDC_COMBO_EEMUSERITEM );

	m_pDummySkill->m_sAPPLY.emBASIC_TYPE = (SKILL::EMTYPES) GetWin_Combo_Sel ( this, IDC_COMBO_EMBASICTYPE );
	m_pDummySkill->m_sAPPLY.emELEMENT = (EMELEMENT) GetWin_Combo_Sel ( this, IDC_COMBO_EMELEMENT);
	m_pDummySkill->m_sAPPLY.emSTATE_BLOW = (EMSTATE_BLOW) GetWin_Combo_Sel ( this, IDC_COMBO_EMSTATEBLOW);
	m_pDummySkill->m_sAPPLY.emADDON = (EMIMPACT_ADDON) GetWin_Combo_Sel ( this, IDC_COMBO_EMADDON);
	m_pDummySkill->m_sAPPLY.emSPEC = (EMSPEC_ADDON) GetWin_Combo_Sel ( this, IDC_COMBO_EMSPEC);
	m_pDummySkill->m_sSPECIAL_SKILL.emSSTYPE = (SKILL::EMSPECIALSKILLTYPE) GetWin_Combo_Sel ( this, IDC_COMBO_EMSSTYPE);

	m_pDummySkill->m_sLEARN.emBRIGHT = (EMBRIGHT) GetWin_Combo_Sel ( this, IDC_COMBO_EMBRIGHT);
	m_pDummySkill->m_sLEARN.sSKILL.wMainID = GetWin_Num_int ( this, IDC_EDIT_REQSKILLMID );
	m_pDummySkill->m_sLEARN.sSKILL.wSubID = GetWin_Num_int ( this, IDC_EDIT_REQSKILLSID ); 

	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CURE_PARALYSIS ), m_pDummySkill->m_sAPPLY.dwCUREFLAG, DIS_NUMB );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CURE_FAINT ), m_pDummySkill->m_sAPPLY.dwCUREFLAG, DIS_STUN  );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CURE_STONE ), m_pDummySkill->m_sAPPLY.dwCUREFLAG, DIS_STONE );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CURE_FLAME ), m_pDummySkill->m_sAPPLY.dwCUREFLAG, DIS_BURN );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CURE_FREEZE ), m_pDummySkill->m_sAPPLY.dwCUREFLAG, DIS_FROZEN );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CURE_MADNESS ), m_pDummySkill->m_sAPPLY.dwCUREFLAG, DIS_MAD  );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CURE_POISON ), m_pDummySkill->m_sAPPLY.dwCUREFLAG, DIS_POISON );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CURE_CURSE ), m_pDummySkill->m_sAPPLY.dwCUREFLAG, DIS_CURSE );

	InverseUpdateSkillSExtData (m_dwEffectType);

	return TRUE;
}
void SkillEditorBasicPage::UpdateSkillSExtData (DWORD dwType)
{
	SetWin_Text ( this, IDC_EDIT_ELEMTYPE, COMMENT::ELEMENT[dwType].c_str() );

	SetWin_Combo_Sel ( this, IDC_COMBO_EMANIMID, (int)m_pDummySkill->m_sEXT_DATA.emANIMTYPE );
	SetWin_Combo_Sel ( this, IDC_COMBO_EMANISID, (int)m_pDummySkill->m_sEXT_DATA.emANISTYPE );

	SetWin_Num_float ( this, IDC_EDIT_DAMAGEHOLD, m_pDummySkill->m_sEXT_DATA.fDELAY4DAMAGE );

	SetWin_Combo_Sel ( this, IDC_COMBO_TAR, (int)m_pDummySkill->m_sEXT_DATA.emTARG );
	SetWin_Combo_Sel ( this, IDC_COMBO_TARPA, (int)m_pDummySkill->m_sEXT_DATA.emTARG_POSA );
	SetWin_Combo_Sel ( this, IDC_COMBO_TARPB, (int)m_pDummySkill->m_sEXT_DATA.emTARG_POSB );
	SetWin_Check ( this, IDC_CHECK_SINGLEEFFECT, m_pDummySkill->m_sEXT_DATA.bTARG_ONE );
	SetWin_Text ( this, IDC_EDIT_TAR, m_pDummySkill->m_sEXT_DATA.strTARG[dwType].c_str() );

	SetWin_Combo_Sel ( this, IDC_COMBO_SELF01, (int)m_pDummySkill->m_sEXT_DATA.emSELFZONE01 );
	SetWin_Combo_Sel ( this, IDC_COMBO_SELFP01, (int)m_pDummySkill->m_sEXT_DATA.emSELFZONE01_POS );
	SetWin_Text ( this, IDC_EDIT_SELF01, m_pDummySkill->m_sEXT_DATA.strSELFZONE01[dwType].c_str() );

	SetWin_Combo_Sel ( this, IDC_COMBO_SELF02, (int)m_pDummySkill->m_sEXT_DATA.emSELFZONE02 );
	SetWin_Combo_Sel ( this, IDC_COMBO_SELFP02, (int)m_pDummySkill->m_sEXT_DATA.emSELFZONE02_POS );
	SetWin_Text ( this, IDC_EDIT_SELF02, m_pDummySkill->m_sEXT_DATA.strSELFZONE02[dwType].c_str() );

	SetWin_Combo_Sel ( this, IDC_COMBO_TAR01, (int)m_pDummySkill->m_sEXT_DATA.emTARGZONE01 );
	SetWin_Combo_Sel ( this, IDC_COMBO_TARP01, (int)m_pDummySkill->m_sEXT_DATA.emTARGZONE01_POS );
	SetWin_Text ( this, IDC_EDIT_TAR01, m_pDummySkill->m_sEXT_DATA.strTARGZONE01[dwType].c_str() );

	SetWin_Combo_Sel ( this, IDC_COMBO_TAR02, (int)m_pDummySkill->m_sEXT_DATA.emTARGZONE02 );
	SetWin_Combo_Sel ( this, IDC_COMBO_TARP02, (int)m_pDummySkill->m_sEXT_DATA.emTARGZONE02_POS );
	SetWin_Text ( this, IDC_EDIT_TAR02, m_pDummySkill->m_sEXT_DATA.strTARGZONE02[dwType].c_str() );

	SetWin_Combo_Sel ( this, IDC_COMBO_SELFBODY, (int)m_pDummySkill->m_sEXT_DATA.emSELFBODY );
	SetWin_Text ( this, IDC_EDIT_SELFBODY, m_pDummySkill->m_sEXT_DATA.strSELFBODY[dwType].c_str() );

	SetWin_Combo_Sel ( this, IDC_COMBO_TARGETBODY1, (int)m_pDummySkill->m_sEXT_DATA.emTARGBODY01 );
	SetWin_Text ( this, IDC_EDIT_TARGETBODY1, m_pDummySkill->m_sEXT_DATA.strTARGBODY01[dwType].c_str() );

	SetWin_Combo_Sel ( this, IDC_COMBO_TARGETBODY2, (int)m_pDummySkill->m_sEXT_DATA.emTARGBODY02 );
	SetWin_Text ( this, IDC_EDIT_TARGETBODY2, m_pDummySkill->m_sEXT_DATA.strTARGBODY02[dwType].c_str() );

	SetWin_Text ( this, IDC_EDIT_HOLDOUT, m_pDummySkill->m_sEXT_DATA.strHOLDOUT.c_str() );
	SetWin_Text ( this, IDC_EDIT_ICON, m_pDummySkill->m_sEXT_DATA.strICONFILE.c_str() );

	SetWin_Num_int ( this, IDC_EDIT_ICON_X, m_pDummySkill->m_sEXT_DATA.sICONINDEX.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_ICON_Y, m_pDummySkill->m_sEXT_DATA.sICONINDEX.wSubID );

	SetWin_Text ( this, IDC_EDIT_SDESC, m_pDummySkill->m_sEXT_DATA.strCOMMENTS.c_str() );

}

BOOL  SkillEditorBasicPage::InverseUpdateSkillSExtData (DWORD dwType)
{
	m_pDummySkill->m_sEXT_DATA.emANIMTYPE = (EMANI_MAINTYPE) GetWin_Combo_Sel ( this, IDC_COMBO_EMANIMID );
	m_pDummySkill->m_sEXT_DATA.emANISTYPE = (EMANI_SUBTYPE) GetWin_Combo_Sel ( this, IDC_COMBO_EMANISID );
	
	m_pDummySkill->m_sEXT_DATA.fDELAY4DAMAGE = GetWin_Num_float ( this, IDC_EDIT_DAMAGEHOLD );
	m_pDummySkill->m_sEXT_DATA.emTARG = (SKILL::EMEFFECTIME) GetWin_Combo_Sel ( this, IDC_COMBO_TAR );
	m_pDummySkill->m_sEXT_DATA.emTARG_POSA = (SKILL::EMEFFECTPOS) GetWin_Combo_Sel ( this, IDC_COMBO_TARPA );
	m_pDummySkill->m_sEXT_DATA.emTARG_POSB = (SKILL::EMEFFECTPOS) GetWin_Combo_Sel ( this, IDC_COMBO_TARPB );
	m_pDummySkill->m_sEXT_DATA.bTARG_ONE = GetWin_Check ( this, IDC_CHECK_SINGLEEFFECT);
	m_pDummySkill->m_sEXT_DATA.strTARG[dwType] = GetWin_Text( this, IDC_EDIT_TAR);
	
	m_pDummySkill->m_sEXT_DATA.emSELFZONE01 = (SKILL::EMEFFECTIME) GetWin_Combo_Sel ( this, IDC_COMBO_SELF01 );
	m_pDummySkill->m_sEXT_DATA.emSELFZONE01_POS = (SKILL::EMEFFECTPOS) GetWin_Combo_Sel ( this, IDC_COMBO_SELFP01);
	m_pDummySkill->m_sEXT_DATA.strSELFZONE01[dwType] = GetWin_Text( this, IDC_EDIT_SELF01 );

	m_pDummySkill->m_sEXT_DATA.emSELFZONE02 = (SKILL::EMEFFECTIME) GetWin_Combo_Sel ( this, IDC_COMBO_SELF02 );
	m_pDummySkill->m_sEXT_DATA.emSELFZONE02_POS = (SKILL::EMEFFECTPOS) GetWin_Combo_Sel ( this, IDC_COMBO_SELFP02);
	m_pDummySkill->m_sEXT_DATA.strSELFZONE02[dwType] = GetWin_Text( this, IDC_EDIT_SELF02 );

	m_pDummySkill->m_sEXT_DATA.emTARGZONE01 = (SKILL::EMEFFECTIME) GetWin_Combo_Sel ( this, IDC_COMBO_TAR01 );
	m_pDummySkill->m_sEXT_DATA.emTARGZONE01_POS = (SKILL::EMEFFECTPOS) GetWin_Combo_Sel ( this, IDC_COMBO_TARP01);
	m_pDummySkill->m_sEXT_DATA.strTARGZONE01[dwType] = GetWin_Text( this, IDC_EDIT_TAR01 );

	m_pDummySkill->m_sEXT_DATA.emTARGZONE02 = (SKILL::EMEFFECTIME) GetWin_Combo_Sel ( this, IDC_COMBO_TAR02 );
	m_pDummySkill->m_sEXT_DATA.emTARGZONE02_POS = (SKILL::EMEFFECTPOS) GetWin_Combo_Sel ( this, IDC_COMBO_TARP02);
	m_pDummySkill->m_sEXT_DATA.strTARGZONE02[dwType] = GetWin_Text( this, IDC_EDIT_TAR02 );

	m_pDummySkill->m_sEXT_DATA.emSELFBODY = (SKILL::EMEFFECTIME) GetWin_Combo_Sel ( this, IDC_COMBO_SELFBODY );
	m_pDummySkill->m_sEXT_DATA.strSELFBODY[dwType] = GetWin_Text( this, IDC_EDIT_SELFBODY );

	m_pDummySkill->m_sEXT_DATA.emTARGBODY01 = (SKILL::EMEFFECTIME) GetWin_Combo_Sel ( this, IDC_COMBO_TARGETBODY1 );
	m_pDummySkill->m_sEXT_DATA.strTARGBODY01[dwType] = GetWin_Text( this, IDC_EDIT_TARGETBODY1 );

	m_pDummySkill->m_sEXT_DATA.emTARGBODY02 = (SKILL::EMEFFECTIME) GetWin_Combo_Sel ( this, IDC_COMBO_TARGETBODY2 );
	m_pDummySkill->m_sEXT_DATA.strTARGBODY02[dwType] = GetWin_Text( this, IDC_EDIT_TARGETBODY2 );

	m_pDummySkill->m_sEXT_DATA.strHOLDOUT = GetWin_Text( this, IDC_EDIT_HOLDOUT );
	m_pDummySkill->m_sEXT_DATA.strICONFILE = GetWin_Text( this, IDC_EDIT_ICON );

	m_pDummySkill->m_sEXT_DATA.sICONINDEX.wMainID =  GetWin_Num_int ( this, IDC_EDIT_ICON_X );
	m_pDummySkill->m_sEXT_DATA.sICONINDEX.wSubID =  GetWin_Num_int ( this, IDC_EDIT_ICON_Y );

	m_pDummySkill->m_sEXT_DATA.strCOMMENTS = GetWin_Text( this, IDC_EDIT_SDESC );

	return TRUE;
}
void SkillEditorBasicPage::OnBnClickedButtonEleminus()
{
	InverseUpdateSkillSExtData (m_dwEffectType);
	if ( m_dwEffectType <= (DWORD) EMELEMENT_SPIRIT ) return ;
	m_dwEffectType--;
	UpdateSkillSExtData( m_dwEffectType );
}

void SkillEditorBasicPage::OnBnClickedButtonEleadd()
{
		InverseUpdateSkillSExtData (m_dwEffectType);
	m_dwEffectType++;
	if ( m_dwEffectType > (DWORD) EMELEMENT_MAXNUM-1 ) m_dwEffectType--;
	UpdateSkillSExtData( m_dwEffectType );
}

void SkillEditorBasicPage::OnCbnSelchangeComboEmanimid()
{
	SetWin_Num_int ( this, IDC_EDIT_ANIMID, GetWin_Combo_Sel ( this, IDC_COMBO_EMANIMID ) );	
}

void SkillEditorBasicPage::OnCbnSelchangeComboEmanisid()
{
	SetWin_Num_int ( this, IDC_EDIT_ANISID, GetWin_Combo_Sel ( this, IDC_COMBO_EMANISID ) );
}

void SkillEditorBasicPage::OnCbnSelchangeComboEmbasictype()
{
	int Selected = GetWin_Combo_Sel ( this, IDC_COMBO_EMBASICTYPE );
	if ( Selected == 7 )
	{
		SetWin_Enable ( this, IDC_CHECK_CURE_PARALYSIS, 1 );
		SetWin_Enable ( this, IDC_CHECK_CURE_FAINT, 1 );
		SetWin_Enable ( this, IDC_CHECK_CURE_STONE, 1 );
		SetWin_Enable ( this, IDC_CHECK_CURE_FLAME, 1 );
		SetWin_Enable ( this, IDC_CHECK_CURE_FREEZE, 1 );
		SetWin_Enable ( this, IDC_CHECK_CURE_MADNESS, 1 );
		SetWin_Enable ( this, IDC_CHECK_CURE_POISON, 1 );
		SetWin_Enable ( this, IDC_CHECK_CURE_CURSE, 1 );
	}
	else
	{
		SetWin_Enable ( this, IDC_CHECK_CURE_PARALYSIS, 0 );
		SetWin_Enable ( this, IDC_CHECK_CURE_FAINT, 0 );
		SetWin_Enable ( this, IDC_CHECK_CURE_STONE, 0 );
		SetWin_Enable ( this, IDC_CHECK_CURE_FLAME, 0 );
		SetWin_Enable ( this, IDC_CHECK_CURE_FREEZE, 0 );
		SetWin_Enable ( this, IDC_CHECK_CURE_MADNESS, 0 );
		SetWin_Enable ( this, IDC_CHECK_CURE_POISON, 0 );
		SetWin_Enable ( this, IDC_CHECK_CURE_CURSE, 0 );
	}
}

void SkillEditorBasicPage::OnCbnSelchangeComboEmelement()
{
	int Selected = GetWin_Combo_Sel ( this, IDC_COMBO_EMELEMENT );
	if ( Selected == 9 )
	{
		SetWin_Enable ( this, IDC_BUTTON_ELEMINUS, 1 );
		SetWin_Enable ( this, IDC_BUTTON_ELEADD, 1 );
	}
	else
	{
		SetWin_Enable ( this, IDC_BUTTON_ELEMINUS, 0 );
		SetWin_Enable ( this, IDC_BUTTON_ELEADD, 0 );

		m_dwEffectType = 0;
		UpdateSkillSExtData( m_dwEffectType );
	}
}

void SkillEditorBasicPage::Loadegp( int nID )
{
	CString szFilter = "Effect File (*.egp)|*.egp|";
	
	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath ();

	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
	}
}
void SkillEditorBasicPage::Loadeffskin( int nID )
{
	CString szFilter = "Effect File (*.effskin_a)|*.effskin_a|";
	
	CFileDialog dlg(TRUE,".effskin_a",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
	}
}
void SkillEditorBasicPage::Loadtexture( int nID )
{
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	
	CFileDialog dlg(TRUE,".x",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(SkillEditorBasicPage*)this);

	dlg.m_ofn.lpstrInitialDir = TextureManager::GetTexturePath();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
	}
}

void SkillEditorBasicPage::OnBnClickedButtonIcon()
{
	Loadtexture( IDC_EDIT_ICON );
}

void SkillEditorBasicPage::OnBnClickedButtonTar01()
{
	Loadegp( IDC_EDIT_TAR01 );
}

void SkillEditorBasicPage::OnBnClickedButtonTar02()
{
	Loadegp( IDC_EDIT_TAR02 );
}

void SkillEditorBasicPage::OnBnClickedButtonSelf01()
{
	Loadegp( IDC_EDIT_SELF01 );
}

void SkillEditorBasicPage::OnBnClickedButtonSelf02()
{
	Loadegp( IDC_EDIT_SELF02 );
}

void SkillEditorBasicPage::OnBnClickedButtonTar()
{
	Loadegp( IDC_EDIT_TAR );
}

void SkillEditorBasicPage::OnBnClickedButtonTargetbody3()
{
	Loadegp( IDC_EDIT_HOLDOUT );
}

void SkillEditorBasicPage::OnBnClickedButtonSelfbody()
{
	Loadeffskin( IDC_EDIT_SELFBODY );
}

void SkillEditorBasicPage::OnBnClickedButtonTargetbody1()
{
	Loadeffskin( IDC_EDIT_TARGETBODY1 );
}

void SkillEditorBasicPage::OnBnClickedButtonTargetbody2()
{
	Loadeffskin( IDC_EDIT_TARGETBODY2 );
}

void SkillEditorBasicPage::ClearData()
{
	m_pDummySkill = NULL;
	m_pSkill = NULL;
}
