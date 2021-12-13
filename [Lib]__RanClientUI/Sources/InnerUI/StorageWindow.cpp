#include "pch.h"
#include "StorageWindow.h"
#include "StoragePage.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "GLGaeaClient.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "InnerInterface.h"
#include "ModalCallerID.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "ModalWindow.h"
#include "UITextControl.h"
#include "GameTextControl.h"
#include "d3dfont.h"
#include "GLGaeaClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CStorageWindow::nDEFAULT_PAGE = 0;

CStorageWindow::CStorageWindow () :
	m_nOpenPage ( nDEFAULT_PAGE ),
	m_pMoneyTextBox ( NULL )
{
	memset ( m_pPage, 0, sizeof ( CStoragePage* ) * nUSE_STORAGE_CHANNEL );
}

CStorageWindow::~CStorageWindow ()
{
}

void CStorageWindow::CreateSubControl ()
{	
	CString strStorageButton[nMAX_STORAGE_CHANNEL] =
	{
		"STORAGE_BUTTON0",
		"STORAGE_BUTTON1",
		"STORAGE_BUTTON2",
		"STORAGE_BUTTON3",
		"STORAGE_BUTTON4",
	};

	CString strStorageButtonFlip[nMAX_STORAGE_CHANNEL] = 
	{
		"STORAGE_BUTTON0_F",
		"STORAGE_BUTTON1_F",
		"STORAGE_BUTTON2_F",
		"STORAGE_BUTTON3_F",
		"STORAGE_BUTTON4_F",
	};

	for ( int i = 0; i < nUSE_STORAGE_CHANNEL; i++ )
	{
		m_pPageButton[i] = CreateButton ( strStorageButton[i].GetString (), strStorageButtonFlip[i].GetString (), STORAGE_BUTTON0 + i );
	}

	{
		for ( int i = 0; i < nUSE_STORAGE_CHANNEL; i++ )
		{
			m_pPage[i] = new CStoragePage;
			m_pPage[i]->CreateSub ( this, "STORAGE_PAGE", UI_FLAG_DEFAULT, STORAGE_PAGE0 + i );
			m_pPage[i]->CreateSubControl ();
			m_pPage[i]->InitStoragePage ( i );
			RegisterControl ( m_pPage[i] );
		}
	}

	CreateControl ( "STORAGE_MONEY_BACK" );
	CBasicButton* pButton = CreateFlipButton ( "STORAGE_MONEY_BUTTON", "STORAGE_MONEY_BUTTON_F", STORAGE_MONEY_BUTTON, CBasicButton::CLICK_FLIP );
	pButton->CreateMouseOver ( "STORAGE_MONEY_BUTTON_F" );
	pButton->SetUseDynamic ( TRUE );

	CD3DFontPar* pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
	m_pMoneyTextBox = CreateStaticControl ( "STORAGE_MONEY_TEXT", pFont8, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );

	m_pBLOCK = new CUIControl;
	m_pBLOCK->CreateSub ( this, "STORAGE_BLOCK" );	
	RegisterControl ( m_pBLOCK );
}

CBasicButton*	CStorageWindow::CreateButton ( const char* szButton, const char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::RADIO_FLIP );
//	pButton->SetFlip ( TRUE );
	RegisterControl ( pButton );
	return pButton;
}

void CStorageWindow::InitStorage ( DWORD dwNPCID = 0 )
{
	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();
	for ( int i = 0; i < nUSE_STORAGE_CHANNEL; i++ )
	{
		pCharacter->ReqGetStorage ( i, dwNPCID );
	}
	m_nOpenPage = nDEFAULT_PAGE;
	SetVisiblePage ( STORAGE_PAGE0 );
}

void CStorageWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );

	if( bVisible )
	{
	}
	else
	{
		GLGaeaClient::GetInstance().GetCharacter()->ReqStorageCloseCard();
	}
}

void CStorageWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	if( ET_CONTROL_TITLE == ControlID  || ET_CONTROL_TITLE_F == ControlID )
	{
		if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
		{
			CInnerInterface::GetInstance().SetDefaultPosInterface( STORAGE_WINDOW );
		}
	}
	if ( ControlID == ET_CONTROL_BUTTON )
	{
		if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
		{
			CInnerInterface::GetInstance().SetStorageWindowClose ();
			return ;
		}
	}		

	if ( STORAGE_BUTTON0 <= ControlID && ControlID < STORAGE_BUTTON_END )
	{
		if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
		{
			int nIndex = ControlID - STORAGE_BUTTON0;
			SetVisiblePage ( nIndex + STORAGE_PAGE0 );
		}
	}
	else if ( STORAGE_PAGE0 <= ControlID && ControlID < STORAGE_PAGE_END )
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			int nPosX, nPosY;
			m_pPage[m_nOpenPage]->GetItemIndex ( &nPosX, &nPosY );

			CDebugSet::ToView ( 1, 3, "[storage] Page:%d %d / %d", m_nOpenPage, nPosX, nPosY );

			if ( nPosX < 0 || nPosY < 0 ) return ;

//			if ( CUIMan::GetFocusControl () == this )
			{
				SINVENITEM sInvenItem = m_pPage[m_nOpenPage]->GetItem ( nPosX, nPosY );
				if ( sInvenItem.sItemCustom.sNativeID != NATIVEID_NULL () )
				{
					CInnerInterface::GetInstance().SHOW_ITEM_INFO ( sInvenItem.sItemCustom, FALSE, FALSE, FALSE, sInvenItem.wPosX, sInvenItem.wPosY );
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
					BOOL bSplitable = GLGaeaClient::GetInstance().GetCharacter()->IsStorageSplitItem ( m_nOpenPage, wSplitPosX, wSplitPosY );
                    if ( bDiffPos && bSplitable )
					{
						DoModal ( ID2GAMEINTEXT ( "SPLIT_ITEM" ), MODAL_QUESTION, EDITBOX_NUMBER, MODAL_STORAGE_SPLIT_ITEM );
					}
					else
					{
						GLGaeaClient::GetInstance().GetCharacter()->ReqStorageTo ( m_nOpenPage, nPosX, nPosY );
					}
				}
				return ;
			}

			//if ( dwMsg & UIMSG_LB_UP )
			//{
			//	GLGaeaClient::GetInstance().GetCharacter()->ReqStorageTo ( m_nOpenPage, nPosX, nPosY );
			//}

			if ( dwMsg & UIMSG_RB_UP )
			{
				GLGaeaClient::GetInstance().GetCharacter()->ReqStorageDrug( m_nOpenPage, nPosX, nPosY );
			}

			if ( dwMsg & UIMSG_LB_DOWN )
			{
				SetSplitPos ( nPosX, nPosY );
				m_bSplitItem = TRUE;
				return ;
			}
		}
	}

	switch ( ControlID )
	{
	case STORAGE_MONEY_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				DoModal ( ID2GAMEINTEXT("PLAYINTERFACE_32"), MODAL_INPUT, EDITBOX_MONEY, MODAL_LOADMONEY );
			}
		}
		break;

	case STORAGE_BUTTON1:
	case STORAGE_BUTTON2:
	case STORAGE_BUTTON3:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				const DWORD cSLOT_CHANNEL = static_cast<DWORD>(ControlID - STORAGE_BUTTON1 + EMSTORAGE_CHANNEL_SPAN);

				GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter ();
				const bool bUSABLE = pCharacter->IsKEEP_STORAGE (cSLOT_CHANNEL);
				if ( bUSABLE )
				{
					if ( CInnerInterface::GetInstance().BEGIN_COMMON_LINEINFO_MULTI () )
					{
						CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI ( ID2GAMEWORD("STORAGE_USABLE",0), NS_UITEXTCOLOR::GREENYELLOW );

						const CTime cTime = pCharacter->GetStorageTime(cSLOT_CHANNEL);			
						CString strCombine = CInnerInterface::GetInstance().MakeString ( ID2GAMEWORD("STORAGE_TIMELEFT_ONELINE"),
							cTime.GetYear (), cTime.GetMonth (), cTime.GetDay (), cTime.GetHour (), cTime.GetMinute () );

						CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI ( strCombine, NS_UITEXTCOLOR::WHITESMOKE );

						CInnerInterface::GetInstance().END_COMMON_LINEINFO_MULTI ();
					}					
				}
				else
				{
					CInnerInterface::GetInstance().SHOW_COMMON_LINEINFO ( ID2GAMEWORD("STORAGE_USABLE",1), NS_UITEXTCOLOR::RED );
				}
			}
		}
		break;

	case STORAGE_BUTTON4:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				bool bPREMIUM = GLGaeaClient::GetInstance().GetCharacter()->m_bPREMIUM;
				if ( bPREMIUM )
				{
					if ( CInnerInterface::GetInstance().BEGIN_COMMON_LINEINFO_MULTI () )
					{
						CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI ( ID2GAMEWORD("CHARACTER_PREMIUM_EXPIRE_DATE_STATIC"), NS_UITEXTCOLOR::DARKORANGE );

						__time64_t tPREMIUM = GLGaeaClient::GetInstance().GetCharacter()->m_tPREMIUM;
						CTime cTime ( tPREMIUM );
						CString strExpireDate = CInnerInterface::GetInstance().MakeString ( ID2GAMEWORD("CHARACTER_PREMIUM_EXPIRE_DATE"),
							cTime.GetYear(), cTime.GetMonth(), cTime.GetDay (), cTime.GetHour (), cTime.GetMinute () );

						CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI ( strExpireDate, NS_UITEXTCOLOR::WHITE );

						CInnerInterface::GetInstance().END_COMMON_LINEINFO_MULTI ();
					}					
				}
				else
				{
					CInnerInterface::GetInstance().SHOW_COMMON_LINEINFO ( ID2GAMEWORD("NOT_USE_PREMIUM"), NS_UITEXTCOLOR::RED );
				}
			}
		}
		break;
	}

	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );
}


