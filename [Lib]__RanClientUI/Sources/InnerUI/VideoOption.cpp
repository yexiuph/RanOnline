#include "pch.h"
#include "VideoOption.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "GameTextControl.h"
#include "UITextControl.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "BasicComboBox.h"
#include "BasicComboBoxRollOver.h"
#include "../[Lib]__EngineUI/Sources/UIMan.h"
#include "DxParamSet.h"

#include "DxShadowMap.h"
#include "DxEffectMan.h"
#include "DxGlobalStage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const char CBasicVideoOption::cRESOLUTION_SEPERATOR = 'x';

CBasicVideoOption::CBasicVideoOption ()
	: m_RollOverID ( NO_ID )
	, m_dwSight(0)
	, m_dwSkinDetail(0)
	, m_dwShadowChar(0)
	, m_dwScrHeight(0)
	, m_dwScrWidth(0)
	, m_emScrFormat(EMSCREEN_F16)
	, m_dwVideoLevel(0)
{
}

CBasicVideoOption::~CBasicVideoOption ()
{
}

void CBasicVideoOption::CreateSubControl ()
{
	m_pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicTextBox* pTextBox = NULL;
	CBasicComboBox* pComboBox = NULL;
	CBasicComboBoxRollOver*	pComboBoxRollOver = NULL;

	{	//	기본 텍스트		
		DWORD dwFontColor = NS_UITEXTCOLOR::DEFAULT;
		int nAlign = TEXT_ALIGN_LEFT;

		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_DEFAULT_STATIC", m_pFont, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 10 ) );

		//	해상도
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_RESOLUTION_STATIC", m_pFont, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 0 ) );

		//	캐릭터 그림자
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_SHADOW_STATIC", m_pFont, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 1 ) );

		//	캐릭터 디테일
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_CHARDETAIL_STATIC", m_pFont, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 2 ) );

		//	시야
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_SIGHT_STATIC", m_pFont, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 3 ) );

		//	버프
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_BUFF_STATIC", m_pFont, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 4 ) );

		//	지형 그림자
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_SHADOWLAND_STATIC", m_pFont, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 5 ) );

		//	실시간 반사
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_REFLECT_STATIC", m_pFont, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 6 ) );

		//	화면 왜곡
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_REFRACT_STATIC", m_pFont, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 7 ) );

		//	글로우 효과
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_GLOW_STATIC", m_pFont, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 8 ) );

		//	뽀샤시 효과
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_POST_STATIC", m_pFont, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 9 ) );

		// 프레임 제한 사용
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_FRAME_LIMIT_STATIC",m_pFont, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 11 ) );
	}

	{	//	기능

		//	콤보박스
		{
			pComboBox = new CBasicComboBox;
			pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, HWOPTION_VIDEO_DEFAULT_COMBO_OPEN );
			pComboBox->CreateBaseComboBox ( "HWOPTION_VIDEO_DEFAULT_COMBO_OPEN" );			
            RegisterControl ( pComboBox );
			m_pComboBoxDefaultOpen = pComboBox;

			pComboBox = new CBasicComboBox;
			pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, HWOPTION_VIDEO_RESOLUTION_COMBO_OPEN );
			pComboBox->CreateBaseComboBox ( "HWOPTION_VIDEO_RESOLUTION_COMBO_OPEN" );			
            RegisterControl ( pComboBox );
			m_pComboBoxResolutionOpen = pComboBox;

			pComboBox = new CBasicComboBox;
			pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, HWOPTION_VIDEO_SHADOW_COMBO_OPEN );
			pComboBox->CreateBaseComboBox ( "HWOPTION_VIDEO_SHADOW_COMBO_OPEN" );			
            RegisterControl ( pComboBox );
			m_pComboBoxShadowOpen = pComboBox;

			pComboBox = new CBasicComboBox;
			pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, HWOPTION_VIDEO_DETAIL_COMBO_OPEN );
			pComboBox->CreateBaseComboBox ( "HWOPTION_VIDEO_DETAIL_COMBO_OPEN" );			
            RegisterControl ( pComboBox );
			m_pComboBoxCharDetailOpen = pComboBox;

			pComboBox = new CBasicComboBox;
			pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, HWOPTION_VIDEO_SIGHT_COMBO_OPEN );
			pComboBox->CreateBaseComboBox ( "HWOPTION_VIDEO_SIGHT_COMBO_OPEN" );			
            RegisterControl ( pComboBox );
			m_pComboBoxSightOpen = pComboBox;
		}

		//	버튼
		{
			m_pBuffButton = CreateFlipButton ( "HWOPTION_VIDEO_BUFF_BUTTON", "HWOPTION_VIDEO_BUFF_BUTTON_F", HWOPTION_VIDEO_BUFF_BUTTON );
			m_pShadowlandButton = CreateFlipButton ( "HWOPTION_VIDEO_SHADOWLAND_BUTTON", "HWOPTION_VIDEO_SHADOWLAND_BUTTON_F", HWOPTION_VIDEO_SHADOWLAND_BUTTON );
			m_pReflectButton = CreateFlipButton ( "HWOPTION_VIDEO_REFLECT_BUTTON", "HWOPTION_VIDEO_REFLECT_BUTTON_F", HWOPTION_VIDEO_REFLECT_BUTTON );
			m_pRefractButton = CreateFlipButton ( "HWOPTION_VIDEO_REFRACT_BUTTON", "HWOPTION_VIDEO_REFRACT_BUTTON_F", HWOPTION_VIDEO_REFRACT_BUTTON );
			m_pGlowButton = CreateFlipButton ( "HWOPTION_VIDEO_GLOW_BUTTON", "HWOPTION_VIDEO_GLOW_BUTTON_F", HWOPTION_VIDEO_GLOW_BUTTON );
			m_pPostButton = CreateFlipButton ( "HWOPTION_VIDEO_POST_BUTTON", "HWOPTION_VIDEO_POST_BUTTON_F", HWOPTION_VIDEO_POSTPROCESSING_BUTTON );
			m_pFrameLimitButton = CreateFlipButton ( "HWOPTION_VIDEO_FRAME_LIMIT_BUTTON", "HWOPTION_VIDEO_FRAME_LIMIT_BUTTON_F", HWOPTION_VIDEO_FRAME_LIMIT_BUTTON );
		}

		//	롤 오버 ( 언제나 가장 마지막에 뿌려져야합니다. )
		{
			pComboBoxRollOver = new CBasicComboBoxRollOver;
			pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, HWOPTION_VIDEO_DEFAULT_COMBO_ROLLOVER );
			pComboBoxRollOver->CreateBaseComboBoxRollOver ( "HWOPTION_VIDEO_DEFAULT_COMBO_ROLLOVER" );			
			pComboBoxRollOver->SetVisibleSingle ( FALSE );
			RegisterControl ( pComboBoxRollOver );
			m_pComboBoxDefaultRollOver = pComboBoxRollOver;

            pComboBoxRollOver = new CBasicComboBoxRollOver;
			pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, HWOPTION_VIDEO_RESOLUTION_COMBO_ROLLOVER );
			pComboBoxRollOver->CreateBaseComboBoxRollOver ( "HWOPTION_VIDEO_RESOLUTION_COMBO_ROLLOVER" );			
			pComboBoxRollOver->SetVisibleSingle ( FALSE );
			RegisterControl ( pComboBoxRollOver );
			m_pComboBoxResolutionRollOver = pComboBoxRollOver;

			pComboBoxRollOver = new CBasicComboBoxRollOver;
			pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, HWOPTION_VIDEO_SHADOW_COMBO_ROLLOVER );
			pComboBoxRollOver->CreateBaseComboBoxRollOver ( "HWOPTION_VIDEO_SHADOW_COMBO_ROLLOVER" );
			pComboBoxRollOver->SetVisibleSingle ( FALSE );
			RegisterControl ( pComboBoxRollOver );
			m_pComboBoxShadowRollOver = pComboBoxRollOver;

			pComboBoxRollOver = new CBasicComboBoxRollOver;
			pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, HWOPTION_VIDEO_CHARDETAIL_COMBO_ROLLOVER );
			pComboBoxRollOver->CreateBaseComboBoxRollOver ( "HWOPTION_VIDEO_CHARDETAIL_COMBO_ROLLOVER" );
			pComboBoxRollOver->SetVisibleSingle ( FALSE );
			RegisterControl ( pComboBoxRollOver );
			m_pComboBoxCharDetailRollOver = pComboBoxRollOver;

			pComboBoxRollOver = new CBasicComboBoxRollOver;
			pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, HWOPTION_VIDEO_SIGHT_COMBO_ROLLOVER );
			pComboBoxRollOver->CreateBaseComboBoxRollOver ( "HWOPTION_VIDEO_SIGHT_COMBO_ROLLOVER" );
			pComboBoxRollOver->SetVisibleSingle ( FALSE );
			RegisterControl ( pComboBoxRollOver );
			m_pComboBoxSightRollOver = pComboBoxRollOver;
		}
	}
}

