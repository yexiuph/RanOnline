#pragma	once

#include "UIWindowEx.h"

const INT nMAX_HAIR_STYLE_M =  6;
const INT nMAX_HAIR_STYLE_FM = 5;

class CBasicTextBox;
class CBasicTextButton;

class CGenderChangeWindow : public CUIWindowEx
{
	enum
	{
		HAIRSTYLE_LEFT = ET_CONTROL_NEXT,
		HAIRSTYLE_RIGHT,
		FACESTYLE_LEFT,
		FACESTYLE_RIGHT,

		GENDER_CHANGE_OK,
		GENDER_CHANGE_CANCEL,
	};

public:
	CGenderChangeWindow ();
	virtual	~CGenderChangeWindow ();

public:
	void CreateSubControl();

	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );

private:
	CBasicTextBox * CreateStaticControl( char* szConatrolKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	CUIControl * CreateControl( const char * szControl ); // Note : 상위 클래스 함수 재정의

public:
	void CalcTextureFace();
	void CalcTextureHair();

private:
	CBasicTextBox * m_pFaceStyleName;
	CBasicTextBox * m_pHairStyleName;
	
	CUIControl * m_pFaceStyle;
//	CUIControl * m_pFaceStyleFM[nMAX_FACE_STYLE_FM];
	CUIControl * m_pHairStyle;
//	CUIControl * m_pHairStyleFM[nMAX_HAIR_STYLE_FM];	

//	CUIControl ** m_pFaceStyle;
//	CUIControl ** m_pHairStyle;
	
//	INT * m_pHairStylePos;
//	INT * m_pFaceStylePos;
	
	char * m_pFaceId2GameText;
	char * m_pHairId2GameText;	

	int m_nHair;
	int m_nHairBack;
	int m_nMaxHair;

	int m_nFace;
	int m_nFaceBack;
	int m_nMaxFace;
	
	int m_nSex;

	BOOL m_bReverse;

	BOOL m_bOKButton;
};