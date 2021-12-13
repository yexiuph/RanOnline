// ItemProperty3.cpp : ���� �����Դϴ�.
//

#include "pch.h"
#include "ItemEdit.h"
#include "ItemSuit.h"
#include "GLOGIC.h"
#include "EtcFunction.h"
#include "GLItem.h"
#include "GLCrowData.h"
#include "GLCharDefine.h"

#include "DlgItem.h"
#include "SheetWithTab.h"
#include ".\itemsuit.h"

// CItemSuit ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CItemSuit, CPropertyPage)
CItemSuit::CItemSuit( LOGFONT logfont )
	: CPropertyPage(CItemSuit::IDD),
	m_pFont ( NULL )
{
	m_bDlgInit = FALSE;
	m_pItem = NULL;
	m_pDummyItem = new SITEM;

	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CItemSuit::~CItemSuit()
{
	SAFE_DELETE ( m_pDummyItem );
	SAFE_DELETE ( m_pFont );

}


void CItemSuit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX, m_listBOX);
	DDX_Control(pDX, IDC_LIST_BOX2, m_listBox_Rand);
	DDX_Control(pDX, IDC_EDIT_QUE_TIME, m_Edit_QueTime);
	DDX_Control(pDX, IDC_EDIT_QUE_EXP, m_Edit_QueExp);
	DDX_Control(pDX, IDC_EDIT_QUE_PARAM1, m_Edit_QueParam1);
	DDX_Control(pDX, IDC_EDIT_QUE_PARAM2, m_Edit_QueParam2);
	DDX_Control(pDX, IDC_LIST_MONSTER, m_list_Mob );
}


BEGIN_MESSAGE_MAP(CItemSuit, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_PREV, OnBnClickedButtonPrev)
	ON_CBN_SELCHANGE(IDC_COMBO_ADDON_NO, OnCbnSelchangeComboAddonNo)
	ON_CBN_DROPDOWN(IDC_COMBO_ADDON_NO, OnCbnDropdownComboAddonNo)
	ON_CBN_SELCHANGE(IDC_COMBO_BLOW, OnCbnSelchangeComboBlow)
	ON_BN_CLICKED(IDC_BUTTON_BOX_ADD, OnBnClickedButtonBoxAdd)
	ON_BN_CLICKED(IDC_BUTTON_BOX_DEL, OnBnClickedButtonBoxDel)
	ON_BN_CLICKED(IDC_BUTTON_BOX_ADD2, OnBnClickedButtonBoxAdd2)
	ON_BN_CLICKED(IDC_BUTTON_BOX_DEL2, OnBnClickedButtonBoxDel2)
	ON_CBN_SELCHANGE(IDC_COMBO_QUESTION, OnCbnSelchangeComboQuestion)
	ON_BN_CLICKED(IDC_BUTTON_BOX_UP1, OnBnClickedButtonBoxUp1)
	ON_BN_CLICKED(IDC_BUTTON_BOX_DW1, OnBnClickedButtonBoxDw1)
	ON_BN_CLICKED(IDC_BUTTON_BOX_UP2, OnBnClickedButtonBoxUp2)
	ON_BN_CLICKED(IDC_BUTTON_BOX_DW2, OnBnClickedButtonBoxDw2)
	ON_BN_CLICKED(IDC_BUTTON_ADD_PETSKIN, OnBnClickedButtonAddPetskin)
	ON_BN_CLICKED(IDC_BUTTON_DEL_PETSKIN, OnBnClickedButtonDelPetskin)
END_MESSAGE_MAP()


// CItemSuit �޽��� ó�����Դϴ�.

BOOL CItemSuit::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	RECT rectCtrl;
	LONG lnWidth;
	LV_COLUMN lvColumn;

	m_list_Mob.SetExtendedStyle ( m_list_Mob.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_list_Mob.GetClientRect ( &rectCtrl );
	lnWidth = rectCtrl.right - rectCtrl.left;

	const int nColumnCount = 4;	
	char* szColumnName1[nColumnCount] = { "Num", "Monser ID", "Scale", "Rate" };
	int nColumnWidthPercent1[nColumnCount] = { 15, 50, 17, 18 };

	for ( int i = 0; i < nColumnCount; i++ )
	{		
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.pszText = szColumnName1[i];
		lvColumn.iSubItem = i;
		lvColumn.cx = ( lnWidth*nColumnWidthPercent1[i] ) / 100;
		m_list_Mob.InsertColumn (i, &lvColumn );
	}

	m_bDlgInit = TRUE;

    InitDefaultCtrls ();
	UpdateItems ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CItemSuit::OnBnClickedButtonCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pItem = NULL;
	m_pSheetTab->ActiveItemSuitTreePage ();
}

void CItemSuit::OnBnClickedButtonOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !InverseUpdateItems() ) return;

	CItemNode sItemNode;
	sItemNode.m_sItem = *m_pDummyItem;

	GLItemMan::GetInstance().InsertItem ( m_pDummyItem->sBasicOp.sNativeID, &sItemNode );
	
	m_pItem = NULL;
	m_pSheetTab->ActiveItemSuitTreePage ();	
}

void CItemSuit::InitAllItem ()
{
	m_pItem = NULL;
}

BOOL CItemSuit::SetItem ( SITEM* pItem )
{
	if ( m_pItem )
	{
		return FALSE;
	}

	m_pItem = pItem;

	//	���̷� ��������
	*m_pDummyItem = *m_pItem;

	if ( m_bDlgInit ) 
	{
		InitDefaultCtrls ();
		UpdateItems ();		
	}
	
	return TRUE;
}


