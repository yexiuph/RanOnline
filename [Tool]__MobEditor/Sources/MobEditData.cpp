// MobEditData.cpp : implementation file
//

#include "pch.h"
#include "MobEdit.h"
#include "MobEditData.h"
#include "SheetWithTab.h"
#include ".\mobeditdata.h"
#include "GLStringTable.h"
#include "EtcFunction.h"
#include "GLOGIC.h"
#include "DxEffcharData.h"
#include "DxSkinPieceContainer.h"
#include "DxSimpleMeshMan.h"


// MobEditData dialog

IMPLEMENT_DYNAMIC(MobEditData, CPropertyPage)
MobEditData::MobEditData(LOGFONT logfont)
	: CPropertyPage(MobEditData::IDD)
	,m_pFont( NULL )
	,m_MobEditTree( logfont )
{
	m_bDlgInit = FALSE;
	m_pCrow = NULL;
	m_pDummyCrow = new SCROWDATA;
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
	bAdd = FALSE;
}

MobEditData::~MobEditData()
{
	SAFE_DELETE ( m_pDummyCrow );
	SAFE_DELETE ( m_pFont );
}

void MobEditData::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PATTERN, m_listPattern);
}


BEGIN_MESSAGE_MAP(MobEditData, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_PATTERNADD, OnBnClickedButtonPatternadd)
	ON_BN_CLICKED(IDC_BUTTON_PATTERNEDIT, OnBnClickedButtonPatternedit)
	ON_BN_CLICKED(IDC_BUTTON_PATTERNDELETE, OnBnClickedButtonPatterndelete)
	ON_BN_CLICKED(IDC_BUTTON_PATTERNOK, OnBnClickedButtonPatternok)
	ON_BN_CLICKED(IDC_BUTTON_SKINFILE, OnBnClickedButtonSkinfile)
	ON_BN_CLICKED(IDC_BUTTON_CONVERSATIONFILE, OnBnClickedButtonConversationfile)
	ON_BN_CLICKED(IDC_BUTTON_SALE1, OnBnClickedButtonSale1)
	ON_BN_CLICKED(IDC_BUTTON_SALE2, OnBnClickedButtonSale2)
	ON_BN_CLICKED(IDC_BUTTON_SALE3, OnBnClickedButtonSale3)
	ON_BN_CLICKED(IDC_BUTTON_BIRTHEFFECT, OnBnClickedButtonBirtheffect)
	ON_BN_CLICKED(IDC_BUTTON_DEATHEFFECT, OnBnClickedButtonDeatheffect)
	ON_BN_CLICKED(IDC_BUTTON_HITEFFECT, OnBnClickedButtonHiteffect)
	ON_BN_CLICKED(IDC_BUTTON_ITEMGEN, OnBnClickedButtonItemgen)
	ON_BN_CLICKED(IDC_BUTTON_ITEMGEN_Q, OnBnClickedButtonItemgenQ)
END_MESSAGE_MAP()


// MobEditData message handlers
BOOL MobEditData::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );
	
	m_bDlgInit = TRUE;
	InitDefaultCtrls ();
	UpdateCrow ();

	return TRUE;  // return TRUE unless you set the focus to a control
}
BOOL MobEditData::SetCrow ( PCROWDATA pCrow )
{
	if ( m_pCrow )
	{
		return FALSE;
	}

	if (pCrow)
	{
		m_pCrow = pCrow;
		m_pDummyCrow = m_pCrow;
	}

	if ( m_bDlgInit ) 
	{
		InitDefaultCtrls ();
		UpdateCrow ();		
	}

	return TRUE;
}
void MobEditData::OnBnClickedButtonOk()
{
	if ( !InverseUpdateCrow() )
	{		
		return;
	}

	WORD MID = m_pDummyCrow->m_sBasic.sNativeID.wMainID;
	WORD SID = m_pDummyCrow->m_sBasic.sNativeID.wSubID;

	GLStringTable::GetInstance().DeleteString ( MID, SID, GLStringTable::CROW );
	GLStringTable::GetInstance().InsertString ( strKeyName, strName, GLStringTable::CROW );

	m_pCrow = NULL;
	m_MobEditTree.UpdateData ( MID,SID,m_pDummyCrow );
	m_pSheetTab->ActiveCrowTreePage ();
}

void MobEditData::OnBnClickedButtonCancel()
{
	m_pCrow = NULL;
	m_pSheetTab->ActiveCrowTreePage ();
}

