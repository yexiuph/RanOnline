#include "pch.h"
#include "KeySettingWindow.h"

#include "BasicTextButton.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "ShotCutBox.h"
#include "BasicLineBox.h"
#include "GameTextControl.h"
#include "UITextControl.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "InnerInterface.h"
#include "../[Lib]__MfcEx/Sources/RANPARAM.h"
#include "UILeftTopGroup.h"


#include "ModalWindow.h"
#include "ModalCallerID.h"

CKeySettingWindow::CKeySettingWindow ()
{
}

CKeySettingWindow::~CKeySettingWindow ()
{
}

void CKeySettingWindow::InitData()
{
	m_DikeyString[DIK_ESCAPE] = "Esc";
	m_DikeyString[DIK_1] = "1";
	m_DikeyString[DIK_2] = "2";
	m_DikeyString[DIK_3] = "3";
	m_DikeyString[DIK_4] = "4";
	m_DikeyString[DIK_5] = "5";
	m_DikeyString[DIK_6] = "6";
	m_DikeyString[DIK_7] = "7";
	m_DikeyString[DIK_8] = "8";
	m_DikeyString[DIK_9] = "9";
	m_DikeyString[DIK_0] = "0";
	m_DikeyString[DIK_MINUS] = "-";
	m_DikeyString[DIK_EQUALS] = "=";
	m_DikeyString[DIK_BACK] = "Back Space";
	m_DikeyString[DIK_TAB] = "Tab";
	m_DikeyString[DIK_Q] = "Q";
	m_DikeyString[DIK_W] = "W";
	m_DikeyString[DIK_E] = "E";
	m_DikeyString[DIK_R] = "R";
	m_DikeyString[DIK_T] = "T";
	m_DikeyString[DIK_Y] = "Y";
	m_DikeyString[DIK_U] = "U";
	m_DikeyString[DIK_I] = "I";
	m_DikeyString[DIK_O] = "O";
	m_DikeyString[DIK_P] = "P";
	m_DikeyString[DIK_LBRACKET] = "[";
	m_DikeyString[DIK_RBRACKET] = "]";
	m_DikeyString[DIK_RETURN] = "Enter";
	m_DikeyString[DIK_LCONTROL] = "L-Ctrl";
	m_DikeyString[DIK_A] = "A";
	m_DikeyString[DIK_S] = "S";
	m_DikeyString[DIK_D] = "D";
	m_DikeyString[DIK_F] = "F";
	m_DikeyString[DIK_G] = "G";
	m_DikeyString[DIK_H] = "H";
	m_DikeyString[DIK_J] = "J";
	m_DikeyString[DIK_K] = "K";
	m_DikeyString[DIK_L] = "L";
	m_DikeyString[DIK_SEMICOLON] = ";";
	m_DikeyString[DIK_APOSTROPHE] = "'";
	m_DikeyString[DIK_GRAVE] = "`";
	m_DikeyString[DIK_LSHIFT] = "L-Shift";
	m_DikeyString[DIK_BACKSLASH] = "\\";
	m_DikeyString[DIK_Z] = "Z";
	m_DikeyString[DIK_X] = "X";
	m_DikeyString[DIK_C] = "C";
	m_DikeyString[DIK_V] = "V";
	m_DikeyString[DIK_B] = "B";
	m_DikeyString[DIK_N] = "N";
	m_DikeyString[DIK_M] = "M";
	m_DikeyString[DIK_COMMA] = ",";
	m_DikeyString[DIK_PERIOD] = ".";
	m_DikeyString[DIK_SLASH] = "/";
	m_DikeyString[DIK_RSHIFT] = "R-Shift";
	m_DikeyString[DIK_MULTIPLY] = "*";
	m_DikeyString[DIK_LMENU] = "L-Alt";
	m_DikeyString[DIK_SPACE] = "Space bar";	
	m_DikeyString[DIK_CAPITAL] = "CapsLock";
	m_DikeyString[DIK_F1] = "F1";
	m_DikeyString[DIK_F2] = "F2";
	m_DikeyString[DIK_F3] = "F3";
	m_DikeyString[DIK_F4] = "F4";
	m_DikeyString[DIK_F5] = "F5";
	m_DikeyString[DIK_F6] = "F6";
	m_DikeyString[DIK_F7] = "F7";
	m_DikeyString[DIK_F8] = "F8";
	m_DikeyString[DIK_F9] = "F9";
	m_DikeyString[DIK_F10] = "F10";
	m_DikeyString[DIK_NUMLOCK] = "NumLock";
	m_DikeyString[DIK_SCROLL] = "Scroll";
	m_DikeyString[DIK_NUMPAD7] = "Num7";
	m_DikeyString[DIK_NUMPAD8] = "Num8";
	m_DikeyString[DIK_NUMPAD9] = "Num9";
	m_DikeyString[DIK_SUBTRACT] = "-";
	m_DikeyString[DIK_NUMPAD4] = "Num4";
	m_DikeyString[DIK_NUMPAD5] = "Num5";
	m_DikeyString[DIK_NUMPAD6] = "Num6";
	m_DikeyString[DIK_ADD] = "+";
	m_DikeyString[DIK_NUMPAD1] = "Num1";
	m_DikeyString[DIK_NUMPAD2] = "Num2";
	m_DikeyString[DIK_NUMPAD3] = "Num3";
	m_DikeyString[DIK_NUMPAD0] = "NUm0";
	m_DikeyString[DIK_DECIMAL] = ".";
	m_DikeyString[DIK_F11] = "F11";
	m_DikeyString[DIK_F12] = "F12";
	m_DikeyString[DIK_F13] = "F13";
	m_DikeyString[DIK_F14] = "F14";
	m_DikeyString[DIK_F15] = "F15";
	m_DikeyString[DIK_KANA] = "Kana";
	m_DikeyString[DIK_ABNT_C1] = "Abnt1";
	m_DikeyString[DIK_CONVERT] = "Convert";
	m_DikeyString[DIK_NOCONVERT] = "NoConvert";
	m_DikeyString[DIK_YEN] = "Yen";
	m_DikeyString[DIK_ABNT_C2] = "Abnt2";
	m_DikeyString[DIK_KANJI] = "Kanji";
	m_DikeyString[DIK_DIVIDE] = "/";
	m_DikeyString[DIK_SYSRQ] = "None";
	m_DikeyString[DIK_RMENU] = "R-Alt";
	m_DikeyString[DIK_PAUSE] = "Pause";
	m_DikeyString[DIK_HOME] = "Home";
	m_DikeyString[DIK_UP] = "UpKey";
	m_DikeyString[DIK_PRIOR] = "PageUp";
	m_DikeyString[DIK_LEFT] = "LeftKey";
	m_DikeyString[DIK_RIGHT] = "RightKey";
	m_DikeyString[DIK_END] = "End";
	m_DikeyString[DIK_DOWN] = "DownKey";
	m_DikeyString[DIK_NEXT] = "PageDown";
	m_DikeyString[DIK_INSERT] = "Insert";
	m_DikeyString[DIK_DELETE] = "Delete";
	m_DikeyString[DIK_LWIN] = "L-Win";
	m_DikeyString[DIK_RWIN] = "R-Win";
	m_DikeyString[DIK_APPS] = "App_Key";
}