void CItemSuit::UpdateItems ()
{
	//	���� Ÿ��
	SetWin_Combo_Sel ( this, IDC_COMBO_SUIT, COMMENT::ITEMSUIT[m_pDummyItem->sSuitOp.emSuit] );

	//	�� ���� ����
	SetWin_Check ( this, IDC_CHECK_TWOH, m_pDummyItem->sSuitOp.IsBOTHHAND() );
	SetWin_Check ( this, IDC_CHECK_BIGH, m_pDummyItem->sSuitOp.IsBIG() );
	SetWin_Check ( this, IDC_CHECK_BROOM, m_pDummyItem->sSuitOp.IsBROOM() );

	SetWin_Check ( this, IDC_RADIO_RIGHT, m_pDummyItem->sSuitOp.emHand==HAND_RIGHT );
	SetWin_Check ( this, IDC_RADIO_LEFT, m_pDummyItem->sSuitOp.emHand==HAND_LEFT );
	SetWin_Check ( this, IDC_RADIO_BOTH, m_pDummyItem->sSuitOp.emHand==HAND_BOTH );

	//	���� Ÿ��
	SetWin_Combo_Sel ( this, IDC_COMBO_ATT_TYPE, COMMENT::ITEMATTACK[m_pDummyItem->sSuitOp.emAttack] );

	//	���� �Ÿ�
	SetWin_Num_int ( this, IDC_EDIT_ATT_RANGE, m_pDummyItem->sSuitOp.wAttRange );
	SetWin_Num_int ( this, IDC_EDIT_REQSP, m_pDummyItem->sSuitOp.wReqSP );

	//	�⺻ �ɼ�
	SetWin_Num_int ( this, IDC_EDIT_HIT, m_pDummyItem->sSuitOp.nHitRate );
	SetWin_Num_int ( this, IDC_EDIT_AVOIDRATE, m_pDummyItem->sSuitOp.nAvoidRate );

	//	���� ���
	// geger009 | GLPADATA still use WORD data type, turn it off for now
	//SetWin_Num_int ( this, IDC_EDIT_DAMAGE_HIGH, m_pDummyItem->sSuitOp.gdDamage.dwHigh );
	//SetWin_Num_int ( this, IDC_EDIT_DAMAGE_LOW, m_pDummyItem->sSuitOp.gdDamage.dwLow );
	SetWin_Num_int ( this, IDC_EDIT_DAMAGE_HIGH, m_pDummyItem->sSuitOp.gdDamage.wHigh );
	SetWin_Num_int ( this, IDC_EDIT_DAMAGE_LOW, m_pDummyItem->sSuitOp.gdDamage.wLow );
	SetWin_Num_int ( this, IDC_EDIT_DEFENSE, m_pDummyItem->sSuitOp.nDefense );

	// ���׷��̵� ��ġ
	SetWin_Num_int ( this, IDC_EDIT_GRADE_1, m_pDummyItem->sBasicOp.wGradeAttack );
	SetWin_Num_int ( this, IDC_EDIT_GRADE_2, m_pDummyItem->sBasicOp.wGradeDefense );

	// ����ġ ���
	SetWin_Num_float ( this, IDC_EDIT_X_EXP, m_pDummyItem->sBasicOp.fExpMultiple );

	//	�Ҹ�ǰ ����
	SetWin_Combo_Sel ( this, IDC_COMBO_DRUG_TYPE, COMMENT::ITEMDRUG[m_pDummyItem->sDrugOp.emDrug] );
	SetWin_Num_int ( this, IDC_EDIT_INSTANCE_NUM, m_pDummyItem->sDrugOp.wPileNum );
	SetWin_Num_int ( this, IDC_EDIT_INSTANCE_VOLUME, m_pDummyItem->sDrugOp.wCureVolume );
	SetWin_Check ( this, IDC_CHECK_CURE_RATIO, m_pDummyItem->sDrugOp.bRatio );

	SetWin_Check ( this, IDC_CHECK_PALSY, m_pDummyItem->sDrugOp.dwCureDISORDER&DIS_NUMB );
	SetWin_Check ( this, IDC_CHECK_SWOON, m_pDummyItem->sDrugOp.dwCureDISORDER&DIS_STUN );
	SetWin_Check ( this, IDC_CHECK_PETRIFY, m_pDummyItem->sDrugOp.dwCureDISORDER&DIS_STONE );
	SetWin_Check ( this, IDC_CHECK_BURN, m_pDummyItem->sDrugOp.dwCureDISORDER&DIS_BURN );
	SetWin_Check ( this, IDC_CHECK_FROZEN, m_pDummyItem->sDrugOp.dwCureDISORDER&DIS_FROZEN );

	SetWin_Check ( this, IDC_CHECK_DERANGE, m_pDummyItem->sDrugOp.dwCureDISORDER&DIS_MAD );
	SetWin_Check ( this, IDC_CHECK_POISON, m_pDummyItem->sDrugOp.dwCureDISORDER&DIS_POISON );
	SetWin_Check ( this, IDC_CHECK_CURSE, m_pDummyItem->sDrugOp.dwCureDISORDER&DIS_CURSE );

	//	����ġ
	SetWin_Num_int ( this, IDC_EDIT_SPIRIT, m_pDummyItem->sSuitOp.sResist.nSpirit );
	SetWin_Num_int ( this, IDC_EDIT_FIRE, m_pDummyItem->sSuitOp.sResist.nFire );
	SetWin_Num_int ( this, IDC_EDIT_ELECTRIC, m_pDummyItem->sSuitOp.sResist.nElectric );
	SetWin_Num_int ( this, IDC_EDIT_ICE, m_pDummyItem->sSuitOp.sResist.nIce );
	SetWin_Num_int ( this, IDC_EDIT_POISON, m_pDummyItem->sSuitOp.sResist.nPoison );

	//	�߰� �ɼ�
	SetWin_Combo_Sel ( this, IDC_COMBO_ADDON_NO, COMMENT::ADDON_NO[0] );
	SetWin_Num_int ( this, IDC_EDIT_ADDON_VAR, m_pDummyItem->sSuitOp.sADDON[0].nVALUE );
	SetWin_Combo_Sel ( this, IDC_COMBO_ADDON_TYPE, COMMENT::ITEMADDON[m_pDummyItem->sSuitOp.sADDON[0].emTYPE] );

	//	��ȭ��
	SetWin_Combo_Sel ( this, IDC_COMBO_VARIATE, COMMENT::ITEMVAR[m_pDummyItem->sSuitOp.sVARIATE.emTYPE] );
	SetWin_Num_float ( this, IDC_EDIT_VARIATE_VAR, m_pDummyItem->sSuitOp.sVARIATE.fVariate );

	// ��ȭ�� by ���
	SetWin_Combo_Sel ( this, IDC_COMBO_VARIATE2, COMMENT::ITEMVOL[m_pDummyItem->sSuitOp.sVOLUME.emTYPE] );
	SetWin_Num_float ( this, IDC_EDIT_VARIATE_VAR2, m_pDummyItem->sSuitOp.sVOLUME.fVolume );

	//	�����̻�
	BOOL bSTATE_BLOW = ( m_pDummyItem->sSuitOp.sBLOW.emTYPE!=EMBLOW_NONE );
	BOOL bBLOW_VAR1(TRUE), bBLOW_VAR2(TRUE);
	const char *szBLOW_VAR1 = COMMENT::BLOW_VAR1[m_pDummyItem->sSuitOp.sBLOW.emTYPE].c_str();
	const char *szBLOW_VAR2 = COMMENT::BLOW_VAR2[m_pDummyItem->sSuitOp.sBLOW.emTYPE].c_str();

	if ( !strcmp(szBLOW_VAR1,COMMENT::BLOW_VAR1[EMBLOW_NONE].c_str()) )		bBLOW_VAR1 = FALSE;
	if ( !strcmp(szBLOW_VAR2,COMMENT::BLOW_VAR2[EMBLOW_NONE].c_str()) )		bBLOW_VAR2 = FALSE;

	SetWin_Enable ( this, IDC_EDIT_BLOW_LIFE, bSTATE_BLOW );
	SetWin_Enable ( this, IDC_EDIT_BLOW_RATE, bSTATE_BLOW );

	SetWin_Text ( this, IDC_STATIC_BLOWVAR1, szBLOW_VAR1 );
	SetWin_Text ( this, IDC_STATIC_BLOWVAR2, szBLOW_VAR2 );
	SetWin_Enable ( this, IDC_EDIT_BLOW_VALUE1, bBLOW_VAR1 );
	SetWin_Enable ( this, IDC_EDIT_BLOW_VALUE2, bBLOW_VAR2 );

	SetWin_Combo_Sel ( this, IDC_COMBO_BLOW, COMMENT::BLOW[m_pDummyItem->sSuitOp.sBLOW.emTYPE] );
	
	SetWin_Num_float ( this, IDC_EDIT_BLOW_RATE, m_pDummyItem->sSuitOp.sBLOW.fRATE );
	SetWin_Num_float ( this, IDC_EDIT_BLOW_LIFE, m_pDummyItem->sSuitOp.sBLOW.fLIFE );
	SetWin_Num_float ( this, IDC_EDIT_BLOW_VALUE1, m_pDummyItem->sSuitOp.sBLOW.fVAR1 );
	SetWin_Num_float ( this, IDC_EDIT_BLOW_VALUE2, m_pDummyItem->sSuitOp.sBLOW.fVAR2 );

	// Question Item by ���
	BOOL bTIME(false);
	BOOL bEXP( m_pDummyItem->sQuestionItem.emType==QUESTION_EXP_GET );
	BOOL bPARAM1(false);
	BOOL bPARAM2(false);

	switch( m_pDummyItem->sQuestionItem.emType )
	{
	case QUESTION_SPEED_UP:
	case QUESTION_ATTACK_UP:
	case QUESTION_EXP_UP:
	case QUESTION_LUCKY:
	case QUESTION_SPEED_UP_M:
	case QUESTION_ATTACK_UP_M:
		bPARAM1 = bTIME = true;
		break;

	case QUESTION_CRAZY:
	case QUESTION_MADNESS:
		bTIME = bPARAM1 = bPARAM2 = true;
		break;

	case QUESTION_BOMB:
	case QUESTION_HEAL:
		bPARAM1 = true;
		break;

	case QUESTION_MOBGEN:
		bPARAM1 = bPARAM2 = true;
		break;
	};

	SetWin_Combo_Sel ( this, IDC_COMBO_QUESTION, COMMENT::ITEM_QUE_TYPE[m_pDummyItem->sQuestionItem.emType]);

	SetWin_Enable( this, IDC_EDIT_QUE_TIME, bTIME );
	SetWin_Enable( this, IDC_EDIT_QUE_EXP, bEXP );
	SetWin_Enable( this, IDC_EDIT_QUE_PARAM1, bPARAM1 );
	SetWin_Enable( this, IDC_EDIT_QUE_PARAM2, bPARAM2 );

	const char *szQUE_VAR1 = COMMENT::ITEM_QUE_VAR1[m_pDummyItem->sQuestionItem.emType].c_str();
	const char *szQUE_VAR2 = COMMENT::ITEM_QUE_VAR2[m_pDummyItem->sQuestionItem.emType].c_str();

	SetWin_Text ( this, IDC_STATIC_QUEVAR1, szQUE_VAR1 );
	SetWin_Text ( this, IDC_STATIC_QUEVAR2, szQUE_VAR2 );

	SetWin_Num_float( this, IDC_EDIT_QUE_TIME, m_pDummyItem->sQuestionItem.fTime );
	SetWin_Num_float( this, IDC_EDIT_QUE_EXP, m_pDummyItem->sQuestionItem.fExp );
	SetWin_Num_int( this, IDC_EDIT_QUE_PARAM1, static_cast<int>( m_pDummyItem->sQuestionItem.wParam1 ) );
	SetWin_Num_int( this, IDC_EDIT_QUE_PARAM2, static_cast<int>( m_pDummyItem->sQuestionItem.wParam2 ) );
	//////////////////////////////////////////////////////////////////////////////////////////////
	
	SetWin_Num_int ( this, IDC_EDIT_SKILL_MID, m_pDummyItem->sSkillBookOp.sSkill_ID.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_SKILL_SID, m_pDummyItem->sSkillBookOp.sSkill_ID.wSubID );

	SetWin_Check ( this, IDC_RADIO_GRIND_ARM, m_pDummyItem->sGrindingOp.emCLASS==EMGRINDING_CLASS_ARM );
	SetWin_Check ( this, IDC_RADIO_GRIND_CLOTH, m_pDummyItem->sGrindingOp.emCLASS==EMGRINDING_CLASS_CLOTH );

	SetWin_Combo_Sel ( this, IDC_COMBO_GRINDING_TYPE, COMMENT::GRINDING_TYPE[m_pDummyItem->sGrindingOp.emTYPE] );
	SetWin_Combo_Sel ( this, IDC_COMBO_GRINDING_LEVEL, COMMENT::GRINDING_LEVEL[m_pDummyItem->sGrindingOp.emGRINDER_TYPE] );


	//////////////////////////////////////////////////////////////////////////
	// ������ ������ PET Skin Pack �ϰ�쿣 ��������/�����ڽ� ����Ʈ�� ������ ���� �ʰ� Pet List�� �������Ѵ�.
	if( m_pDummyItem->sBasicOp.emItemType == ITEM_PET_SKIN_PACK )
	{
		m_listBOX.ShowWindow( SW_HIDE );
		m_listBox_Rand.ShowWindow( SW_HIDE );
		SetWin_ShowWindow( this, IDC_STATIC_PRESENT, SW_HIDE ); 
		SetWin_ShowWindow( this, IDC_STATIC_RANDOM, SW_HIDE );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_UP1, SW_HIDE );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_DW1, SW_HIDE );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_UP2, SW_HIDE );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_DW2, SW_HIDE );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_ADD, SW_HIDE );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_DEL, SW_HIDE );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_ADD2, SW_HIDE );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_DEL2, SW_HIDE );

		m_list_Mob.ShowWindow( SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_ADD_PETSKIN, SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_DEL_PETSKIN, SW_SHOW );
		SetWin_ShowWindow( this, IDC_STATIC_PETSKINPACK, SW_SHOW );
		SetWin_ShowWindow( this, IDC_STATIC_MOB_SCALE, SW_SHOW );
		SetWin_ShowWindow( this, IDC_EDIT_MOB_SCALE, SW_SHOW );
		SetWin_ShowWindow( this, IDC_STATIC_MOB_SKINTIME, SW_SHOW );
		SetWin_ShowWindow( this, IDC_EDIT_MOB_SKINTIME, SW_SHOW );

		SetWin_Num_int( this, IDC_EDIT_MOB_SKINTIME, m_pDummyItem->sPetSkinPack.dwPetSkinTime );

		InitPetSkinPack();
	}
	else
	{
		m_list_Mob.ShowWindow( SW_HIDE );
		SetWin_ShowWindow( this, IDC_BUTTON_ADD_PETSKIN, SW_HIDE );
		SetWin_ShowWindow( this, IDC_BUTTON_DEL_PETSKIN, SW_HIDE );
		SetWin_ShowWindow( this, IDC_STATIC_PETSKINPACK, SW_HIDE );
		SetWin_ShowWindow( this, IDC_STATIC_MOB_SCALE, SW_HIDE );
		SetWin_ShowWindow( this, IDC_EDIT_MOB_SCALE, SW_HIDE );
		SetWin_ShowWindow( this, IDC_STATIC_MOB_SKINTIME, SW_HIDE );
		SetWin_ShowWindow( this, IDC_EDIT_MOB_SKINTIME, SW_HIDE );

		m_listBOX.ShowWindow( SW_SHOW );
		m_listBox_Rand.ShowWindow( SW_SHOW );
		SetWin_ShowWindow( this, IDC_STATIC_PRESENT, SW_SHOW ); 
		SetWin_ShowWindow( this, IDC_STATIC_RANDOM, SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_UP1, SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_DW1, SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_UP2, SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_DW2, SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_ADD, SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_DEL, SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_ADD2, SW_SHOW );
		SetWin_ShowWindow( this, IDC_BUTTON_BOX_DEL2, SW_SHOW );

		InitBox ();
		InitBox_Rand();
	}

	SetReadMode();
}
bool CItemSuit::InverseUpdateItems ()
{
	//	���� Ÿ��
	m_pDummyItem->sSuitOp.emSuit = static_cast<EMSUIT> ( GetWin_Combo_Sel ( this, IDC_COMBO_SUIT ) );

	//	�� ���� ����
	GetWin_Check ( this, IDC_CHECK_TWOH, m_pDummyItem->sSuitOp.dwHAND, ITEM::EMHAND_BOTHHAND );
	GetWin_Check ( this, IDC_CHECK_BIGH, m_pDummyItem->sSuitOp.dwHAND, ITEM::EMHAND_BIG );
	GetWin_Check ( this, IDC_CHECK_BROOM, m_pDummyItem->sSuitOp.dwHAND, ITEM::EMHAND_BROOM );

	if ( GetWin_Check ( this, IDC_RADIO_RIGHT ) )	m_pDummyItem->sSuitOp.emHand = HAND_RIGHT;
	if ( GetWin_Check ( this, IDC_RADIO_LEFT ) )	m_pDummyItem->sSuitOp.emHand = HAND_LEFT;
	if ( GetWin_Check ( this, IDC_RADIO_BOTH ) )	m_pDummyItem->sSuitOp.emHand = HAND_BOTH;

	//	���� Ÿ��
	m_pDummyItem->sSuitOp.emAttack = static_cast<GLITEM_ATT> ( GetWin_Combo_Sel ( this, IDC_COMBO_ATT_TYPE ) );

	//	���� �Ÿ�
	m_pDummyItem->sSuitOp.wAttRange = GetWin_Num_int ( this, IDC_EDIT_ATT_RANGE );

	m_pDummyItem->sSuitOp.wReqSP = GetWin_Num_int ( this, IDC_EDIT_REQSP );

	//	�⺻ �ɼ�
	m_pDummyItem->sSuitOp.nHitRate = GetWin_Num_int ( this, IDC_EDIT_HIT );
	m_pDummyItem->sSuitOp.nAvoidRate = GetWin_Num_int ( this, IDC_EDIT_AVOIDRATE );

	//	���� ���
	// geger009 | GLPADATA still use WORD data type, turn it off for now
	//m_pDummyItem->sSuitOp.gdDamage.dwHigh = GetWin_Num_int ( this, IDC_EDIT_DAMAGE_HIGH );
	//m_pDummyItem->sSuitOp.gdDamage.dwLow = GetWin_Num_int ( this, IDC_EDIT_DAMAGE_LOW );
	m_pDummyItem->sSuitOp.gdDamage.wHigh = GetWin_Num_int ( this, IDC_EDIT_DAMAGE_HIGH );
	m_pDummyItem->sSuitOp.gdDamage.wLow = GetWin_Num_int ( this, IDC_EDIT_DAMAGE_LOW );
	m_pDummyItem->sSuitOp.nDefense = GetWin_Num_int ( this, IDC_EDIT_DEFENSE );

	// ���׷��̵� ��ġ
	m_pDummyItem->sBasicOp.wGradeAttack = GetWin_Num_int( this, IDC_EDIT_GRADE_1 );
	m_pDummyItem->sBasicOp.wGradeDefense = GetWin_Num_int( this, IDC_EDIT_GRADE_2 );

	// ����ġ ���
	m_pDummyItem->sBasicOp.fExpMultiple = GetWin_Num_float( this, IDC_EDIT_X_EXP );

	//	�Ҹ�ǰ ����
	m_pDummyItem->sDrugOp.emDrug = static_cast<EMITEM_DRUG> ( GetWin_Combo_Sel ( this, IDC_COMBO_DRUG_TYPE ) );
	m_pDummyItem->sDrugOp.wPileNum = GetWin_Num_int ( this, IDC_EDIT_INSTANCE_NUM );
	m_pDummyItem->sDrugOp.wCureVolume = GetWin_Num_int ( this, IDC_EDIT_INSTANCE_VOLUME );
	m_pDummyItem->sDrugOp.bRatio = GetWin_Check ( this, IDC_CHECK_CURE_RATIO );

	DWORD &dwCURE = m_pDummyItem->sDrugOp.dwCureDISORDER;
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_PALSY ), dwCURE, DIS_NUMB );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SWOON ), dwCURE, DIS_STUN );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_PETRIFY ), dwCURE, DIS_STONE );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_BURN ), dwCURE, DIS_BURN );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_FROZEN ), dwCURE, DIS_FROZEN );
	
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_DERANGE ), dwCURE, DIS_MAD );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_POISON ), dwCURE, DIS_POISON );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_CURSE ), dwCURE, DIS_CURSE );

	//	����ġ
	m_pDummyItem->sSuitOp.sResist.nSpirit = GetWin_Num_int ( this, IDC_EDIT_SPIRIT );
	m_pDummyItem->sSuitOp.sResist.nFire = GetWin_Num_int ( this, IDC_EDIT_FIRE );
	m_pDummyItem->sSuitOp.sResist.nElectric = GetWin_Num_int ( this, IDC_EDIT_ELECTRIC );
	m_pDummyItem->sSuitOp.sResist.nIce = GetWin_Num_int ( this, IDC_EDIT_ICE );
	m_pDummyItem->sSuitOp.sResist.nPoison = GetWin_Num_int ( this, IDC_EDIT_POISON );

	//	�߰� �ɼ�
	DWORD dwIndex = GetWin_Combo_Sel ( this, IDC_COMBO_ADDON_NO );
	m_pDummyItem->sSuitOp.sADDON[dwIndex].nVALUE = GetWin_Num_int ( this, IDC_EDIT_ADDON_VAR );
	m_pDummyItem->sSuitOp.sADDON[dwIndex].emTYPE = static_cast<EMITEM_ADDON> ( GetWin_Combo_Sel ( this, IDC_COMBO_ADDON_TYPE ) );

	//	��ȭ��
	m_pDummyItem->sSuitOp.sVARIATE.emTYPE = static_cast<EMITEM_VAR> ( GetWin_Combo_Sel ( this, IDC_COMBO_VARIATE ) );
	m_pDummyItem->sSuitOp.sVARIATE.fVariate = GetWin_Num_float ( this, IDC_EDIT_VARIATE_VAR );

	//	��ȭ�� by ���
	m_pDummyItem->sSuitOp.sVOLUME.emTYPE = static_cast<EMITEM_VAR> ( GetWin_Combo_Sel ( this, IDC_COMBO_VARIATE2 ) );
	m_pDummyItem->sSuitOp.sVOLUME.fVolume = GetWin_Num_float ( this, IDC_EDIT_VARIATE_VAR2 );

	//	�����̻�
	m_pDummyItem->sSuitOp.sBLOW.emTYPE = static_cast<EMSTATE_BLOW> ( GetWin_Combo_Sel ( this, IDC_COMBO_BLOW ) );

	m_pDummyItem->sSuitOp.sBLOW.fRATE = GetWin_Num_float ( this, IDC_EDIT_BLOW_RATE );
	m_pDummyItem->sSuitOp.sBLOW.fLIFE = GetWin_Num_float ( this, IDC_EDIT_BLOW_LIFE );
	m_pDummyItem->sSuitOp.sBLOW.fVAR1 = GetWin_Num_float ( this, IDC_EDIT_BLOW_VALUE1 );
	m_pDummyItem->sSuitOp.sBLOW.fVAR2 = GetWin_Num_float ( this, IDC_EDIT_BLOW_VALUE2 );

	//	Question Item by ���
	m_pDummyItem->sQuestionItem.emType = static_cast<EMITEM_QUESTION>( GetWin_Combo_Sel ( this, IDC_COMBO_QUESTION ) );

	m_pDummyItem->sQuestionItem.fTime	= GetWin_Num_float ( this, IDC_EDIT_QUE_TIME );
	m_pDummyItem->sQuestionItem.fExp	= GetWin_Num_float ( this, IDC_EDIT_QUE_EXP );
	m_pDummyItem->sQuestionItem.wParam1	= static_cast<WORD>( GetWin_Num_int ( this, IDC_EDIT_QUE_PARAM1 ) );
	m_pDummyItem->sQuestionItem.wParam2	= static_cast<WORD>( GetWin_Num_int ( this, IDC_EDIT_QUE_PARAM2 ) );

	ITEM::SSATE_BLOW &sBLOW = m_pDummyItem->sSuitOp.sBLOW;
	switch ( sBLOW.emTYPE )
	{
	case EMBLOW_NUMB:
		//	�̵��ӵ�.
		LIMIT ( sBLOW.fVAR1, 1.0f, -1.0f );
		//	������
		LIMIT ( sBLOW.fVAR2, 10.0f, -1.0f );
		break;
	case EMBLOW_STUN:
		break;
	case EMBLOW_STONE:
		//	�̵��ӵ�.
		LIMIT ( sBLOW.fVAR1, 1.0f, -1.0f );
		break;
	case EMBLOW_BURN:
		break;
	case EMBLOW_FROZEN:
		//	�̵��ӵ�.
		LIMIT ( sBLOW.fVAR1, 1.0f, -1.0f );
		//	Ÿ������
		LIMIT ( sBLOW.fVAR2, 10.0f, -1.0f );
		break;

	case EMBLOW_MAD:
		break;
	case EMBLOW_POISON:
		break;
	case EMBLOW_CURSE:
		break;
	};

	m_pDummyItem->sSkillBookOp.sSkill_ID.wMainID = GetWin_Num_int ( this, IDC_EDIT_SKILL_MID );
	m_pDummyItem->sSkillBookOp.sSkill_ID.wSubID = GetWin_Num_int ( this, IDC_EDIT_SKILL_SID );

	if ( GetWin_Check ( this, IDC_RADIO_GRIND_ARM ) )	m_pDummyItem->sGrindingOp.emCLASS = EMGRINDING_CLASS_ARM;
	if ( GetWin_Check ( this, IDC_RADIO_GRIND_CLOTH ) )	m_pDummyItem->sGrindingOp.emCLASS = EMGRINDING_CLASS_CLOTH;

	m_pDummyItem->sGrindingOp.emTYPE = (EMGRINDING_TYPE) GetWin_Combo_Sel ( this, IDC_COMBO_GRINDING_TYPE );
	m_pDummyItem->sGrindingOp.emGRINDER_TYPE = (EMGRINDER_TYPE) GetWin_Combo_Sel ( this, IDC_COMBO_GRINDING_LEVEL );

	if( m_pDummyItem->sBasicOp.emItemType == ITEM_PET_SKIN_PACK )
	{
		if( !m_pDummyItem->sPetSkinPack.VALID() )
		{
			MessageBox( "Pet Skin Pack�� ������ ���͸� �����ϼ���.", "�˸�" );
			return FALSE;
		}

		float fPercent = 0.0f;
		for( int i = 0; i < (int)m_pDummyItem->sPetSkinPack.vecPetSkinData.size(); i++ )
		{
			fPercent += m_pDummyItem->sPetSkinPack.vecPetSkinData[i].fRate;
		}

		if( fPercent != 100.0f )
		{
			MessageBox( "Pet Skin Pack�� ������ ���� ���� �ۼ�Ʈ�� ���� 100%�� �ƴմϴ�.", "�˸�" );
			return FALSE;
		}

		m_pDummyItem->sPetSkinPack.dwPetSkinTime = (DWORD)GetWin_Num_int ( this, IDC_EDIT_MOB_SKINTIME );

		if( m_pDummyItem->sPetSkinPack.dwPetSkinTime == 0 )
		{
			MessageBox( "Pet Skin Pack ����ð��� 0 �Դϴ�.", "�˸�" );
			return FALSE;
		}


	}
	return TRUE;
}