CBasicButton* CBasicVideoOption::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::RADIO_FLIP );
	pButton->SetControlNameEx ( szButton );
	RegisterControl ( pButton );
	return pButton;
}

CBasicTextBox* CBasicVideoOption::CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );
	return pStaticText;
}

void CBasicVideoOption::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( m_RollOverID == NO_ID )
	{
		CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	}
	else
	{
		ResetMessageEx ();

		CUIControl::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		CUIControl* pControl = m_ControlContainer.FindControl ( m_RollOverID );
		if ( !pControl )
		{
			GASSERT ( 0 && "심각한 오류, 노드가 널입니다." );
			return ;
		}

		pControl->Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		DWORD dwControlMsg = pControl->GetMessageEx ();
		NS_UIDEBUGSET::BlockBegin ();
		if ( dwControlMsg ) TranslateUIMessage ( m_RollOverID, dwControlMsg );
		NS_UIDEBUGSET::BlockEnd ();

		//	스크롤바에 관계된 메시지가 아니고		
		if ( !(dwControlMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL) )
		{
			DWORD dwMsg = GetMessageEx ();
			if ( dwMsg & UIMSG_LB_UP )
			{
				if ( !m_bFirstLBUP )
				{
					m_RollOverID = NO_ID;
					pControl->SetVisibleSingle ( FALSE );					
				}
				m_bFirstLBUP = FALSE;
			}
		}
	}

	//CDebugSet::ToView ( 1, 18, "%d", m_RollOverID );
}