CString CKeySettingWindow::GetdwKeyToString(int dwKey)
{
	STRDWKEY::iterator	pos;

	pos = m_DikeyString.find(dwKey);

	if(pos != m_DikeyString.end())
		return pos->second;

   	return CString("None");
}

int CKeySettingWindow::IsOverrapping()
{
	std::map<CString, int>	MapShotcut;
	std::map<CString, int>::iterator pos;
	CString strTemp;
	int dwKey;

	for( int i = 0; i < MAX_SETTING_KEY; ++i){
		strTemp = m_pEditBox[i]->GetEditString();
		if(strTemp == ""){
			// 정의되지 않은 키가 있습니다.
			return DIK_SPACE;
		}

		pos = MapShotcut.find(strTemp);

		if( pos != MapShotcut.end() ){
            // strTemp가 중복 정의 되었습니다.
			return pos->second;
		}else{
			dwKey = GetStringTodwKey(strTemp);
			MapShotcut.insert(std::pair<CString, int>(strTemp,dwKey));
		}
	}

	return 0;

}

int CKeySettingWindow::GetStringTodwKey(CString& dwKeystr)
{
	if( dwKeystr == "None")
		return -1;

	STRDWKEY::iterator	pos;
	
	for( pos = m_DikeyString.begin(); pos != m_DikeyString.end(); ++pos){
		if( pos->second == dwKeystr ){
			return pos->first;
		}
	}

	return -1;
}