void CItemSuit::OnBnClickedButtonPrev()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( !InverseUpdateItems() ) return;

	CItemNode sItemNode;
	sItemNode.m_sItem = *m_pDummyItem;

	GLItemMan::GetInstance().InsertItem ( m_pDummyItem->sBasicOp.sNativeID, &sItemNode );

	m_pSheetTab->ActiveItemParentPage ( m_CallPage, m_pItem );
	m_pItem = NULL;
}

void CItemSuit::InitDefaultCtrls ()
{
	GetDlgItem ( IDC_BUTTON_NEXT )->EnableWindow ( FALSE );

	SetWin_Combo_Init ( this, IDC_COMBO_SUIT, COMMENT::ITEMSUIT, SUIT_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_ATT_TYPE, COMMENT::ITEMATTACK, ITEMATT_NSIZE );

	//	"��/��" �� ��ų������ ���ǹǷ� ������ �����Ϳ����� ������.
	SetWin_Combo_Del ( this, IDC_COMBO_ATT_TYPE, COMMENT::ITEMATTACK[ITEMATT_SWORDSABER].c_str() );

	SetWin_Combo_Init ( this, IDC_COMBO_ADDON_NO, COMMENT::ADDON_NO, ITEM::SSUIT::ADDON_SIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_ADDON_TYPE, COMMENT::ITEMADDON, EMADD_SIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_VARIATE, COMMENT::ITEMVAR, EMVAR_SIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_VARIATE2, COMMENT::ITEMVOL, EMVAR_SIZE ); // by ���
	SetWin_Combo_Init ( this, IDC_COMBO_QUESTION, COMMENT::ITEM_QUE_TYPE, QUESTION_SIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_BLOW, COMMENT::BLOW, EMBLOW_SIZE );

	SetWin_Combo_Init ( this, IDC_COMBO_DRUG_TYPE, COMMENT::ITEMDRUG, ITEM_DRUG_SIZE );

	SetWin_Combo_Init ( this, IDC_COMBO_GRINDING_TYPE, COMMENT::GRINDING_TYPE, EMGRINDING_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_GRINDING_LEVEL, COMMENT::GRINDING_LEVEL, EMGRINDER_SIZE );

}

