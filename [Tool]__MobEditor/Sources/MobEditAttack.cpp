// MobEditAttack.cpp : implementation file
//

#include "pch.h"
#include "MobEdit.h"
#include "MobEditAttack.h"
#include "SheetWithTab.h"
#include "GLStringTable.h"
#include "EtcFunction.h"
#include "GLOGIC.h"
#include ".\mobeditattack.h"
#include "DxSkinDataDummy.h"
#include "DxSkinAniMan.h"
#include "DxEffcharData.h"
#include "StringUtils.h"


// MobEditAttack dialog

IMPLEMENT_DYNAMIC(MobEditAttack, CPropertyPage)
MobEditAttack::MobEditAttack(LOGFONT logfont)
	: CPropertyPage(MobEditAttack::IDD)
	,m_pFont( NULL )
	,m_MobEditTree( logfont )
{
	m_bDlgInit = FALSE;
	m_pCrow = NULL;
	m_pDummyCrow = new SCROWDATA;
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
	iIndex = 0;
}

MobEditAttack::~MobEditAttack()
{
	SAFE_DELETE ( m_pDummyCrow );
	SAFE_DELETE ( m_pFont );
}

void MobEditAttack::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ANI, m_listAni);
	DDX_Control(pDX, IDC_LIST_ANIDIV, m_listAniDiv);
}


BEGIN_MESSAGE_MAP(MobEditAttack, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_BACK, OnBnClickedButtonBack)
	ON_BN_CLICKED(IDC_BUTTON_INDEXBACK, OnBnClickedButtonIndexback)
	ON_BN_CLICKED(IDC_BUTTON_INDEXNEXT, OnBnClickedButtonIndexnext)
	ON_BN_CLICKED(IDC_BUTTON_ANIUSE, OnBnClickedButtonAniuse)
	ON_CBN_SELCHANGE(IDC_COMBO_TROUBLETYPE, OnCbnSelchangeComboTroubletype)
	ON_BN_CLICKED(IDC_BUTTON_SELFBODYEFFECT, OnBnClickedButtonSelfbodyeffect)
	ON_BN_CLICKED(IDC_BUTTON_TARGETBODYEFFECT, OnBnClickedButtonTargetbodyeffect)
	ON_BN_CLICKED(IDC_BUTTON_TARGETEFFECT, OnBnClickedButtonTargeteffect)
END_MESSAGE_MAP()


// MobEditAttack message handlers
BOOL MobEditAttack::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );
	
	m_bDlgInit = TRUE;
	InitDefaultCtrls ();
	UpdateCrow (0);
	OnCbnSelchangeComboTroubletype();

	return TRUE;  // return TRUE unless you set the focus to a control
}
BOOL MobEditAttack::SetCrow ( PCROWDATA pCrow )
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
		UpdateCrow (0);	
		OnCbnSelchangeComboTroubletype();
	}

	return TRUE;
}
void MobEditAttack::OnBnClickedButtonOk()
{
	if ( !InverseUpdateCrow (iIndex) )
	{		
		return;
	}

	WORD MID = m_pDummyCrow->m_sBasic.sNativeID.wMainID;
	WORD SID = m_pDummyCrow->m_sBasic.sNativeID.wSubID;

	m_pCrow = NULL;
	iIndex = 0;
	m_MobEditTree.UpdateData ( MID,SID,m_pDummyCrow );
	m_pSheetTab->ActiveCrowTreePage ();
}

void MobEditAttack::OnBnClickedButtonCancel()
{
	m_pCrow = NULL;
	iIndex = 0;
	m_pSheetTab->ActiveCrowTreePage ();
}

void MobEditAttack::OnBnClickedButtonBack()
{
	if ( !InverseUpdateCrow (iIndex) ) 
	{		
		return;
	}

	m_pSheetTab->ActiveCrowDataPage ( m_CallPage,m_pDummyCrow );
	m_pCrow = NULL;
	iIndex = 0;
}
void MobEditAttack::OnBnClickedButtonIndexback()
{
	if ( iIndex <= 0 ) return ;
	InverseUpdateCrow (iIndex);
	iIndex--;
	UpdateCrow (iIndex);
	OnCbnSelchangeComboTroubletype();
}