void MobEditData::OnBnClickedButtonNext()
{
	if ( !InverseUpdateCrow() )
	{		
		return;
	}

	WORD MID = m_pDummyCrow->m_sBasic.sNativeID.wMainID;
	WORD SID = m_pDummyCrow->m_sBasic.sNativeID.wSubID;

	GLStringTable::GetInstance().DeleteString ( MID, SID, GLStringTable::CROW );
	GLStringTable::GetInstance().InsertString ( strKeyName, strName, GLStringTable::CROW );

	m_pSheetTab->ActiveCrowAttackPage ( m_CallPage,m_pDummyCrow );
	m_pCrow = NULL;
}
void MobEditData::InitDefaultCtrls ()
{
	SetWin_Combo_Init ( this, IDC_COMBO_EMTRIBE, COMMENT::TRIBE, TRIBE_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_EMCROW, COMMENT::CROW, CROW_NUM );
	SetWin_Combo_Init ( this, IDC_COMBO_EMBRIGHT, COMMENT::BRIGHT, BRIGHT_SIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_PETTYPE, COMMENT::PET_TYPE, PETTYPE_SIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_GATHERANIMATION, COMMENT::ANI_SUBTYPE_00, AN_SUB_00_SIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_ACTIONUP, COMMENT::szCROWACT_UP, EMCROWACT_UP_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_ACTIONDN, COMMENT::szCROWACT_DN, EMCROWACT_DN_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_MOVETYPE, COMMENT::MOVETYPE, MOVETYPE_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_ACTPATTERN, COMMENT::szCROWACT_UP, EMCROWACT_UP_NSIZE );
}
void MobEditData::UpdateCrow ()
{	//
	//scrowbasic
	SetWin_Num_int ( this, IDC_CROW_MID, m_pDummyCrow->m_sBasic.sNativeID.wMainID );
	SetWin_Num_int ( this, IDC_CROW_SID, m_pDummyCrow->m_sBasic.sNativeID.wSubID );
	SetWin_Text ( this, IDC_CROW_ID, m_pDummyCrow->m_sBasic.m_szName );
	SetWin_Text ( this, IDC_CROW_NAME, m_pDummyCrow->GetName() );
	SetWin_Combo_Sel ( this, IDC_COMBO_EMTRIBE, (int)m_pDummyCrow->m_sBasic.m_emTribe );
	SetWin_Combo_Sel ( this, IDC_COMBO_EMCROW, (int)m_pDummyCrow->m_sBasic.m_emCrow );
	SetWin_Combo_Sel ( this, IDC_COMBO_EMBRIGHT, (int)m_pDummyCrow->m_sBasic.m_emBright );
	SetWin_Combo_Sel ( this, IDC_COMBO_PETTYPE, (int)m_pDummyCrow->m_sBasic.m_emPetType );
	SetWin_Combo_Sel ( this, IDC_COMBO_GATHERANIMATION, (int)m_pDummyCrow->m_sBasic.m_wGatherAnimation );
	SetWin_Check ( this, IDC_CHECK_OVERLAPATTACK, (BOOL) m_pDummyCrow->m_sBasic.m_bOverlapAttack );
	SetWin_Num_int ( this, IDC_EDIT_LEVEL, m_pDummyCrow->m_sBasic.m_wLevel );
	SetWin_Num_int ( this, IDC_EDIT_HP, m_pDummyCrow->m_sBasic.m_dwHP );
	SetWin_Num_int ( this, IDC_EDIT_MP, m_pDummyCrow->m_sBasic.m_wMP );
	SetWin_Num_int ( this, IDC_EDIT_SP, m_pDummyCrow->m_sBasic.m_wSP );
	SetWin_Num_float ( this, IDC_EDIT_HPREC, m_pDummyCrow->m_sBasic.m_fIncHP );
	SetWin_Num_float ( this, IDC_EDIT_MPREC, m_pDummyCrow->m_sBasic.m_fIncMP );
	SetWin_Num_float ( this, IDC_EDIT_SPREC, m_pDummyCrow->m_sBasic.m_fIncSP );
	SetWin_Num_int ( this, IDC_EDIT_AVOIDRATE, m_pDummyCrow->m_sBasic.m_wAvoidRate );
	SetWin_Num_int ( this, IDC_EDIT_HITRATE, m_pDummyCrow->m_sBasic.m_wHitRate );
	SetWin_Num_int ( this, IDC_EDIT_VIEWDISTANCE, m_pDummyCrow->m_sBasic.m_wViewRange );
	SetWin_Num_int ( this, IDC_EDIT_DEFENSE, m_pDummyCrow->m_sBasic.m_wDefense );
	SetWin_Num_int ( this, IDC_EDIT_RES_FIRE, m_pDummyCrow->m_sBasic.m_sResist.nFire );
	SetWin_Num_int ( this, IDC_EDIT_RES_ELECT, m_pDummyCrow->m_sBasic.m_sResist.nElectric );
	SetWin_Num_int ( this, IDC_EDIT_RES_SPIRIT, m_pDummyCrow->m_sBasic.m_sResist.nSpirit );
	SetWin_Num_int ( this, IDC_EDIT_RES_ICE, m_pDummyCrow->m_sBasic.m_sResist.nIce );
	SetWin_Num_int ( this, IDC_EDIT_RES_POISON, m_pDummyCrow->m_sBasic.m_sResist.nPoison );
	SetWin_Num_int ( this, IDC_EDIT_BONUSEXP, m_pDummyCrow->m_sBasic.m_wBonusExp );
	SetWin_Num_int ( this, IDC_EDIT_GENTIME, m_pDummyCrow->m_sBasic.m_dwGenTime );
	SetWin_Num_int ( this, IDC_EDIT_GATHERTIMELOW, m_pDummyCrow->m_sBasic.m_wGatherTimeLow );
	SetWin_Num_int ( this, IDC_EDIT_GATHERTIMEHIGH, m_pDummyCrow->m_sBasic.m_wGatherTimeHigh );
	SetWin_Num_float ( this, IDC_EDIT_GATHERRATE, m_pDummyCrow->m_sBasic.m_fGatherRate );

	//scrowaction
	SetWin_Num_int ( this, IDC_EDIT_BODYRADIUS, m_pDummyCrow->m_sAction.m_wBodyRadius );
	SetWin_Text ( this, IDC_EDIT_SKINFILE, m_pDummyCrow->m_sAction.m_strSkinObj.c_str() );
	SetWin_Text ( this, IDC_EDIT_CONVERSATIONFILE, m_pDummyCrow->m_sAction.m_strTalkFile.c_str() );
	SetWin_Text ( this, IDC_EDIT_SALE1, m_pDummyCrow->m_sAction.m_strSaleFile[0].c_str() );
	SetWin_Text ( this, IDC_EDIT_SALE2, m_pDummyCrow->m_sAction.m_strSaleFile[1].c_str() );
	SetWin_Text ( this, IDC_EDIT_SALE3, m_pDummyCrow->m_sAction.m_strSaleFile[2].c_str() );
	SetWin_Check ( this, IDC_CHECK_AFTERFALL, (BOOL) m_pDummyCrow->m_sAction.m_bAfterFall_NoBody );
	SetWin_Text ( this, IDC_EDIT_BIRTHEFFECT, m_pDummyCrow->m_sAction.m_strBirthEffect.c_str() );
	SetWin_Text ( this, IDC_EDIT_DEATHEFFECT, m_pDummyCrow->m_sAction.m_strFallingEffect.c_str() );
	SetWin_Text ( this, IDC_EDIT_HITEFFECT, m_pDummyCrow->m_sAction.m_strBlowEffect.c_str() );
	SetWin_Combo_Sel ( this, IDC_COMBO_ACTIONUP, (int)m_pDummyCrow->m_sAction.m_emActionUP );
	SetWin_Combo_Sel ( this, IDC_COMBO_ACTIONDN, (int)m_pDummyCrow->m_sAction.m_emActionDN );
	SetWin_Combo_Sel ( this, IDC_COMBO_MOVETYPE, (int)m_pDummyCrow->m_sAction.m_emMoveType );
	SetWin_Num_float ( this, IDC_EDIT_FLOATING, m_pDummyCrow->m_sAction.m_fDriftHeight );
	SetWin_Num_float ( this, IDC_EDIT_WALKSPEED, m_pDummyCrow->m_sAction.m_fWalkVelo );
	SetWin_Check ( this, IDC_CHECK_RUN, (BOOL) m_pDummyCrow->m_sAction.m_bRun );
	SetWin_Num_float ( this, IDC_EDIT_RUNSPEED, m_pDummyCrow->m_sAction.m_fRunVelo );
	SetWin_Num_float ( this, IDC_EDIT_ACTIONDNRATE, m_pDummyCrow->m_sAction.m_fActionDNRate );
	SetWin_Check ( this, IDC_CHECK_MOBLINK, (BOOL) m_pDummyCrow->m_sAction.m_bMobLink );
	SetWin_Num_int ( this, IDC_EDIT_MOBLINKMID, m_pDummyCrow->m_sAction.m_sMobLinkID.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_MOBLINKSID, m_pDummyCrow->m_sAction.m_sMobLinkID.wSubID );
	SetWin_Num_float ( this, IDC_EDIT_MOBLINKSCALE, m_pDummyCrow->m_sAction.m_fMobLinkScale );
	SetWin_Num_float ( this, IDC_EDIT_MOBLINKDELAY, m_pDummyCrow->m_sAction.m_fMobLinkDelay );

	//scrowgen
	SetWin_Num_int ( this, IDC_EDIT_GENMONEY, m_pDummyCrow->m_sGenerate.m_dwGenMoney );
	SetWin_Num_int ( this, IDC_EDIT_GENMONEY_RATE, m_pDummyCrow->m_sGenerate.m_wGenMoney_Rate );
	SetWin_Num_int ( this, IDC_EDIT_GENITEM_MID, m_pDummyCrow->m_sGenerate.m_sGenItemID.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_GENITEM_SID, m_pDummyCrow->m_sGenerate.m_sGenItemID.wSubID );
	SetWin_Num_int ( this, IDC_EDIT_GENITEM_RATE, m_pDummyCrow->m_sGenerate.m_wGenItem_Rate );
	SetWin_Text ( this, IDC_EDIT_ITEMGEN, m_pDummyCrow->m_sGenerate.m_strGenItem.c_str() );
	SetWin_Text ( this, IDC_EDIT_ITEMGEN_Q, m_pDummyCrow->m_sGenerate.m_strQtGenItem.c_str() );

	//flags
	SetWin_Num_int ( this, IDC_EDIT_ACTFLAG, m_pDummyCrow->m_sAction.m_dwActFlag );
	SetWin_Num_float ( this, IDC_EDIT_LIVETIME, m_pDummyCrow->m_sAction.m_fLiveTime );
	SetWin_Check ( this, IDC_CHECK_EMCROWACT_TARSHORT, m_pDummyCrow->m_sAction.m_dwActFlag&EMCROWACT_TARSHORT );
	SetWin_Check ( this, IDC_CHECK_EMCROWACT_CDCERTIFY, m_pDummyCrow->m_sAction.m_dwActFlag&EMCROWACT_CDCERTIFY );
	SetWin_Check ( this, IDC_CHECK_EMCROWACT_INVISIBLE, m_pDummyCrow->m_sAction.m_dwActFlag&EMCROWACT_INVISIBLE );
	SetWin_Check ( this, IDC_CHECK_EMCROWACT_RECVISIBLE, m_pDummyCrow->m_sAction.m_dwActFlag&EMCROWACT_RECVISIBLE );
	SetWin_Check ( this, IDC_CHECK_EMCROWACT_BARRIER, m_pDummyCrow->m_sAction.m_dwActFlag&EMCROWACT_BARRIER );
	SetWin_Check ( this, IDC_CHECK_EMCROWACT_POSHOLD, m_pDummyCrow->m_sAction.m_dwActFlag&EMCROWACT_POSHOLD );
	SetWin_Check ( this, IDC_CHECK_EMCROWACT_DIRHOLD, m_pDummyCrow->m_sAction.m_dwActFlag&EMCROWACT_DIRHOLD );
	SetWin_Check ( this, IDC_CHECK_EMCROWACT_KNOCK, m_pDummyCrow->m_sAction.m_dwActFlag&EMCROWACT_KNOCK );
	SetWin_Check ( this, IDC_CHECK_EMCROWACT_BOSS, m_pDummyCrow->m_sAction.m_dwActFlag&EMCROWACT_BOSS );
	SetWin_Check ( this, IDC_CHECK_EMCROWACT_BUSUNIT, m_pDummyCrow->m_sAction.m_dwActFlag&EMCROWACT_BUSUNIT );
	SetWin_Check ( this, IDC_CHECK_EMCROWACT_IGNORE_SHOCK, m_pDummyCrow->m_sAction.m_dwActFlag&EMCROWACT_IGNORE_SHOCK );
	SetWin_Check ( this, IDC_CHECK_EMCROWACT_AUTODROP, m_pDummyCrow->m_sAction.m_dwActFlag&EMCROWACT_AUTODROP );
	
	//SetWin_Check ( this, IDC_CHECK_EMCROWACT_CONTROLLER, m_pDummyCrow->m_sAction.m_dwActFlag&EMCROWACT_CONTROLLER );
	//SetWin_Check ( this, IDC_CHECK_EMCROWACT_SPECIAL, m_pDummyCrow->m_sAction.m_dwActFlag&EMCROWACT_SPECIAL );
	
	//school req
	//SetWin_Check ( this, IDC_CHECK_EMCROWACT_SG, m_pDummyCrow->m_sAction.dwMobReqSchool&GLSCHOOL_00  );
	//SetWin_Check ( this, IDC_CHECK_EMCROWACT_MP, m_pDummyCrow->m_sAction.dwMobReqSchool&GLSCHOOL_01  );
	//SetWin_Check ( this, IDC_CHECK_EMCROWACT_PHX, m_pDummyCrow->m_sAction.dwMobReqSchool&GLSCHOOL_02  );
	//SetWin_Num_int ( this, IDC_EDIT_SCHOOLREQ, m_pDummyCrow->m_sAction.dwMobReqSchool );

	//scrowpattern
	SetWin_Num_int ( this, IDC_EDIT_PATTERNSIZE, m_pDummyCrow->m_sAction.m_vecPatternList.size() );
	m_listPattern.ResetContent();

	std::vector<SCROWPATTERN>::iterator iter = m_pDummyCrow->m_sAction.m_vecPatternList.begin();
	std::vector<SCROWPATTERN>::iterator iter_end = m_pDummyCrow->m_sAction.m_vecPatternList.end();

	for ( ; iter!=iter_end; ++iter )
	{
		float m_fPatternDNRate = iter->m_fPatternDNRate;
		EMCROWACT_UP m_emActPattern = iter->m_emActPattern;
		DWORD m_dwPatternAttackSet = iter->m_dwPatternAttackSet;

		CString strNAME = "[Pattern]";

		strNAME.Format ( "DN Rate: %.02f / Action:%s / AttackSet:%d", m_fPatternDNRate, COMMENT::szCROWACT_UP[m_emActPattern].c_str() , m_dwPatternAttackSet ); 

		int nIndex = m_listPattern.AddString ( strNAME );
		DWORD dwData = DWORD(iter-m_pDummyCrow->m_sAction.m_vecPatternList.begin());
		m_listPattern.SetItemData ( nIndex, dwData );
	}
}