void CItemSuit::OnCbnSelchangeComboAddonNo()
{
	// ���ο� �� ����.
	DWORD dwIndex = GetWin_Combo_Sel ( this, IDC_COMBO_ADDON_NO );
	SetWin_Num_int ( this, IDC_EDIT_ADDON_VAR, m_pDummyItem->sSuitOp.sADDON[dwIndex].nVALUE );
	SetWin_Combo_Sel ( this, IDC_COMBO_ADDON_TYPE, COMMENT::ITEMADDON[m_pDummyItem->sSuitOp.sADDON[dwIndex].emTYPE] );
}

void CItemSuit::OnCbnDropdownComboAddonNo()
{
	// ������ �� ��������.
	DWORD dwIndex = GetWin_Combo_Sel ( this, IDC_COMBO_ADDON_NO );
	m_pDummyItem->sSuitOp.sADDON[dwIndex].nVALUE = GetWin_Num_int ( this, IDC_EDIT_ADDON_VAR );
	m_pDummyItem->sSuitOp.sADDON[dwIndex].emTYPE = static_cast<EMITEM_ADDON> ( GetWin_Combo_Sel ( this, IDC_COMBO_ADDON_TYPE ) );
}

void CItemSuit::InitBox ()
{
	m_listBOX.ResetContent();

	for ( int i=0; i<ITEM::SBOX::ITEM_SIZE; ++i )
	{
		SNATIVEID sITEM = m_pDummyItem->sBox.sITEMS[i].nidITEM;
		DWORD dwAMOUNT = m_pDummyItem->sBox.sITEMS[i].dwAMOUNT;
		if ( sITEM==SNATIVEID(false) )		continue;

		CString strNAME = "[Error Item]";
		SITEM* pItem = GLItemMan::GetInstance().GetItem ( sITEM );
		if ( pItem )	strNAME.Format ( "[%d/%d] %d / %s", sITEM.wMainID, sITEM.wSubID, dwAMOUNT, pItem->GetName() ); 

		int nIndex = m_listBOX.AddString ( strNAME );
		m_listBOX.SetItemData ( nIndex, i );
	}
}

