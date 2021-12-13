#include "pch.h"

#include "ItemBankWindow.h"
#include "ItemBankPage.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "GLGaeaClient.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "InnerInterface.h"
#include "ModalCallerID.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "ModalWindow.h"
#include "UITextControl.h"
#include "GameTextControl.h"
#include "BasicTextButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CItemBankWindow::CItemBankWindow () :
	m_pPage ( NULL )
{
}

CItemBankWindow::~CItemBankWindow ()
{
}

void CItemBankWindow::CreateSubControl ()
{	
//	CreateTextButton ( "ITEMBANK_REFRESH_BUTTON", ITEMBANK_REFRESH_BUTTON, const_cast<char*>(ID2GAMEWORD("ITEMBANK_REFRESH_BUTTON")) );

	CItemBankPage* pItemBankPage = new CItemBankPage;
	pItemBankPage->CreateSub ( this, "ITEMBANK_PAGE", UI_FLAG_DEFAULT, ITEMBANK_PAGE );
	pItemBankPage->CreateSubControl ();	
	RegisterControl ( pItemBankPage );
	m_pPage = pItemBankPage;
}

CBasicTextButton*  CItemBankWindow::CreateTextButton ( char* szButton, UIGUID ControlID, char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pTextButton = new CBasicTextButton;
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pTextButton );

	return pTextButton;
}


void CItemBankWindow::InitItemBank ()
{
	m_pPage->UnLoadItemPage ();

	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();
	m_pPage->LoadItemPage ( pCharacter->m_cInvenCharged );
}

void CItemBankWindow::ClearItemBank()
{ 
	m_pPage->UnLoadItemPage ();
}

void CItemBankWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	
	if ( ET_CONTROL_TITLE == ControlID || ET_CONTROL_TITLE_F == ControlID ) 
	{
		if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
		{
			CInnerInterface::GetInstance().SetDefaultPosInterface( ITEMBANK_WINDOW );
			return;
		}		
	}
	else if ( ITEMBANK_PAGE == ControlID )
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			int nPosX, nPosY;
			m_pPage->GetItemIndex ( &nPosX, &nPosY );

			//CDebugSet::ToView ( 1, 3, "[itembank] Page:%d %d / %d", nPosX, nPosY );

			if ( nPosX < 0 || nPosY < 0 ) return ;

			SINVENITEM sInvenItem = m_pPage->GetItem ( nPosX, nPosY );
			if ( sInvenItem.sItemCustom.sNativeID != NATIVEID_NULL () )
			{
				CInnerInterface::GetInstance().SHOW_ITEM_INFO ( sInvenItem.sItemCustom, FALSE, FALSE, FALSE, sInvenItem.wPosX, sInvenItem.wPosY );
			}

			if ( dwMsg & UIMSG_LB_UP )
			{
				GLGaeaClient::GetInstance().GetCharacter()->ReqChargedItemTo ( static_cast<WORD>(nPosX), static_cast<WORD>(nPosY) );
				return ;
			}
		}
	}
}