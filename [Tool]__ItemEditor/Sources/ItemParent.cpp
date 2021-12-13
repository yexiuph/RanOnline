// ItemProperty1.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ItemEdit.h"
#include "ItemParent.h"
#include "GLOGIC.h"

#include "DxEffcharData.h"
#include "DxSkinPieceContainer.h"
//geger009 | Add ABL and ABF code (not implemented yet turn it off for now)
//#include "DxSkinPieceRootData.h"
#include "DxSimpleMeshMan.h"
#include "EtcFunction.h"

#include "SheetWithTab.h"

#include "GLStringTable.h"
#include ".\itemparent.h"

#include "GLPet.h"
#include "GLVEHICLE.h"

// CItemParent 대화 상자입니다.

IMPLEMENT_DYNAMIC(CItemParent, CPropertyPage)
CItemParent::CItemParent( LOGFONT logfont )
	: CPropertyPage(CItemParent::IDD),
	m_pFont ( NULL )
{
	m_bDlgInit = FALSE;
	m_pItem = NULL;
	m_pDummyItem = new SITEM;

	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CItemParent::~CItemParent()
{
	SAFE_DELETE ( m_pDummyItem );
	SAFE_DELETE ( m_pFont );
}

void CItemParent::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);	
}


BEGIN_MESSAGE_MAP(CItemParent, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_INVENFILE, OnBnClickedButtonInvenfile)
	ON_BN_CLICKED(IDC_BUTTON_FIELDFILE, OnBnClickedButtonFieldfile)
	
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C1, OnBnClickedButtonWearingfileC1)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C2, OnBnClickedButtonWearingfileC2)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C3, OnBnClickedButtonWearingfileC3)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C4, OnBnClickedButtonWearingfileC4)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C5, OnBnClickedButtonWearingfileC5)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C6, OnBnClickedButtonWearingfileC6)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C7, OnBnClickedButtonWearingfileC7)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C8, OnBnClickedButtonWearingfileC8)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C9, OnBnClickedButtonWearingfileC9)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C10, OnBnClickedButtonWearingfileC10)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C11, OnBnClickedButtonWearingfileC11)
	
	ON_BN_CLICKED(IDC_BUTTON_SKIN_EFFECTFILE, OnBnClickedButtonSkinEffectfile)
	ON_BN_CLICKED(IDC_BUTTON_EFFECTFILE, OnBnClickedButtonEffectfile)
	ON_BN_CLICKED(IDC_BUTTON_SKIN_TAR_EFFECTFILE, OnBnClickedButtonSkinTarEffectfile)
	ON_EN_CHANGE(IDC_EDIT_ITEMNAME, OnEnChangeEditItemname)
	ON_EN_CHANGE(IDC_EDIT_ITEMDESC, OnEnChangeEditItemdesc)

	ON_CBN_SELCHANGE(IDC_COMBO_ITEM_TYPE, OnCbnSelchangeComboItemType)
	ON_EN_CHANGE(IDC_EDIT_SPECID, OnEnChangeEditSpecid)
	ON_BN_CLICKED(IDC_BUTTON_ABL_M_LEFT, OnBnClickedButtonAblMLeft)
	ON_BN_CLICKED(IDC_BUTTON_ABL_M_RIGHT, OnBnClickedButtonAblMRight)
	ON_BN_CLICKED(IDC_BUTTON_ABL_W_LEFT, OnBnClickedButtonAblWLeft)
	ON_BN_CLICKED(IDC_BUTTON_ABL_W_RIGHT, OnBnClickedButtonAblWRight)
	ON_BN_CLICKED(IDC_BUTTON_VEHICLE_PART1, OnBnClickedButtonVehiclePart1)
	ON_BN_CLICKED(IDC_BUTTON_VEHICLE_PART2, OnBnClickedButtonVehiclePart2)
	ON_BN_CLICKED(IDC_BUTTON_VEHICLE_PART3, OnBnClickedButtonVehiclePart3)
	ON_BN_CLICKED(IDC_BUTTON_VEHICLE_PART4, OnBnClickedButtonVehiclePart4)
	ON_BN_CLICKED(IDC_BUTTON_VEHICLE_PART5, OnBnClickedButtonVehiclePart5)
	ON_CBN_SELCHANGE(IDC_COMBO_VEHICLE_TYPE, OnCbnSelchangeComboVehicleType)

	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C14, OnBnClickedButtonWearingfileC14)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C15, OnBnClickedButtonWearingfileC15)
	ON_BN_CLICKED(IDC_RADIO_ALL, OnBnClickedRadioAll)
	ON_BN_CLICKED(IDC_RADIO_MAN, OnBnClickedRadioMan)
	ON_BN_CLICKED(IDC_RADIO_WOMAN, OnBnClickedRadioWoman)
END_MESSAGE_MAP()


// CItemParent 메시지 처리기입니다.

BOOL CItemParent::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );
	
	m_bDlgInit = TRUE;
	InitDefaultCtrls ();
	UpdateItems ();

	SetReadMode();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CItemParent::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pItem = NULL;
	m_pSheetTab->ActiveItemSuitTreePage ();
}

void CItemParent::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( !InverseUpdateItems() )
	{		
		return;
	}

	CItemNode sItemNode;
	sItemNode.m_sItem = *m_pDummyItem;

	GLItemMan::GetInstance().InsertItem ( m_pDummyItem->sBasicOp.sNativeID, &sItemNode );

	GLStringTable::GetInstance().InsertString ( strKeyName, strName, GLStringTable::ITEM );
	GLStringTable::GetInstance().InsertString ( strKeyDesc, strDesc, GLStringTable::ITEM );

	m_pItem = NULL;
	m_pSheetTab->ActiveItemSuitTreePage ();
}

void CItemParent::InitAllItem ()
{
	m_pItem = NULL;
}

BOOL CItemParent::SetItem ( SITEM* pItem )
{
	if ( m_pItem )
	{
		return FALSE;
	}

	m_pItem = pItem;

	//	더미로 가져가기
	*m_pDummyItem = *m_pItem;

	if ( m_bDlgInit ) 
	{
		InitDefaultCtrls ();
		UpdateItems ();		
	}

	return TRUE;
}