void CBasicVideoOption::LoadResolution ()
{
	m_dwScrWidth = DXPARAMSET::GetInstance().m_dwScrWidth;
	m_dwScrHeight = DXPARAMSET::GetInstance().m_dwScrHeight;
	m_emScrFormat = DXPARAMSET::GetInstance().m_emScrFormat;
	SetTextResolutionCombo( m_dwScrWidth, m_dwScrHeight, /*DXPARAMSET::GetInstance().m_uScrRefreshHz,*/ m_emScrFormat);
}

void CBasicVideoOption::LoadShadow ()
{
	m_dwShadowChar = DXPARAMSET::GetInstance().m_dwShadowChar;
	SetTextShadowCharCombo( m_dwShadowChar );
}

void CBasicVideoOption::LoadSkinDetail ()
{
	m_dwSkinDetail = DXPARAMSET::GetInstance().m_dwSkinDetail;
	SetTextSkinDetailCombo( m_dwSkinDetail );
}

void CBasicVideoOption::LoadSight ()
{
	m_dwSight = DXPARAMSET::GetInstance().m_dwFogRange;
	SetTextSightCombo( m_dwSight );
}

void CBasicVideoOption::LoadDefaultOption()
{
	m_dwVideoLevel = DXPARAMSET::GetInstance().m_dwVideoLevel;
	SetTextDefaultOptionCombo( m_dwVideoLevel );
}