void CKeySettingWindow::SetShotCutKey()
{	
	CString strTemp;

	int i =0;

	for( i = 0; i<QUICK_SLOT_NUM; ++i)
	{
		m_QuickSlot[i] = m_pEditBox[i]->GetEditString();
		RANPARAM::QuickSlot[i] = GetStringTodwKey( m_QuickSlot[i] );
		CInnerInterface::GetInstance().SetShotcutText( i, m_QuickSlot[i] );
	}

	for ( i = 0; i<QUICK_SKILL_NUM; ++i)
	{
		m_SkillSlot[i] = m_pEditBox[i + QUICK_SLOT_NUM]->GetEditString();
		RANPARAM::SkillSlot[i] = GetStringTodwKey( m_SkillSlot[i] );
		CInnerInterface::GetInstance().SetShotcutText( i+QUICK_SLOT_NUM, m_SkillSlot[i] );
	}

	for ( i = 0; i<MENU_SHOTCUT_NUM; ++i)
	{
		m_MenuShotcut[i] = m_pEditBox[i + QUICK_SLOT_NUM+QUICK_SKILL_NUM]->GetEditString();
		RANPARAM::MenuShotcut[RANPARAM::KeySettingToRanparam[i]] = GetStringTodwKey( m_MenuShotcut[i] );
		CInnerInterface::GetInstance().SetShotcutText( i+QUICK_SLOT_NUM+QUICK_SKILL_NUM, m_MenuShotcut[i] );
	}

	
}