void CItemParent::UpdateItems ()
{
	int temp = 100;


	//	고유 ID
	SetWin_Num_int ( this, IDC_EDIT_MID, m_pDummyItem->sBasicOp.sNativeID.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_SID, m_pDummyItem->sBasicOp.sNativeID.wSubID );

	//	아이템 이름
	SetWin_Text ( this, IDC_EDIT_ITEMNAME, m_pDummyItem->sBasicOp.strName.c_str() );
	//SetWin_Text ( this, IDC_EDIT_NAME, GLStringTable::GetInstance().GetName( m_pDummyItem->GetName() ) );

	//	아이템 래밸
	//SetWin_Combo_Sel ( this, IDC_COMBO_LEVEL, COMMENT::ITEMLEVEL[m_pDummyItem->sBasicOp.emLevel] );
	SetWin_Combo_Sel ( this, IDC_COMBO_LEVEL, (int)m_pDummyItem->sBasicOp.emLevel );

	//	Trade
	SetWin_Check ( this, IDC_CHECK_SELLENABLE, m_pDummyItem->sBasicOp.dwFlags&TRADE_SALE );
	SetWin_Check ( this, IDC_CHECK_BUYABLE, m_pDummyItem->sBasicOp.dwFlags&TRADE_EXCHANGE );
	SetWin_Check ( this, IDC_CHECK_THROW, m_pDummyItem->sBasicOp.dwFlags&TRADE_THROW );
	SetWin_Check ( this, IDC_CHECK_GARBAGE, m_pDummyItem->sBasicOp.dwFlags&TRADE_GARBAGE );
	SetWin_Check ( this, IDC_CHECK_EVENT_SGL, m_pDummyItem->sBasicOp.dwFlags&TRADE_EVENT_SGL );

	SetWin_Check ( this, IDC_CHECK_DISGUISE, m_pDummyItem->sBasicOp.dwFlags&ITEM_DISGUISE );
	SetWin_Check ( this, IDC_CHECK_TIMELMT, m_pDummyItem->sBasicOp.dwFlags&ITEM_TIMELMT );
	
	//	구입가격
	SetWin_Num_int ( this, IDC_EDIT_PRICE, m_pDummyItem->sBasicOp.dwBuyPrice );
	SetWin_Num_int ( this, IDC_EDIT_PRICE2, m_pDummyItem->sBasicOp.dwSellPrice );

	//	아이템 생성 규칙
	SetWin_Num_int ( this, IDC_EDIT_SPECID, m_pDummyItem->sGenerateOp.dwSpecID );
	SetWin_Num_int ( this, IDC_EDIT_TIME_SET, m_pDummyItem->sGenerateOp.dwLimitTime );
	SetWin_Num_int ( this, IDC_EDIT_TIME_LIMIT, m_pDummyItem->sGenerateOp.dwLimitTimeGen );

	//	인벤 크기.
	SetWin_Num_int ( this, IDC_EDIT_INVENSIZEX, m_pDummyItem->sBasicOp.wInvenSizeX );
	SetWin_Num_int ( this, IDC_EDIT_INVENSIZEY, m_pDummyItem->sBasicOp.wInvenSizeY );

	//	아이콘 인덱스.
	SetWin_Num_int ( this, IDC_EDIT_INVENICON_X, m_pDummyItem->sBasicOp.sICONID.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_INVENICON_Y, m_pDummyItem->sBasicOp.sICONID.wSubID );

	//	형상 파일.
	SetWin_Text ( this, IDC_EDIT_FIELDFILE, m_pDummyItem->GetFieldFile() );
	SetWin_Text ( this, IDC_EDIT_INVENFILE, m_pDummyItem->GetInventoryFile() );

	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C1, m_pDummyItem->GetWearingFile((EMCHARINDEX)0) ); // 격투남 	
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C2, m_pDummyItem->GetWearingFile((EMCHARINDEX)6) ); // 격투여
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C3, m_pDummyItem->GetWearingFile((EMCHARINDEX)1) ); // 검도남
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C4, m_pDummyItem->GetWearingFile((EMCHARINDEX)7) ); // 검도여
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C5, m_pDummyItem->GetWearingFile((EMCHARINDEX)8) ); // 양궁남
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C6, m_pDummyItem->GetWearingFile((EMCHARINDEX)2) ); // 양궁여	
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C7, m_pDummyItem->GetWearingFile((EMCHARINDEX)9) ); // 기예남
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C8, m_pDummyItem->GetWearingFile((EMCHARINDEX)3) ); // 기예여

	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C9, m_pDummyItem->GetWearingFile((EMCHARINDEX)4) );
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C10, m_pDummyItem->GetWearingFile((EMCHARINDEX)5) );

	// geger009 | gunner and assasin not implemented yet, turn it off for now
	/*SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C12, m_pDummyItem->GetWearingFile((EMCHARINDEX)10) );
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C13, m_pDummyItem->GetWearingFile((EMCHARINDEX)12) );

	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C14, m_pDummyItem->GetWearingFile((EMCHARINDEX)11) );
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C15, m_pDummyItem->GetWearingFile((EMCHARINDEX)13) );*/
	
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C11, m_pDummyItem->GetPetWearingFile () );
	SetWin_Text ( this, IDC_EDIT_ITEMDESC, m_pDummyItem->sBasicOp.strComment.c_str() );
	
	//	빛, 어둠 속성.	
	SetWin_Check ( this, IDC_RADIO_BRIGHT_LIGHT, m_pDummyItem->sBasicOp.emReqBright==BRIGHT_LIGHT);
	SetWin_Check ( this, IDC_RADIO_BRIGHT_DARK,  m_pDummyItem->sBasicOp.emReqBright==BRIGHT_DARK );
	SetWin_Check ( this, IDC_RADIO_BRIGHT_BOTH,  m_pDummyItem->sBasicOp.emReqBright==BRIGHT_BOTH );

	//	사용가능 캐릭터.
	SetWin_Check ( this, IDC_CHECK_FIGHTER_M, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_FIGHTER_M );
	SetWin_Check ( this, IDC_CHECK_ARMS_M, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_ARMS_M );
	SetWin_Check ( this, IDC_CHECK_ARCHER_W, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_ARCHER_W );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_W, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_SPIRIT_W );

	SetWin_Check ( this, IDC_CHECK_FIGHTER_W, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_FIGHTER_W );
	SetWin_Check ( this, IDC_CHECK_ARMS_W, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_ARMS_W );
	SetWin_Check ( this, IDC_CHECK_ARCHER_M, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_ARCHER_M );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_M, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_SPIRIT_M );

	SetWin_Check ( this, IDC_CHECK_EXTREME_M, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_EXTREME_M );
	SetWin_Check ( this, IDC_CHECK_EXTREME_W, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_EXTREME_W );

	// geger009 | Not implemented yet turn off it for now
	/*SetWin_Check ( this, IDC_CHECK_SCIENCE_M, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_SCIENTIST_M );
	SetWin_Check ( this, IDC_CHECK_SCIENCE_W, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_SCIENTIST_W );

	SetWin_Check ( this, IDC_CHECK_ASSASSIN_M, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_ASSASSIN_M );
	SetWin_Check ( this, IDC_CHECK_ASSASSIN_W, m_pDummyItem->sBasicOp.dwReqCharClass&GLCC_ASSASSIN_W );*/


	//	사용가능 학원.
	SetWin_Check ( this, IDC_CHECK_SCHOOL1, m_pDummyItem->sBasicOp.dwReqSchool&GLSCHOOL_00 );
	SetWin_Check ( this, IDC_CHECK_SCHOOL2, m_pDummyItem->sBasicOp.dwReqSchool&GLSCHOOL_01 );
	SetWin_Check ( this, IDC_CHECK_SCHOOL3, m_pDummyItem->sBasicOp.dwReqSchool&GLSCHOOL_02 );

	//	요구 수치.
	SetWin_Num_int ( this, IDC_EDIT_NEEDLEVELDW, m_pDummyItem->sBasicOp.wReqLevelDW );
	SetWin_Num_int ( this, IDC_EDIT_NEEDLEVELUP, m_pDummyItem->sBasicOp.wReqLevelUP );

	SetWin_Num_int ( this, IDC_EDIT_POWER, m_pDummyItem->sBasicOp.sReqStats.wPow );
	SetWin_Num_int ( this, IDC_EDIT_STRENGTH, m_pDummyItem->sBasicOp.sReqStats.wStr );
	SetWin_Num_int ( this, IDC_EDIT_SPIRIT, m_pDummyItem->sBasicOp.sReqStats.wSpi );
	SetWin_Num_int ( this, IDC_EDIT_DEXTERITY, m_pDummyItem->sBasicOp.sReqStats.wDex );
	SetWin_Num_int ( this, IDC_EDIT_INTELLIGENT, m_pDummyItem->sBasicOp.sReqStats.wInt );
	SetWin_Num_int ( this, IDC_EDIT_STAMINA, m_pDummyItem->sBasicOp.sReqStats.wSta );

	SetWin_Num_int ( this, IDC_EDIT_REQ_PA, m_pDummyItem->sBasicOp.wReqPA );
	SetWin_Num_int ( this, IDC_EDIT_REQ_SA, m_pDummyItem->sBasicOp.wReqSA );

	//	아이템 종류.
	SetWin_Combo_Sel ( this, IDC_COMBO_ITEM_TYPE, COMMENT::ITEMTYPE[m_pDummyItem->sBasicOp.emItemType] );
	SetWin_Combo_Sel ( this, IDC_COMBO_ITEM_TYPE, (int)m_pDummyItem->sBasicOp.emItemType );

	// PET
	SetWin_Combo_Sel ( this, IDC_COMBO_PET_TYPE, (int)m_pDummyItem->sPet.emPetType );
	SetWin_Num_int ( this, IDC_EDIT_PET_MID, m_pDummyItem->sPet.sPetID.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_PET_SID, m_pDummyItem->sPet.sPetID.wSubID );

	// 탈것
	SetWin_Combo_Sel ( this, IDC_COMBO_VEHICLE_TYPE, (int)m_pDummyItem->sVehicle.emVehicleType );

	//	개조 가능 횟수.
	SetWin_Num_int ( this, IDC_EDIT_REMODEL_NUM, m_pDummyItem->sSuitOp.wReModelNum );
	
	//	소모형.
	SetWin_Check ( this, IDC_CHECK_INSTANCE, m_pDummyItem->sDrugOp.bInstance );

	CTimeSpan cSPAN(m_pDummyItem->sDrugOp.tTIME_LMT);
	SetWin_Num_int ( this, IDC_EDIT_LMT_D, (int)cSPAN.GetDays() );
	SetWin_Num_int ( this, IDC_EDIT_LMT_H, cSPAN.GetHours() );
	SetWin_Num_int ( this, IDC_EDIT_LMT_M, cSPAN.GetMinutes() );

	SetWin_Text ( this, IDC_EDIT_SKIN_EFFECTFILE, m_pDummyItem->GetSelfBodyEffect() );
	SetWin_Text ( this, IDC_EDIT_TAR_EFFECTFILE, m_pDummyItem->GetTargBodyEffect() );
	SetWin_Text ( this, IDC_EDIT_EFFECTFILE, m_pDummyItem->GetTargetEffect() );

	SetWin_Text ( this, IDC_EDIT_RANDOM_OPT, m_pDummyItem->sRandomOpt.szNAME );

	SetWin_Num_int ( this, IDC_EDIT_MAP_MID, m_pDummyItem->sBasicOp.sSubID.wMainID );
	SetWin_Num_int ( this, IDC_EDIT_MAP_SID, m_pDummyItem->sBasicOp.sSubID.wSubID );
	SetWin_Num_int ( this, IDC_EDIT_MAP_POSX, m_pDummyItem->sBasicOp.wPosX );
	SetWin_Num_int ( this, IDC_EDIT_MAP_POSY, m_pDummyItem->sBasicOp.wPosY );

	//Cool down
	SetWin_Combo_Sel ( this, IDC_COMBO_COOL_TYPE, COMMENT::COOLTYPE[m_pDummyItem->sBasicOp.emCoolType] );
	SetWin_Num_int ( this, IDC_EDIT_COOL_TIME, m_pDummyItem->sBasicOp.dwCoolTime );

	// geger009 | ABL and VPS not implemented yet, turn it off for now
	//ABL File
	/*SetWin_Text ( this, IDC_EDIT_ABL_M_LEFT, m_pDummyItem->GetCpsFile((EMCPSINDEX)0) ); // 격투남 	
	SetWin_Text ( this, IDC_EDIT_ABL_M_RIGHT, m_pDummyItem->GetCpsFile((EMCPSINDEX)1) ); // 격투여
	SetWin_Text ( this, IDC_EDIT_ABL_W_LEFT, m_pDummyItem->GetCpsFile((EMCPSINDEX)2) ); // 검도남
	SetWin_Text ( this, IDC_EDIT_ABL_W_RIGHT, m_pDummyItem->GetCpsFile((EMCPSINDEX)3) ); // 검도여

	//VPS File
	SetWin_Text ( this, IDC_EDIT_VEHICLE_PART1, m_pDummyItem->GetVpsFile((EMVPSINDEX)0) );
	SetWin_Text ( this, IDC_EDIT_VEHICLE_PART2, m_pDummyItem->GetVpsFile((EMVPSINDEX)1) );
	SetWin_Text ( this, IDC_EDIT_VEHICLE_PART3, m_pDummyItem->GetVpsFile((EMVPSINDEX)2) );
	SetWin_Text ( this, IDC_EDIT_VEHICLE_PART4, m_pDummyItem->GetVpsFile((EMVPSINDEX)3) );
	SetWin_Text ( this, IDC_EDIT_VEHICLE_PART5, m_pDummyItem->GetVpsFile((EMVPSINDEX)4) );*/

	OnCbnSelchangeComboVehicleType();

}