void CBasicVideoOption::LoadCurrentOption()
{
	LoadComboData();		// 기본 콤보 스트링 만들기
	LoadDefaultOption();
	LoadResolution();		// 현재 해상도 얻어오기
							// 현재 주사율 얻어오기
	LoadShadow();			// 그림자 상태
    LoadSkinDetail();		// 스킨 디테일
	LoadSight();			// 시야 상태
	LoadBuffButton();		// 버프
	LoadShadowlandButton();	// 지면 그림자
	LoadReflectButton();	// 실시간 반사
	LoadRefractButton();	// 왜곡
	LoadGlowButton();		// 글로우
	LoadPostButton();		// 뽀샤시
	LoadFrameLimitButton(); // 프레임제한
}

void CBasicVideoOption::LoadBuffButton()
{
	m_bBuff = DXPARAMSET::GetInstance().m_bBuff;
	m_pBuffButton->SetFlip ( m_bBuff );
}

void	CBasicVideoOption::LoadShadowlandButton ()
{
	m_bShadowLand = DXPARAMSET::GetInstance().m_bShadowLand;
	m_pShadowlandButton->SetFlip ( m_bShadowLand );
}

void	CBasicVideoOption::LoadReflectButton ()
{
	m_bRealReflect = DXPARAMSET::GetInstance().m_bRealReflect;
	m_pReflectButton->SetFlip ( m_bRealReflect );
}

void	CBasicVideoOption::LoadRefractButton ()
{
	m_bRefract = DXPARAMSET::GetInstance().m_bRefract;
	m_pRefractButton->SetFlip ( m_bRefract );
}

void	CBasicVideoOption::LoadGlowButton ()
{
	m_bGlow = DXPARAMSET::GetInstance().m_bGlow;
	m_pGlowButton->SetFlip ( m_bGlow );
}

void	CBasicVideoOption::LoadPostButton()
{
	m_bPost = DXPARAMSET::GetInstance().m_bPost;
	m_pPostButton->SetFlip( m_bPost );
}

void	CBasicVideoOption::LoadFrameLimitButton()
{
	m_bFrameLimit = DXPARAMSET::GetInstance().m_bFrameLimit;
	m_pFrameLimitButton->SetFlip( m_bFrameLimit );
}