void CKeySettingWindow::CreateSubControl()
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE18;
	CBasicTextBox* pTextBox = NULL;
	CShotCutBox* pEditBox = NULL;
	CBasicLineBox* pBasicLineBox = NULL;
	
	InitData();

	m_pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	
	DWORD dwFontColor = NS_UITEXTCOLOR::DEFAULT;
	int nAlign = TEXT_ALIGN_LEFT;

	//	배경
	pBasicLineBox = new CBasicLineBox;
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OPTION", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxOption ( "KEYSETTING_BACK_BOX" );
	RegisterControl ( pBasicLineBox );

	// 기본설정 버튼 KEYSETTING_DEFAULTSETTING
	CBasicTextButton* pDefaultButton = new CBasicTextButton;
	pDefaultButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, KEYSETTING_DEFAULTSETTING );
	pDefaultButton->CreateBaseButton ( "KEYSETTING_DEFAULTSETTING", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "KEYSETTING_APPLYOK", 0 ) );
	RegisterControl ( pDefaultButton );

	// 적용 버튼
	CBasicTextButton* pApplyButton = new CBasicTextButton;
	pApplyButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, KEYSETTING_APPLY );
	pApplyButton->CreateBaseButton ( "KEYSETTINGN_APPLY_BUTTON", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "KEYSETTING_APPLYOK", 1 ) );
	RegisterControl ( pApplyButton );

	// 확인 버튼 
	CBasicTextButton* pOKButton = new CBasicTextButton;
	pOKButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, KEYSETTING_OK );
	pOKButton->CreateBaseButton ( "KEYSETTING_OK_BUTTON", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "KEYSETTING_APPLYOK", 2 ) );
	RegisterControl ( pOKButton );	


	char strTemp[50];

	int i = 0;
	
	// 퀵슬롯 
	for(i = 0; i < QUICK_SLOT_NUM; ++i){
		sprintf_s(strTemp,"KEY_SETTING_QUICKSLOT%d", i);
		pTextBox = CreateStaticControl ( strTemp, m_pFont, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "KEYSETTING_QUICKSLOT", i ) );
	}

	// 스킬 슬롯
	for (i = 0; i < QUICK_SKILL_NUM; ++i){
		sprintf_s(strTemp,"KEY_SETTING_QUICKSKILL%d", i);
		pTextBox = CreateStaticControl ( strTemp, m_pFont, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "KEYSETTING_QUICKSKILL", i ) );		
	}

	// 메뉴 슬롯
	for (i = 0; i < MENU_SHOTCUT_NUM; ++i){
		sprintf_s(strTemp,"KEY_SETTING_MENUSHOTCUT%d", i);
		pTextBox = CreateStaticControl (strTemp, m_pFont, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "KEYSETTING_MENUSHOTCUT", i ) );		
	}

	// EditBox배경 생성 ( 퀵슬롯 )
	for(i = 0; i < QUICK_SLOT_NUM; ++i){
		sprintf_s(strTemp,"KEY_SETTING_QUICKEDIT_BACK%d", i);
		pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox ( strTemp );
		RegisterControl ( pBasicLineBox );        
	}

	// EditBox배경 생성 ( 스킬슬롯)
	for(i = 0; i < QUICK_SKILL_NUM; ++i){
		sprintf_s(strTemp,"KEY_SETTING_SKILLEDIT_BACK%d", i);
		pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox ( strTemp );
		RegisterControl ( pBasicLineBox );        
	}

	// EditBox배경 생성 ( 메뉴숏컷 )
	for(i = 0; i < MENU_SHOTCUT_NUM; ++i){
		sprintf_s(strTemp,"KEY_SETTING_SHOTCUTEDIT_BACK%d", i);
		pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox ( strTemp );
		RegisterControl ( pBasicLineBox );        
	}

    // EditBox 생성 ( 퀵슬롯 )
	for(i = 0; i < QUICK_SLOT_NUM; ++i){
		sprintf_s(strTemp,"KEY_SETTING_QUICKEDIT%d", i);		
		pEditBox = new CShotCutBox;
		pEditBox->CreateSub ( this, strTemp, UI_FLAG_XSIZE | UI_FLAG_BOTTOM, KEY_SETTING_QUICKEDIT0+i );
		pEditBox->CreateCarrat ( "KEY_SETTING_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetFont ( m_pFont );
		pEditBox->SetEditString ( m_QuickSlot[i] );	
		RegisterControl ( pEditBox );
		m_pEditBox[i] = pEditBox;
	}


	// EditBox 생성 ( 스킬슬롯 )
	for(i = 0; i < QUICK_SKILL_NUM; ++i){
		sprintf_s(strTemp,"KEY_SETTING_SKILLEDIT%d", i);		
		pEditBox = new CShotCutBox;
		pEditBox->CreateSub ( this, strTemp, UI_FLAG_XSIZE | UI_FLAG_BOTTOM, KEY_SETTING_SKILLEDIT0+i );
		pEditBox->CreateCarrat ( "KEY_SETTING_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetFont ( m_pFont );
		pEditBox->SetEditString ( m_SkillSlot[i] );	
		RegisterControl ( pEditBox );
		m_pEditBox[i+QUICK_SLOT_NUM] = pEditBox;
	}


	// EditBox배경 생성 ( 메뉴숏컷 )
	for(i = 0; i < MENU_SHOTCUT_NUM; ++i){
		sprintf_s(strTemp,"KEY_SETTING_SHOTCUTEDIT%d", i);		
		pEditBox = new CShotCutBox;
		pEditBox->CreateSub ( this, strTemp, UI_FLAG_XSIZE | UI_FLAG_BOTTOM, KEY_SETTING_SHOTCUTEDIT0+i );
		pEditBox->CreateCarrat ( "KEY_SETTING_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetFont ( m_pFont );
		pEditBox->SetEditString ( m_MenuShotcut[i] );	
		RegisterControl ( pEditBox );
		m_pEditBox[i+QUICK_SLOT_NUM+QUICK_SKILL_NUM] = pEditBox;
	}

}


void CKeySettingWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	// EditBox
	int result = 0;
	CString strTemp;
	if( ControlID >= KEY_SETTING_QUICKEDIT0 && ControlID <= KEY_SETTING_SHOTCUTEDIT15)
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			if ( UIMSG_LB_UP & dwMsg )
			{
				EDIT_BEGIN (ControlID - KEY_SETTING_QUICKEDIT0);				
			}
		}
		else
		{
			if ( UIMSG_LB_DOWN & dwMsg || UIMSG_LB_UP & dwMsg )
				EDIT_END (ControlID - KEY_SETTING_QUICKEDIT0);
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
					CInnerInterface::GetInstance().SetDefaultPosInterface( KEY_SETTING_WINDOW );
				}
			}
			break;
		case KEYSETTING_DEFAULTSETTING:
			{
				if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
				{
					SetDefaultKey();
				}
				break;
			}
		case KEYSETTING_APPLY:
			{
				if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
				{
					result = IsOverrapping();
					if( result == 0){
						SetShotCutKey();
					}else{
						strTemp.Format("%s%s",GetdwKeyToString(result),ID2GAMEINTEXT("MODAL_KEY_SETTING_ERROR"));
						DoModal(strTemp, MODAL_ERROR, OK, NO_ID );
					}
				}
				break;
			}
		case KEYSETTING_OK:
			{
				if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
				{
					result = IsOverrapping();
					if(result == 0){
						SetShotCutKey();
						CInnerInterface::GetInstance().HideGroup ( GetWndID () );
					}else{
						strTemp.Format("%s%s",GetdwKeyToString(result),ID2GAMEINTEXT("MODAL_KEY_SETTING_ERROR"));
						DoModal( strTemp, MODAL_ERROR, OK, NO_ID );
					}
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
void CKeySettingWindow::EDIT_BEGIN(int num)
{
	
	m_BeforeShotCut = m_pEditBox[num]->GetEditString();

	if( m_pEditBox[num])
	{
		m_pEditBox[num]->EndEdit ();
		m_pEditBox[num]->ClearEdit ();

		m_pEditBox[num]->SetEditString ("");
		m_pEditBox[num]->BeginEdit ();	
	}
}

void CKeySettingWindow::EDIT_END(int num)
{
	if( m_pEditBox[num] ){
		CString strTemp = m_pEditBox[num]->GetEditString();
		if( strTemp == "" ||  strTemp == " " ){
			m_pEditBox[num]->SetEditString (m_BeforeShotCut);		
		}
		
		m_pEditBox[num]->EndEdit();
	}
}

void CKeySettingWindow::GetShotCutKey(){
	
	int i = 0;
	int nIndex = 0;
	// 퀵슬롯 단축키값 얻어오기
	for( i = 0; i< QUICK_SLOT_NUM; ++i){
		m_QuickSlot[i] = GetdwKeyToString(RANPARAM::QuickSlot[i]);
		m_pEditBox[i]->SetEditString ( m_QuickSlot[i] );	
	}

	// 스킬슬롯 단축키값 얻어오기
	for( i = 0; i< QUICK_SKILL_NUM; ++i){
		m_SkillSlot[i] = GetdwKeyToString(RANPARAM::SkillSlot[i]);
		m_pEditBox[i+QUICK_SLOT_NUM]->SetEditString ( m_SkillSlot[i] );	
	}

	// 메뉴 단축키값 얻어오기
	for(i = 0; i< MENU_SHOTCUT_NUM; ++i)
	{
		nIndex = RANPARAM::KeySettingToRanparam[i];
		m_MenuShotcut[i] = GetdwKeyToString(RANPARAM::MenuShotcut[nIndex]);
		m_pEditBox[i+QUICK_SLOT_NUM+QUICK_SKILL_NUM]->SetEditString ( m_MenuShotcut[i] );	
	}
}

void CKeySettingWindow::SetDefaultKey()
{
	int i = 0;
	int nIndex = 0;
	for( i = 0; i< QUICK_SLOT_NUM; ++i){
		m_QuickSlot[i] = GetdwKeyToString( RANPARAM::DefaultShotcutKey[i] );
		m_pEditBox[i]->SetEditString ( m_QuickSlot[i] );	
	}

	// 스킬슬롯 단축키값 얻어오기
	for( i = 0; i< QUICK_SKILL_NUM; ++i){
		m_SkillSlot[i] = GetdwKeyToString( RANPARAM::DefaultShotcutKey[i+QUICK_SLOT_NUM]);
		m_pEditBox[i+QUICK_SLOT_NUM]->SetEditString ( m_SkillSlot[i] );	
	}

	// 메뉴 단축키값 얻어오기
	for(i = 0; i< MENU_SHOTCUT_NUM; ++i){
		nIndex = RANPARAM::KeySettingToRanparam[i];
		m_MenuShotcut[i] = GetdwKeyToString( RANPARAM::DefaultShotcutKey[nIndex+QUICK_SLOT_NUM+QUICK_SKILL_NUM]);
		m_pEditBox[i+QUICK_SLOT_NUM+QUICK_SKILL_NUM]->SetEditString ( m_MenuShotcut[i] );	
	}
	
}