BOOL CItemParent::InverseUpdateItems ()
{
	//	고유 ID
	m_pDummyItem->sBasicOp.sNativeID.wMainID = GetWin_Num_int ( this, IDC_EDIT_MID );
	m_pDummyItem->sBasicOp.sNativeID.wSubID = GetWin_Num_int ( this, IDC_EDIT_SID );

	//	아이템 이름
	m_pDummyItem->sBasicOp.strName = GetWin_Text ( this, IDC_EDIT_ITEMNAME ).GetString();
	strKeyName = GetWin_Text ( this, IDC_EDIT_ITEMNAME ).GetString();
	strName	= GetWin_Text ( this, IDC_EDIT_NAME ).GetString();

	//	아이템 래밸
	m_pDummyItem->sBasicOp.emLevel = (EMITEMLEVEL) GetWin_Combo_Sel ( this, IDC_COMBO_LEVEL );

	//	Trade
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SELLENABLE ), m_pDummyItem->sBasicOp.dwFlags, TRADE_SALE );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_BUYABLE ), m_pDummyItem->sBasicOp.dwFlags, TRADE_EXCHANGE );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_THROW ), m_pDummyItem->sBasicOp.dwFlags, TRADE_THROW );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_GARBAGE ), m_pDummyItem->sBasicOp.dwFlags, TRADE_GARBAGE );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EVENT_SGL ), m_pDummyItem->sBasicOp.dwFlags, TRADE_EVENT_SGL );

	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_DISGUISE ), m_pDummyItem->sBasicOp.dwFlags, ITEM_DISGUISE );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_TIMELMT ), m_pDummyItem->sBasicOp.dwFlags, ITEM_TIMELMT );

	//	구입가격
	m_pDummyItem->sBasicOp.dwBuyPrice  = GetWin_Num_int ( this, IDC_EDIT_PRICE );
	//  판매가격
	m_pDummyItem->sBasicOp.dwSellPrice = GetWin_Num_int ( this, IDC_EDIT_PRICE2 );

	//	아이템 생성 규칙
	m_pDummyItem->sGenerateOp.dwSpecID = GetWin_Num_int ( this, IDC_EDIT_SPECID );
	m_pDummyItem->sGenerateOp.dwLimitTime = GetWin_Num_int ( this, IDC_EDIT_TIME_SET );
	m_pDummyItem->sGenerateOp.dwLimitTimeGen = GetWin_Num_int ( this, IDC_EDIT_TIME_LIMIT );

    //	인벤 크기.
	m_pDummyItem->sBasicOp.wInvenSizeX = GetWin_Num_int ( this, IDC_EDIT_INVENSIZEX );
	m_pDummyItem->sBasicOp.wInvenSizeY = GetWin_Num_int ( this, IDC_EDIT_INVENSIZEY );

	//	아이콘 인덱스.
	m_pDummyItem->sBasicOp.sICONID.wMainID = GetWin_Num_int ( this, IDC_EDIT_INVENICON_X );
	m_pDummyItem->sBasicOp.sICONID.wSubID = GetWin_Num_int ( this, IDC_EDIT_INVENICON_Y );

	//	형상 파일.
	m_pDummyItem->sBasicOp.strFieldFile = GetWin_Text ( this, IDC_EDIT_FIELDFILE ).GetString();
	m_pDummyItem->sBasicOp.strInventoryFile = GetWin_Text ( this, IDC_EDIT_INVENFILE ).GetString();

	m_pDummyItem->sBasicOp.strWearingFile[GLCI_FIGHTER_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C1 ).GetString();
	m_pDummyItem->sBasicOp.strWearingFile[GLCI_FIGHTER_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C2 ).GetString();
	m_pDummyItem->sBasicOp.strWearingFile[GLCI_ARMS_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C3 ).GetString();
	m_pDummyItem->sBasicOp.strWearingFile[GLCI_ARMS_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C4 ).GetString();
	m_pDummyItem->sBasicOp.strWearingFile[GLCI_ARCHER_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C5 ).GetString();
	m_pDummyItem->sBasicOp.strWearingFile[GLCI_ARCHER_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C6 ).GetString();
	m_pDummyItem->sBasicOp.strWearingFile[GLCI_SPIRIT_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C7 ).GetString();	
	m_pDummyItem->sBasicOp.strWearingFile[GLCI_SPIRIT_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C8 ).GetString();	
	m_pDummyItem->sBasicOp.strWearingFile[GLCI_EXTREME_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C9 ).GetString();
	m_pDummyItem->sBasicOp.strWearingFile[GLCI_EXTREME_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C10 ).GetString();
	// geger009 | gunner and assasin class not implemented yet, turn it off for now
	/*m_pDummyItem->sBasicOp.strWearingFile[GLCI_SCIENTIST_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C12 ).GetString();
	m_pDummyItem->sBasicOp.strWearingFile[GLCI_SCIENTIST_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C13 ).GetString();
	m_pDummyItem->sBasicOp.strWearingFile[GLCI_ASSASSIN_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C14 ).GetString();
	m_pDummyItem->sBasicOp.strWearingFile[GLCI_ASSASSIN_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C15 ).GetString();*/
	
	m_pDummyItem->sBasicOp.strPetWearingFile = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C11 ).GetString();

	strKeyDesc = GetWin_Text ( this, IDC_EDIT_ITEMDESC ).GetString();
	CString strComment = GetWin_Text ( this, IDC_EDIT_COMMENT );
	//if ( strComment.GetLength()+1 < ITEM_SZCOMMENT )	m_pDummyItem->sBasicOp.strComment = strComment.GetString();
	if ( strComment.GetLength()+1 < ITEM_SZCOMMENT ) strDesc = strComment.GetString();

	//	빛, 어둠 속성.
	if ( GetWin_Check ( this, IDC_RADIO_BRIGHT_LIGHT) )	m_pDummyItem->sBasicOp.emReqBright = BRIGHT_LIGHT;
	if ( GetWin_Check ( this, IDC_RADIO_BRIGHT_DARK ) )	m_pDummyItem->sBasicOp.emReqBright = BRIGHT_DARK;
	if ( GetWin_Check ( this, IDC_RADIO_BRIGHT_BOTH ) )	m_pDummyItem->sBasicOp.emReqBright = BRIGHT_BOTH;

	//	사용가능 캐릭터.
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_FIGHTER_M ),m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_FIGHTER_M );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_ARMS_M ),   m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_ARMS_M );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_ARCHER_W ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_ARCHER_W );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SPIRIT_W ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_SPIRIT_W );

	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_FIGHTER_W ),m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_FIGHTER_W );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_ARMS_W ),   m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_ARMS_W );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_ARCHER_M ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_ARCHER_M );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SPIRIT_M ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_SPIRIT_M );

	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EXTREME_M ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_EXTREME_M );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_EXTREME_W ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_EXTREME_W );

	// geger009 | gunner and assasin class not implemented yet, turn it off for now
	/*SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SCIENCE_M ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_SCIENTIST_M );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SCIENCE_W ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_SCIENTIST_W );

	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_ASSASSIN_M ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_ASSASSIN_M );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_ASSASSIN_W ), m_pDummyItem->sBasicOp.dwReqCharClass, GLCC_ASSASSIN_W );*/


	//	사용가능 학원.
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SCHOOL1 ), m_pDummyItem->sBasicOp.dwReqSchool, GLSCHOOL_00 );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SCHOOL2 ), m_pDummyItem->sBasicOp.dwReqSchool, GLSCHOOL_01 );
	SetCheck_Flags ( GetWin_Check ( this, IDC_CHECK_SCHOOL3 ), m_pDummyItem->sBasicOp.dwReqSchool, GLSCHOOL_02 );

	if ( m_pDummyItem->sBasicOp.dwReqCharClass == GLCC_NONE )
	{
		MessageBox ( "An usable class is not selected.", "Caution", MB_OK | MB_ICONEXCLAMATION );
		return FALSE;
	}

	//	요구 수치.
	m_pDummyItem->sBasicOp.wReqLevelDW = GetWin_Num_int ( this, IDC_EDIT_NEEDLEVELDW );
	m_pDummyItem->sBasicOp.wReqLevelUP = GetWin_Num_int ( this, IDC_EDIT_NEEDLEVELUP );

	m_pDummyItem->sBasicOp.sReqStats.wPow = GetWin_Num_int ( this, IDC_EDIT_POWER );
	m_pDummyItem->sBasicOp.sReqStats.wStr = GetWin_Num_int ( this, IDC_EDIT_STRENGTH );
	m_pDummyItem->sBasicOp.sReqStats.wSpi = GetWin_Num_int ( this, IDC_EDIT_SPIRIT );
	m_pDummyItem->sBasicOp.sReqStats.wDex = GetWin_Num_int ( this, IDC_EDIT_DEXTERITY );
	m_pDummyItem->sBasicOp.sReqStats.wInt = GetWin_Num_int ( this, IDC_EDIT_INTELLIGENT );
	m_pDummyItem->sBasicOp.sReqStats.wSta = GetWin_Num_int ( this, IDC_EDIT_STAMINA );

	m_pDummyItem->sBasicOp.wReqPA = GetWin_Num_int ( this, IDC_EDIT_REQ_PA );
	m_pDummyItem->sBasicOp.wReqSA = GetWin_Num_int ( this, IDC_EDIT_REQ_SA );

	//	아이템 종류.
	m_pDummyItem->sBasicOp.emItemType = (EMITEM_TYPE) GetWin_Combo_Sel ( this, IDC_COMBO_ITEM_TYPE );

	// PET
	m_pDummyItem->sPet.sPetID.wMainID = GetWin_Num_int ( this, IDC_EDIT_PET_MID );
	m_pDummyItem->sPet.sPetID.wSubID = GetWin_Num_int ( this, IDC_EDIT_PET_SID );
	m_pDummyItem->sPet.emPetType = (PETTYPE) GetWin_Combo_Sel ( this, IDC_COMBO_PET_TYPE );

	// 탈것 타입
	m_pDummyItem->sVehicle.emVehicleType = (VEHICLE_TYPE) GetWin_Combo_Sel ( this, IDC_COMBO_VEHICLE_TYPE );

	//	개조 가능 횟수.
	m_pDummyItem->sSuitOp.wReModelNum = GetWin_Num_int ( this, IDC_EDIT_REMODEL_NUM );
	if ( m_pDummyItem->sSuitOp.wReModelNum > 0xff )	m_pDummyItem->sSuitOp.wReModelNum = 0xff;
	
	//	소모형.
	m_pDummyItem->sDrugOp.bInstance = GetWin_Check ( this, IDC_CHECK_INSTANCE );

	LONG lDays = GetWin_Num_int ( this, IDC_EDIT_LMT_D );
	int nHours = GetWin_Num_int ( this, IDC_EDIT_LMT_H );
	int nMins = GetWin_Num_int ( this, IDC_EDIT_LMT_M );
	
	CTimeSpan cSPAN(lDays,nHours,nMins,0);
	m_pDummyItem->sDrugOp.tTIME_LMT = cSPAN.GetTimeSpan();

	m_pDummyItem->sBasicOp.strSelfBodyEffect = GetWin_Text(this,IDC_EDIT_SKIN_EFFECTFILE).GetString();
	m_pDummyItem->sBasicOp.strTargBodyEffect = GetWin_Text(this,IDC_EDIT_TAR_EFFECTFILE).GetString();
	m_pDummyItem->sBasicOp.strTargetEffect = GetWin_Text(this,IDC_EDIT_EFFECTFILE).GetString();

	StringCchCopy ( m_pDummyItem->sRandomOpt.szNAME, SRANDOM_DATA::NAME_LEN, GetWin_Text(this,IDC_EDIT_RANDOM_OPT).GetString() );

	m_pDummyItem->sBasicOp.sSubID.wMainID = GetWin_Num_int ( this, IDC_EDIT_MAP_MID );
	m_pDummyItem->sBasicOp.sSubID.wSubID = GetWin_Num_int ( this, IDC_EDIT_MAP_SID );
	m_pDummyItem->sBasicOp.wPosX = GetWin_Num_int ( this, IDC_EDIT_MAP_POSX );
	m_pDummyItem->sBasicOp.wPosY = GetWin_Num_int ( this, IDC_EDIT_MAP_POSY );

	//Cool down
	m_pDummyItem->sBasicOp.emCoolType = (EMCOOL_TYPE) GetWin_Combo_Sel ( this, IDC_COMBO_COOL_TYPE );
	m_pDummyItem->sBasicOp.dwCoolTime = (DWORD)GetWin_Num_int ( this, IDC_EDIT_COOL_TIME );

	// geger009 | ABL and VPS not implemented yet, turn it off for now
	//ABL File
	/*m_pDummyItem->sBasicOp.strABLFile[CPSIND_M_LEFT] = GetWin_Text ( this, IDC_EDIT_ABL_M_LEFT ).GetString();
	m_pDummyItem->sBasicOp.strABLFile[CPSIND_M_RIGHT] = GetWin_Text ( this, IDC_EDIT_ABL_M_RIGHT ).GetString();
	m_pDummyItem->sBasicOp.strABLFile[CPSIND_W_LEFT] = GetWin_Text ( this, IDC_EDIT_ABL_W_LEFT ).GetString();
	m_pDummyItem->sBasicOp.strABLFile[CPSIND_W_RIGHT] = GetWin_Text ( this, IDC_EDIT_ABL_W_RIGHT ).GetString();

	//VPS File
	m_pDummyItem->sBasicOp.strVPSFile[VPSIND_SCUTER] = GetWin_Text ( this, IDC_EDIT_VEHICLE_PART1 ).GetString();
	m_pDummyItem->sBasicOp.strVPSFile[VPSIND_BT5] = GetWin_Text ( this, IDC_EDIT_VEHICLE_PART2 ).GetString();
	m_pDummyItem->sBasicOp.strVPSFile[VPSIND_BT7] = GetWin_Text ( this, IDC_EDIT_VEHICLE_PART3 ).GetString();
	m_pDummyItem->sBasicOp.strVPSFile[VPSIND_BT9] = GetWin_Text ( this, IDC_EDIT_VEHICLE_PART4 ).GetString();
	m_pDummyItem->sBasicOp.strVPSFile[VPSIND_RARE] = GetWin_Text ( this, IDC_EDIT_VEHICLE_PART5 ).GetString();*/

	return TRUE;
}