BOOL MobEditData::InverseUpdateCrow()
{
	//scrowbasic
	strKeyName = GetWin_Text ( this, IDC_CROW_ID ).GetString();
	strName	= GetWin_Text ( this, IDC_CROW_NAME ).GetString();
	m_pDummyCrow->m_sBasic.m_emTribe = (EMTRIBE) GetWin_Combo_Sel ( this, IDC_COMBO_EMTRIBE );
	m_pDummyCrow->m_sBasic.m_emCrow = (EMCROW) GetWin_Combo_Sel ( this, IDC_COMBO_EMCROW );
	m_pDummyCrow->m_sBasic.m_emBright = (EMBRIGHT) GetWin_Combo_Sel ( this, IDC_COMBO_EMBRIGHT );
	m_pDummyCrow->m_sBasic.m_emPetType = (PETTYPE) GetWin_Combo_Sel ( this, IDC_COMBO_PETTYPE );
	m_pDummyCrow->m_sBasic.m_wGatherAnimation = (EMANI_SUBTYPE) GetWin_Combo_Sel ( this, IDC_COMBO_GATHERANIMATION );
	m_pDummyCrow->m_sBasic.m_bOverlapAttack = GetWin_Check ( this, IDC_CHECK_OVERLAPATTACK );
	m_pDummyCrow->m_sBasic.m_wLevel = GetWin_Num_int ( this, IDC_EDIT_LEVEL );
	m_pDummyCrow->m_sBasic.m_dwHP = GetWin_Num_int ( this, IDC_EDIT_HP );
	m_pDummyCrow->m_sBasic.m_wMP = GetWin_Num_int ( this, IDC_EDIT_MP );
	m_pDummyCrow->m_sBasic.m_wSP = GetWin_Num_int ( this, IDC_EDIT_SP );
	m_pDummyCrow->m_sBasic.m_fIncHP = GetWin_Num_float ( this, IDC_EDIT_HPREC );
	m_pDummyCrow->m_sBasic.m_fIncMP = GetWin_Num_float ( this, IDC_EDIT_MPREC );
	m_pDummyCrow->m_sBasic.m_fIncSP = GetWin_Num_float ( this, IDC_EDIT_SPREC );
	m_pDummyCrow->m_sBasic.m_wAvoidRate = GetWin_Num_int ( this, IDC_EDIT_AVOIDRATE );
	m_pDummyCrow->m_sBasic.m_wHitRate = GetWin_Num_int ( this, IDC_EDIT_HITRATE );
	m_pDummyCrow->m_sBasic.m_wViewRange = GetWin_Num_int ( this, IDC_EDIT_VIEWDISTANCE );
	m_pDummyCrow->m_sBasic.m_wDefense = GetWin_Num_int ( this, IDC_EDIT_DEFENSE );
	m_pDummyCrow->m_sBasic.m_sResist.nFire = GetWin_Num_int ( this, IDC_EDIT_RES_FIRE );
	m_pDummyCrow->m_sBasic.m_sResist.nElectric = GetWin_Num_int ( this, IDC_EDIT_RES_ELECT );
	m_pDummyCrow->m_sBasic.m_sResist.nSpirit = GetWin_Num_int ( this, IDC_EDIT_RES_SPIRIT );
	m_pDummyCrow->m_sBasic.m_sResist.nIce = GetWin_Num_int ( this, IDC_EDIT_RES_ICE );
	m_pDummyCrow->m_sBasic.m_sResist.nPoison = GetWin_Num_int ( this, IDC_EDIT_RES_POISON );
	m_pDummyCrow->m_sBasic.m_wBonusExp = GetWin_Num_int ( this, IDC_EDIT_BONUSEXP );
	m_pDummyCrow->m_sBasic.m_dwGenTime = GetWin_Num_int ( this, IDC_EDIT_GENTIME );
	m_pDummyCrow->m_sBasic.m_wGatherTimeLow = GetWin_Num_int ( this, IDC_EDIT_GATHERTIMELOW );
	m_pDummyCrow->m_sBasic.m_wGatherTimeHigh = GetWin_Num_int ( this, IDC_EDIT_GATHERTIMEHIGH );
	m_pDummyCrow->m_sBasic.m_fGatherRate = GetWin_Num_float ( this, IDC_EDIT_GATHERRATE );

	//scrowaction
	m_pDummyCrow->m_sAction.m_wBodyRadius = GetWin_Num_int ( this, IDC_EDIT_BODYRADIUS );
	m_pDummyCrow->m_sAction.m_strSkinObj = GetWin_Text( this, IDC_EDIT_SKINFILE );
	m_pDummyCrow->m_sAction.m_strTalkFile = GetWin_Text( this, IDC_EDIT_CONVERSATIONFILE );
	m_pDummyCrow->m_sAction.m_strSaleFile[0] = GetWin_Text( this, IDC_EDIT_SALE1 );
	m_pDummyCrow->m_sAction.m_strSaleFile[1] = GetWin_Text( this, IDC_EDIT_SALE2 );
	m_pDummyCrow->m_sAction.m_strSaleFile[2] = GetWin_Text( this, IDC_EDIT_SALE3 );
	m_pDummyCrow->m_sAction.m_bAfterFall_NoBody = GetWin_Check ( this, IDC_CHECK_AFTERFALL );
	m_pDummyCrow->m_sAction.m_strBirthEffect = GetWin_Text( this, IDC_EDIT_BIRTHEFFECT );
	m_pDummyCrow->m_sAction.m_strFallingEffect = GetWin_Text( this, IDC_EDIT_DEATHEFFECT );
	m_pDummyCrow->m_sAction.m_strBlowEffect = GetWin_Text( this, IDC_EDIT_HITEFFECT );
	m_pDummyCrow->m_sAction.m_emActionUP = (EMCROWACT_UP) GetWin_Combo_Sel ( this, IDC_COMBO_ACTIONUP );
	m_pDummyCrow->m_sAction.m_emActionDN = (EMCROWACT_DN) GetWin_Combo_Sel ( this, IDC_COMBO_ACTIONDN );
	m_pDummyCrow->m_sAction.m_emMoveType = (EMMOVETYPE) GetWin_Combo_Sel ( this, IDC_COMBO_MOVETYPE );
	m_pDummyCrow->m_sAction.m_fDriftHeight = GetWin_Num_float ( this, IDC_EDIT_FLOATING );
	m_pDummyCrow->m_sAction.m_fWalkVelo = GetWin_Num_float ( this, IDC_EDIT_WALKSPEED );
	m_pDummyCrow->m_sAction.m_bRun = GetWin_Check ( this, IDC_CHECK_RUN );
	m_pDummyCrow->m_sAction.m_fRunVelo = GetWin_Num_float ( this, IDC_EDIT_RUNSPEED );
	m_pDummyCrow->m_sAction.m_fActionDNRate = GetWin_Num_float ( this, IDC_EDIT_ACTIONDNRATE );
	m_pDummyCrow->m_sAction.m_bMobLink = GetWin_Check ( this, IDC_CHECK_MOBLINK );
	m_pDummyCrow->m_sAction.m_sMobLinkID.wMainID = GetWin_Num_int ( this, IDC_EDIT_MOBLINKMID );
	m_pDummyCrow->m_sAction.m_sMobLinkID.wSubID = GetWin_Num_int ( this, IDC_EDIT_MOBLINKSID );
	m_pDummyCrow->m_sAction.m_fMobLinkScale = GetWin_Num_float ( this, IDC_EDIT_MOBLINKSCALE );
	m_pDummyCrow->m_sAction.m_fMobLinkDelay = GetWin_Num_float ( this, IDC_EDIT_MOBLINKDELAY );

	//scrowgen
	m_pDummyCrow->m_sGenerate.m_dwGenMoney = GetWin_Num_int ( this, IDC_EDIT_GENMONEY );
	m_pDummyCrow->m_sGenerate.m_wGenMoney_Rate = GetWin_Num_int ( this, IDC_EDIT_GENMONEY_RATE );
	m_pDummyCrow->m_sGenerate.m_sGenItemID.wMainID = GetWin_Num_int ( this, IDC_EDIT_GENITEM_MID );
	m_pDummyCrow->m_sGenerate.m_sGenItemID.wSubID = GetWin_Num_int ( this, IDC_EDIT_GENITEM_SID );
	m_pDummyCrow->m_sGenerate.m_wGenItem_Rate = GetWin_Num_int ( this, IDC_EDIT_GENITEM_RATE );
	m_pDummyCrow->m_sGenerate.m_strGenItem = GetWin_Text( this, IDC_EDIT_ITEMGEN );
	m_pDummyCrow->m_sGenerate.m_strQtGenItem = GetWin_Text( this, IDC_EDIT_ITEMGEN_Q );

	//flags
	m_pDummyCrow->m_sAction.m_fLiveTime = GetWin_Num_float ( this, IDC_EDIT_LIVETIME );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EMCROWACT_TARSHORT ), m_pDummyCrow->m_sAction.m_dwActFlag,EMCROWACT_TARSHORT );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EMCROWACT_CDCERTIFY ), m_pDummyCrow->m_sAction.m_dwActFlag,EMCROWACT_CDCERTIFY );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EMCROWACT_INVISIBLE ), m_pDummyCrow->m_sAction.m_dwActFlag,EMCROWACT_INVISIBLE );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EMCROWACT_RECVISIBLE ), m_pDummyCrow->m_sAction.m_dwActFlag,EMCROWACT_RECVISIBLE );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EMCROWACT_BARRIER ), m_pDummyCrow->m_sAction.m_dwActFlag,EMCROWACT_BARRIER );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EMCROWACT_POSHOLD ), m_pDummyCrow->m_sAction.m_dwActFlag,EMCROWACT_POSHOLD );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EMCROWACT_DIRHOLD ), m_pDummyCrow->m_sAction.m_dwActFlag,EMCROWACT_DIRHOLD );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EMCROWACT_KNOCK ), m_pDummyCrow->m_sAction.m_dwActFlag,EMCROWACT_KNOCK );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EMCROWACT_BOSS ), m_pDummyCrow->m_sAction.m_dwActFlag,EMCROWACT_BOSS );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EMCROWACT_BUSUNIT ), m_pDummyCrow->m_sAction.m_dwActFlag,EMCROWACT_BUSUNIT );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EMCROWACT_IGNORE_SHOCK ), m_pDummyCrow->m_sAction.m_dwActFlag,EMCROWACT_IGNORE_SHOCK );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EMCROWACT_AUTODROP ), m_pDummyCrow->m_sAction.m_dwActFlag,EMCROWACT_AUTODROP );
	
	//SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EMCROWACT_CONTROLLER ), m_pDummyCrow->m_sAction.m_dwActFlag,EMCROWACT_CONTROLLER );
	//SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EMCROWACT_SPECIAL ), m_pDummyCrow->m_sAction.m_dwActFlag,EMCROWACT_SPECIAL );
	//
	//school req
	//SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EMCROWACT_SG ), m_pDummyCrow->m_sAction.dwMobReqSchool,GLSCHOOL_00  );
	//SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EMCROWACT_MP ), m_pDummyCrow->m_sAction.dwMobReqSchool,GLSCHOOL_01  );
	//SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EMCROWACT_PHX ), m_pDummyCrow->m_sAction.dwMobReqSchool,GLSCHOOL_02  );
	return TRUE;
}

