#pragma once

#include "UIWindowEx.h"
#include "GLDefine.h"

class CItemImage;
class CBasicTextBox;

class CSummonPosionDisplay : public CUIWindowEx
{

	static const D3DCOLOR dwDEFAULT_TRANSPARENCY;
	static const D3DCOLOR dwFULL_TRANSPARENCY;

	enum
	{
		SUMMON_POSION_IMAGE = ET_CONTROL_NEXT,
	};
public:
	CSummonPosionDisplay(void);
	~CSummonPosionDisplay(void);

public:
	void		   CreateSubControl ();
	CBasicTextBox* CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	CItemImage*	   CreateItemImage ( const char* szControl, UIGUID ControlID );
public:
	virtual void   Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void   TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual void   SetVisibleSingle ( BOOL bVisible );
protected:
	void	SetItem ( SNATIVEID sICONINDEX, const char* szTexture );	
	void	ResetItem ();

private:
	CItemImage*		m_pPosionImage;
	CBasicTextBox*  m_pAmountText;
};
