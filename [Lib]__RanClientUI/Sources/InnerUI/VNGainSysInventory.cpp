#include "pch.h"

#include "VNGainSysInventory.h"
#include "VNGainSysPage.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "GLGaeaClient.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "InnerInterface.h"
#include "ModalCallerID.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "BasicLineBox.h"
#include "ModalWindow.h"
#include "UITextControl.h"
#include "GameTextControl.h"
#include "BasicTextButton.h"
#include "d3dfont.h"
#include "DxGlobalStage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CVNGainSysInventory::CVNGainSysInventory () 
		: m_pPage ( NULL )
		, m_pMoneyTextBox( NULL )
		, m_pExpNumberBox( NULL )
		, m_pItemNumberBox( NULL )
		, m_pItemResetButton( NULL )
{
}

CVNGainSysInventory::~CVNGainSysInventory ()
{
}

CBasicTextBox*	CVNGainSysInventory::CreateTextBox ( char* szKeyword, CD3DFontPar* pFont, int nAlign )
{
	CBasicTextBox* pTextBox = new CBasicTextBox;
	pTextBox->CreateSub ( this, szKeyword );
	pTextBox->SetFont ( pFont );
	pTextBox->SetTextAlign ( nAlign );	
	RegisterControl ( pTextBox );

	return pTextBox;
}



