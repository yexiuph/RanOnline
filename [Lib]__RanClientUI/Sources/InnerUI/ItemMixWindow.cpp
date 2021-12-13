#include "pch.h"
#include "ItemMixWindow.h"


#include "ItemImage.h"
#include "BasicTextButton.h"
#include "GLItemMan.h"
#include "GameTextControl.h"
#include "D3DFont.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "InnerInterface.h"
#include "GLGaeaClient.h"
#include "ItemMove.h"
#include "ModalWindow.h"
#include "ModalCallerID.h"
#include "UITextControl.h"
#include "../[Lib]__EngineUI/Sources/BasicAnimationBox.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const float fRusltTime = 3.0f;

//--------------------------------------------------------------------
// CItemGarbage
//--------------------------------------------------------------------
CItemMixWindow::CItemMixWindow()	// 휴지통
	: m_pOkButton(NULL)	
	, m_pAniBox(NULL)
	, m_pBackImage(NULL)
	, m_pSuccessImage( NULL ) 
	, m_pFailImage ( NULL ) 
	, m_pMsgBox ( NULL ) 
	, m_bOK(false)
	, m_dwNpcID(UINT_MAX)
	, m_bResult(false)
	, m_fTime( 0.0f )
{
	memset( m_pMeterialItemImage, NULL, sizeof( CItemImage* ) * ITEMMIX_ITEMNUM );
	memset( m_pMeterialItemNum, NULL, sizeof( CBasicTextBox* ) * ITEMMIX_ITEMNUM );
	memset( m_pMeterialBackImage, NULL, sizeof( CUIControl* ) * ITEMMIX_ITEMNUM );
}

CItemMixWindow::~CItemMixWindow()
{
}

void CItemMixWindow::Reset()
{
	m_bOK = false;
	m_pAniBox->Reset();
	m_pAniBox->SetVisibleSingle( FALSE );

	m_bResult = false;
	m_fTime = 0.0f;
	m_pSuccessImage->SetVisibleSingle( FALSE );
	m_pFailImage->SetVisibleSingle( FALSE );
	m_pMsgBox->SetText( m_strBasicMsg, NS_UITEXTCOLOR::SILVER );
}

CBasicTextBox* CItemMixWindow::CreateTextBox ( const char* szTextBox )
{
	CD3DFontPar* pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

	CBasicTextBox* pTextBox = new CBasicTextBox;
	pTextBox->CreateSub ( this, szTextBox );
    pTextBox->SetFont ( pFont8 );	
	RegisterControl ( pTextBox );
	return pTextBox;
}