void	CBasicVideoOption::LoadComboData ()
{	
	CString strTemp;

	{	// 기본설정
		m_pComboBoxDefaultRollOver->ClearText ();
		for ( int i = 0; i < DEFAULT_OPTION_SIZE; i++ )
		{
			strTemp = ID2GAMEWORD ( "HWOPTION_DEFAULT_OPTION", i );
			m_pComboBoxDefaultRollOver->AddText ( strTemp );
		}
	}

	{	//	그림자
		m_pComboBoxShadowRollOver->ClearText ();
		for ( int i = 0; i < SHADOW_NSIZE; i++ )
		{
			strTemp = ID2GAMEWORD ( "HWOPTION_VIDEO_SHADOW", i );
			m_pComboBoxShadowRollOver->AddText ( strTemp );
		}
	}

	{	//	스킨 디테일
		m_pComboBoxCharDetailRollOver->ClearText ();
		for ( int i = 0; i < SKD_NSIZE; i++ )
		{
			strTemp = ID2GAMEWORD ( "HWOPTION_VIDEO_SKINDETAIL", i );
			m_pComboBoxCharDetailRollOver->AddText ( strTemp );
		}
	}

	{	//	시야
		m_pComboBoxSightRollOver->ClearText ();
		for ( int i = 0; i < SKD_NSIZE; i++ )
		{
			strTemp = ID2GAMEWORD ( "HWOPTION_VIDEO_SIGHT", i );
			m_pComboBoxSightRollOver->AddText ( strTemp );
		}
	}

	CD3DApplication* const pD3DApp = DxGlobalStage::GetInstance().GetD3DApp ();
	if ( !pD3DApp )
	{
		GASSERT ( pD3DApp && "디바이스가 생성되지 않았습니다." );
		return ;
	}
	D3DDeviceInfo* const pDeviceInfo = pD3DApp->GetCurDeviceInfo();

	{	//	해상도
		m_pComboBoxResolutionRollOver->ClearText ();
		for ( DWORD i = 0; i < pDeviceInfo->dwNumModes; i++ )
		{	
			DWORD dwWidth  = pDeviceInfo->modes[i].Width;
			DWORD dwHeight = pDeviceInfo->modes[i].Height;
			DWORD dwFormat = 0;

			switch ( pDeviceInfo->modes[i].Format )
			{
			case D3DFMT_R8G8B8:
			case D3DFMT_A8R8G8B8:
			case D3DFMT_X8R8G8B8:
				dwFormat = 32;
				break;
			case D3DFMT_R5G6B5:
			case D3DFMT_X1R5G5B5:
			case D3DFMT_A1R5G5B5:
				dwFormat = 16;
				break;
			}

			strTemp.Format ( "%d%c%d%c%d", dwWidth, cRESOLUTION_SEPERATOR, dwHeight, cRESOLUTION_SEPERATOR, dwFormat );			
			m_pComboBoxResolutionRollOver->AddText ( strTemp );
		}
	}

	//int index = 0;
	//DEVMODE dm;
	//ZeroMemory(&dm, sizeof(dm));
	//dm.dmSize = sizeof(dm);

	//m_pComboBoxResolutionRollOver->ClearText ();

	//while( EnumDisplaySettings(NULL, index++, &dm))
	//{
	//	DWORD dwWidth  = dm.dmPelsWidth;
	//	DWORD dwHeight = dm.dmPelsHeight;
	//	DWORD dwFormat = dm.dmBitsPerPel;

	//	if( dwWidth >= 800 && dwHeight >= 600 && dwFormat >= 16 )
	//	{
	//		strTemp.Format ( "%d%c%d%c%d", dwWidth, cRESOLUTION_SEPERATOR, dwHeight, cRESOLUTION_SEPERATOR, dwFormat );			
	//		m_pComboBoxResolutionRollOver->AddText ( strTemp );
	//	}

	//	ZeroMemory(&dm, sizeof(dm));
	//	dm.dmSize = sizeof(dm);
	//}

	//{	//	주파수
		//UINT* pHertz = NULL;
		//pDeviceInfo->modes[m_dwCurModeIndex].RefreshRate.SetHead();
		//while ( !pDeviceInfo->modes[m_dwCurModeIndex].RefreshRate.IsEnd() )
		//{
		//	pHertz = pDeviceInfo->modes[m_dwCurModeIndex].RefreshRate.GetCurrent();
		//	Temp.Format("%d Hz", *pHertz);
		//	((CAdvListBox*)pCont)->AddString(Temp);

		//	pDeviceInfo->modes[m_dwCurModeIndex].RefreshRate.GoNext();
		//}
	//}
}

void CBasicVideoOption::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		LoadCurrentOption();
	}
}

void CBasicVideoOption::SetTextSightCombo( DWORD dwIndex )
{
	if ( 0 <= dwIndex && dwIndex < EMFR_NSIZE )
	{
		CString strTemp( ID2GAMEWORD ( "HWOPTION_VIDEO_SIGHT", dwIndex ) );
		m_pComboBoxSightOpen->SetText ( strTemp );
	}
	else
	{
		GASSERT ( 0 && "스킨 디테일 범위를 넘어섭니다." );
	}
}

void CBasicVideoOption::SetTextSkinDetailCombo( DWORD dwIndex )
{
	if ( 0 <= dwIndex && dwIndex < SKD_NSIZE )
	{
		CString strTemp( ID2GAMEWORD ( "HWOPTION_VIDEO_SKINDETAIL", dwIndex ) );
		m_pComboBoxCharDetailOpen->SetText ( strTemp );
	}
	else
	{
		GASSERT ( 0 && "스킨 디테일 범위를 넘어섭니다." );
	}
}

void CBasicVideoOption::SetTextShadowCharCombo( DWORD dwIndex )
{
	if ( 0 <= dwIndex && dwIndex < SHADOW_NSIZE )
	{
		CString strTemp( ID2GAMEWORD ( "HWOPTION_VIDEO_SHADOW", dwIndex ) );
		m_pComboBoxShadowOpen->SetText ( strTemp );
	}
	else
	{
		GASSERT ( 0 && "그림자 범위를 넘어섭니다." );
	}
}