void CItemParent::OnBnClickedButtonNext()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( !InverseUpdateItems() )
	{
		return;
	}

	CItemNode sItemNode;
	sItemNode.m_sItem = *m_pDummyItem;

	GLItemMan::GetInstance().InsertItem ( m_pDummyItem->sBasicOp.sNativeID, &sItemNode );

	GLStringTable::GetInstance().InsertString ( strKeyName, strName, GLStringTable::ITEM );
	GLStringTable::GetInstance().InsertString ( strKeyDesc, strDesc, GLStringTable::ITEM );

	m_pSheetTab->ActiveItemSuitPage ( m_CallPage, m_pItem );
	m_pItem = NULL;
}

void CItemParent::InitDefaultCtrls ()
{
	GetDlgItem ( IDC_BUTTON_PREV )->EnableWindow ( FALSE );

	SetWin_Combo_Init ( this, IDC_COMBO_LEVEL, COMMENT::ITEMLEVEL, LEVEL_NSIZE );
	SetWin_Combo_Init ( this, IDC_COMBO_ITEM_TYPE, COMMENT::ITEMTYPE, ITEM_NSIZE );

	// PET
	SetWin_Combo_Init ( this, IDC_COMBO_PET_TYPE, COMMENT::PET_TYPE, PETTYPE_SIZE );

	// VEHICLE
	SetWin_Combo_Init ( this, IDC_COMBO_VEHICLE_TYPE, COMMENT::VEHICLE_TYPE, VEHICLE_TYPE_SIZE );

	// COOL DOWN
	SetWin_Combo_Init ( this, IDC_COMBO_COOL_TYPE, COMMENT::COOLTYPE, EMCOOL_SIZE );
}

