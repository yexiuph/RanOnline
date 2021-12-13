#include "pch.h"
#include "d3dfont.h"
#include "ItemImage.h"
#include "InnerInterface.h"
#include "BasicComboBox.h"
#include "BasicTextButton.h"
#include "GLItemMan.h"
#include "UITextControl.h"
#include "GLGaeaClient.h"

#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"

#include ".\summonposiondisplay.h"

const D3DCOLOR CSummonPosionDisplay::dwDEFAULT_TRANSPARENCY = D3DCOLOR_ARGB ( 160, 255, 255, 255 );;
const D3DCOLOR CSummonPosionDisplay::dwFULL_TRANSPARENCY = D3DCOLOR_ARGB ( 255, 255, 255, 255 );;

CSummonPosionDisplay::CSummonPosionDisplay(void)
			:	m_pPosionImage(NULL)
			,	m_pAmountText(NULL)
{
}

CSummonPosionDisplay::~CSummonPosionDisplay(void)
{
}

void CSummonPosionDisplay::CreateSubControl()
{
	m_pPosionImage = CreateItemImage( "SUMMON_POSION_IMAGE", SUMMON_POSION_IMAGE );

	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
	m_pAmountText = CreateStaticControl ( "SUMMON_POTION_IMAGE_AMOUNT_SINGLE", pFont, TEXT_ALIGN_LEFT );
}

CBasicTextBox* CSummonPosionDisplay::CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

CItemImage*	CSummonPosionDisplay::CreateItemImage ( const char* szControl, UIGUID ControlID )
{
	CItemImage* pItemImage = new CItemImage;
	pItemImage->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pItemImage->CreateSubControl ();
	RegisterControl ( pItemImage );

	return pItemImage;
}


void CSummonPosionDisplay::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		if( !GLGaeaClient::GetInstance().GetSummonClient() ) return;
		GLSummonClient* pSummonData = GLGaeaClient::GetInstance().GetSummonClient();
		if( pSummonData && pSummonData->IsVALID() )
		{
		}else{
			CInnerInterface::GetInstance().HideGroup( SUMMON_POSION_DISPLAY );
		}
	}else
	{
		CInnerInterface::GetInstance().HideGroup( SUMMON_POSION_DISPLAY );
	}
}

void CSummonPosionDisplay::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case SUMMON_POSION_IMAGE:
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			GLSummonClient* pSummonData = GLGaeaClient::GetInstance().GetSummonClient();
			if( pSummonData && pSummonData->m_sPosionID != NATIVEID_NULL() )
			{
				SITEMCUSTOM sItemCustom = SITEMCUSTOM( pSummonData->m_sPosionID.dwID );
				CInnerInterface::GetInstance().SHOW_ITEM_INFO ( sItemCustom, FALSE, FALSE, FALSE, USHRT_MAX, USHRT_MAX );
			}
		}
		break;
	}
}

void CSummonPosionDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{

	GLSummonClient* pSummonData = GLGaeaClient::GetInstance().GetSummonClient();
	if( pSummonData && pSummonData->IsVALID() )
	{
		if( pSummonData->m_sPosionID != NATIVEID_NULL() )
		{
			GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter ();
			DWORD dwAmount = pCharacter->m_cInventory.CountTurnItem( pSummonData->m_sPosionID );

			CString strNumber;
			strNumber.Format ( "%d", dwAmount );
			if ( m_pAmountText ) m_pAmountText->SetOneLineText ( strNumber, NS_UITEXTCOLOR::WHITE );

			SITEM* pItemData = GLItemMan::GetInstance().GetItem ( pSummonData->m_sPosionID );
			m_pPosionImage->SetItem ( pItemData->sBasicOp.sICONID, pItemData->GetInventoryFile(), pItemData->sBasicOp.sNativeID );

			m_pPosionImage->SetVisibleSingle( TRUE );
			m_pAmountText->SetVisibleSingle( TRUE );

			if ( dwAmount <= 0 )	m_pPosionImage->SetDiffuse ( dwDEFAULT_TRANSPARENCY );
			else					m_pPosionImage->SetDiffuse ( dwFULL_TRANSPARENCY );
		}else{
			m_pPosionImage->SetVisibleSingle( FALSE );
			m_pAmountText->SetVisibleSingle( FALSE );
		}
	}else{
		CInnerInterface::GetInstance().HideGroup( SUMMON_POSION_DISPLAY );
	}

	

	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}