void MobEditData::OnBnClickedButtonPatternadd()
{
	DWORD PatternNum = m_pDummyCrow->m_sAction.m_vecPatternList.size();

	if ( PatternNum == SCROWACTION::PATTERNNUM ) // cannot add if more than 10
	{
		MessageBox ( "Max Pattern Num Reached!" );
		return;
	}
	
	SetWin_Enable( this, IDC_EDIT_PATTERNDNRATE, TRUE );
	SetWin_Enable( this, IDC_COMBO_ACTPATTERN, TRUE );
	SetWin_Enable( this, IDC_EDIT_PATTERNATTACKSET, TRUE );
	SetWin_Enable( this, IDC_BUTTON_PATTERNOK, TRUE );

	bAdd = true;
	float fDummyValue = 90;
	int iDummyValue = 0;
	int iDummyValue1 = 3;
	SetWin_Num_float ( this, IDC_EDIT_PATTERNDNRATE, fDummyValue );
	SetWin_Combo_Sel ( this, IDC_COMBO_ACTPATTERN, (int)iDummyValue );
	SetWin_Num_int ( this, IDC_EDIT_PATTERNATTACKSET, iDummyValue1 );

}

void MobEditData::OnBnClickedButtonPatternedit()
{
	InverseUpdateCrow();

	bAdd = false;
	int nIndex = m_listPattern.GetCurSel();
	if ( LB_ERR == nIndex )	return;
	
	DWORD dwIndex = (DWORD) m_listPattern.GetItemData ( nIndex );
	SCROWPATTERN &sPATTERN = MobEditData::m_pDummyCrow->m_sAction.m_vecPatternList[dwIndex];

	SetWin_Enable( this, IDC_EDIT_PATTERNDNRATE, TRUE );
	SetWin_Enable( this, IDC_COMBO_ACTPATTERN, TRUE );
	SetWin_Enable( this, IDC_EDIT_PATTERNATTACKSET, TRUE );
	SetWin_Enable( this, IDC_BUTTON_PATTERNOK, TRUE );

	SetWin_Num_float ( this, IDC_EDIT_PATTERNDNRATE, sPATTERN.m_fPatternDNRate );
	SetWin_Combo_Sel ( this, IDC_COMBO_ACTPATTERN, (int)sPATTERN.m_emActPattern );
	SetWin_Num_int ( this, IDC_EDIT_PATTERNATTACKSET, sPATTERN.m_dwPatternAttackSet );
}

