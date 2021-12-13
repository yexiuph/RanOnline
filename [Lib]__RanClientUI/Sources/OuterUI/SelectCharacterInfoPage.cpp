#include "pch.h"
#include "SelectCharacterInfoPage.h"
#include "d3dfont.h"
#include "GameTextControl.h"
#include "UITextControl.h"
#include "DxGlobalStage.h"
#include "GLGaeaClient.h"

#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__EngineUI/Sources/BasicProgressBar.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSelectCharacterInfoPage::CSelectCharacterInfoPage ()
	: m_pInfo0Box(NULL)
	, m_pInfo1Box(NULL)
	, m_pInfo2Box(NULL)
	, m_pBrightFrame(NULL)
	, m_pBrightSlider(NULL)
	, m_pEXP(NULL)
	, m_pEXPText(NULL)
	, m_pCharInfoBack(NULL)
{
}

CSelectCharacterInfoPage::~CSelectCharacterInfoPage ()
{
}

void CSelectCharacterInfoPage::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	
	CreateControl ( "SELECT_CHARACTER_INFO_BACK" );
	m_pInfo0Box = CreateStaticControl( "SELECT_CHARACTER_INFO_0", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_CENTER_X );
	m_pInfo1Box = CreateStaticControl( "SELECT_CHARACTER_INFO_1", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_CENTER_X );
	m_pInfo2Box = CreateStaticControl( "SELECT_CHARACTER_INFO_2", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_CENTER_X );

	{
		CBasicTextBox* pTextBox = NULL;
		pTextBox = CreateStaticControl ( "SELECT_CHARACTER_EXP_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("SELECT_CHARACTER_STATIC", 0)  );
		pTextBox = CreateStaticControl ( "SELECT_CHARACTER_BRIGHT_STATIC", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT );
		pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ("SELECT_CHARACTER_STATIC", 1) );
	}

	{
		CD3DFontPar* pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

		CBasicProgressBar * pBar = new CBasicProgressBar;
		pBar->CreateSub ( this, "SELECT_CHARACTER_EXP" );
		pBar->CreateOverImage ( "SELECT_CHARACTER_EXP_OVERIMAGE" );
		pBar->SetControlNameEx ( "EXP 프로그래스바" );
		RegisterControl ( pBar );
		m_pEXP = pBar;

		m_pEXPText = CreateStaticControl( "SELECT_CHARACTER_EXP_TEXT", pFont8, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_RIGHT );
	}

	m_pBrightFrame = CreateControl ( "SELECT_CHARACTER_BRIGHTBAR" );        
	m_pBrightSlider = CreateControl ( "SELECT_CHARACTER_BRIGHTBAR_THUMB" );
}

void	CSelectCharacterInfoPage::ResetAll ()
{
	if ( m_pInfo0Box ) m_pInfo0Box->ClearText();
	if ( m_pInfo1Box ) m_pInfo1Box->ClearText();
	if ( m_pInfo2Box ) m_pInfo2Box->ClearText();

	if ( m_pEXPText ) m_pEXPText->ClearText();
	if ( m_pEXP ) m_pEXP->SetPercent( 0.0f );

	if( m_pBrightFrame && m_pBrightSlider)
	{
		const UIRECT& rcFramePos = m_pBrightFrame->GetLocalPos ();
		const UIRECT& rcThumbPos = m_pBrightSlider->GetLocalPos ();

		float fMOVE = (rcFramePos.sizeX - rcThumbPos.sizeX) * 0.5f;
		float fLeft = rcFramePos.left + fMOVE;

		m_pBrightSlider->SetLocalPos( D3DXVECTOR2( fLeft, rcThumbPos.top ) );
	}

	m_pCharInfoBack = NULL;
}

void CSelectCharacterInfoPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIOuterWindow::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	SCHARINFO_LOBBY* pCharInfo = DxGlobalStage::GetInstance().GetLobyStage()->GetSelectCharInfo();
	if( pCharInfo )
	{
		if( pCharInfo != m_pCharInfoBack )
		{
			if( m_pInfo0Box && m_pInfo1Box )
			{
				const CString strClassName = COMMENT::CHARCLASS[CharClassToIndex(pCharInfo->m_emClass)].c_str();

				CString strCombine;

				strCombine.Format( "%s Lv.%d", pCharInfo->m_szName, pCharInfo->m_wLevel );
				m_pInfo0Box->SetOneLineText( strCombine );

				strCombine.Format( "%s/%s", ID2GAMEWORD("ACADEMY_NAME", pCharInfo->m_wSchool), strClassName );
				m_pInfo1Box->SetOneLineText( strCombine );

				CString strName( GLGaeaClient::GetInstance().GetMapName( pCharInfo->m_sSaveMapID ) );
				if( strName == _T("(null)") ) strName.Empty();

				strCombine.Format( "%s:%s", ID2GAMEWORD("SELECT_CHARACTER_STATIC", 2), strName );
				m_pInfo2Box->SetOneLineText( strCombine );
			}

			if( m_pEXP )
			{
				__int64 lnNow = pCharInfo->m_sExperience.lnNow;
				__int64 lnMax = pCharInfo->m_sExperience.lnMax;
				if ( !lnMax ) lnMax = 1;
				float fPercent = float(lnNow) / float(lnMax);
				m_pEXP->SetPercent ( fPercent );

				if( m_pEXPText )
				{
					CString strText;
					float fPercent(0.0f);
					if( lnNow )
					{
						fPercent = float(lnNow) / float(lnMax);
					}

					strText.Format( "%.2f%%", fPercent * 100.0f );
					m_pEXPText->SetOneLineText( strText, NS_UITEXTCOLOR::WHITE );
				}
			}

			if( m_pBrightFrame && m_pBrightSlider)
			{
				const int nMIN = -100;
				const int nMAX = 100;
				const int nRANGE = nMAX - nMIN;

				int nPercent = pCharInfo->m_nBright;
				// 이미지가 반대로 뒤집혀있어서.
				nPercent = -(nPercent);
				if ( nPercent < nMIN ) nPercent = nMIN;
				if ( nPercent > nMAX ) nPercent = nMAX;

				nPercent += nMAX;

				float fPercent = static_cast<float>(nPercent) / static_cast<float>(nRANGE);

				const UIRECT& rcFramePos = m_pBrightFrame->GetLocalPos();
				const UIRECT& rcThumbPos = m_pBrightSlider->GetLocalPos();

				float fMOVE = (rcFramePos.sizeX - rcThumbPos.sizeX) * fPercent;
				float fLeft = rcFramePos.left + fMOVE;

				m_pBrightSlider->SetLocalPos( D3DXVECTOR2( fLeft, rcThumbPos.top ) );
			}

			m_pCharInfoBack = pCharInfo;
		}
	}
	else
	{
		ResetAll();
	}
}