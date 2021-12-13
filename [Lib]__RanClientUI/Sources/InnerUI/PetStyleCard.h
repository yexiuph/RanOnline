#pragma	once

#include "UIWindowEx.h"
#include "GLItem.h"

const INT nMAX_PETSTYLE = 20;

class CBasicTextBox;
class CBasicTextButton;

class CPetStyleCard : public CUIWindowEx
{
	enum
	{
		PETSTYLECARD_LEFT = ET_CONTROL_NEXT,
		PETSTYLECARD_RIGHT,

		PETSTYLECARD_OK,
		PETSTYLECARD_CANCEL,
	};

public:
	CPetStyleCard ();
	virtual	~CPetStyleCard ();

public:
	void CreateSubControl();

	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );

private:
	CBasicTextBox * CreateStaticControl( char* szConatrolKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	CUIControl * CreateControl( const char * szControl ); // Note : 상위 클래스 함수 재정의

private:
	CBasicTextBox * m_pStyleName;
	CUIControl * m_pPetStyle[PETTYPE_SIZE][nMAX_PETSTYLE];
	PETTYPE m_emTYPE;

	INT * m_pPetStylePos;
	TCHAR * m_pId2GameText;

	WORD m_wPet;
	WORD m_wPetBack;

	BOOL m_bOKButton;
};