void MobEditAttack::OnBnClickedButtonIndexnext()
{
	InverseUpdateCrow (iIndex);
	iIndex++;
	if ( iIndex >= SCROWDATA::EMMAXATTACK ) iIndex--;
	UpdateCrow ( iIndex);
	OnCbnSelchangeComboTroubletype();
}
void MobEditAttack::InitDefaultCtrls ()
{
	SetWin_Combo_Init ( this, IDC_COMBO_ATTACKRGTYPE, COMMENT::ATTACK_RGTYPE, EMATT_SIZE );
	//SetWin_Combo_Init ( this, IDC_COMBO_SKTAR, COMMENT::CROWSKTAR_TYPE, CROWSKTAR_SIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_TROUBLETYPE, COMMENT::BLOW, EMBLOW_SIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_ANIMID, COMMENT::ANI_MAINTYPE, AN_TYPE_SIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_ANISID, COMMENT::ANI_SUBTYPE, AN_SUB_00_SIZE );
	OnCbnSelchangeComboTroubletype();
}
void MobEditAttack::UpdateCrow (int iIndex)
{
	SetWin_Num_int ( this, IDC_EDIT_INDEX, iIndex );
	SetWin_Check ( this, IDC_CHECK_USED, (BOOL) m_pDummyCrow->m_sCrowAttack[iIndex].bUsed );
	SetWin_Combo_Sel ( this, IDC_COMBO_ATTACKRGTYPE, (int)m_pDummyCrow->m_sCrowAttack[iIndex].emAttRgType );
	SetWin_Num_int ( this, IDC_EDIT_RANGE, m_pDummyCrow->m_sCrowAttack[iIndex].wRange );
	SetWin_Num_int ( this, IDC_EDIT_DAMAGE_LOW, m_pDummyCrow->m_sCrowAttack[iIndex].sDamage.wLow );
	SetWin_Num_int ( this, IDC_EDIT_DAMAGE_HIGH, m_pDummyCrow->m_sCrowAttack[iIndex].sDamage.wHigh );
	//SetWin_Num_int ( this, IDC_EDIT_DAMAGE_LOW2, m_pDummyCrow->m_sCrowAttack[iIndex].wDamageLow );
	//SetWin_Num_int ( this, IDC_EDIT_DAMAGE_HIGH2, m_pDummyCrow->m_sCrowAttack[iIndex].wDamageHigh );
	SetWin_Num_float ( this, IDC_EDIT_DELAY, m_pDummyCrow->m_sCrowAttack[iIndex].fDelay );
	SetWin_Num_int ( this, IDC_EDIT_SPUSAGE, m_pDummyCrow->m_sCrowAttack[iIndex].wUse_SP );
	SetWin_Num_int ( this, IDC_EDIT_SKILL_MID, m_pDummyCrow->m_sCrowAttack[iIndex].skill_id.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_SKILL_SID, m_pDummyCrow->m_sCrowAttack[iIndex].skill_id.wSubID );
	SetWin_Num_int ( this, IDC_EDIT_SKILL_LEVEL, m_pDummyCrow->m_sCrowAttack[iIndex].skill_lev );
	SetWin_Combo_Sel ( this, IDC_COMBO_SKTAR, (int)m_pDummyCrow->m_sCrowAttack[iIndex].skill_tar );
	SetWin_Combo_Sel ( this, IDC_COMBO_TROUBLETYPE, (int)m_pDummyCrow->m_sCrowAttack[iIndex].emBLOW_TYPE );
	SetWin_Num_float ( this, IDC_EDIT_BLOW_RATE, m_pDummyCrow->m_sCrowAttack[iIndex].fBLOW_RATE );
	SetWin_Num_float ( this, IDC_EDIT_BLOW_LIFE, m_pDummyCrow->m_sCrowAttack[iIndex].fBLOW_LIFE );
	SetWin_Num_float ( this, IDC_EDIT_BLOW_VAR1, m_pDummyCrow->m_sCrowAttack[iIndex].fBLOW_VAR1 );
	SetWin_Num_float ( this, IDC_EDIT_BLOW_VAR2, m_pDummyCrow->m_sCrowAttack[iIndex].fBLOW_VAR2 );
	SetWin_Text ( this, IDC_EDIT_SELFBODYEFFECT, m_pDummyCrow->m_sCrowAttack[iIndex].strSelfBodyEffect.c_str() );
	SetWin_Text ( this, IDC_EDIT_TARGETBODYEFFECT, m_pDummyCrow->m_sCrowAttack[iIndex].strTargBodyEffect.c_str() );
	SetWin_Text ( this, IDC_EDIT_TARGETEFFECT, m_pDummyCrow->m_sCrowAttack[iIndex].strTargetEffect.c_str() );

	//animation info (not perfect)

	SetWin_Text ( this, IDC_EDIT_ANINAME, m_pDummyCrow->m_sCrowAttack[iIndex].strAniFile.c_str());

	m_listAni.ResetContent();
	DxSkinDataDummy DataDummy;
	BOOL bOK = DataDummy.LoadFile ( m_pDummyCrow->m_sAction.m_strSkinObj.c_str() );

	if (bOK)
	{
		for( int i = 0; i < (int)DataDummy.m_vecANIMINFO.size(); i++ )
		{
			SANIMCONINFO* pANIMINFO = DataDummy.m_vecANIMINFO[i];
			m_listAni.InsertString(i, pANIMINFO->m_szName);
		}
	}

	SANIMCONINFO AnimInfo;
	CString strFileName = m_pDummyCrow->m_sCrowAttack[iIndex].strAniFile.c_str() ;
	CString strCfgFile;

	STRUTIL::ChangeExt ( strFileName, ".x", strCfgFile, ".cfg" );

	BOOL bisOK = AnimInfo.LoadFile ( strCfgFile );

	if (bisOK)
	{
		SetWin_Num_int ( this, IDC_EDIT_ANISTIME, AnimInfo.m_dwSTime );
		SetWin_Num_int ( this, IDC_EDIT_ANIETIME, AnimInfo.m_dwETime );
		SetWin_Num_int ( this, IDC_EDIT_ANIUNITTIME, AnimInfo.m_UNITTIME );
		SetWin_Num_int ( this, IDC_EDIT_FLAGS, AnimInfo.m_dwFlag );
		SetWin_Combo_Sel ( this, IDC_COMBO_ANIMID, (int)AnimInfo.m_MainType );
		SetWin_Combo_Sel ( this, IDC_COMBO_ANISID, (int)AnimInfo.m_SubType );
		SetWin_Num_int ( this, IDC_EDIT_ANIDIVCOUNT, AnimInfo.m_wStrikeCount );

		m_listAniDiv.ResetContent();
		int DivCount = (int)AnimInfo.m_wStrikeCount;
	
		for ( int j=0; j < DivCount ; ++j )
		{
			DWORD DivFrame = AnimInfo.m_sStrikeEff[j].m_dwFrame;
			CString strNAME = "[:P]";
			strNAME.Format ( "[%d]", DivFrame ); 

			int nIndex = m_listAniDiv.AddString ( strNAME );
			m_listAniDiv.SetItemData ( nIndex, j );
		}
	}
}
BOOL MobEditAttack::InverseUpdateCrow (int iIndex)
{
	m_pDummyCrow->m_sCrowAttack[iIndex].bUsed = GetWin_Check ( this, IDC_CHECK_USED );
	m_pDummyCrow->m_sCrowAttack[iIndex].emAttRgType = (EMATT_RGTYPE) GetWin_Combo_Sel ( this, IDC_COMBO_ATTACKRGTYPE );
	m_pDummyCrow->m_sCrowAttack[iIndex].wRange = GetWin_Num_int ( this, IDC_EDIT_RANGE );
	m_pDummyCrow->m_sCrowAttack[iIndex].sDamage.wLow = GetWin_Num_int ( this, IDC_EDIT_DAMAGE_LOW );
	m_pDummyCrow->m_sCrowAttack[iIndex].sDamage.wHigh = GetWin_Num_int ( this, IDC_EDIT_DAMAGE_HIGH);
	//m_pDummyCrow->m_sCrowAttack[iIndex].wDamageLow = GetWin_Num_int ( this, IDC_EDIT_DAMAGE_LOW2);
	//m_pDummyCrow->m_sCrowAttack[iIndex].wDamageHigh = GetWin_Num_int ( this, IDC_EDIT_DAMAGE_HIGH2);
	m_pDummyCrow->m_sCrowAttack[iIndex].fDelay = GetWin_Num_float ( this, IDC_EDIT_DELAY );
	m_pDummyCrow->m_sCrowAttack[iIndex].wUse_SP = GetWin_Num_int ( this, IDC_EDIT_SPUSAGE);
	m_pDummyCrow->m_sCrowAttack[iIndex].skill_id.wMainID = GetWin_Num_int ( this, IDC_EDIT_SKILL_MID);
	m_pDummyCrow->m_sCrowAttack[iIndex].skill_id.wSubID = GetWin_Num_int ( this, IDC_EDIT_SKILL_SID);
	m_pDummyCrow->m_sCrowAttack[iIndex].skill_lev = GetWin_Num_int ( this, IDC_EDIT_SKILL_LEVEL);
	m_pDummyCrow->m_sCrowAttack[iIndex].skill_tar = (EMCROWSKTAR) GetWin_Combo_Sel ( this, IDC_COMBO_SKTAR );
	m_pDummyCrow->m_sCrowAttack[iIndex].emBLOW_TYPE = (EMSTATE_BLOW) GetWin_Combo_Sel ( this, IDC_COMBO_TROUBLETYPE );
	m_pDummyCrow->m_sCrowAttack[iIndex].fBLOW_RATE = GetWin_Num_float ( this, IDC_EDIT_BLOW_RATE );
	m_pDummyCrow->m_sCrowAttack[iIndex].fBLOW_LIFE = GetWin_Num_float ( this, IDC_EDIT_BLOW_LIFE );
	m_pDummyCrow->m_sCrowAttack[iIndex].fBLOW_VAR1 = GetWin_Num_float ( this, IDC_EDIT_BLOW_VAR1);
	m_pDummyCrow->m_sCrowAttack[iIndex].fBLOW_VAR2 = GetWin_Num_float ( this, IDC_EDIT_BLOW_VAR2);
	m_pDummyCrow->m_sCrowAttack[iIndex].strSelfBodyEffect = GetWin_Text( this, IDC_EDIT_SELFBODYEFFECT );
	m_pDummyCrow->m_sCrowAttack[iIndex].strTargBodyEffect = GetWin_Text( this, IDC_EDIT_TARGETBODYEFFECT );
	m_pDummyCrow->m_sCrowAttack[iIndex].strTargetEffect = GetWin_Text( this, IDC_EDIT_TARGETEFFECT );

	//save animation info
	m_pDummyCrow->m_sCrowAttack[iIndex].strAniFile = GetWin_Text( this, IDC_EDIT_ANINAME );

	//read info from animation file
	SANIMCONINFO AnimInfo;
	CString strFileName = GetWin_Text( this, IDC_EDIT_ANINAME );
	CString strCfgFile;

	STRUTIL::ChangeExt ( strFileName, ".x", strCfgFile, ".cfg" );

	BOOL bisOK = AnimInfo.LoadFile ( strCfgFile );

	//only save information if reading animation info is successful
	if (bisOK)
	{
		m_pDummyCrow->m_sCrowAttack[iIndex].sAniAttack.m_dwSTime = AnimInfo.m_dwSTime;
		m_pDummyCrow->m_sCrowAttack[iIndex].sAniAttack.m_dwETime = AnimInfo.m_dwETime;
		m_pDummyCrow->m_sCrowAttack[iIndex].sAniAttack.m_UNITTIME = AnimInfo.m_UNITTIME;
		m_pDummyCrow->m_sCrowAttack[iIndex].sAniAttack.m_dwFlags = AnimInfo.m_dwFlag;
		m_pDummyCrow->m_sCrowAttack[iIndex].sAniAttack.m_MainType = AnimInfo.m_MainType;
		m_pDummyCrow->m_sCrowAttack[iIndex].sAniAttack.m_SubType = AnimInfo.m_SubType;
		m_pDummyCrow->m_sCrowAttack[iIndex].sAniAttack.m_wDivCount = AnimInfo.m_wStrikeCount;

		// i dont know if this is correct way of saving data in loop haha
		int DivCount = (int)AnimInfo.m_wStrikeCount;
		for ( int i=0; i < DivCount ; ++i )
		{
			DWORD dwValue = AnimInfo.m_sStrikeEff[i].m_dwFrame;
			m_pDummyCrow->m_sCrowAttack[iIndex].sAniAttack.m_wDivFrame[i] = (WORD)dwValue;
		}	
	}
	else
	{
		MessageBox ( "Cant Load ANI CFG! CrowData Animation info Not Modified" );
	}
	return TRUE;
}

