#include "pch.h"

#include "PrivateMarketWindow.h"
#include "PrivateMarketPage.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "GLGaeaClient.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "InnerInterface.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "UITextControl.h"
#include "GameTextControl.h"
#include "BasicTextButton.h"
#include "GLItemMan.h"
#include "GLCrowRenList.h"
#include "BasicLineBox.h"
#include "ModalCallerID.h"
#include "ModalWindow.h"
#include "NameDisplayMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPrivateMarketWindow::CPrivateMarketWindow ()
{
}

CPrivateMarketWindow::~CPrivateMarketWindow ()
{
}

void	CPrivateMarketWindow::CreateSubControl ()
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	
	CBasicLineBox* pBasicLineBox = new CBasicLineBox;
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestList ( "PRIVATE_MARKET_ADVERTISE_LINEBOX" );
	RegisterControl ( pBasicLineBox );		

	m_pTextBox = new CBasicTextBox;
	m_pTextBox->CreateSub ( this, "PRIVATE_MARKET_ADVERTISE_TEXTBOX" );
	m_pTextBox->SetFont ( pFont );
	RegisterControl ( m_pTextBox );

	m_pPage = new CPrivateMarketPage;
	m_pPage->CreateSub ( this, "PRIVATE_MARKET_PAGE", UI_FLAG_DEFAULT, PRIVATE_MARKET_PAGE );
	m_pPage->CreateSubControl ();	
	RegisterControl ( m_pPage );

	m_pSTART = CreateTextButton ( "PRIVATE_MARKET_START", PRIVATE_MARKET_START, const_cast<char*>(ID2GAMEWORD("PRIVATE_MARKET_BUTTON",0)) );
	m_pSTOP = CreateTextButton ( "PRIVATE_MARKET_STOP", PRIVATE_MARKET_STOP, const_cast<char*>(ID2GAMEWORD("PRIVATE_MARKET_BUTTON",1)) );
}

void CPrivateMarketWindow::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	//	의도적으로 윈도우 이동을 막음
	// CUIWindowEx::TranslateUIMessage ( cID, dwMsg );

	switch ( cID )
	{
	case PRIVATE_MARKET_START:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//	시작
				GLGaeaClient::GetInstance().GetCharacter ()->ReqPMarketOpen ();
			}
		}
		break;

	case PRIVATE_MARKET_PAGE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nPosX, nPosY;
				m_pPage->GetItemIndex ( &nPosX, &nPosY );

				//CDebugSet::ToView ( 1, 3, "[private market] Page:%d %d / %d", nPosX, nPosY );

				if ( nPosX < 0 || nPosY < 0 ) return ;

				SetItemIndex ( nPosX, nPosY );

				SINVENITEM sInvenItem = m_pPage->GetItem ( nPosX, nPosY );
				if ( sInvenItem.sItemCustom.sNativeID != NATIVEID_NULL () )
				{
					CInnerInterface::GetInstance().SHOW_ITEM_INFO ( sInvenItem.sItemCustom, FALSE, FALSE, TRUE, sInvenItem.wPosX, sInvenItem.wPosY );
				}

				if ( dwMsg & UIMSG_LB_UP )
				{
					WORD wPosX = static_cast<WORD>(nPosX);
					WORD wPosY = static_cast<WORD>(nPosY);

					if ( m_bOPENER )
					{
						GLPrivateMarket &sPMarket = GLGaeaClient::GetInstance().GetCharacter()->m_sPMarket;
						if ( sPMarket.IsOpen () ) return ;

						if ( sInvenItem.sItemCustom.sNativeID == NATIVEID_NULL () ) return ;
						GLGaeaClient::GetInstance().GetCharacter()->ReqPMarketDisItem ( wPosX, wPosY );
					}
					else
					{
						//	이미 다 팔린 상태라면 사기를 진행하지 않음
						if ( m_pPage->IsFlipItem ( wPosX, wPosY ) )
						{
							return ;
						}

						//	NOTE
						//		아이템 바로 사기
						//if ( DxInputDevice::GetInstance().GetKeyState ( DIK_LCONTROL ) & DXKEY_PRESSED )
						//{
						//	if ( dwMsg & UIMSG_LB_UP )
						//	{
						//		SNATIVEID sNativeID = GLGaeaClient::GetInstance().GetCharacter()->GET_HOLD_ITEM().sNativeID;
						//		if ( sNativeID != NATIVEID_NULL () ) return ;

						//		GLGaeaClient::GetInstance().GetCharacter()->ReqPMarketBuy ( m_dwGaeaID, nPosX, nPosY, 1 );								
						//		return ;
						//	}
						//}

						if ( dwMsg & UIMSG_LB_UP )
						{
							SNATIVEID sNativeID = GLGaeaClient::GetInstance().GetCharacter()->GET_HOLD_ITEM().sNativeID;
							if ( sNativeID != NATIVEID_NULL () ) return ;

							SITEM* pITEM = GLItemMan::GetInstance().GetItem ( sInvenItem.sItemCustom.sNativeID );
							if ( pITEM )
							{
								//	Note : 겹침이 가능하고 한개씩 파는 아이템의 경우 구입 수량 입력. ( 화살묶음, 부적묶음. )
								if ( pITEM->ISPILE() && (pITEM->GETAPPLYNUM()==1) )
								{
									SetModalData ( nPosX, nPosY );
									DoModal ( ID2GAMEINTEXT ( "BUY_PILE_ITEM" ), MODAL_INPUT, EDITBOX_NUMBER, MODAL_PRIVATE_MARKET_PILE );
									return;
								}
								else
								{
									//	NOTE
									//		물건 사기
									const std::string& strItemName = pITEM->GetName();
									CString strCombine = CInnerInterface::GetInstance().MakeString ( ID2GAMEINTEXT("BUY_ITEM_CONFIRM"), strItemName.c_str() );
									SetModalData ( nPosX, nPosY );
									DoModal ( strCombine, MODAL_QUESTION, YESNO, MODAL_PRIVATE_MARKET_BUY );							
									return ;
								}
							}
						}

					//	DoModal ( "BUYING" );
					}

					return ;
				}
			}
		}
		break;

	case ET_CONTROL_BUTTON:
	case PRIVATE_MARKET_STOP:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{				
				if ( m_bOPENER )
				{
					DoModal ( ID2GAMEINTEXT("PRIVATE_MARKET_QUIT"), MODAL_QUESTION, YESNO, MODAL_PRIVATE_MARKET_QUIT );
				}
				else
				{
					CInnerInterface::GetInstance().SetPrivateMarketClose ();
				}
			}
		}
		break;
	}
}