void CItemParent::OnBnClickedButtonInvenfile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".x",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemParent*)this);

	dlg.m_ofn.lpstrInitialDir = DxSimpleMeshMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, IDC_EDIT_INVENFILE, dlg.GetFileName().GetString() );
	}
}

void CItemParent::OnBnClickedButtonFieldfile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Item File (*.x)|*.x|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".x",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemParent*)this);

	dlg.m_ofn.lpstrInitialDir = DxSimpleMeshMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, IDC_EDIT_FIELDFILE, dlg.GetFileName().GetString() );
	}
}



void CItemParent::OnBnClickedButtonSkinEffectfile()
{
	CString szFilter = "Effect File (*.effskin_a)|*.effskin_a|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".effskin_a",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_SKIN_EFFECTFILE )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}

void CItemParent::OnBnClickedButtonEffectfile()
{
	CString szFilter = "Effect File (*.egp)|*.egp|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_EFFECTFILE )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}

void CItemParent::OnBnClickedButtonSkinTarEffectfile()
{
	CString szFilter = "Effect File (*.effskin_a)|*.effskin_a|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".effskin_a",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = DxEffcharDataMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_TAR_EFFECTFILE )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}

void CItemParent::OnEnChangeEditItemname()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	const char * szTemp =  GLStringTable::GetInstance().GetString( GetWin_Text( this, IDC_EDIT_ITEMNAME ).GetString(),
																	GLStringTable::ITEM );
	if( szTemp )
	{
		SetWin_Text ( this, IDC_EDIT_NAME, szTemp );	
	}
	else
	{
		SetWin_Text ( this, IDC_EDIT_NAME, "" );
	}
}

void CItemParent::OnEnChangeEditItemdesc()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	const char * szTemp = GLStringTable::GetInstance().GetString( GetWin_Text( this, IDC_EDIT_ITEMDESC ).GetString(), 
																	GLStringTable::ITEM );
	if( szTemp )
	{
		SetWin_Text ( this, IDC_EDIT_COMMENT, szTemp );	
	}
	else
	{
		SetWin_Text ( this, IDC_EDIT_COMMENT, "" );
	}
}


void CItemParent::OnCbnSelchangeComboItemType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_pDummyItem->sBasicOp.emItemType = (EMITEM_TYPE) GetWin_Combo_Sel ( this, IDC_COMBO_ITEM_TYPE );

	//if ( m_pDummyItem->sBasicOp.emItemType != ITEM_PET_CARD ) 
	//{
	//	GetDlgItem ( IDC_COMBO_PET_TYPE )->EnableWindow ( FALSE );
	//	GetDlgItem ( IDC_EDIT_PET_MID )->EnableWindow ( FALSE );
	//	GetDlgItem ( IDC_EDIT_PET_SID )->EnableWindow ( FALSE );
	//}
	//else
	//{
	//	//GetDlgItem ( IDC_COMBO_PET_TYPE )->EnableWindow ();
	//	GetDlgItem ( IDC_EDIT_PET_MID )->EnableWindow ();
	//	GetDlgItem ( IDC_EDIT_PET_SID )->EnableWindow ();
	//}
}

