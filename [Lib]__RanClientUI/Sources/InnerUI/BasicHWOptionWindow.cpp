#include "pch.h"
#include "BasicHwOptionWindow.h"
#include "BasicTextButton.h"
#include "GameTextControl.h"
#include "BasicLineBox.h"
#include "GLGaeaClient.h"

#include "VideoOption.h"
#include "AudioOption.h"
#include "GamePlayOption.h"

#include "DxParamSet.h"
#include "RANPARAM.h"
#include "DxGlobalStage.h"
#include "./InnerInterface.h"

#include "../[Lib]__EngineSound/Sources/DxSound/DxSoundMan.h"
#include "../[Lib]__EngineSound/Sources/DxSound/BgmSound.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBasicHWOptionWindow::CBasicHWOptionWindow ()
{
}

CBasicHWOptionWindow::~CBasicHWOptionWindow ()
{
}

void CBasicHWOptionWindow::CreateSubControl ()
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE18;
	CBasicTextButton* pVideoButton = new CBasicTextButton;
	pVideoButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, HWOPTION_VIDEO );
	pVideoButton->CreateBaseButton ( "HWOPTION_VIDEO_BUTTON", nBUTTONSIZE, CBasicButton::RADIO_FLIP, (char*)ID2GAMEWORD ( "HWOPTION_AV_NAME", 0 ) );
	pVideoButton->SetFlip ( TRUE );
	RegisterControl ( pVideoButton );
	m_pVideoButton = pVideoButton;

	CBasicTextButton* pAudioButton = new CBasicTextButton;
	pAudioButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, HWOPTION_AUDIO );
	pAudioButton->CreateBaseButton ( "HWOPTION_AUDIO_BUTTON", nBUTTONSIZE, CBasicButton::RADIO_FLIP, (char*)ID2GAMEWORD ( "HWOPTION_AV_NAME", 1 ) );
	pAudioButton->SetFlip ( FALSE );
	RegisterControl ( pAudioButton );
	m_pAudioButton = pAudioButton;

	CBasicTextButton* pGamePlayButton = new CBasicTextButton;
	pGamePlayButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, HWOPTION_GAMEPLAY );
	pGamePlayButton->CreateBaseButton ( "HWOPTION_GAMEPLAY_BUTTON", nBUTTONSIZE, CBasicButton::RADIO_FLIP, (char*)ID2GAMEWORD ( "HWOPTION_AV_NAME", 2 ) );
	pGamePlayButton->SetFlip ( FALSE );
	RegisterControl ( pGamePlayButton );
	m_pGamePlayButton = pGamePlayButton;

	//	배경판떼기
	CBasicLineBox* pBasicLineBox = new CBasicLineBox;
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OPTION", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxOption ( "HWOPTION_BACK_BOX" );
	RegisterControl ( pBasicLineBox );

	CBasicTextButton* pApplyButton = new CBasicTextButton;
	pApplyButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, HWOPTION_APPLY );
	pApplyButton->CreateBaseButton ( "HWOPTION_APPLY_BUTTON", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "HWOPTION_APPLYOK", 0 ) );
	RegisterControl ( pApplyButton );

	CBasicTextButton* pOKButton = new CBasicTextButton;
	pOKButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, HWOPTION_OK );
	pOKButton->CreateBaseButton ( "HWOPTION_OK_BUTTON", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "HWOPTION_APPLYOK", 1 ) );
	RegisterControl ( pOKButton );

	CBasicVideoOption* pVideoOption = new CBasicVideoOption;
	pVideoOption->CreateSub ( this, "HWOPTION_SHEET", UI_FLAG_DEFAULT, HWOPTION_VIDEOPAGE );
	pVideoOption->CreateSubControl ();
	pVideoOption->SetVisibleSingle ( TRUE );
	RegisterControl ( pVideoOption );
	m_pVideoOption = pVideoOption;

	CBasicAudioOption* pAudioOption = new CBasicAudioOption;
	pAudioOption->CreateSub ( this, "HWOPTION_SHEET", UI_FLAG_DEFAULT, HWOPTION_AUDIOPAGE );
	pAudioOption->CreateSubControl ();
	pAudioOption->SetVisibleSingle ( FALSE );
	RegisterControl ( pAudioOption );
	m_pAudioOption = pAudioOption;

	CBasicGamePlayOption* pGamePlayOption = new CBasicGamePlayOption;
	pGamePlayOption->CreateSub ( this, "HWOPTION_SHEET", UI_FLAG_DEFAULT, HWOPTION_GAMEPLAYPAGE );
	pGamePlayOption->CreateSubControl ();
	pGamePlayOption->SetVisibleSingle ( FALSE );
	RegisterControl ( pGamePlayOption );
	m_pGamePlayOption = pGamePlayOption;

	m_bFirstSetting = TRUE;
}

void CBasicHWOptionWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case HWOPTION_VIDEO:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					SetVisiblePage ( HWOPTION_VIDEOPAGE );
				}
			}
		}
		break;

	case HWOPTION_AUDIO:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					SetVisiblePage ( HWOPTION_AUDIOPAGE );
				}
			}
		}
		break;

	case HWOPTION_GAMEPLAY:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					SetVisiblePage ( HWOPTION_GAMEPLAYPAGE );
				}
			}
		}
		break;

	case HWOPTION_APPLY:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if( m_pVideoOption->IsVisible() )			VideoOption_OK();
				else if( m_pAudioOption->IsVisible() )		AudioOption_OK();
				else if( m_pGamePlayOption->IsVisible() )	GamePlayOption_OK();
			}
		}
		break;

	case HWOPTION_OK:
	case ET_CONTROL_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if( m_pVideoOption->IsVisible() )			VideoOption_OK();
				else if( m_pAudioOption->IsVisible() )		AudioOption_OK();
				else if( m_pGamePlayOption->IsVisible() )	GamePlayOption_OK();

				CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			}
		}
		break;
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
			{
				CInnerInterface::GetInstance().SetDefaultPosInterface( OPTION_HW_WINDOW );
			}
		}
		break;
	}

	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );
}


void CBasicHWOptionWindow::SetVisiblePage ( int nPage )
{
	if ( nPage < HWOPTION_VIDEOPAGE || HWOPTION_GAMEPLAYPAGE < nPage )
	{
		GASSERT ( 0 && "영역을 넘어서는 페이지입니다." );
		return ;
	}

	CUIControl* pVisiblePage = NULL;
	CBasicTextButton* pVisibleButton = NULL;

	{	//	리셋
		m_pVideoOption->SetVisibleSingle ( FALSE );
		m_pAudioOption->SetVisibleSingle ( FALSE );
		m_pGamePlayOption->SetVisibleSingle ( FALSE );
	}
	{	//	리셋
		m_pVideoButton->SetFlip ( FALSE );
		m_pAudioButton->SetFlip ( FALSE );
		m_pGamePlayButton->SetFlip ( FALSE );
	}

	switch ( nPage )
	{
	case HWOPTION_VIDEOPAGE:
		{
			pVisiblePage = m_pVideoOption;
			pVisibleButton = m_pVideoButton;
		}
		break;

	case HWOPTION_AUDIOPAGE:
		{
			pVisiblePage = m_pAudioOption;
			pVisibleButton = m_pAudioButton;
		}
		break;

	case HWOPTION_GAMEPLAYPAGE:
		{
			pVisiblePage = m_pGamePlayOption;
			pVisibleButton = m_pGamePlayButton;
		}
		break;
	}

	pVisiblePage->SetVisibleSingle ( TRUE );
	pVisibleButton->SetFlip ( TRUE );
}

void CBasicHWOptionWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		m_pVideoOption->LoadCurrentOption();
		m_pAudioOption->LoadCurrentOption();
		m_pGamePlayOption->LoadCurrentOption();
		
		if( m_bFirstSetting )
		{
			GLGaeaClient::GetInstance().GetCharacter()->ReqNonRebirth( RANPARAM::bNON_Rebirth );
		}

		m_bFirstSetting = FALSE;
	}
}