CBasicTextButton* CPrivateMarketWindow::CreateTextButton ( char* szButton, UIGUID ControlID, char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pTextButton = new CBasicTextButton;
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pTextButton );
	return pTextButton;
}

void CPrivateMarketWindow::InitPrivateMarket ( const bool& bOPENER, const DWORD& dwGaeaID )
{
	CNameDisplayMan *pNAME_DISP = CInnerInterface::GetInstance().GetDispName();
	if ( !pNAME_DISP )	return;

	m_bOPENER = bOPENER;
	m_dwGaeaID = dwGaeaID;

	if ( m_bOPENER )
	{
		{	//	광고글

			const CString& strTitle = CInnerInterface::GetInstance().GetPrivateMarketTitle ();

			CString strTemp;
			strTemp.Format ( "%s", GLGaeaClient::GetInstance().GetCharacter ()->GetName () );
			m_pTextBox->ClearText ();
			m_pTextBox->AddText ( strTemp, UINT_MAX );
			m_pTextBox->AddText ( strTitle, UINT_MAX );
		}

		m_pSTART->SetVisibleSingle ( TRUE );
		m_pSTOP->SetVisibleSingle ( TRUE );
	}
	else
	{
		{	//	광고글
			const CROWREN* pINFO = pNAME_DISP->GET_DISP_INFO ( CROW_PC, dwGaeaID );
			if ( !pINFO )	 return;

			CString strTemp;
			strTemp.Format ( "%s", pINFO->m_szNAME );
			m_pTextBox->ClearText ();
			m_pTextBox->AddText ( strTemp, NS_UITEXTCOLOR::GOLD );
			m_pTextBox->AddText ( pINFO->m_szPMARKET, UINT_MAX );
		}

		m_pSTART->SetVisibleSingle ( FALSE );
		m_pSTOP->SetVisibleSingle ( FALSE );	

		if ( m_pPage )
		{
			m_pPage->UnLoadItemPage ();

			GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();
			pCharacter->ReqPMarketInfo ( dwGaeaID );
		}
	}
}

HRESULT CPrivateMarketWindow::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIWindowEx::RestoreDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	const UIRECT& rcWindow = GetGlobalPos ();

	const long lResolution = CInnerInterface::GetInstance().GetResolution ();
	WORD X_RES = HIWORD ( lResolution );
	WORD Y_RES = LOWORD ( lResolution );

	D3DXVECTOR2 vPos;
	vPos.x = ((X_RES) / 2.0f) - rcWindow.sizeX;
	vPos.y = (Y_RES - rcWindow.sizeY) / 2.0f;
	SetGlobalPos ( vPos );

	return S_OK;
}

void CPrivateMarketWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	

	if ( m_bOPENER )
	{
		GLPrivateMarket &sPMarket = GLGaeaClient::GetInstance().GetCharacter()->m_sPMarket;		

		m_pPage->LoadItemPage ( sPMarket.GetInven(), m_bOPENER );
		m_pPage->DoFlipItemPage ( sPMarket );		
		
		const bool bOPEN = sPMarket.IsOpen ();
		if ( bOPEN )	m_pSTART->SetVisibleSingle ( FALSE );		
	}
	else
	{
		PGLCHARCLIENT pCLIENT = GLGaeaClient::GetInstance().GetChar ( m_dwGaeaID );
		if ( !pCLIENT ) return ;

		GLPrivateMarket &sPMarket = pCLIENT->m_sPMarket;

		m_pPage->LoadItemPage ( sPMarket.GetInven (), m_bOPENER );
		m_pPage->DoFlipItemPage ( sPMarket );	

		const bool bOPEN = sPMarket.IsOpen ();
		if ( !bOPEN )	CInnerInterface::GetInstance().SetPrivateMarketClose ();
	}
}

void CPrivateMarketWindow::GetItemIndex ( int* pnPosX, int* pnPosY )
{
	*pnPosX = m_nPosX;
	*pnPosY = m_nPosY;
}

void CPrivateMarketWindow::SetItemIndex ( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

SINVENITEM&	CPrivateMarketWindow::GetItem ( int nPosX, int nPosY )
{
	return m_pPage->GetItem ( nPosX, nPosY );
}