void CItemSuit::InitBox_Rand()
{
	m_listBox_Rand.ResetContent();

	std::vector<ITEM::SRANDOMITEM>::iterator iter = m_pDummyItem->sRandomBox.vecBOX.begin();
	std::vector<ITEM::SRANDOMITEM>::iterator iter_end = m_pDummyItem->sRandomBox.vecBOX.end();

	for ( ; iter!=iter_end; ++iter )
	{
		SNATIVEID sITEM = iter->nidITEM;
		float fRate = iter->fRATE;
		if ( sITEM==SNATIVEID(false) )		continue;

		CString strNAME = "[Error Item]";
		SITEM* pItem = GLItemMan::GetInstance().GetItem ( sITEM );
		if ( pItem )	strNAME.Format ( "[%d/%d] %.2f / %s", sITEM.wMainID, sITEM.wSubID, fRate, pItem->GetName() ); 

		int nIndex = m_listBox_Rand.AddString ( strNAME );
		m_listBox_Rand.SetItemData ( nIndex, sITEM.dwID );
	}
}

void CItemSuit::InitPetSkinPack()
{
	m_list_Mob.DeleteAllItems();

	int skinSize = (int)m_pDummyItem->sPetSkinPack.vecPetSkinData.size();
	char szTempChar[256];
	for ( int i = 0; i < skinSize; i++ )
	{
		ITEM::SPETSKINPACKITEMDATA sPetSkinPack = m_pDummyItem->sPetSkinPack.vecPetSkinData[i];
		PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( sPetSkinPack.sMobID );

		sprintf_s( szTempChar, "%d", i );
		m_list_Mob.InsertItem( i, szTempChar );
		sprintf_s( szTempChar, "%s(%d/%d)", pCrowData->GetName(), sPetSkinPack.sMobID.wMainID, sPetSkinPack.sMobID.wSubID );
		m_list_Mob.SetItemText( i, 1, szTempChar );
		sprintf_s( szTempChar, "%.2f%%", sPetSkinPack.fScale );
		m_list_Mob.SetItemText( i, 2, szTempChar );
		sprintf_s( szTempChar, "%.2f%%", sPetSkinPack.fRate );
		m_list_Mob.SetItemText( i, 3, szTempChar );
	}
    

}