void CItemParent::LoadWearingfile( int nID )
{
	// geger009 | ABL and VCF not impletemented yet, turn it off for now
	//CString szFilter = "Wearing File (*.cps; *.abl; *.vcf)|*.cps; *.abl; *.vcf|";
	CString szFilter = "Wearing File (*.cps)|*.cps|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemParent*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
	}
}

void CItemParent::OnBnClickedButtonWearingfileC1()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C1 );
}

void CItemParent::OnBnClickedButtonWearingfileC2()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C2 );
}

void CItemParent::OnBnClickedButtonWearingfileC3()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C3 );
}

void CItemParent::OnBnClickedButtonWearingfileC4()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C4 );
}

void CItemParent::OnBnClickedButtonWearingfileC5()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C5 );
}

void CItemParent::OnBnClickedButtonWearingfileC6()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C6 );
}

void CItemParent::OnBnClickedButtonWearingfileC7()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C7 );
}

void CItemParent::OnBnClickedButtonWearingfileC8()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C8 );
}


void CItemParent::OnBnClickedButtonWearingfileC9()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C9 );
}


void CItemParent::OnBnClickedButtonWearingfileC10()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C10 );
}

void CItemParent::OnBnClickedButtonWearingfileC11()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C11 );
}

void CItemParent::OnBnClickedButtonWearingfileC12()
{
	// TODO: Add your control notification handler code here
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C12 );
}