VOID CItemMixWindow::CreateSubControl()
{

	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, "ITEM_MIX_BACK", UI_FLAG_DEFAULT );	
	RegisterControl ( pControl );

	{
		for( int i = 0; i < ITEMMIX_ITEMNUM; ++i )
		{
			CString strTemp;
			strTemp.Format("ITEM_MIX_SUB_SLOT%d", i );

			m_pMeterialItemImage[i] = new CItemImage;
			m_pMeterialItemImage[i]->CreateSub( this, strTemp, UI_FLAG_DEFAULT, ITEM_MIX_SUB_SLOT0+i );
			m_pMeterialItemImage[i]->CreateSubControl();
			RegisterControl( m_pMeterialItemImage[i] );


			strTemp.Format("ITEM_MIX_SUB_SLOT%d_AMOUNT", i );
			m_pMeterialItemNum[i] = CreateTextBox ( strTemp );	
			m_pMeterialItemNum[i]->SetTextAlign ( TEXT_ALIGN_RIGHT );

			strTemp.Format("ITEM_MIX_SUB_SLOT%d_BACK", i );
			m_pMeterialBackImage[i] = new CUIControl;
			m_pMeterialBackImage[i]->CreateSub ( this, strTemp, UI_FLAG_DEFAULT );	
			RegisterControl ( m_pMeterialBackImage[i] );			
		}
	}

	m_pBackImage = new CUIControl;
	m_pBackImage->CreateSub ( this, "ITEM_MIX_BACK_IMAGE", UI_FLAG_DEFAULT );	
	RegisterControl ( m_pBackImage );

	{
		CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 9 );
		m_pMsgBox = CreateStaticControl( "ITEM_MIX_INFO_TEXT_STATIC", pFont, NS_UITEXTCOLOR::SILVER, TEXT_ALIGN_CENTER_X );

		m_strBasicMsg = ID2GAMEINTEXT( "ITEM_MIX_INFO" );
		m_pMsgBox->AddText( m_strBasicMsg, NS_UITEXTCOLOR::SILVER );
	}

	// 승인, 거절
	{
		m_pOkButton = new CBasicTextButton;
		m_pOkButton->CreateSub( this, "BASIC_TEXT_BUTTON16", UI_FLAG_DEFAULT, ITEM_MIX_OK_BUTTON );
		m_pOkButton->CreateBaseButton( "ITEM_MIX_OK_BUTTON", CBasicTextButton::SIZE16,
									CBasicButton::CLICK_FLIP, ID2GAMEWORD( "ITEM_MIX_BUTTON", 0 ) );
		m_pOkButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		RegisterControl( m_pOkButton );

		CBasicTextButton * pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON16", UI_FLAG_DEFAULT, ITEM_MIX_CANCEL_BUTTON );
		pButton->CreateBaseButton( "ITEM_MIX_CANCEL_BUTTON", CBasicTextButton::SIZE16,
									CBasicButton::CLICK_FLIP, ID2GAMEWORD( "ITEM_MIX_BUTTON", 1 ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
		RegisterControl( pButton );
	}

	{
		CBasicAnimationBox* pAnibox = new CBasicAnimationBox;
		pAnibox->CreateSubEx(this,"ITEM_MIX_IMAGE");
		pAnibox->CreateAni("ITEM_MIX_ANIMATION_SHUFFLE0", CBasicAnimationBox::NORMAL_ANI);
		pAnibox->AddAni("ITEM_MIX_ANIMATION_SHUFFLE1");
		pAnibox->AddAni("ITEM_MIX_ANIMATION_SHUFFLE2");
		pAnibox->AddAni("ITEM_MIX_ANIMATION_SHUFFLE3");
		pAnibox->SetAniTime(0.1f);
		pAnibox->SetEndTime(2.0f);
		pAnibox->SetLoop(TRUE);
		RegisterControl( pAnibox );
		m_pAniBox = pAnibox;
	}

	m_pSuccessImage = new CUIControl;
	m_pSuccessImage->CreateSub ( this, "ITEM_MIX_SUCCESS_IMAGE", UI_FLAG_DEFAULT );	
	m_pSuccessImage->SetTransparentOption( TRUE );
	RegisterControl ( m_pSuccessImage );
	
	
	m_pFailImage = new CUIControl;
	m_pFailImage->CreateSub ( this, "ITEM_MIX_FAIL_IMAGE", UI_FLAG_DEFAULT );	
	m_pFailImage->SetTransparentOption( TRUE );
	RegisterControl ( m_pFailImage );
}

void CItemMixWindow::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );

	Reset();
}

VOID CItemMixWindow::Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	for ( int i = 0; i < ITEMMIX_ITEMNUM; ++i )
		if ( !m_pMeterialItemImage[i] ) return;

	CString strNum;

	for ( int i = 0; i < ITEMMIX_ITEMNUM; ++i )
	{
		const SITEMCUSTOM sItemMix = GLGaeaClient::GetInstance().GetCharacter()->GET_ITEM_MIX(i);

		if( sItemMix.sNativeID == NATIVEID_NULL() )
		{
			m_pMeterialItemImage[i]->ResetItem();
			m_pMeterialItemNum[i]->ClearText();
			m_pMeterialBackImage[i]->SetVisibleSingle( FALSE );
		}
		else
		{
			SITEM* pItem = GLItemMan::GetInstance().GetItem( sItemMix.sNativeID );
			if( pItem )
			{
				strNum.Format( "%d", sItemMix.wTurnNum );
				m_pMeterialItemImage[i]->SetItem( pItem->sBasicOp.sICONID, pItem->GetInventoryFile() );
				m_pMeterialItemNum[i]->SetText( strNum, NS_UITEXTCOLOR::WHITE );
				m_pMeterialBackImage[i]->SetVisibleSingle( TRUE );
			}
			else
			{
				m_pMeterialItemImage[i]->ResetItem();
				m_pMeterialItemNum[i]->ClearText();
				m_pMeterialBackImage[i]->SetVisibleSingle( FALSE );
			}
		}
	}

	if ( m_bOK && m_pAniBox->GetEndAnimate() )
	{
		m_pAniBox->SetVisibleSingle( FALSE );
		m_pBackImage->SetVisibleSingle( TRUE );
		GLGaeaClient::GetInstance().GetCharacter()->ReqItemMix( m_dwNpcID );
		m_bOK = false;
	}

	if ( m_bResult ) 
	{
		m_fTime -= fElapsedTime;

		if ( m_fTime < 0.0f )
		{
			m_bResult = false;
			m_fTime = 0.0f;

			m_pSuccessImage->SetFadeTime( 0.2f );
			m_pSuccessImage->SetFadeOut();
			m_pFailImage->SetFadeTime( 0.2f );
			m_pFailImage->SetFadeOut();

			m_pMsgBox->SetText( m_strBasicMsg, NS_UITEXTCOLOR::SILVER );
		}

	}
}

