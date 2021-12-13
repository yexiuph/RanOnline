#include "pch.h"
#include "BlockProgramAlarm.h"
#include "BasicLineBox.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "BasicTextButton.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "GLCharacter.h"
#include "GLGaeaClient.h"
#include "DxGlobalStage.h"
#include "InnerInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBlockProgramAlarm::CBlockProgramAlarm () :
	m_pTextBox ( NULL ),
    m_pAlarmOKButton ( NULL )
{
}

CBlockProgramAlarm::~CBlockProgramAlarm ()
{
}

void CBlockProgramAlarm::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBox* pLineBox = new CBasicLineBox;
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_MAPMOVE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxMapMove ( "BLOCK_PROGRAM_ALARM_LINE_BOX" );
	RegisterControl ( pLineBox );	

	CBasicTextBox* pMapMoveTextBox = new CBasicTextBox;
	pMapMoveTextBox->CreateSub ( this, "BLOCK_PROGRAM_ALARM_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pMapMoveTextBox->SetFont ( pFont9 );
	pMapMoveTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	pMapMoveTextBox->AddText ( ID2GAMEINTEXT("BLOCK_PROGRAM_ALARM") );
	RegisterControl ( pMapMoveTextBox );
	m_pTextBox = pMapMoveTextBox;

	m_pAlarmOKButton = CreateTextButton ( "BLOCK_PROGRAM_ALARM_OK", ALARM_OKBUTTON, (char*)ID2GAMEWORD ( "MAPMOVE_OKBUTTON" ) );
//	m_pAlarmOKButton->SetShortcutKey ( DIK_SPACE );
}

CBasicTextButton* CBlockProgramAlarm::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

void CBlockProgramAlarm::DoALARM ()
{
	GLGaeaClient::GetInstance().GetCharacter()->ResetPASSIVITY();
}

void CBlockProgramAlarm::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case ALARM_OKBUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_MOUSEIN_ALARMBUTTON );

				if ( UIMSG_LB_UP & dwMsg )
				{
					//	Note : 오토마우스가 아니라는 의사표시
					//
					DoALARM ();

					CInnerInterface::GetInstance().SetBlockProgramAlarm ( false );
				}
			}
		}
        break;
	}
}