void CItemParent::OnBnClickedButtonWearingfileC13()
{
	// TODO: Add your control notification handler code here
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C13 );
}

void CItemParent::OnBnClickedButtonWearingfileC14()
{
	// TODO: Add your control notification handler code here
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C14 );
}

void CItemParent::OnBnClickedButtonWearingfileC15()
{
	// TODO: Add your control notification handler code here
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C15 );
}

void CItemParent::LoadABLFile( int nID )
{
	// geger009 | Add ABL code (not implemented yet, turn it off for now)
	return;
	/*CString szFilter = "ABL File (*.abl)|*.abl|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".abl",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemParent*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceRootDataContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
	}*/
}

void CItemParent::SetReadMode ()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

	const int nSkipNum = 6;
	const int nSkipID[nSkipNum] = { IDC_EDIT_NAME, IDC_EDIT_COMMENT, IDC_BUTTON_PREV,
									IDC_BUTTON_NEXT, IDC_BUTTON_CANCEL, IDC_BUTTON_OK };

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
void CItemParent::OnEnChangeEditSpecid()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CItemParent::OnBnClickedButtonAblMLeft()
{
	// TODO: Add your control notification handler code here
	// geger009 | Add ABL code (not implemented yet, turn it off for now)
	return;
	//LoadABLFile ( IDC_EDIT_ABL_M_LEFT );
}

void CItemParent::OnBnClickedButtonAblMRight()
{
	// TODO: Add your control notification handler code here
	// geger009 | Add ABL code (not implemented yet, turn it off for now)
	return;
	//LoadABLFile ( IDC_EDIT_ABL_M_RIGHT );
}

void CItemParent::OnBnClickedButtonAblWLeft()
{
	// TODO: Add your control notification handler code here
	// geger009 | Add ABL code (not implemented yet, turn it off for now)
	return;
	//LoadABLFile ( IDC_EDIT_ABL_W_LEFT );
}

void CItemParent::OnBnClickedButtonAblWRight()
{
	// TODO: Add your control notification handler code here
	// geger009 | Add ABL code (not implemented yet, turn it off for now)
	return;
	//LoadABLFile ( IDC_EDIT_ABL_W_RIGHT );
}

void CItemParent::LoadVPSFile ( int nID )
{
	// geger009 | Add VPS code (not implemented yet, turn it off for now)
	return; 
	/*CString szFilter = "VPS File (*.vps)|*.vps|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".vps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemParent*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
	}*/
}

void CItemParent::OnBnClickedButtonVehiclePart1()
{
	// TODO: Add your control notification handler code here
	// geger009 | Add VPS code (not implemented yet, turn it off for now)
	return;
	//LoadVPSFile ( IDC_EDIT_VEHICLE_PART1 );
}

void CItemParent::OnBnClickedButtonVehiclePart2()
{
	// TODO: Add your control notification handler code here
	// geger009 | Add VPS code (not implemented yet, turn it off for now)
	return;
	//LoadVPSFile ( IDC_EDIT_VEHICLE_PART2 );
}

void CItemParent::OnBnClickedButtonVehiclePart3()
{
	// TODO: Add your control notification handler code here
	// geger009 | Add VPS code (not implemented yet, turn it off for now)
	return;
	//LoadVPSFile ( IDC_EDIT_VEHICLE_PART3 );
}