void CItemSuit::OnBnClickedButtonBoxAdd()
{


	CDlgItem dlgITEM;
	dlgITEM.m_bRateEnable = false;
	dlgITEM.m_bNumEnable = true;


	if ( IDOK == dlgITEM.DoModal() )
	{
		SNATIVEID sITEM;
		
		std::map<DWORD, DWORD>::iterator iter = dlgITEM.m_SelMap.begin();		
		for ( ; iter !=  dlgITEM.m_SelMap.end(); ++iter )
		{
			sITEM.dwID = iter->first;
			m_pDummyItem->sBox.INSERT ( sITEM, iter->second );
			InitBox ();
		}

	}



}

void CItemSuit::OnBnClickedButtonBoxDel()
{
	int nIndex = m_listBOX.GetCurSel();
	if ( nIndex==LB_ERR )	return;

	DWORD_PTR dwID = m_listBOX.GetItemData ( nIndex );

	m_pDummyItem->sBox.DEL ( (int)dwID );

	m_listBOX.DeleteString ( nIndex );

	InitBox ();
}

// �߰� : 2004.9.6 By ���
//
void CItemSuit::OnBnClickedButtonBoxAdd2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDlgItem dlgITEM;
	dlgITEM.m_bRateEnable = true;
	dlgITEM.m_bNumEnable = false;

	if ( IDOK == dlgITEM.DoModal() )
	{
		SNATIVEID sITEM;
		
		std::map<DWORD, DWORD>::iterator iter = dlgITEM.m_SelMap.begin();		
		for ( ; iter !=  dlgITEM.m_SelMap.end(); ++iter )
		{
			sITEM.dwID = iter->first;
			float fRate = (float)iter->second / 100.0f;
			m_pDummyItem->sRandomBox.INSERT ( sITEM, fRate );
			InitBox ();
		}


	}

	InitBox_Rand();
}