VOID CItemMixWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch( ControlID )
	{
	case ITEM_MIX_SUB_SLOT0:
	case ITEM_MIX_SUB_SLOT1:
	case ITEM_MIX_SUB_SLOT2:
	case ITEM_MIX_SUB_SLOT3:
	case ITEM_MIX_SUB_SLOT4:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{
				int nIndex = ControlID - ITEM_MIX_SUB_SLOT0;

				CItemMove* pItemMove = CInnerInterface::GetInstance().GetItemMove();
				if( !pItemMove )
				{
					GASSERT( 0 && "CItemMixWindow::TranslateUIMessage, pItemMove == NULL" );
					break;
				}

				SNATIVEID sItem = pItemMove->GetItem();
				if( sItem != NATIVEID_NULL() )
				{
					const UIRECT& rcSlotPos = m_pMeterialItemImage[nIndex]->GetGlobalPos();
					pItemMove->SetGlobalPos( rcSlotPos );
					CInnerInterface::GetInstance().SetSnapItem();
				}

				if ( !m_bOK && !m_bResult )
				{
					if( dwMsg & UIMSG_LB_UP )
					{
						GLGaeaClient::GetInstance().GetCharacter()->SetItemMixMoveItem( nIndex );
					}

					if( dwMsg & UIMSG_RB_UP )
					{
						GLGaeaClient::GetInstance().GetCharacter()->ReSetItemMixItem( nIndex );
					}
				}

				SITEMCUSTOM sItemCustom = GLGaeaClient::GetInstance().GetCharacter()->GET_ITEM_MIX(nIndex);
				if( sItemCustom.sNativeID != NATIVEID_NULL() )
					CInnerInterface::GetInstance().SHOW_ITEM_INFO( sItemCustom, FALSE, FALSE, FALSE, 0, 0 );
			}
		}
		break;

	case ITEM_MIX_OK_BUTTON:
		if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
		{
			if ( !m_bOK )
			{
				DoItemMix();
			}
			SetMessageEx( dwMsg &= ~UIMSG_KEY_FOCUSED ); // Focus 문제로 메세지 삭제
		}
		break;

	case ITEM_MIX_CANCEL_BUTTON:
		if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			CInnerInterface::GetInstance().CloseItemMixWindow();
		break;

	case ET_CONTROL_BUTTON:
		if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			CInnerInterface::GetInstance().CloseItemMixWindow();
		break;
	}

	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}

void CItemMixWindow::SetAnimation()
{

	m_bResult = false;
	m_fTime = 0.0f;
	m_pSuccessImage->SetVisibleSingle( FALSE );
	m_pFailImage->SetVisibleSingle( FALSE );
	m_pMsgBox->SetText( m_strBasicMsg, NS_UITEXTCOLOR::SILVER );


	m_bOK = true;
	m_pAniBox->InitAnimateFrame();
	m_pAniBox->SetPause(FALSE);
	m_pAniBox->SetVisibleSingle( TRUE );
	m_pBackImage->SetVisibleSingle( FALSE );
}

VOID CItemMixWindow::DoItemMix()
{

	DWORD dwPrice = GLGaeaClient::GetInstance().GetCharacter()->GetItemMixMoney();

	if ( dwPrice == UINT_MAX ) 
	{
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMITEM_MIX_FB_NOMIX") );
		SetItemMixResult( ID2GAMEINTEXT("EMITEM_MIX_FB_NOMIX") );
		return;
	}

	if ( dwPrice > 0 )
	{
		CString strTemp;
		strTemp.Format( ID2GAMEINTEXT("ITEM_MIX_MONEY_OKQUESTION"), dwPrice );
		DoModal( strTemp, MODAL_QUESTION, YESNO, MODAL_ITEMMIX_OKQUESTION );
	}
	else
	{
		DoModal( ID2GAMEINTEXT("ITEM_MIX_OKQUESTION"), MODAL_QUESTION, YESNO, MODAL_ITEMMIX_OKQUESTION );
	}
}

void CItemMixWindow::SetItemMixResult( CString strMsg, bool bSuccess, bool bFail )
{
	DWORD dwColor = NS_UITEXTCOLOR::SILVER;
		
	if ( bSuccess ) 
	{
		m_pSuccessImage->SetFadeTime( 0.2f );
		m_pSuccessImage->SetFadeIn();
		dwColor = NS_UITEXTCOLOR::PALEGREEN;
	}

	if ( bFail ) 
	{
		m_pFailImage->SetFadeTime( 0.2f );
		m_pFailImage->SetFadeIn();
		dwColor = NS_UITEXTCOLOR::DISABLE;
	}

	m_strResultMsg = strMsg;
	m_pMsgBox->SetText( m_strResultMsg, dwColor );

	m_bResult = true;
	m_fTime = fRusltTime;
}