void CItemParent::OnBnClickedButtonVehiclePart4()
{
	// TODO: Add your control notification handler code here
	// geger009 | Add VPS code (not implemented yet, turn it off for now)
	return;
	//LoadVPSFile ( IDC_EDIT_VEHICLE_PART4 );
}

void CItemParent::OnBnClickedButtonVehiclePart5()
{
	// TODO: Add your control notification handler code here
	// geger009 | Add VPS code (not implemented yet, turn it off for now)
	return;
	//LoadVPSFile ( IDC_EDIT_VEHICLE_PART5 );
}

void CItemParent::OnCbnSelchangeComboVehicleType()
{
	// TODO: Add your control notification handler code here
	// geger009 | Add VPS code (not implemented yet, turn it off for now)
	return;
	/*int nType = GetWin_Combo_Sel ( this, IDC_COMBO_VEHICLE_TYPE );

	if ( nType == VEHICLE_TYPE_BOARD )
	{
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART1, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART2, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART3, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART4, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART5, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART1, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART2, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART3, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART4, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART5, SW_HIDE );
	}
	else if ( nType == VEHICLE_TYPE_SCUTER )
	{
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART1, SW_SHOW );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART2, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART3, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART4, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART5, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART1, SW_SHOW );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART2, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART3, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART4, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART5, SW_HIDE );
	}
	else if ( nType == VEHICLE_TYPE_BIKE_BT5 )
	{
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART1, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART2, SW_SHOW );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART3, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART4, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART5, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART1, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART2, SW_SHOW );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART3, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART4, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART5, SW_HIDE );
	}
	else if ( nType == VEHICLE_TYPE_BIKE_BT7 )
	{
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART1, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART2, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART3, SW_SHOW );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART4, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART5, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART1, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART2, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART3, SW_SHOW );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART4, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART5, SW_HIDE );
	}
	else if ( nType == VEHICLE_TYPE_BIKE_BT9 )
	{
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART1, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART2, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART3, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART4, SW_SHOW );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART5, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART1, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART2, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART3, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART4, SW_SHOW );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART5, SW_HIDE );
	}
	else if ( nType == VEHICLE_TYPE_BIKE_RARE )
	{
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART1, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART2, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART3, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART4, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_BUTTON_VEHICLE_PART5, SW_SHOW );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART1, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART2, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART3, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART4, SW_HIDE );
		SetWin_ShowWindow ( this, IDC_EDIT_VEHICLE_PART5, SW_SHOW );
	}*/
}

void CItemParent::OnBnClickedRadioAll()
{
	// TODO: Add your control notification handler code here
	BOOL bCheck = GetWin_Check ( this, IDC_RADIO_ALL );

	SetWin_Check ( this, IDC_CHECK_FIGHTER_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARMS_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARCHER_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_W, bCheck );

	SetWin_Check ( this, IDC_CHECK_FIGHTER_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARMS_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARCHER_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_M, bCheck );

	SetWin_Check ( this, IDC_CHECK_EXTREME_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_EXTREME_W, bCheck );

	// geger009 | gunner and assasin not implemented yet, turn it off for now
	/*SetWin_Check ( this, IDC_CHECK_SCIENCE_M, bCheck ); 
	SetWin_Check ( this, IDC_CHECK_SCIENCE_W, bCheck ); 

	SetWin_Check ( this, IDC_CHECK_ASSASSIN_M, bCheck ); 
	SetWin_Check ( this, IDC_CHECK_ASSASSIN_W, bCheck );*/
}

void CItemParent::OnBnClickedRadioMan()
{
	// TODO: Add your control notification handler code here
	BOOL bCheck = GetWin_Check ( this, IDC_RADIO_MAN );

	SetWin_Check ( this, IDC_CHECK_FIGHTER_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARMS_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARCHER_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_M, bCheck );
	SetWin_Check ( this, IDC_CHECK_EXTREME_M, bCheck );
	// geger009 | gunner and assasin not implemented yet, turn it off for now
	//SetWin_Check ( this, IDC_CHECK_SCIENCE_M, bCheck ); 
	//SetWin_Check ( this, IDC_CHECK_ASSASSIN_M, bCheck ); 

	SetWin_Check ( this, IDC_CHECK_FIGHTER_W, false );
	SetWin_Check ( this, IDC_CHECK_ARMS_W, false );
	SetWin_Check ( this, IDC_CHECK_ARCHER_W, false );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_W, false );
	SetWin_Check ( this, IDC_CHECK_EXTREME_W, false );
	// geger009 | gunner and assasin not implemented yet, turn it off for now
	//SetWin_Check ( this, IDC_CHECK_SCIENCE_W, false ); 
	//SetWin_Check ( this, IDC_CHECK_ASSASSIN_W, false ); 
}

void CItemParent::OnBnClickedRadioWoman()
{
	// TODO: Add your control notification handler code here
	BOOL bCheck = GetWin_Check ( this, IDC_RADIO_WOMAN );

	SetWin_Check ( this, IDC_CHECK_FIGHTER_M, false );
	SetWin_Check ( this, IDC_CHECK_ARMS_M, false );
	SetWin_Check ( this, IDC_CHECK_ARCHER_M, false );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_M, false );
	SetWin_Check ( this, IDC_CHECK_EXTREME_M, false );
	// geger009 | gunner and assasin not implemented yet, turn it off for now
	//SetWin_Check ( this, IDC_CHECK_SCIENCE_M, false ); 
	//SetWin_Check ( this, IDC_CHECK_ASSASSIN_M, false ); 

	SetWin_Check ( this, IDC_CHECK_FIGHTER_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARMS_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_ARCHER_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_SPIRIT_W, bCheck );
	SetWin_Check ( this, IDC_CHECK_EXTREME_W, bCheck );
	// geger009 | gunner and assasin not implemented yet, turn it off for now
	//SetWin_Check ( this, IDC_CHECK_SCIENCE_W, bCheck ); 
	//SetWin_Check ( this, IDC_CHECK_ASSASSIN_W, bCheck ); 
}