// �߰� : 2004.9.6 By ���
//
void CItemSuit::OnBnClickedButtonBoxDel2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex = m_listBox_Rand.GetCurSel();
	if ( nIndex==LB_ERR )	return;

	m_pDummyItem->sRandomBox.DEL ( nIndex );

	m_listBox_Rand.DeleteString ( nIndex );

	InitBox_Rand();
}

void CItemSuit::OnCbnSelchangeComboBlow()
{
	//	�����̻�
	m_pDummyItem->sSuitOp.sBLOW.emTYPE = static_cast<EMSTATE_BLOW> ( GetWin_Combo_Sel ( this, IDC_COMBO_BLOW ) );

	BOOL bSTATE_BLOW = ( m_pDummyItem->sSuitOp.sBLOW.emTYPE!=EMBLOW_NONE );
	BOOL bBLOW_VAR1(TRUE), bBLOW_VAR2(TRUE);
	const char *szBLOW_VAR1 = COMMENT::BLOW_VAR1[m_pDummyItem->sSuitOp.sBLOW.emTYPE].c_str();
	const char *szBLOW_VAR2 = COMMENT::BLOW_VAR2[m_pDummyItem->sSuitOp.sBLOW.emTYPE].c_str();

	if ( !strcmp(szBLOW_VAR1,COMMENT::BLOW_VAR1[EMBLOW_NONE].c_str()) )		bBLOW_VAR1 = FALSE;
	if ( !strcmp(szBLOW_VAR2,COMMENT::BLOW_VAR2[EMBLOW_NONE].c_str()) )		bBLOW_VAR2 = FALSE;

	SetWin_Enable ( this, IDC_EDIT_BLOW_LIFE, bSTATE_BLOW );
	SetWin_Enable ( this, IDC_EDIT_BLOW_RATE, bSTATE_BLOW );

	SetWin_Text ( this, IDC_STATIC_BLOWVAR1, szBLOW_VAR1 );
	SetWin_Text ( this, IDC_STATIC_BLOWVAR2, szBLOW_VAR2 );
	SetWin_Enable ( this, IDC_EDIT_BLOW_VALUE1, bBLOW_VAR1 );
	SetWin_Enable ( this, IDC_EDIT_BLOW_VALUE2, bBLOW_VAR2 );

	SetReadMode();
}

