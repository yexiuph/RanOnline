#include "pch.h"
#include "AudioOption.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "GameTextControl.h"
#include "UITextControl.h"
#include "../[Lib]__EngineUI/Sources/BasicProgressBar.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "RANPARAM.h"
#include "DxSoundMan.h"
#include "BgmSound.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	NOTE
//		-2000 ~ 0까지의 볼륨을 제어하겠다는 이야기가 된다.
const long CBasicAudioOption::lCONTROLVOLUME = 2000;

CBasicAudioOption::CBasicAudioOption () :
	m_pSfxProgressBar ( NULL ),
	m_pEnvirProgressBar ( NULL ),
	m_pBgmProgressBar ( NULL )
{
}

CBasicAudioOption::~CBasicAudioOption ()
{
}

void CBasicAudioOption::CreateSubControl ()
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CUIControl*	pDummyVolume = NULL;
	CBasicTextBox* pTextBox = NULL;
	CBasicProgressBar* pProgressBar = NULL;
	
	{	//	볼륨 프로그래스바
		pProgressBar = new CBasicProgressBar;
		pProgressBar->CreateSub ( this, "HWOPTION_AUDIO_SFX_PROGRESS" );
        pProgressBar->CreateOverImage ( "HWOPTION_AUDIO_SFX_PROGRESS_OVERIMAGE" );
		RegisterControl ( pProgressBar );
		m_pSfxProgressBar = pProgressBar;

		pDummyVolume = new CUIControl;
		pDummyVolume->CreateSub ( this, "HWOPTION_AUDIO_SFX_PROGRESS_DUMMY", UI_FLAG_DEFAULT, HWOPTION_AUDIO_SFX_PROGRESS );
		RegisterControl ( pDummyVolume );


		pProgressBar = new CBasicProgressBar;
		pProgressBar->CreateSub ( this, "HWOPTION_AUDIO_ENVIR_PROGRESS" );
        pProgressBar->CreateOverImage ( "HWOPTION_AUDIO_ENVIR_PROGRESS_OVERIMAGE" );
		RegisterControl ( pProgressBar );
		m_pEnvirProgressBar = pProgressBar;

		pDummyVolume = new CUIControl;
		pDummyVolume->CreateSub ( this, "HWOPTION_AUDIO_ENVIR_PROGRESS_DUMMY", UI_FLAG_DEFAULT, HWOPTION_AUDIO_ENVIR_PROGRESS );
		RegisterControl ( pDummyVolume );


		pProgressBar = new CBasicProgressBar;
		pProgressBar->CreateSub ( this, "HWOPTION_AUDIO_BGM_PROGRESS" );
        pProgressBar->CreateOverImage ( "HWOPTION_AUDIO_BGM_PROGRESS_OVERIMAGE" );
		RegisterControl ( pProgressBar );
		m_pBgmProgressBar = pProgressBar;		

		pDummyVolume = new CUIControl;
		pDummyVolume->CreateSub ( this, "HWOPTION_AUDIO_BGM_PROGRESS_DUMMY", UI_FLAG_DEFAULT, HWOPTION_AUDIO_BGM_PROGRESS );
		RegisterControl ( pDummyVolume );
	}

	{	//	볼륨 텍스트
		pTextBox = CreateStaticControl ( "HWOPTION_AUDIO_SFX_TEXT", pFont );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_AUDIO", 0 ) );
		pTextBox = CreateStaticControl ( "HWOPTION_AUDIO_ENVIR_TEXT", pFont );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_AUDIO", 1 ) );
		pTextBox = CreateStaticControl ( "HWOPTION_AUDIO_BGM_TEXT", pFont );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_AUDIO", 2 ) );
	}

	{	//	소리 설정
		m_pSfxButton = CreateFlipButton ( "HWOPTION_SFX_BUTTON", "HWOPTION_SFX_BUTTON_F", HWOPTION_SFX_BUTTON );
		m_pEnvirButton = CreateFlipButton ( "HWOPTION_ENVIR_BUTTON", "HWOPTION_ENVIR_BUTTON_F", HWOPTION_ENVIR_BUTTON );
		m_pBgmButton = CreateFlipButton ( "HWOPTION_BGM_BUTTON", "HWOPTION_BGM_BUTTON_F", HWOPTION_BGM_BUTTON );
	}
}