void MobEditData::OnBnClickedButtonPatterndelete()
{
	InverseUpdateCrow();

	int nIndex = m_listPattern.GetCurSel();
	if ( LB_ERR == nIndex )	return;
	DWORD dwIndex = (DWORD) m_listPattern.GetItemData ( nIndex );
	SCROWPATTERN &sPATTERN = MobEditData::m_pDummyCrow->m_sAction.m_vecPatternList[dwIndex];

	int nReturn = MessageBox ( "Do you want to delete 'Pattern'?" , "WARNING" ,MB_YESNO );
	if ( nReturn==IDYES )
	{
		std::vector<SCROWPATTERN>::iterator iter = m_pDummyCrow->m_sAction.m_vecPatternList.begin()+dwIndex;
		MobEditData::m_pDummyCrow->m_sAction.m_vecPatternList.erase ( iter, iter+1 );
		UpdateCrow ();
	}
}

void MobEditData::OnBnClickedButtonPatternok()
{
	InverseUpdateCrow();

	WORD attackset = GetWin_Num_int ( this, IDC_EDIT_PATTERNATTACKSET );
	//if ( attackset > SCROWACTION::ATTACKSETNUM ) // i think 3 is the limit
	if ( attackset > 3 )
	{
		MessageBox ( "Max Attack Set Reached!" );
		return;
	}
	else
	{
		if (!bAdd )
		{
			int nIndex = m_listPattern.GetCurSel();
			if ( LB_ERR == nIndex )	return;
	
			DWORD dwIndex = (DWORD) m_listPattern.GetItemData ( nIndex );
			SCROWPATTERN &sPATTERN = MobEditData::m_pDummyCrow->m_sAction.m_vecPatternList[dwIndex];
			sPATTERN.m_fPatternDNRate = GetWin_Num_float ( this, IDC_EDIT_PATTERNDNRATE );
			sPATTERN.m_emActPattern = ( EMCROWACT_UP ) GetWin_Combo_Sel ( this, IDC_COMBO_ACTPATTERN );
			sPATTERN.m_dwPatternAttackSet = GetWin_Num_int ( this, IDC_EDIT_PATTERNATTACKSET );
		}
		else
		{
			SCROWPATTERN sPATTERN;
			sPATTERN.m_fPatternDNRate = GetWin_Num_float ( this, IDC_EDIT_PATTERNDNRATE );
			sPATTERN.m_emActPattern = ( EMCROWACT_UP ) GetWin_Combo_Sel ( this, IDC_COMBO_ACTPATTERN );
			sPATTERN.m_dwPatternAttackSet = GetWin_Num_int ( this, IDC_EDIT_PATTERNATTACKSET );

			MobEditData::m_pDummyCrow->m_sAction.m_vecPatternList.push_back(sPATTERN);
		}
		SetWin_Enable( this, IDC_EDIT_PATTERNDNRATE, FALSE );
		SetWin_Enable( this, IDC_COMBO_ACTPATTERN, FALSE );
		SetWin_Enable( this, IDC_EDIT_PATTERNATTACKSET, FALSE );
		SetWin_Enable( this, IDC_BUTTON_PATTERNOK, FALSE );
		UpdateCrow ();
	}
}
void MobEditData::LoadGENITEM( int nID )
{
	CString szFilter = "Item Gen File (*.genitem)|*.GENITEM|";
	
	CFileDialog dlg(TRUE,".genitem",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetServerPath ();

	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
	}
}
void MobEditData::LoadCROWSALE( int nID )
{
	CString szFilter = "Item Sale File (*.crowsale)|*.CROWSALE|";
	
	CFileDialog dlg(TRUE,".crowsale",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();

	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
	}
}
void MobEditData::LoadNTK( int nID )
{
	CString szFilter = "Npc Conversation File (*.ntk)|*.NTK|";
	
	CFileDialog dlg(TRUE,".ntk",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = CNpcDialogueSet::GetPath ();

	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
	}
}
void MobEditData::LoadCHF( int nID )
{
	CString szFilter = "Skin File (*.CHF)|*.CHF|";
	
	CFileDialog dlg(TRUE,".chf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = DxSkinCharDataContainer::GetInstance().GetPath ();

	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
	}
}
void MobEditData::LoadEGP( int nID )
{
	CString szFilter = "Effect File (*.egp)|*.egp|";
	
	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath ();

	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
	}
}
void MobEditData::LoadEFFSKIN( int nID )
{
	CString szFilter = "Effect File (*.effskin_a)|*.effskin_a|";
	
	CFileDialog dlg(TRUE,".effskin_a",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
	}
}
void MobEditData::LoadTEXTURE( int nID )
{
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	
	CFileDialog dlg(TRUE,".x",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(MobEditData*)this);

	dlg.m_ofn.lpstrInitialDir = TextureManager::GetTexturePath();
		//DxSimpleMeshMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
	}
}
void MobEditData::OnBnClickedButtonSkinfile()
{
	LoadCHF (IDC_EDIT_SKINFILE);
}

void MobEditData::OnBnClickedButtonConversationfile()
{
	LoadNTK (IDC_EDIT_CONVERSATIONFILE);
}

void MobEditData::OnBnClickedButtonSale1()
{
	LoadCROWSALE (IDC_EDIT_SALE1);
}

void MobEditData::OnBnClickedButtonSale2()
{
	LoadCROWSALE (IDC_EDIT_SALE2);
}

void MobEditData::OnBnClickedButtonSale3()
{
	LoadCROWSALE (IDC_EDIT_SALE3);
}

void MobEditData::OnBnClickedButtonBirtheffect()
{
	LoadEGP (IDC_EDIT_BIRTHEFFECT);
}

void MobEditData::OnBnClickedButtonDeatheffect()
{
	LoadEGP (IDC_EDIT_DEATHEFFECT);
}

void MobEditData::OnBnClickedButtonHiteffect()
{
	LoadEGP (IDC_EDIT_HITEFFECT);
}

void MobEditData::OnBnClickedButtonItemgen()
{
	LoadGENITEM (IDC_EDIT_ITEMGEN);
}

void MobEditData::OnBnClickedButtonItemgenQ()
{
	LoadGENITEM (IDC_EDIT_ITEMGEN_Q);
}

	