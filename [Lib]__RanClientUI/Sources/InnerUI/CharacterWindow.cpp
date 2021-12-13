#include "pch.h"
#include "./CharacterWindow.h"

#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__EngineUI/Sources/BasicProgressBar.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "../[Lib]__Engine/Sources/DxTools/d3dfont.h"

#include "./BasicTextButton.h"
#include "./UITextControl.h"
#include "./GameTextControl.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Client/GLGaeaClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCharacterWindow::CCharacterWindow () :
	m_pNameText ( NULL ),

	m_pEXP ( NULL ),
	m_pEXPText ( NULL ),

	m_pPowButton ( NULL ),
	m_pDexButton ( NULL ),
	m_pSpiButton ( NULL ),
	//m_pIntButton ( NULL ),
	m_pStrButton ( NULL ),
	m_pStaButton ( NULL ),

	m_pPowDiaableButton ( NULL ),
	m_pDexDiaableButton ( NULL ),
	m_pSpiDiaableButton ( NULL ),
	//m_pIntDiaableButton ( NULL ),
	m_pStrDiaableButton ( NULL ),
	m_pStaDiaableButton ( NULL ),

	m_pPowText ( NULL ),
	m_pDexText ( NULL ),
	m_pSpiText ( NULL ),
	//m_pIntText ( NULL ),
	m_pStrText ( NULL ),
	m_pStaText ( NULL ),

	m_pPointDisable ( NULL ),
	m_pPointEnable ( NULL ),
	m_pPointText ( NULL ),

	m_pLevelText ( NULL ),
	m_pDamageText ( NULL ),
	m_pDefenseText ( NULL ),
	m_pReqPaText ( NULL ),
	m_pReqSaText ( NULL ),	
	m_pHPText ( NULL ),
	m_pMPText ( NULL ),
	m_pSPText ( NULL ),
	m_pHitRateText ( NULL ),
	m_pAvoidRateText ( NULL ),

	m_pFireText ( NULL ),
	m_pColdText ( NULL ),
	m_pElectricText ( NULL ),
	m_pPoisonText ( NULL ),
	m_pSpiritText ( NULL ),

	m_pPKStateText ( NULL ),
	m_pGradeText ( NULL ),
	m_pBrightSlider ( NULL ),
	m_pActivityPoint( NULL ),
	m_pAcademy( NULL ),
	m_pDepartment( NULL ),

	m_pMobileText( NULL )
{
}

CCharacterWindow::~CCharacterWindow ()
{
}


CBasicTextBox* CCharacterWindow::CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