void CBasicVideoOption::SetTextResolutionCombo( DWORD dwWidth, DWORD dwHeight, EMSCREEN_FORMAT emFormat)
{
	int nBIT(0);

	if ( emFormat == EMSCREEN_F16 ) nBIT = 16;
	else							nBIT = 32;	

	CString strTemp;
	strTemp.Format( "%d%c%d%c%d", dwWidth, cRESOLUTION_SEPERATOR, dwHeight, cRESOLUTION_SEPERATOR, nBIT );
	m_pComboBoxResolutionOpen->SetText ( strTemp );
}

void CBasicVideoOption::SetTextDefaultOptionCombo( DWORD dwIndex )
{
	if ( 0 <= dwIndex && dwIndex < DEFAULT_OPTION_SIZE )
	{
		CString strTemp( ID2GAMEWORD ( "HWOPTION_DEFAULT_OPTION", dwIndex ) );
		m_pComboBoxDefaultOpen->SetText ( strTemp );
		m_dwVideoLevel = dwIndex;
	}
	else
	{
		GASSERT ( 0 && "그림자 범위를 넘어섭니다." );
	}
}

void CBasicVideoOption::SetVideoLevel(int nIndex)
{
	switch(nIndex)
	{
	case 1:
		SetLowLevel();
		break;
	case 2:
		SetMediumLevel();
		break;
	case 3:
		SetHighLevel();
		break;
	default:
		break;
	}
}

void CBasicVideoOption::SetHighLevel()
{
	m_bBuff = true;
	m_bShadowLand = true;
	m_bRealReflect = true;
	m_bRefract = true;
	m_bGlow = true;
	m_bPost = true;

	m_dwShadowChar = 4;
	m_dwSight = 2;
	m_dwSkinDetail = 2;

	m_pBuffButton->SetFlip( m_bBuff );
	m_pShadowlandButton->SetFlip( m_bShadowLand );
	m_pReflectButton->SetFlip( m_bRealReflect );
	m_pRefractButton->SetFlip( m_bRefract );
	m_pGlowButton->SetFlip( m_bGlow );
	m_pPostButton->SetFlip( m_bPost );

	SetTextShadowCharCombo( m_dwShadowChar );
	SetTextSightCombo( m_dwSight );
	SetTextSkinDetailCombo( m_dwSkinDetail );
}

void CBasicVideoOption::SetMediumLevel()
{
	m_bBuff = true;
	m_bShadowLand = true;
	m_bRealReflect = false;
	m_bRefract = false;
	m_bGlow = false;
	m_bPost = false;

	m_dwShadowChar = 2;
	m_dwSight = 1;
	m_dwSkinDetail = 1;

	m_pBuffButton->SetFlip( m_bBuff );
	m_pShadowlandButton->SetFlip( m_bShadowLand );
	m_pReflectButton->SetFlip( m_bRealReflect );
	m_pRefractButton->SetFlip( m_bRefract );
	m_pGlowButton->SetFlip( m_bGlow );
	m_pPostButton->SetFlip( m_bPost );

	SetTextShadowCharCombo( m_dwShadowChar );
	SetTextSightCombo( m_dwSight );
	SetTextSkinDetailCombo( m_dwSkinDetail );
}

void CBasicVideoOption::SetLowLevel()
{
	m_bBuff = false;
	m_bShadowLand = false;
	m_bRealReflect = false;
	m_bRefract = false;
	m_bGlow = false;
	m_bPost = false;

	m_dwShadowChar = 1;
	m_dwSight = 0;
	m_dwSkinDetail = 0;

	m_pBuffButton->SetFlip( m_bBuff );
	m_pShadowlandButton->SetFlip( m_bShadowLand );
	m_pReflectButton->SetFlip( m_bRealReflect );
	m_pRefractButton->SetFlip( m_bRefract );
	m_pGlowButton->SetFlip( m_bGlow );
	m_pPostButton->SetFlip( m_bPost );

	SetTextShadowCharCombo( m_dwShadowChar );
	SetTextSightCombo( m_dwSight );
	SetTextSkinDetailCombo( m_dwSkinDetail );


}