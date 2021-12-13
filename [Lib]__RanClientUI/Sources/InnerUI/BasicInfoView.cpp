#include "pch.h"
#include "BasicInfoView.h"

#include "../[Lib]__EngineUI/Sources/BasicProgressBar.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "d3dfont.h"

#include "GLGaeaClient.h"
#include "UITextControl.h"
#include "InnerInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBasicInfoView::CBasicInfoView () :
	m_pHP ( NULL ),
	m_pMP ( NULL ),
	m_pSP ( NULL ),
	m_pEXP ( NULL ),
	m_pHPText ( NULL ),
	m_pMPText ( NULL ),
	m_pSPText ( NULL ),
	m_pEXPText ( NULL ),
	m_bFirstGap ( FALSE ),
	m_wHPBACK ( USHRT_MAX ),
	m_wMPBACK ( USHRT_MAX ),
	m_wSPBACK ( USHRT_MAX ),
	m_lEXPBACK( -1 )
{
}

CBasicInfoView::~CBasicInfoView ()
{
}

void CBasicInfoView::CreateSubControl ()
{
	CD3DFontPar* pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

	m_pHP = new CBasicProgressBar;
	m_pHP->CreateSub ( this, "BASIC_INFO_VIEW_HP" );
	m_pHP->CreateOverImage ( "BASIC_INFO_VIEW_HP_OVERIMAGE" );
	m_pHP->SetControlNameEx ( "HP 프로그래스바" );
	RegisterControl ( m_pHP );

	m_pMP = new CBasicProgressBar;
	m_pMP->CreateSub ( this, "BASIC_INFO_VIEW_MP" );
	m_pMP->CreateOverImage ( "BASIC_INFO_VIEW_MP_OVERIMAGE" );
	m_pMP->SetControlNameEx ( "MP 프로그래스바" );
	RegisterControl ( m_pMP );

	m_pSP = new CBasicProgressBar;
	m_pSP->CreateSub ( this, "BASIC_INFO_VIEW_SP" );
	m_pSP->CreateOverImage ( "BASIC_INFO_VIEW_SP_OVERIMAGE" );
	m_pSP->SetControlNameEx ( "SP 프로그래스바" );
	RegisterControl ( m_pSP );

	m_pEXP = new CBasicProgressBar;
	m_pEXP->CreateSub ( this, "BASIC_INFO_VIEW_EXP" );
	m_pEXP->CreateOverImage ( "BASIC_INFO_VIEW_EXP_OVERIMAGE" );
	m_pEXP->SetControlNameEx ( "EXP 프로그래스바" );
	RegisterControl ( m_pEXP );

	m_pHPText = new CBasicTextBox;
	m_pHPText->CreateSub ( this, "BASIC_INFO_VIEW_HP_TEXT", UI_FLAG_DEFAULT, HP_TEXT );
    m_pHPText->SetFont ( pFont8 );
	m_pHPText->SetTextAlign ( TEXT_ALIGN_RIGHT );
	m_pHPText->SetControlNameEx ( "HP 텍스트박스" );	
	RegisterControl ( m_pHPText );

	m_pMPText = new CBasicTextBox;
	m_pMPText->CreateSub ( this, "BASIC_INFO_VIEW_MP_TEXT", UI_FLAG_DEFAULT, MP_TEXT );
    m_pMPText->SetFont ( pFont8 );
	m_pMPText->SetTextAlign ( TEXT_ALIGN_RIGHT );
	m_pMPText->SetControlNameEx ( "MP 텍스트박스" );	
	RegisterControl ( m_pMPText );

	m_pSPText = new CBasicTextBox;
	m_pSPText->CreateSub ( this, "BASIC_INFO_VIEW_SP_TEXT", UI_FLAG_DEFAULT, SP_TEXT );
    m_pSPText->SetFont ( pFont8 );
	m_pSPText->SetTextAlign ( TEXT_ALIGN_RIGHT );
	m_pSPText->SetControlNameEx ( "SP 텍스트박스" );	
	RegisterControl ( m_pSPText );

	m_pEXPText = new CBasicTextBox;
	m_pEXPText->CreateSub ( this, "BASIC_INFO_VIEW_EXP_TEXT", UI_FLAG_DEFAULT, EXP_TEXT );
    m_pEXPText->SetFont ( pFont8 );
	m_pEXPText->SetTextAlign ( TEXT_ALIGN_RIGHT );
	m_pEXPText->SetControlNameEx ( "EXP 텍스트박스" );	
	RegisterControl ( m_pEXPText );
}

