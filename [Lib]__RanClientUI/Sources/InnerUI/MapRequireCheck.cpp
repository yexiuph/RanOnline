#include "pch.h"
#include "BasicLineBox.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "BasicTextButton.h"

#include "GameTextControl.h"
#include "GLGaeaClient.h"
#include "DxGlobalStage.h"
#include "InnerInterface.h"

#include ".\maprequirecheck.h"

const float CMapRequireCheck::fDEFAULT_TIME_LEFT = 5.0f;

CMapRequireCheck::CMapRequireCheck(void)
								: m_pTextBox(NULL)
								, m_pButton(NULL)
								, m_fLeftTime(fDEFAULT_TIME_LEFT)
								, m_emMapCheckType(EMMAPCHECK_LEVELUP)
{
}

CMapRequireCheck::~CMapRequireCheck(void)
{
}

void CMapRequireCheck::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBox* pLineBox = new CBasicLineBox;
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_WAITSERVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxWaitServer ( "MAP_REQUIRE_CHECK_LINE_BOX" );
	RegisterControl ( pLineBox );	

	m_pButton = CreateTextButton ( "MAP_REQUIRE_CHECK_BUTTON", MAP_REQUIRE_CHECK_BUTTON, (char*)ID2GAMEWORD ( "GAMBLE_OKCANCEL" ) );
	CBasicTextBox* pTextBox = new CBasicTextBox;
	pTextBox->CreateSub ( this, "MAP_REQUIRE_CHECK_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pTextBox->SetFont ( pFont9 );
	pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	pTextBox->SetPartInterval ( 5.0f );
	RegisterControl ( pTextBox );
	m_pTextBox = pTextBox;

}


CBasicTextButton* CMapRequireCheck::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}


void CMapRequireCheck::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case MAP_REQUIRE_CHECK_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CInnerInterface::GetInstance().HideGroup( GetWndID() );
				GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();
				if( pCharacter == NULL ) return;

				GLMSG::SNETPC_REQ_MUST_LEAVE_MAP NetMsg;
				switch( m_emMapCheckType )
				{
					case EMMAPCHECK_LEVELUP:
					{
						NetMsg.sMAPID   = pCharacter->m_sStartMapID;
						NetMsg.dwGATEID = pCharacter->m_dwStartGate;
						NETSENDTOFIELD( &NetMsg );
					}
					break;
					case EMMAPCHECK_ENTRYFAIL:					
					{
						NetMsg.sMAPID   = GLCONST_CHAR::nidSTARTMAP[pCharacter->m_wSchool];
						NetMsg.dwGATEID = GLCONST_CHAR::dwSTARTGATE[pCharacter->m_wSchool];
						NETSENDTOFIELD( &NetMsg );
					}
					break;
					case EMMAPCHECK_LIMITTIME:
					{
						GLMSG::SNETREQ_GATEOUT_REQ NetMsgGateOut;
						NetMsgGateOut.dwGateID = 0;
						NETSENDTOFIELD ( &NetMsgGateOut );

						pCharacter->SetSTATE(EM_REQ_GATEOUT);						
					}						
					break;
				}
				
			}
		}
		break;
	}
}

void CMapRequireCheck::SetVisibleSingle ( BOOL bVisible )
{
	if( bVisible )
	{
		m_fLeftTime = fDEFAULT_TIME_LEFT;
	}
	CUIGroup::SetVisibleSingle ( bVisible );
}

void CMapRequireCheck::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	m_fLeftTime -= fElapsedTime;

	switch( m_emMapCheckType )
	{
	case EMMAPCHECK_LEVELUP:
		m_pTextBox->SetText ( ID2GAMEINTEXT("LEVELUP_MAP_REQUIRE_MSG") );
		break;
	case EMMAPCHECK_ENTRYFAIL:	
		m_pTextBox->SetText ( ID2GAMEINTEXT("REGEN_ENTRY_FAIL") );	
		break;
	case EMMAPCHECK_LIMITTIME:
		m_pTextBox->SetText ( ID2GAMEINTEXT("LIMITTIME_OVER") );	
		break;
	}

	CString strLeftTime;
	strLeftTime.Format ( "%1.0f%s", floor(m_fLeftTime) + 1.0f, ID2GAMEWORD("WAITSERVER_TIMELEFT_UNIT"));		
	m_pTextBox->AddText ( strLeftTime );

	if( m_fLeftTime <= 0.0f )
	{
		CInnerInterface::GetInstance().HideGroup( GetWndID() );
		GLMSG::SNETPC_REQ_MUST_LEAVE_MAP NetMsg;
		GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();
		if( pCharacter == NULL ) return;
		switch( m_emMapCheckType )
		{
		case EMMAPCHECK_LEVELUP:
			NetMsg.sMAPID   = pCharacter->m_sStartMapID;
			NetMsg.dwGATEID = pCharacter->m_dwStartGate;
			NETSENDTOFIELD( &NetMsg );
			break;
		case EMMAPCHECK_ENTRYFAIL:
			NetMsg.sMAPID   = GLCONST_CHAR::nidSTARTMAP[pCharacter->m_wSchool];
			NetMsg.dwGATEID = GLCONST_CHAR::dwSTARTGATE[pCharacter->m_wSchool];
			NETSENDTOFIELD( &NetMsg );
			break;
		case EMMAPCHECK_LIMITTIME:
			GLMSG::SNETREQ_GATEOUT_REQ NetMsgGateOut;
			NetMsgGateOut.dwGateID = 0;
			NETSENDTOFIELD ( &NetMsgGateOut );
			
			pCharacter->SetSTATE(EM_REQ_GATEOUT);
			break;
		}		
	}

	CUIWindow::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

}
