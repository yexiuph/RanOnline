#pragma once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class CBasicButton;
class CBasicVarTextBox;

class CPassKeyboard : public CUIGroup
{
	enum
	{
		PASS_KEYBOARD_FORM = NO_ID + 1,
		PASS_KEYBOARD_UP_CHAR,
		PASS_KEYBOARD_LOW_CHAR,
		PASS_KEYBOARD_NUMBER,
		
		KEY_BUTTON_BACK,
		KEY_BUTTON_TAB,
		KEY_BUTTON_CAPSL,
		KEY_BUTTON_CAPSL_LED,

		KEY_BUTTON_0 = 48,
		KEY_BUTTON_1,
		KEY_BUTTON_2,
		KEY_BUTTON_3,
		KEY_BUTTON_4,
		KEY_BUTTON_5,
		KEY_BUTTON_6,
		KEY_BUTTON_7,
		KEY_BUTTON_8,
		KEY_BUTTON_9,

		KEY_BUTTON_A = 65,
		KEY_BUTTON_B,
		KEY_BUTTON_C,
		KEY_BUTTON_D,
		KEY_BUTTON_E,
		KEY_BUTTON_F,
		KEY_BUTTON_G,
		KEY_BUTTON_H,
		KEY_BUTTON_I,
		KEY_BUTTON_J,
		KEY_BUTTON_K,
		KEY_BUTTON_L,
		KEY_BUTTON_M,
		KEY_BUTTON_N,
		KEY_BUTTON_O,
		KEY_BUTTON_P,
		KEY_BUTTON_Q,
		KEY_BUTTON_R,
		KEY_BUTTON_S,
		KEY_BUTTON_T,
		KEY_BUTTON_U,
		KEY_BUTTON_V,
		KEY_BUTTON_W,
		KEY_BUTTON_X,
		KEY_BUTTON_Y,
		KEY_BUTTON_Z
	};

public:
	CPassKeyboard();
	virtual	~CPassKeyboard();

	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );

	void CreateSubControl();

protected:
	void CreateKeyButton( const char * szControlKey, const char * szFlipKey, UIGUID nID );
	void TranslateKeyMsg( UIGUID ControlID, DWORD dwMsg, BOOL bNumber = FALSE );
	void TranslateBackKey();
	void TranslateTabKey();
	void SetVisibleKeyChar( BOOL bCase );

protected:
	CUIControl * m_pPassKeyboardUpChar;
	CUIControl * m_pPassKeyboardLowChar;
	CBasicButton * m_pKeyCapsLockLed;

	BOOL m_bUpperCase; // Note : 대문자이면 TRUE
};