void CBasicHWOptionWindow::GamePlayOption_OK()
{
	DXPARAMSET::GetInstance().m_bDIS_CONFT		= m_pGamePlayOption->m_bConft;
	DXPARAMSET::GetInstance().m_bDIS_TRADE		= m_pGamePlayOption->m_bTrade;
	DXPARAMSET::GetInstance().m_bDIS_PARTY		= m_pGamePlayOption->m_bParty;
	DXPARAMSET::GetInstance().m_bSHOW_SIMPLEHP	= m_pGamePlayOption->m_bSimpleHP;
	RANPARAM::bDIS_FRIEND		= m_pGamePlayOption->m_bFriend;
	RANPARAM::bMOVABLE_ON_CHAT	= m_pGamePlayOption->m_bMovableOnChat;
	RANPARAM::bSHOW_TIP			= m_pGamePlayOption->m_bShowTip;
	RANPARAM::bFORCED_ATTACK	= m_pGamePlayOption->m_bForcedAttack;
	RANPARAM::bNAME_DISPLAY		= m_pGamePlayOption->m_bNameDisplay;
	RANPARAM::bNON_Rebirth		= m_pGamePlayOption->m_bNon_Rebirth;

	GLGaeaClient::GetInstance().GetCharacter()->ReqNonRebirth( RANPARAM::bNON_Rebirth );

	DXPARAMSET::GetInstance().GameOK();
}

void CBasicHWOptionWindow::AudioOption_OK()
{
	RANPARAM::bSndMuteSfx	= !m_pAudioOption->m_bSndMuteSfx;
	RANPARAM::bSndMuteMap	= !m_pAudioOption->m_bSndMuteMap;
	RANPARAM::bSndMuteMusic = !m_pAudioOption->m_bSndMuteMusic;

	DxSoundMan::GetInstance().SetSfxMute( RANPARAM::bSndMuteSfx );
	DxSoundMan::GetInstance().SetMapMute( RANPARAM::bSndMuteMap );
	DxBgmSound::GetInstance().SetMute( RANPARAM::bSndMuteMusic );

	DXPARAMSET::GetInstance().AudioOK();
}

void CBasicHWOptionWindow::VideoOption_OK()
{
	DXPARAMSET::GetInstance().m_dwVideoLevel	= m_pVideoOption->m_dwVideoLevel;
	DXPARAMSET::GetInstance().m_dwFogRange		= m_pVideoOption->m_dwSight;
	DXPARAMSET::GetInstance().m_dwSkinDetail	= m_pVideoOption->m_dwSkinDetail;
	DXPARAMSET::GetInstance().m_dwShadowChar	= m_pVideoOption->m_dwShadowChar;
	DXPARAMSET::GetInstance().m_dwScrWidth		= m_pVideoOption->m_dwScrWidth;
	DXPARAMSET::GetInstance().m_dwScrHeight		= m_pVideoOption->m_dwScrHeight;
	DXPARAMSET::GetInstance().m_emScrFormat		= m_pVideoOption->m_emScrFormat;

	DXPARAMSET::GetInstance().m_bBuff			= m_pVideoOption->m_bBuff;
	DXPARAMSET::GetInstance().m_bShadowLand		= m_pVideoOption->m_bShadowLand;
	DXPARAMSET::GetInstance().m_bRealReflect	= m_pVideoOption->m_bRealReflect;
	DXPARAMSET::GetInstance().m_bRefract		= m_pVideoOption->m_bRefract;
	DXPARAMSET::GetInstance().m_bGlow			= m_pVideoOption->m_bGlow;
	DXPARAMSET::GetInstance().m_bPost			= m_pVideoOption->m_bPost;
	DXPARAMSET::GetInstance().m_bFrameLimit		= m_pVideoOption->m_bFrameLimit;

	DXPARAMSET::GetInstance().VideoOK ( DxGlobalStage::GetInstance().GetD3DApp () );
}