void CCharacterWindow::CreateSubControl ()
{
	CD3DFontPar* pFont8Shadow = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	const int nAlignLeft = TEXT_ALIGN_LEFT;
	const int nAlignCenter = TEXT_ALIGN_CENTER_X;		
	const DWORD& dwSilver = NS_UITEXTCOLOR::SILVER;
	const DWORD& dwSubTitle = NS_UITEXTCOLOR::PALEGOLDENROD;

    CBasicTextBox* pTextBox = NULL;

	{	//	이름        
		CreateControl ( "CHARACTER_NAMEBACK_L" );
		CreateControl ( "CHARACTER_NAMEBACK_R" );
		m_pNameText = CreateStaticControl ( "CHARACTER_NAMEBOX", pFont, nAlignCenter );
	}

	{	//	경험치
		m_pEXP = new CBasicProgressBar;
		m_pEXP->CreateSub ( this, "CHARACTER_EXP" );
		m_pEXP->CreateOverImage ( "CHARACTER_EXP_OVERIMAGE" );
		m_pEXP->SetControlNameEx ( "EXP 프로그래스바" );
		RegisterControl ( m_pEXP );

		CreateControl ( "CHARACTER_EXPBACK" );

		m_pEXPText = CreateStaticControl ( "CHARACTER_EXPNUMBER_TEXTBOX", pFont8Shadow, nAlignCenter );
	}
	
	{	//	Stats
		pTextBox = CreateStaticControl ( "CHARACTER_EXP_STATIC", pFont, nAlignCenter );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_EXP_STATIC"), dwSilver );		

		m_pPowTextStatic = CreateStaticControl ( "CHARACTER_POW_STATIC", pFont, nAlignLeft );
		m_pPowTextStatic->AddText ( ID2GAMEWORD("CHARACTER_STAT_STATIC", 0), dwSilver );

		m_pDexTextStatic = CreateStaticControl ( "CHARACTER_DEX_STATIC", pFont, nAlignLeft );
		m_pDexTextStatic->AddText ( ID2GAMEWORD("CHARACTER_STAT_STATIC", 1), dwSilver );

		m_pSpiTextStatic = CreateStaticControl ( "CHARACTER_SPI_STATIC", pFont, nAlignLeft );
		m_pSpiTextStatic->AddText ( ID2GAMEWORD("CHARACTER_STAT_STATIC", 2), dwSilver );

		//m_pIntTextStatic = CreateStaticControl ( "CHARACTER_INT_STATIC", pFont, nAlignLeft );
		//m_pIntTextStatic->AddText ( ID2GAMEWORD("CHARACTER_STAT_STATIC", 3), dwSilver );
		//m_pIntTextStatic->SetVisibleSingle ( FALSE );

		m_pStrTextStatic = CreateStaticControl ( "CHARACTER_STR_STATIC", pFont, nAlignLeft );
		m_pStrTextStatic->AddText ( ID2GAMEWORD("CHARACTER_STAT_STATIC", 4), dwSilver );

		m_pStaTextStatic = CreateStaticControl ( "CHARACTER_STA_STATIC", pFont, nAlignLeft );
		m_pStaTextStatic->AddText ( ID2GAMEWORD("CHARACTER_STAT_STATIC", 5), dwSilver );
	
		m_pPowDiaableButton = CreateControl ( "CHARACTER_POW_BUTTON_DISABLE" );
		m_pPowButton = CreateFlipButton ( "CHARACTER_POW_BUTTON", "CHARACTER_POW_BUTTON_F", CHARACTER_POW_BUTTON, CBasicButton::MOUSEIN_FLIP );		

		m_pDexDiaableButton = CreateControl ( "CHARACTER_DEX_BUTTON_DISABLE" );
		m_pDexButton = CreateFlipButton ( "CHARACTER_DEX_BUTTON", "CHARACTER_DEX_BUTTON_F", CHARACTER_DEX_BUTTON, CBasicButton::MOUSEIN_FLIP );

		m_pSpiDiaableButton = CreateControl ( "CHARACTER_SPI_BUTTON_DISABLE" );
		m_pSpiButton = CreateFlipButton ( "CHARACTER_SPI_BUTTON", "CHARACTER_SPI_BUTTON_F", CHARACTER_SPI_BUTTON, CBasicButton::MOUSEIN_FLIP );
		
		//m_pIntDiaableButton = CreateControl ( "CHARACTER_INT_BUTTON_DISABLE" );
		//m_pIntDiaableButton->SetVisibleSingle ( FALSE );
		//m_pIntButton = CreateFlipButton ( "CHARACTER_INT_BUTTON", "CHARACTER_INT_BUTTON_F", CHARACTER_INT_BUTTON, CBasicButton::MOUSEIN_FLIP );
		//m_pIntButton->SetVisibleSingle ( FALSE );

		m_pStrDiaableButton = CreateControl ( "CHARACTER_STR_BUTTON_DISABLE" );
		m_pStrButton = CreateFlipButton ( "CHARACTER_STR_BUTTON", "CHARACTER_STR_BUTTON_F", CHARACTER_STR_BUTTON, CBasicButton::MOUSEIN_FLIP );

		m_pStaDiaableButton = CreateControl ( "CHARACTER_STA_BUTTON_DISABLE" );
		m_pStaButton = CreateFlipButton ( "CHARACTER_STA_BUTTON", "CHARACTER_STA_BUTTON_F", CHARACTER_STA_BUTTON, CBasicButton::MOUSEIN_FLIP );

		m_pPowText = CreateStaticControl ( "CHARACTER_POW_TEXT", pFont8Shadow, nAlignLeft );
		m_pDexText = CreateStaticControl ( "CHARACTER_DEX_TEXT", pFont8Shadow, nAlignLeft );
		m_pSpiText = CreateStaticControl ( "CHARACTER_SPI_TEXT", pFont8Shadow, nAlignLeft );
		//m_pIntText = CreateStaticControl ( "CHARACTER_INT_TEXT", pFont8Shadow, nAlignLeft );
		//m_pIntText->SetVisibleSingle ( FALSE );
		m_pStrText = CreateStaticControl ( "CHARACTER_STR_TEXT", pFont8Shadow, nAlignLeft );
		m_pStaText = CreateStaticControl ( "CHARACTER_STA_TEXT", pFont8Shadow, nAlignLeft );
	}

	{	//	가용포인트
		m_pPointDisable = CreateControl ( "CHARACTER_POINT_DISABLE" );
		m_pPointEnable = CreateControl ( "CHARACTER_POINT_ENABLE" );

		CreateControl ( "CHARACTER_POINT_BACK" );
		m_pPointText = CreateStaticControl ( "CHARACTER_POINT_TEXT", pFont8Shadow, nAlignCenter );
	}

	{	//	Status
        CreateControl ( "CHARACTER_STATUS_BACK_L" );
		CreateControl ( "CHARACTER_STATUS_BACK_R" );
		pTextBox = CreateStaticControl ( "CHARACTER_STATUS_STATIC_TITLE", pFont, nAlignCenter );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 0), dwSubTitle );

		pTextBox = CreateStaticControl ( "CHARACTER_LEVEL_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 1), dwSilver );

		pTextBox = CreateStaticControl ( "CHARACTER_DAMAGE_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 2), dwSilver );

		pTextBox = CreateStaticControl ( "CHARACTER_DEFENSE_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 3), dwSilver );

		pTextBox = CreateStaticControl ( "CHARACTER_REQPA_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 4), dwSilver );

		pTextBox = CreateStaticControl ( "CHARACTER_REQSA_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 5), dwSilver );

		pTextBox = CreateStaticControl ( "CHARACTER_REQMA_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 11), dwSilver );

		pTextBox = CreateStaticControl ( "CHARACTER_HP_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 6), dwSilver );

		pTextBox = CreateStaticControl ( "CHARACTER_MP_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 7), dwSilver );

		pTextBox = CreateStaticControl ( "CHARACTER_SP_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 8), dwSilver );

		pTextBox = CreateStaticControl ( "CHARACTER_HITRATE_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 9), dwSilver );

		pTextBox = CreateStaticControl ( "CHARACTER_AVOIDRATE_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 10), dwSilver );

		m_pPremiumTextStatic = CreateStaticControl ( "CHARACTER_PREMIUM_STATIC", pFont, nAlignLeft );
		m_pPremiumTextStatic->AddText ( ID2GAMEWORD("CHARACTER_STATUS_STATIC", 12), dwSilver );

		m_pLevelText = CreateStaticControl ( "CHARACTER_LEVEL_TEXT", pFont8Shadow, nAlignLeft );
		m_pDamageText = CreateStaticControl ( "CHARACTER_DAMAGE_TEXT", pFont8Shadow, nAlignLeft );
		m_pDefenseText = CreateStaticControl ( "CHARACTER_DEFENSE_TEXT", pFont8Shadow, nAlignLeft );
		m_pReqPaText = CreateStaticControl ( "CHARACTER_REQPA_TEXT", pFont8Shadow, nAlignLeft );
		m_pReqSaText = CreateStaticControl ( "CHARACTER_REQSA_TEXT", pFont8Shadow, nAlignLeft );
		m_pReqMaText = CreateStaticControl ( "CHARACTER_REQMA_TEXT", pFont8Shadow, nAlignLeft );
		
		m_pHPText = CreateStaticControl ( "CHARACTER_HP_TEXT", pFont8Shadow, nAlignLeft );
		m_pMPText = CreateStaticControl ( "CHARACTER_MP_TEXT", pFont8Shadow, nAlignLeft );
		m_pSPText = CreateStaticControl ( "CHARACTER_SP_TEXT", pFont8Shadow, nAlignLeft );
		m_pHitRateText = CreateStaticControl ( "CHARACTER_HITRATE_TEXT", pFont8Shadow, nAlignLeft );
		m_pAvoidRateText = CreateStaticControl ( "CHARACTER_AVOIDRATE_TEXT", pFont8Shadow, nAlignLeft );
		m_pPremiumText = CreateStaticControl ( "CHARACTER_PREMIUM_TEXT", pFont8Shadow, nAlignLeft, CHARACTER_PREMIUM_TEXT );
	}

	{	//	저항력
        CreateControl ( "CHARACTER_RESI_BACK_L" );
		CreateControl ( "CHARACTER_RESI_BACK_R" );
		pTextBox = CreateStaticControl ( "CHARACTER_RESI_STATIC_TITLE", pFont, nAlignCenter );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_RESI_STATIC", 0), dwSubTitle );

		pTextBox = CreateStaticControl ( "CHARACTER_FIRE_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_RESI_STATIC", 1), dwSilver );

		pTextBox = CreateStaticControl ( "CHARACTER_COLD_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_RESI_STATIC", 2), dwSilver );

		pTextBox = CreateStaticControl ( "CHARACTER_ELECTRIC_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_RESI_STATIC", 3), dwSilver );

		pTextBox = CreateStaticControl ( "CHARACTER_POISON_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_RESI_STATIC", 4), dwSilver );

		pTextBox = CreateStaticControl ( "CHARACTER_SPIRIT_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_RESI_STATIC", 5), dwSilver );

		m_pFireText = CreateStaticControl ( "CHARACTER_FIRE_TEXT", pFont8Shadow, nAlignLeft );
		m_pColdText = CreateStaticControl ( "CHARACTER_COLD_TEXT", pFont8Shadow, nAlignLeft );
		m_pElectricText = CreateStaticControl ( "CHARACTER_ELECTRIC_TEXT", pFont8Shadow, nAlignLeft );
		m_pPoisonText = CreateStaticControl ( "CHARACTER_POISON_TEXT", pFont8Shadow, nAlignLeft );
		m_pSpiritText = CreateStaticControl ( "CHARACTER_SPIRIT_TEXT", pFont8Shadow, nAlignLeft );
	}

	{	//	사회성향
        CreateControl ( "CHARACTER_SOCIAL_BACK_L" );
		CreateControl ( "CHARACTER_SOCIAL_BACK_R" );
		pTextBox = CreateStaticControl ( "CHARACTER_SOCIAL_STATIC_TITLE", pFont, nAlignCenter );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_SOCIAL_STATIC", 0), dwSubTitle );

		pTextBox = CreateStaticControl ( "CHARACTER_ACADEMY_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_SOCIAL_STATIC", 1), dwSilver );

		pTextBox = CreateStaticControl ( "CHARACTER_ACTIVITYPOINT_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_SOCIAL_STATIC", 2), dwSilver );

		pTextBox = CreateStaticControl ( "CHARACTER_PKSTATE_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_SOCIAL_STATIC", 3), dwSilver );


		pTextBox = CreateStaticControl ( "CHARACTER_DEPARTMENT_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_SOCIAL_STATIC", 4), dwSilver );

		pTextBox = CreateStaticControl ( "CHARACTER_BRIGHT_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_SOCIAL_STATIC", 5), dwSilver );

		pTextBox = CreateStaticControl ( "CHARACTER_GRADE_STATIC", pFont, nAlignLeft );
		pTextBox->AddText ( ID2GAMEWORD("CHARACTER_SOCIAL_STATIC", 6), dwSilver );

		m_pAcademy = CreateStaticControl ( "CHARACTER_ACADEMY_TEXT", pFont, nAlignLeft );		
		m_pActivityPoint = CreateStaticControl ( "CHARACTER_ACTIVITYPOINT_TEXT", pFont8Shadow, nAlignLeft );
		m_pPKStateText = CreateStaticControl ( "CHARACTER_PKSTATE_TEXT", pFont, nAlignLeft );

		m_pDepartment = CreateStaticControl ( "CHARACTER_DEPARTMENT_TEXT", pFont, nAlignLeft );
		m_pBrightFrame = CreateControl ( "CHARACTER_BRIGHTBAR" );		
		m_pBrightSlider = CreateControl ( "CHARACTER_BRIGHTBAR_THUMB" );
		m_pGradeText = CreateStaticControl ( "CHARACTER_GRADE_TEXT", pFont, nAlignLeft );

		CreateControl ( "CHARACTER_BRIGHTBAR_DUMMY", CHARACTER_BIRGHTER );

#if defined(KRT_PARAM)
		CBasicTextButton * pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, CHARACTER_MOBILE_BUTTON );
		pButton->CreateBaseButton( _T("CHARACTER_MOBILE_BUTTON"), 
									CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, 
									(char*)ID2GAMEWORD("CHARACTER_MOBILE_BUTTON") );
		RegisterControl( pButton );

		m_pMobileText = CreateStaticControl ( "CHARACTER_MOBILE_TEXT", pFont8Shadow, nAlignLeft );
#endif
	}

	{
		CUIControl	TempControl;
		TempControl.Create ( 1, "CHARACTER_SCHOOL_MARK" );

		const UIRECT& rcLocalPos = TempControl.GetLocalPos ();

		CString strSchoolMark[] = 
		{
			"NAME_DISPLAY_SCHOOL1",
			"NAME_DISPLAY_SCHOOL2",
			"NAME_DISPLAY_SCHOOL3"
		};

		for ( int i = 0; i < nSCHOOL_MARK; ++i )
		{
			m_pSchoolMark[i] = CreateControl ( strSchoolMark[i].GetString() );
			m_pSchoolMark[i]->SetLocalPos ( rcLocalPos );
			m_pSchoolMark[i]->SetVisibleSingle ( FALSE );
		}
	}
}

void CCharacterWindow::SetGlobalPos(const D3DXVECTOR2& vPos)
{
	CUIGroup::SetGlobalPos( vPos );

	const GLCHARLOGIC& sCharData = GLGaeaClient::GetInstance().GetCharacterLogic ();	
	
	SetBright( sCharData );
}