void CBasicInfoView::SetHP ( WORD wNOW, WORD wMAX )
{
	GASSERT( m_pHPText && m_pHP );

	if ( m_wHPBACK != wNOW )
	{
		m_wHPBACK = wNOW;

		CString strTemp;
		strTemp.Format ( "%d", wNOW );
		m_pHPText->SetOneLineText ( strTemp, NS_UITEXTCOLOR::WHITE );

		const float fPercent = float(wNOW) / float(wMAX);
		m_pHP->SetPercent ( fPercent );
	}
}

void CBasicInfoView::SetMP ( WORD wNOW, WORD wMAX )
{
	GASSERT( m_pMPText && m_pMP );

	if ( m_wMPBACK != wNOW )
	{
		m_wMPBACK = wNOW;

		CString strTemp;
		strTemp.Format ( "%d", wNOW );
		m_pMPText->SetOneLineText ( strTemp, NS_UITEXTCOLOR::WHITE );

		const float fPercent = float(wNOW) / float(wMAX);
		m_pMP->SetPercent ( fPercent );
	}
}

void CBasicInfoView::SetSP ( WORD wNOW, WORD wMAX )
{
	GASSERT( m_pSPText && m_pSP );

	if ( m_wSPBACK != wNOW )
	{
		m_wSPBACK = wNOW;
        
		CString strTemp;
		strTemp.Format ( "%d", wNOW );
		m_pSPText->SetOneLineText ( strTemp, NS_UITEXTCOLOR::WHITE );

		const float fPercent = float(wNOW) / float(wMAX);
		m_pSP->SetPercent ( fPercent );
	}
}

void CBasicInfoView::SetEXP( LONGLONG lNOW, LONGLONG lMAX )
{
	GASSERT( m_pEXPText && m_pEXP );

	if ( m_lEXPBACK != lNOW )
	{
		m_lEXPBACK = lNOW;

		CString strTemp;
		float fPercent(0.0f);
		if( lNOW )
		{
			fPercent = float(lNOW) / float(lMAX);	
		}

		strTemp.Format ( "%2.2f%%", fPercent * 100.0f );
		m_pEXPText->SetOneLineText ( strTemp, NS_UITEXTCOLOR::WHITE );

		m_pEXP->SetPercent ( fPercent );
	}
}