void CItemSuit::OnCbnSelchangeComboQuestion() // by ���
{
	// Question Item
	m_pDummyItem->sQuestionItem.emType = static_cast<EMITEM_QUESTION> ( GetWin_Combo_Sel ( this, IDC_COMBO_QUESTION ) );
	BOOL bTIME(false);
	BOOL bEXP( m_pDummyItem->sQuestionItem.emType==QUESTION_EXP_GET );
	BOOL bPARAM1(false);
	BOOL bPARAM2(false);

	switch( m_pDummyItem->sQuestionItem.emType )
	{
	case QUESTION_SPEED_UP:
	case QUESTION_ATTACK_UP:
	case QUESTION_EXP_UP:
	case QUESTION_LUCKY:
	case QUESTION_SPEED_UP_M:
	case QUESTION_ATTACK_UP_M:
		bPARAM1 = bTIME = true;
		break;

	case QUESTION_CRAZY:
	case QUESTION_MADNESS:
		bTIME = bPARAM1 = bPARAM2 = true;
		break;

	case QUESTION_BOMB:
	case QUESTION_HEAL:
		bPARAM1 = true;
		break;

	case QUESTION_MOBGEN:
		bPARAM1 = bPARAM2 = true;
		break;
	};

	const char *szQUE_VAR1 = COMMENT::ITEM_QUE_VAR1[m_pDummyItem->sQuestionItem.emType].c_str();
	const char *szQUE_VAR2 = COMMENT::ITEM_QUE_VAR2[m_pDummyItem->sQuestionItem.emType].c_str();

	SetWin_Text ( this, IDC_STATIC_QUEVAR1, szQUE_VAR1 );
	SetWin_Text ( this, IDC_STATIC_QUEVAR2, szQUE_VAR2 );

	SetWin_Enable( this, IDC_EDIT_QUE_TIME, bTIME );
	SetWin_Enable( this, IDC_EDIT_QUE_EXP, bEXP );
	SetWin_Enable( this, IDC_EDIT_QUE_PARAM1, bPARAM1 );
	SetWin_Enable( this, IDC_EDIT_QUE_PARAM2, bPARAM2 );

	SetReadMode();
}

void CItemSuit::OnBnClickedButtonBoxUp1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nSel = m_listBOX.GetCurSel();
	int nCnt = m_listBOX.GetCount();

	if ( nSel <=0 || nSel >= nCnt ) return;

	ITEM::SBOX_ITEM sSelItem;

	sSelItem = m_pDummyItem->sBox.sITEMS[nSel];
	m_pDummyItem->sBox.sITEMS[nSel] = m_pDummyItem->sBox.sITEMS[nSel-1];
	m_pDummyItem->sBox.sITEMS[nSel-1] = sSelItem;

	InitBox();

	m_listBOX.SetCurSel( nSel-1);
}

void CItemSuit::OnBnClickedButtonBoxDw1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	int nSel = m_listBOX.GetCurSel();
	int nCnt = m_listBOX.GetCount();

	if ( nSel <0 || nSel >= nCnt-1 ) return;

	ITEM::SBOX_ITEM sSelItem;

	sSelItem = m_pDummyItem->sBox.sITEMS[nSel];
	m_pDummyItem->sBox.sITEMS[nSel] = m_pDummyItem->sBox.sITEMS[nSel+1];
	m_pDummyItem->sBox.sITEMS[nSel+1] = sSelItem;

	InitBox();
	m_listBOX.SetCurSel( nSel+1);
}

void CItemSuit::OnBnClickedButtonBoxUp2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nSel = m_listBox_Rand.GetCurSel();
	int nCnt = m_listBox_Rand.GetCount();

	if ( nSel <=0 || nSel >= nCnt ) return;

	ITEM::SRANDOMITEM sSelItem;	

	sSelItem = m_pDummyItem->sRandomBox.vecBOX[nSel];
	m_pDummyItem->sRandomBox.vecBOX[nSel] = m_pDummyItem->sRandomBox.vecBOX[nSel-1];
	m_pDummyItem->sRandomBox.vecBOX[nSel-1] = sSelItem;

	InitBox_Rand();

	m_listBox_Rand.SetCurSel( nSel-1 );
}

void CItemSuit::OnBnClickedButtonBoxDw2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nSel = m_listBox_Rand.GetCurSel();
	int nCnt = m_listBox_Rand.GetCount();

	if ( nSel <0 || nSel >= nCnt-1 ) return;

	ITEM::SRANDOMITEM sSelItem;	

	sSelItem = m_pDummyItem->sRandomBox.vecBOX[nSel];
	m_pDummyItem->sRandomBox.vecBOX[nSel] = m_pDummyItem->sRandomBox.vecBOX[nSel+1];
	m_pDummyItem->sRandomBox.vecBOX[nSel+1] = sSelItem;

	InitBox_Rand();

	m_listBox_Rand.SetCurSel( nSel+1 );
}



void CItemSuit::OnBnClickedButtonAddPetskin()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDlgItem dlgMonster;
	dlgMonster.m_bRateEnable = true;
	dlgMonster.m_bNumEnable  = false;
	dlgMonster.m_bUseMonster = true;

	if( m_pDummyItem->sPetSkinPack.vecPetSkinData.size() >= 10 )
	{
		MessageBox( "10�� �̻� �߰��� �� �����ϴ�.", "�˸�" );
		return;
	}


	if ( IDOK == dlgMonster.DoModal() )
	{
		CString str = GetWin_Text( this, IDC_EDIT_MOB_SCALE );
		SNATIVEID mobID;
		mobID.wMainID = dlgMonster.m_wMonsterMid;
		mobID.wSubID  = dlgMonster.m_wMonsterSid;
		m_pDummyItem->sPetSkinPack.INSERT( mobID, dlgMonster.m_fRate, (float)atof(str.GetString()) );

		InitPetSkinPack();        		
	}
}


void CItemSuit::OnBnClickedButtonDelPetskin()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nSelect = m_list_Mob.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if( nSelect == -1 ) return;

	m_pDummyItem->sPetSkinPack.DEL( nSelect );

	InitPetSkinPack();
}

void CItemSuit::SetReadMode()
{
//  �б����� ����϶��� �����Ѵ�. 
//  ���ҽ� �߰��� ���� ��� 
#ifdef READTOOL_PARAM

	const int nSkipNum = 5;
	const int nSkipID[nSkipNum] = { IDC_BUTTON_PREV, IDC_BUTTON_NEXT, IDC_BUTTON_CANCEL, IDC_BUTTON_OK, IDC_COMBO_ADDON_NO };

	int nID = 0;
	bool bOK = false;

	CWnd* pChildWnd = GetWindow(GW_CHILD);

	while (pChildWnd)
	{
		bOK = false;
		nID = pChildWnd->GetDlgCtrlID();	

		for ( int i = 0; i < nSkipNum; ++i )
		{
			if ( nID == nSkipID[i] )
			{
				bOK = true;	
				break;
			}
		}

		if ( !bOK )	pChildWnd->EnableWindow( FALSE );

		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
	}
#endif
}