void CStorageWindow::SetVisiblePage ( int nPage )
{
	if ( nPage < STORAGE_PAGE0 || STORAGE_PAGE_END < nPage )
	{
		GASSERT ( 0 && "영역을 넘어서는 페이지입니다." );
		return ;
	}

	{	//	리셋
		for ( int i = 0; i < nUSE_STORAGE_CHANNEL; i++ )
		{
			m_pPage[i]->SetVisibleSingle ( FALSE );
			m_pPageButton[i]->SetFlip ( FALSE );
			m_pPageButton[i]->DoFlipUpdate ();
		}
	}

	m_nOpenPage = nPage - STORAGE_PAGE0;
	m_pPage[m_nOpenPage]->SetVisibleSingle ( TRUE );
	m_pPageButton[m_nOpenPage]->SetFlip ( TRUE );
	m_pPageButton[m_nOpenPage]->DoFlipUpdate ();

	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();
    if ( pCharacter->IsKEEP_STORAGE(m_nOpenPage) )
		ResetBLOCK ();		
	else
		SetBLOCK ();		
}

void CStorageWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	LONGLONG lnMoney = GLGaeaClient::GetInstance().GetCharacter ()->m_lnStorageMoney;
	SetMoney ( lnMoney );
}

void CStorageWindow::SetMoney ( LONGLONG Money )
{
	//	돈자리수
	CString strTemp = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT ( Money, 3, "," );
	m_pMoneyTextBox->SetOneLineText ( strTemp );
}

void CStorageWindow::GetSplitPos ( WORD* pwPosX, WORD* pwPosY )
{
	*pwPosX = m_wSplitItemPosX;
	*pwPosY = m_wSplitItemPosY;
}

void CStorageWindow::SetSplitPos ( WORD wPosX, WORD wPosY )
{
	m_wSplitItemPosX = wPosX;
	m_wSplitItemPosY = wPosY;
}

void CStorageWindow::SetBLOCK ()
{
	m_pBLOCK->SetVisibleSingle ( TRUE );
	m_pBLOCK->SetUseRender ( TRUE );
	m_pBLOCK->SetDiffuse ( D3DCOLOR_ARGB(125,0,0,0) );
}

void CStorageWindow::ResetBLOCK ()
{
	m_pBLOCK->SetUseRender ( FALSE );
	m_pBLOCK->SetVisibleSingle ( FALSE );
}