CBasicButton* CBasicAudioOption::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::RADIO_FLIP );
	pButton->SetControlNameEx ( szButton );
	RegisterControl ( pButton );
	return pButton;
}

CBasicTextBox* CBasicAudioOption::CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	RegisterControl ( pStaticText );
	return pStaticText;
}

void CBasicAudioOption::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( bFirstControl )
	{
		if ( m_pSfxProgressBar && m_pSfxProgressBar->IsExclusiveSelfControl() )
		{
			const float fSize = m_pSfxProgressBar->GetGlobalPos ().sizeX;
			const float fPos = float(x) - m_pSfxProgressBar->GetGlobalPos ().left;
			const float fPercent = fPos / fSize;
			m_pSfxProgressBar->SetPercent ( fPercent );
		}

		if ( m_pEnvirProgressBar && m_pEnvirProgressBar->IsExclusiveSelfControl() )
		{
			const float fSize = m_pEnvirProgressBar->GetGlobalPos ().sizeX;
			const float fPos = float(x) - m_pEnvirProgressBar->GetGlobalPos ().left;
			const float fPercent = fPos / fSize;
			m_pEnvirProgressBar->SetPercent ( fPercent );
		}

		if ( m_pBgmProgressBar && m_pBgmProgressBar->IsExclusiveSelfControl() )
		{
			const float fSize = m_pBgmProgressBar->GetGlobalPos ().sizeX;
			const float fPos = float(x) - m_pBgmProgressBar->GetGlobalPos ().left;
			const float fPercent = fPos / fSize;
			m_pBgmProgressBar->SetPercent ( fPercent );
		}
	}
}

void CBasicAudioOption::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case HWOPTION_AUDIO_SFX_PROGRESS:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )		//	컨트롤 내부에 마우스가 위치하고,
			{
				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )	//	마우스를 좌클릭으로 눌렀을 때,
				{			
					m_pSfxProgressBar->SetExclusiveControl();	//	단독 컨트롤로 등록하고,
				}
				else if ( CHECK_LB_UP_LIKE ( dwMsg ) )	//	좌클릭했던것을 놓으면
				{
					m_pSfxProgressBar->ResetExclusiveControl();	//	단독 컨트롤을 해제하고
				}
			}
			else if ( CHECK_LB_UP_LIKE ( dwMsg ) )		//	컨트롤 영역밖에서 버튼을 떼는 경우가
			{											//	발생하더라도
				m_pSfxProgressBar->ResetExclusiveControl();		//	단독 컨트롤을 해제하고,
			}

			long lVolume = long (lCONTROLVOLUME * m_pSfxProgressBar->GetPercent ());
			if ( lVolume < 1 )	RANPARAM::nSndSfx = DSBVOLUME_MIN;
			else				RANPARAM::nSndSfx = lVolume - lCONTROLVOLUME;
            DxSoundMan::GetInstance().SetSfxVolume ( RANPARAM::nSndSfx );
		}
		break;

	case HWOPTION_AUDIO_ENVIR_PROGRESS:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )		//	컨트롤 내부에 마우스가 위치하고,
			{
				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )	//	마우스를 좌클릭으로 눌렀을 때,
				{			
					m_pEnvirProgressBar->SetExclusiveControl();	//	단독 컨트롤로 등록하고,
				}
				else if ( CHECK_LB_UP_LIKE ( dwMsg ) )	//	좌클릭했던것을 놓으면
				{
					m_pEnvirProgressBar->ResetExclusiveControl();	//	단독 컨트롤을 해제하고
				}
			}
			else if ( CHECK_LB_UP_LIKE ( dwMsg ) )		//	컨트롤 영역밖에서 버튼을 떼는 경우가
			{											//	발생하더라도
				m_pEnvirProgressBar->ResetExclusiveControl();		//	단독 컨트롤을 해제하고,
			}
			long lVolume = long(lCONTROLVOLUME * m_pEnvirProgressBar->GetPercent ());
			if ( lVolume < 1 )	RANPARAM::nSndMap = DSBVOLUME_MIN;
			else				RANPARAM::nSndMap = lVolume - lCONTROLVOLUME;
			DxSoundMan::GetInstance().SetMapVolume ( RANPARAM::nSndMap );
		}
		break;

	case HWOPTION_AUDIO_BGM_PROGRESS:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )		//	컨트롤 내부에 마우스가 위치하고,
			{
				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )	//	마우스를 좌클릭으로 눌렀을 때,
				{			
					m_pBgmProgressBar->SetExclusiveControl();	//	단독 컨트롤로 등록하고,
				}
				else if ( CHECK_LB_UP_LIKE ( dwMsg ) )	//	좌클릭했던것을 놓으면
				{
					m_pBgmProgressBar->ResetExclusiveControl();	//	단독 컨트롤을 해제하고
				}
			}
			else if ( CHECK_LB_UP_LIKE ( dwMsg ) )		//	컨트롤 영역밖에서 버튼을 떼는 경우가
			{											//	발생하더라도
				m_pBgmProgressBar->ResetExclusiveControl();		//	단독 컨트롤을 해제하고,
			}
			long lVolume = long(lCONTROLVOLUME * m_pBgmProgressBar->GetPercent ());
			if ( lVolume < 1 )	RANPARAM::nSndMusic = DSBVOLUME_MIN;
			else				RANPARAM::nSndMusic = lVolume - lCONTROLVOLUME;
			DxBgmSound::GetInstance().SetVolume ( RANPARAM::nSndMusic );
		}
		break;

	case HWOPTION_SFX_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//RANPARAM::bSndMuteSfx = !RANPARAM::bSndMuteSfx;
				//m_pSfxButton->SetFlip ( !RANPARAM::bSndMuteSfx );
				//DxSoundMan::GetInstance().SetSfxMute ( RANPARAM::bSndMuteSfx );
				m_bSndMuteSfx = !m_bSndMuteSfx;
				m_pSfxButton->SetFlip( m_bSndMuteSfx );
			}
		}
		break;

	case HWOPTION_ENVIR_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{	
				//RANPARAM::bSndMuteMap = !RANPARAM::bSndMuteMap;
				//m_pEnvirButton->SetFlip ( !RANPARAM::bSndMuteMap );
				//DxSoundMan::GetInstance().SetMapMute ( RANPARAM::bSndMuteMap );
				m_bSndMuteMap = !m_bSndMuteMap;
				m_pEnvirButton->SetFlip( m_bSndMuteMap );
			}
		}
		break;

	case HWOPTION_BGM_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//RANPARAM::bSndMuteMusic = !RANPARAM::bSndMuteMusic;
				//m_pBgmButton->SetFlip ( !RANPARAM::bSndMuteMusic );				
				//DxBgmSound::GetInstance().SetMute ( RANPARAM::bSndMuteMusic );
				m_bSndMuteMusic = !m_bSndMuteMusic;
				m_pBgmButton->SetFlip( m_bSndMuteMusic );
			}
		}
		break;
	}
}

