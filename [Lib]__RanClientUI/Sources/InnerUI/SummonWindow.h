#pragma once

#include "UIWindowEx.h"
#include "GLItem.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSEIN_SUMMON_POSIONSLOT = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class CSummonWindow : public CUIWindowEx
{
	static const D3DCOLOR dwDEFAULT_TRANSPARENCY;
	static const D3DCOLOR dwFULL_TRANSPARENCY;

	enum
	{
		SUMMON_CONFIRM_BUTTON = ET_CONTROL_NEXT,
		POSION_IMAGE,
	};
public:
	CSummonWindow(void);
	~CSummonWindow(void);

public:
	void			  CreateSubControl ();
	CBasicTextButton* CreateTextButton ( char* szButton, UIGUID ControlID, char* szText );
	CBasicTextBox*	  CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	CItemImage*		  CreateItemImage ( const char* szControl, UIGUID ControlID );

	void			  LoadItem ( SNATIVEID& ref_sItemID );
	void			  UnLoadItem ();

public:
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CBasicTextBox* m_pNameText;
	CBasicTextBox* m_pLevelText;
	CBasicTextBox* m_pHPText;

	CBasicProgressBar*	m_pHP;

	CBasicTextBox* m_pDamageText;
	CBasicTextBox* m_pDefenseText;
	CBasicTextBox* m_pHitText;
	CBasicTextBox* m_pDodgeText;

	CBasicTextBox* m_pRegFireText;
	CBasicTextBox* m_pRegColdText;
	CBasicTextBox* m_pRegElectricText;
	CBasicTextBox* m_pRegPoisonText;
	CBasicTextBox* m_pRegSpritsText;

	CBasicTextBox* m_pAmountText;

	CUIControl* m_pPosionBack;
	CUIControl*	m_pPosionDisplay;
	CItemImage*	m_pPosionImage;

	CUIControl*	m_pMouseOver;

	SNATIVEID	m_sPosionID;
};