void CBasicInfoView::CheckMouseState ()
{
	const DWORD dwMsg = GetMessageEx ();
	if ( CHECK_MOUSE_IN ( dwMsg ) )
	{
		if( UIMSG_LB_DUP & dwMsg )
		{
			// 윈도우를 디폴트 포지션으로 이동
			AddMessageEx( UIMSG_BASICINFOVIEW_LBDUP );
			SetDiffuse ( D3DCOLOR_ARGB ( 0xFF, 0xFF, 0xFF, 0xFF ) );
			return ;
		}
		
		if ( dwMsg & UIMSG_LB_DOWN )
		{
			//	단독 컨트롤로 등록하고,
			SetExclusiveControl();	

			//	최초 포지션 갭을 기록한다.
			if ( !m_bFirstGap )
			{
				UIRECT rcPos = GetGlobalPos ();
				m_vGap.x = m_PosX - rcPos.left;
				m_vGap.y = m_PosY - rcPos.top;
				m_bFirstGap = TRUE;

				SetDiffuse ( D3DCOLOR_ARGB ( 0xB4, 0xFF, 0xFF, 0xFF ) );
			}
		}
		else if ( CHECK_LB_UP_LIKE ( dwMsg ) )
		{
			//	단독 컨트롤을 해제하고
			ResetExclusiveControl();
			//	최초 포지션갭을 해제한다.
			m_bFirstGap = FALSE;				

			SetDiffuse ( D3DCOLOR_ARGB ( 0xFF, 0xFF, 0xFF, 0xFF ) );
		}
	}
	//	컨트롤 영역밖에서 버튼을 떼는 경우가 발생하더라도
	else if ( CHECK_LB_UP_LIKE ( dwMsg ) )		
	{								
		//	단독 컨트롤을 해제하고,
		ResetExclusiveControl();
		//	최초 포지션갭을 해제한다.
		m_bFirstGap = FALSE;					

		SetDiffuse ( D3DCOLOR_ARGB ( 0xFF, 0xFF, 0xFF, 0xFF ) );
	}
}


void CBasicInfoView::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	m_bCHECK_MOUSE_STATE = false;

	m_PosX = x;
	m_PosY = y;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( IsExclusiveSelfControl() )
	{
		SetGlobalPos ( D3DXVECTOR2 ( x - m_vGap.x, y - m_vGap.y ) );
	}

	const GLCHARLOGIC& sCharData = GLGaeaClient::GetInstance().GetCharacterLogic ();
	SetHP ( sCharData.m_sHP.wNow, sCharData.m_sHP.wMax );
	SetMP ( sCharData.m_sMP.wNow, sCharData.m_sMP.wMax );
	SetSP ( sCharData.m_sSP.wNow, sCharData.m_sSP.wMax );
	SetEXP ( sCharData.m_sExperience.lnNow, sCharData.m_sExperience.lnMax );
}

void CBasicInfoView::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	if ( !m_bCHECK_MOUSE_STATE )
	{
		CheckMouseState ();
		m_bCHECK_MOUSE_STATE = true;
	}

	switch ( ControlID )
	{
	case HP_TEXT:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				const GLCHARLOGIC& sCharData = GLGaeaClient::GetInstance().GetCharacterLogic ();

				CString strText;
				strText.Format ( "%d/%d", sCharData.m_sHP.wNow, sCharData.m_sHP.wMax );
	
				CInnerInterface::GetInstance().SHOW_COMMON_LINEINFO ( strText, NS_UITEXTCOLOR::DEFAULT );
			}
		}
		break;

	case MP_TEXT:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				const GLCHARLOGIC& sCharData = GLGaeaClient::GetInstance().GetCharacterLogic ();

				CString strText;
				strText.Format ( "%d/%d", sCharData.m_sMP.wNow, sCharData.m_sMP.wMax );
	
				CInnerInterface::GetInstance().SHOW_COMMON_LINEINFO ( strText, NS_UITEXTCOLOR::DEFAULT );
			}
		}
		break;

	case SP_TEXT:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				const GLCHARLOGIC& sCharData = GLGaeaClient::GetInstance().GetCharacterLogic ();

				CString strText;
				strText.Format ( "%d/%d", sCharData.m_sSP.wNow, sCharData.m_sSP.wMax );
	
				CInnerInterface::GetInstance().SHOW_COMMON_LINEINFO ( strText, NS_UITEXTCOLOR::DEFAULT );
			}
		}
		break;

	case EXP_TEXT:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				const GLCHARLOGIC& sCharData = GLGaeaClient::GetInstance().GetCharacterLogic ();

				CString strText;
				strText.Format( "%I64d/%I64d", sCharData.m_sExperience.lnNow, sCharData.m_sExperience.lnMax );

				CInnerInterface::GetInstance().SHOW_COMMON_LINEINFO ( strText, NS_UITEXTCOLOR::DEFAULT );
			}
		}
		break;
	}
}