void CVNGainSysInventory::CreateSubControl ()
{	
	CD3DFontPar* pFont8Shadow = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	
	const DWORD& dwSilver = NS_UITEXTCOLOR::SILVER;
	const DWORD& dwSubTitle = NS_UITEXTCOLOR::PALEGOLDENROD;

	CVNGainInvenPage* pVNGainSysPage = new CVNGainInvenPage;
	pVNGainSysPage->CreateSub ( this, "VNGAININVEN_PAGE", UI_FLAG_DEFAULT, VNGAININVEN_PAGE );
	pVNGainSysPage->CreateSubControl ();	
	RegisterControl ( pVNGainSysPage );
	m_pPage = pVNGainSysPage;


	CBasicTextBox* pTextBox = NULL;	

	{	//	뒷 배경
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "VNGAINSYSTEM_LINEBOX" );
		RegisterControl ( pBasicLineBox );		
	}

	//// 돈에 관한 것 추가

	CBasicButton* pButton = NULL;
	pButton = CreateFlipButton ( "VNGAINSYSINVENTORY_MONEY_BUTTON", "VNGAINSYSINVENTORY_MONEY_BUTTON_F", VNGAINSYSINVENTORY_MONEY_BUTTON, 
		CBasicButton::CLICK_FLIP );
	pButton->CreateMouseOver ( "VNGAINSYSINVENTORY_MONEY_BUTTON_F" );	
	pButton->SetAlignFlag ( UI_FLAG_BOTTOM );
	pButton->SetUseDynamic ( TRUE );

	CD3DFontPar* pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
	m_pMoneyTextBox = CreateStaticControl ( "VNGAINSYSINVENTORY_MONEY_TEXT", pFont8Shadow, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
	m_pMoneyTextBox->SetAlignFlag ( UI_FLAG_BOTTOM );

	// 경험치 EXP
	pTextBox = CreateTextBox ( "VNGAINSYSTEM_EXP_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextBox->AddText ( ID2GAMEWORD("VNGAINSYSTEM_EXP_STATIC",0) );
	
	m_pExpNumberBox = CreateTextBox ( "VNGAINSYSTEM_EXPNUMBER_TEXTBOX", pFont8Shadow, TEXT_ALIGN_LEFT );

	//// 아이템 등록 갯수
	pTextBox = CreateTextBox ( "VNGAINSYSTEM_ITEMCNT_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextBox->AddText ( ID2GAMEWORD("VNGAINSYSTEM_ITEMCNT_STATIC",0) );

	m_pItemNumberBox = CreateTextBox ( "VNGAINSYSTEM_ITEMCNT_TEXTBOX", pFont8Shadow, TEXT_ALIGN_LEFT );

	m_pItemResetButton = CreateTextButton ( "VNGAINSYSTEM_ITEMRESET_BUTTON", VNGAINSYS_ITEMRESET_BUTTON, (char*)ID2GAMEWORD ( "VNGAINSYSTEM_ITEMRESET_BUTTON", 0 ) );
	m_pItemResetButton->SetFlip( FALSE );


}

CBasicTextButton*  CVNGainSysInventory::CreateTextButton ( char* szButton, UIGUID ControlID, char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pTextButton = new CBasicTextButton;
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pTextButton );

	return pTextButton;
}


void CVNGainSysInventory::InitVNGainSys ()
{
	m_pPage->UnLoadItemPage ();

	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();
	m_pPage->LoadItemPage ( pCharacter->m_cInvenCharged );
}

void CVNGainSysInventory::SetMoney ( LONGLONG Money )
{
	//	돈자리수
	CString strTemp = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT ( Money, 3, "," );
	m_pMoneyTextBox->SetOneLineText ( strTemp );
}

void CVNGainSysInventory::SetExp( LONGLONG Exp )
{
	int nIndex = 0;

	if ( m_pExpNumberBox ) m_pExpNumberBox->ClearText ();

	CString strCombine;
	strCombine.Format ( "%I64d", Exp );
	if ( m_pExpNumberBox ) nIndex = m_pExpNumberBox->AddTextNoSplit ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CVNGainSysInventory::SetItemCnt( int ItemCnt )
{
	int nIndex = 0;

	if ( m_pItemNumberBox ) m_pItemNumberBox->ClearText ();

	CString strCombine;
	strCombine.Format ( "%d", ItemCnt );
	if ( m_pItemNumberBox ) nIndex = m_pItemNumberBox->AddTextNoSplit ( strCombine, NS_UITEXTCOLOR::WHITE );
}

void CVNGainSysInventory::ClearVNGainSys()
{ 
	m_pPage->UnLoadItemPage ();
}

void CVNGainSysInventory::GetSplitPos ( WORD* pwPosX, WORD* pwPosY )
{
	*pwPosX = m_wSplitItemPosX;
	*pwPosY = m_wSplitItemPosY;
}

void CVNGainSysInventory::SetSplitPos ( WORD wPosX, WORD wPosY )
{
	m_wSplitItemPosX = wPosX;
	m_wSplitItemPosY = wPosY;
}


void CVNGainSysInventory::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	

	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	GLCHARLOGIC& sCharData = GLGaeaClient::GetInstance().GetCharacterLogic ();	
	SetExp( sCharData.m_lVNGainSysExp );
	SetMoney( sCharData.m_lVNGainSysMoney );
	SetItemCnt( sCharData.m_dwVietnamInvenCount );

	GLInventory& ref_Inventory = sCharData.m_cVietnamInventory;	
	m_pPage->LoadItemPage ( ref_Inventory );
	

}


void CVNGainSysInventory::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case VNGAINSYS_ITEMRESET_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
		{
			//	삭제 버튼 누름
			//
			GLGaeaClient::GetInstance().GetCharacter()->ReqVNInveReset();
		}
		break;

	case VNGAININVEN_PAGE:
		{
			

			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nPosX, nPosY;
				m_pPage->GetItemIndex ( &nPosX, &nPosY );

				//CDebugSet::ToView ( 1, 1, "인벤 : %d %d", nPosX, nPosY );

				if ( nPosX < 0 || nPosY < 0 ) return;

				BOOL bMarketOpen = CInnerInterface::GetInstance().IsMarketWindowOpen ();
				//				if ( CUIMan::GetFocusControl () == this )
				{
					SINVENITEM sInvenItem = m_pPage->GetItem ( nPosX, nPosY );
					if ( sInvenItem.sItemCustom.sNativeID != NATIVEID_NULL () )
					{						
						CInnerInterface::GetInstance().SHOW_ITEM_INFO ( sInvenItem.sItemCustom, bMarketOpen, FALSE, FALSE, sInvenItem.wPosX, sInvenItem.wPosY );
					}
				}

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( m_bSplitItem )
					{
						m_bSplitItem = FALSE;

						WORD wSplitPosX, wSplitPosY;
						GetSplitPos ( &wSplitPosX, &wSplitPosY );
						BOOL bDiffPos = !( nPosX == wSplitPosX && nPosY == wSplitPosY );
						BOOL bSplitable = GLGaeaClient::GetInstance().GetCharacter()->IsInvenSplitItem ( wSplitPosX, wSplitPosY, TRUE );
						/*if ( bDiffPos && bSplitable )
						{
							DoModal ( ID2GAMEINTEXT ( "SPLIT_ITEM" ), MODAL_QUESTION, EDITBOX_NUMBER, MODAL_SPLIT_ITEM );
						}
						else*/
						{
							GLGaeaClient::GetInstance().GetCharacter()->ReqVNInvenTo ( nPosX, nPosY );
						}
					}
					return ;
				}

				if ( dwMsg & UIMSG_RB_UP )
				{
					GLGaeaClient::GetInstance().GetCharacter()->ReqVietemInvenTo ( nPosX, nPosY );
					return ;
				}

				if ( dwMsg & UIMSG_LB_DOWN )
				{
					SetSplitPos ( nPosX, nPosY );
					m_bSplitItem = TRUE;
					return ;
				}
			}
		}
		break;
	}

	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );
}