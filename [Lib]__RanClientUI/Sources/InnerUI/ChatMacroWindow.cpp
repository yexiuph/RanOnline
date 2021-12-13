#include "pch.h"
#include "ChatMacroWindow.h"

#include "BasicTextButton.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "BasicLineBox.h"
#include "GameTextControl.h"
#include "UITextControl.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__EngineUI/Sources/UIEditBox.h"
#include "./InnerInterface.h"
#include "RANPARAM.h"

CChatMacroWindow::CChatMacroWindow ()
{
}

CChatMacroWindow::~CChatMacroWindow ()
{
}

void CChatMacroWindow::SetChatMacro()
{	
	for(int i = 0; i<CHAT_MACRO_SIZE; ++i)
	{
		RANPARAM::ChatMacro[i] = m_pEditBox[i]->GetEditString();
	}
}


void CChatMacroWindow::GetChatMacro(){
	
	for(int i = 0; i< CHAT_MACRO_SIZE; ++i){
		m_ChatMacro[i] = RANPARAM::ChatMacro[i];
		m_pEditBox[i]->SetEditString ( m_ChatMacro[i] );	
	}
}

void CChatMacroWindow::CreateSubControl()
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE18;
	CUIEditBox* pEditBox = NULL;
	CBasicLineBox* pBasicLineBox = NULL;
	CBasicTextBox* pTextBox = NULL;

	m_pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	
	DWORD dwFontColor = NS_UITEXTCOLOR::DEFAULT;
	int nAlign = TEXT_ALIGN_LEFT;

	//	배경
	pBasicLineBox = new CBasicLineBox;
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OPTION", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxOption ( "CHATMACRO_WINDOW_BACK_BOX" );
	RegisterControl ( pBasicLineBox );


	// 확인 버튼 
	CBasicTextButton* pOKButton = new CBasicTextButton;
	pOKButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, CHAT_MACRO_OK );
	pOKButton->CreateBaseButton ( "CHATMACRO_OKBUTTON", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "CHATMACRO_OKBUTTON", 0 ) );
	RegisterControl ( pOKButton );	


	int i = 0;

	char strTemp[50];
	
	// 채팅 매크로
	for(i = 0; i < CHAT_MACRO_SIZE; ++i){

		sprintf_s(strTemp,"CHAT_MACRO_KEYTEXT%d", i);
		pTextBox = CreateStaticControl ( strTemp, m_pFont, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "CHATMACRO_KEYTEXT", i ) );
	}

	// EditBox배경 생성
	for(i = 0; i < CHAT_MACRO_SIZE; ++i){
		sprintf_s(strTemp, "CHAT_MACRO_EDIT_BACK%d", i );
		pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox ( strTemp);
		RegisterControl ( pBasicLineBox );        
	}

    // EditBox 생성
	for(i = 0; i < CHAT_MACRO_SIZE; ++i){
		sprintf_s(strTemp, "CHAT_MACRO_EDIT%d", i );
		pEditBox = new CUIEditBox;
		pEditBox->CreateSub ( this, strTemp, UI_FLAG_XSIZE | UI_FLAG_BOTTOM, CHAT_MACRO_EDIT0+i );
		pEditBox->CreateCarrat ( "CHATMACRO_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetFont ( m_pFont );
		pEditBox->SetLimitInput ( CHATMACRO_EDIT_LIMIT );
		RegisterControl ( pEditBox );
		m_pEditBox[i] = pEditBox;
	}
}


void CChatMacroWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	// EditBox
	int result = 0;
	CString strTemp;
	if( ControlID >= CHAT_MACRO_EDIT0 && ControlID <= CHAT_MACRO_EDIT9)
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			if ( UIMSG_LB_UP & dwMsg )
			{
				CString strName = m_pEditBox[ControlID - CHAT_MACRO_EDIT0]->GetEditString ();
				EDIT_BEGIN (ControlID - CHAT_MACRO_EDIT0, strName);				
			}
		}
		else
		{
			if ( UIMSG_LB_DOWN & dwMsg || UIMSG_LB_UP & dwMsg )
				EDIT_END (ControlID - CHAT_MACRO_EDIT0);
		}
	}
	else
	{
		switch ( ControlID )
		{
		case ET_CONTROL_TITLE:
		case ET_CONTROL_TITLE_F:
			{
				if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
				{
					CInnerInterface::GetInstance().SetDefaultPosInterface( CHATMACRO_WINDOW );
				}
				break;
			}			
		case CHAT_MACRO_OK:
			{
				if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
				{
					SetChatMacro();
					CInnerInterface::GetInstance().HideGroup ( GetWndID () );
				}
				break;
			}		
		case ET_CONTROL_BUTTON:
			{
				if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
				{
					CInnerInterface::GetInstance().HideGroup ( GetWndID () );
				}
				break;
			}
		}
	}
	
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );
}
void CChatMacroWindow::EDIT_BEGIN(int num, const CString& strName)
{	
	if( m_pEditBox[num])
	{
		m_pEditBox[num]->EndEdit ();
		m_pEditBox[num]->ClearEdit ();

		m_pEditBox[num]->SetEditString (strName);
		m_pEditBox[num]->BeginEdit ();	
	}
}

void CChatMacroWindow::EDIT_END(int num)
{
	if( m_pEditBox[num] ){
		m_ChatMacro[num] = m_pEditBox[num]->GetEditString();				
		m_pEditBox[num]->EndEdit();
	}
}