int	CStorageWindow::GetChannel ()
{
	return m_nOpenPage;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

const int CClubStorageWindow::nDEFAULT_PAGE = 0;

CClubStorageWindow::CClubStorageWindow () :
	m_nOpenPage ( nDEFAULT_PAGE ),
	m_pMoneyTextBox ( NULL )
{
	memset ( m_pPage, 0, sizeof ( CClubStoragePage* ) * nUSE_STORAGE_CHANNEL );
}

CClubStorageWindow::~CClubStorageWindow ()
{
}

void CClubStorageWindow::CreateSubControl ()
{	
	CString strStorageButton[nMAX_STORAGE_CHANNEL] =
	{
		"CLUB_STORAGE_BUTTON0",
		"CLUB_STORAGE_BUTTON1",
		"CLUB_STORAGE_BUTTON2",
		"CLUB_STORAGE_BUTTON3",
		"CLUB_STORAGE_BUTTON4",
	};
	CString strStorageButtonFlip[nMAX_STORAGE_CHANNEL] = 
	{
		"CLUB_STORAGE_BUTTON0_F",
		"CLUB_STORAGE_BUTTON1_F",
		"CLUB_STORAGE_BUTTON2_F",
		"CLUB_STORAGE_BUTTON3_F",
		"CLUB_STORAGE_BUTTON4_F",
	};

	for ( int i = 0; i < nUSE_STORAGE_CHANNEL; i++ )
	{
		m_pPageButton[i] = CreateButton ( strStorageButton[i].GetString (), strStorageButtonFlip[i].GetString (), CLUB_STORAGE_BUTTON0 + i );
	}

	{
		for ( int i = 0; i < nUSE_STORAGE_CHANNEL; i++ )
		{
			CClubStoragePage* pClubStoragePage = new CClubStoragePage;
			pClubStoragePage->CreateSub ( this, "STORAGE_PAGE", UI_FLAG_DEFAULT, CLUB_STORAGE_PAGE0 + i );
			pClubStoragePage->CreateSubControl ();
			pClubStoragePage->InitStoragePage ( i );
			RegisterControl ( pClubStoragePage );
			m_pPage[i] = pClubStoragePage;
		}
	}

	CBasicButton* pButton = NULL;

	CreateControl ( "STORAGE_MONEY_BACK" );
	pButton = CreateFlipButton ( "STORAGE_MONEY_BUTTON", "STORAGE_MONEY_BUTTON_F", CLUB_STORAGE_MONEY_BUTTON, CBasicButton::CLICK_FLIP );
	pButton->CreateMouseOver ( "STORAGE_MONEY_BUTTON_F" );
	pButton->SetUseDynamic ( TRUE );

	CD3DFontPar* pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
	m_pMoneyTextBox = CreateStaticControl ( "STORAGE_MONEY_TEXT", pFont8, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );

	CUIControl* pBlock = new CUIControl;
	pBlock->CreateSub ( this, "STORAGE_BLOCK" );	
	RegisterControl ( pBlock );
	m_pBLOCK = pBlock;
}

CBasicButton*	CClubStorageWindow::CreateButton ( const char* szButton, const char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::RADIO_FLIP );
	RegisterControl ( pButton );
	return pButton;
}

void	CClubStorageWindow::InitStorage ()
{
	m_nOpenPage = nDEFAULT_PAGE;
	SetVisiblePage ( CLUB_STORAGE_PAGE0 );
}

void CClubStorageWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	if( ET_CONTROL_TITLE == ControlID || ET_CONTROL_TITLE_F == ControlID)
	{
		if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
		{
			CInnerInterface::GetInstance().SetDefaultPosInterface( CLUB_STORAGE_WINDOW );
		}
	}
	if ( ControlID == ET_CONTROL_BUTTON )
	{
		if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
		{
			CInnerInterface::GetInstance().SetClubStorageWindowClose ();
			return ;
		}
	}		

	if ( CLUB_STORAGE_BUTTON0 <= ControlID && ControlID < CLUB_STORAGE_BUTTON_END )
	{
		if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
		{
			int nIndex = ControlID - CLUB_STORAGE_BUTTON0;
			SetVisiblePage ( nIndex + CLUB_STORAGE_PAGE0 );
		}
	}
	else if ( CLUB_STORAGE_PAGE0 <= ControlID && ControlID < CLUB_STORAGE_PAGE_END )
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			int nPosX, nPosY;
			m_pPage[m_nOpenPage]->GetItemIndex ( &nPosX, &nPosY );

			CDebugSet::ToView ( 1, 3, "[storage] Page:%d %d / %d", m_nOpenPage, nPosX, nPosY );

			if ( nPosX < 0 || nPosY < 0 ) return ;