void CBasicAudioOption::LoadCurrentOption ()
{
	long lVolume = 0;
	float fPercent = 0.0f;

	//	SFX	
	if ( RANPARAM::nSndSfx == DSBVOLUME_MIN )
		lVolume = 0;
	else
		lVolume = RANPARAM::nSndSfx + lCONTROLVOLUME;
    fPercent = float(lVolume) / float(lCONTROLVOLUME);
	m_pSfxProgressBar->SetPercent ( fPercent );

	//	Environment
	if ( RANPARAM::nSndMap == DSBVOLUME_MIN )
		lVolume = 0;
	else
		lVolume = RANPARAM::nSndMap + lCONTROLVOLUME;
    fPercent = float(lVolume) / float(lCONTROLVOLUME);
	m_pEnvirProgressBar->SetPercent ( fPercent );

	//	BGM
	if ( RANPARAM::nSndMusic == DSBVOLUME_MIN )
		lVolume = 0;
	else
		lVolume = RANPARAM::nSndMusic + lCONTROLVOLUME;
    fPercent = float(lVolume) / float(lCONTROLVOLUME);
	m_pBgmProgressBar->SetPercent ( fPercent );

	//	소리 설정
	m_bSndMuteSfx = !RANPARAM::bSndMuteSfx;
	m_pSfxButton->SetFlip( m_bSndMuteSfx );

	m_bSndMuteMap = !RANPARAM::bSndMuteMap;
	m_pEnvirButton->SetFlip( m_bSndMuteMap );

	m_bSndMuteMusic = !RANPARAM::bSndMuteMusic;
	m_pBgmButton->SetFlip( m_bSndMuteMusic );
}

void CBasicAudioOption::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		LoadCurrentOption();
	}
}