void MobEditAttack::OnBnClickedButtonAniuse()
{
	InverseUpdateCrow (iIndex);
	CString strTemp("");
	int nIndex = m_listAni.GetCurSel();
	if ( nIndex != LB_ERR )
	{
		m_listAni.GetText (nIndex,strTemp) ;
	}
	m_pDummyCrow->m_sCrowAttack[iIndex].strAniFile = strTemp.GetString();
	UpdateCrow ( iIndex);
}

void MobEditAttack::OnCbnSelchangeComboTroubletype()
{
	CString strTemp("");
	DWORD ThisType = (EMSTATE_BLOW) GetWin_Combo_Sel ( this, IDC_COMBO_TROUBLETYPE );
	SetWin_Text( this ,IDC_TEXT_BLOWVAR1 , COMMENT::BLOW_VAR1[ThisType].c_str() );
	SetWin_Text( this ,IDC_TEXT_BLOWVAR2 , COMMENT::BLOW_VAR2[ThisType].c_str() );
}

void MobEditAttack::LoadEGP( int nID )
{
	CString szFilter = "Effect File (*.egp)|*.egp|";
	
	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath ();

	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
	}
}
void MobEditAttack::LoadEFFSKIN( int nID )
{
	CString szFilter = "Effect File (*.effskin_a)|*.effskin_a|";
	
	CFileDialog dlg(TRUE,".effskin_a",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
	}
}
void MobEditAttack::OnBnClickedButtonSelfbodyeffect()
{
	LoadEFFSKIN (IDC_EDIT_SELFBODYEFFECT);
}

void MobEditAttack::OnBnClickedButtonTargetbodyeffect()
{
	LoadEFFSKIN (IDC_EDIT_TARGETBODYEFFECT);
}

void MobEditAttack::OnBnClickedButtonTargeteffect()
{
	LoadEGP (IDC_EDIT_TARGETEFFECT);
}