//			if ( CUIMan::GetFocusControl () == this )
			{
				SINVENITEM sInvenItem = m_pPage[m_nOpenPage]->GetItem ( nPosX, nPosY );
				if ( sInvenItem.sItemCustom.sNativeID != NATIVEID_NULL () )
				{
					CInnerInterface::GetInstance().SHOW_ITEM_INFO ( sInvenItem.sItemCustom, FALSE, FALSE, FALSE, sInvenItem.wPosX, sInvenItem.wPosY );
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
					BOOL bSplitable = GLGaeaClient::GetInstance().GetCharacter()->IsClubStorageSplitItem ( m_nOpenPage, wSplitPosX, wSplitPosY );
                    if ( bDiffPos && bSplitable )
					{
						DoModal ( ID2GAMEINTEXT ( "SPLIT_ITEM" ), MODAL_QUESTION, EDITBOX_NUMBER, MODAL_CLUB_STORAGE_SPLIT_ITEM );
					}
					else
					{
						GLGaeaClient::GetInstance().GetCharacter()->ReqClubStorageTo ( m_nOpenPage, nPosX, nPosY );
					}
				}
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

	switch ( ControlID )
	{
	case CLUB_STORAGE_MONEY_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				DoModal ( ID2GAMEINTEXT("PLAYINTERFACE_32"), MODAL_INPUT, EDITBOX_MONEY, MODAL_CLUB_LOADMONEY );
			}
		}
		break;
	}

	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );
}


void CClubStorageWindow::SetVisiblePage ( int nPage )
{
	if ( nPage < CLUB_STORAGE_PAGE0 || CLUB_STORAGE_PAGE_END < nPage )
	{
		GASSERT ( 0 && "영역을 넘어서는 페이지입니다." );
		return ;
	}

	{	//	리셋
		for ( int i = 0; i < nUSE_STORAGE_CHANNEL; i++ )
		{
			m_pPage[i]->SetVisibleSingle ( FALSE );
			m_pPageButton[i]->SetFlip ( FALSE );
			m_pPageButton[i]->DoFlipUpdate ();
		}
	}

	m_nOpenPage = nPage - CLUB_STORAGE_PAGE0;
	m_pPage[m_nOpenPage]->SetVisibleSingle ( TRUE );
	m_pPageButton[m_nOpenPage]->SetFlip ( TRUE );
	m_pPageButton[m_nOpenPage]->DoFlipUpdate ();

	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();
    if ( pCharacter->IsKEEP_CLUB_STORAGE(m_nOpenPage) )
		ResetBLOCK ();		
	else
		SetBLOCK ();		
}

void CClubStorageWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();
    if ( pCharacter->IsKEEP_CLUB_STORAGE(m_nOpenPage) )
		ResetBLOCK ();		
	else
		SetBLOCK ();

	LONGLONG lnMoney = pCharacter->m_sCLUB.m_lnStorageMoney;
	SetMoney ( lnMoney );
}

void CClubStorageWindow::SetMoney ( LONGLONG Money )
{
	//	돈자리수
	CString strTemp = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT ( Money, 3, "," );
	m_pMoneyTextBox->SetOneLineText ( strTemp );
}

void CClubStorageWindow::GetSplitPos ( WORD* pwPosX, WORD* pwPosY )
{
	*pwPosX = m_wSplitItemPosX;
	*pwPosY = m_wSplitItemPosY;
}

void CClubStorageWindow::SetSplitPos ( WORD wPosX, WORD wPosY )
{
	m_wSplitItemPosX = wPosX;
	m_wSplitItemPosY = wPosY;
}

void CClubStorageWindow::SetBLOCK ()
{
	m_pBLOCK->SetVisibleSingle ( TRUE );
	m_pBLOCK->SetUseRender ( TRUE );
	m_pBLOCK->SetDiffuse ( D3DCOLOR_ARGB(125,0,0,0) );
}

void CClubStorageWindow::ResetBLOCK ()
{
	m_pBLOCK->SetUseRender ( FALSE );
	m_pBLOCK->SetVisibleSingle ( FALSE );
}

int	CClubStorageWindow::GetChannel ()
{
	return m_nOpenPage;
}