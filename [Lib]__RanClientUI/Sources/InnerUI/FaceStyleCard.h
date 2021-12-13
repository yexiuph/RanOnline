#pragma	once

#include "UIWindowEx.h"

class CBasicTextBox;

class CFaceStyleCard : public CUIWindowEx
{
	enum
	{
		FACESTYLECARD_LEFT = ET_CONTROL_NEXT,
		FACESTYLECARD_RIGHT,

		FACESTYLECARD_OK,
		FACESTYLECARD_CANCEL,
	};

public:
	CFaceStyleCard ();
	virtual	~CFaceStyleCard ();

public:
	void CreateSubControl();

	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );

private:
	CBasicTextBox * CreateStaticControl( char* szConatrolKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	CUIControl * CreateControl( const char * szControl ); // Note : 상위 클래스 함수 재정의

public:
	void CalcTexture();

private:
	CBasicTextBox * m_pStyleName;
	CUIControl * m_pFaceStyle;
	char * m_pId2GameText;

	int m_nFace;
	int m_nFaceBack;
	int m_nMaxFace;
	int m_nSex;

	BOOL m_bOKButton;
};