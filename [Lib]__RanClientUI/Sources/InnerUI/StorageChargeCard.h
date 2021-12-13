#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicButton;
class	CBasicTextBox;
class	CBasicLineBox;

class	CStorageChargeCard : public CUIGroup
{
private:
	enum
	{
		STORAGE_NO2 = NO_ID + 1,
		STORAGE_NO3,
		STORAGE_NO4,

		STORAGE_TEXT_NO2,
		STORAGE_TEXT_NO3,
		STORAGE_TEXT_NO4,

		STORAGE_BUTTON_NO2,
		STORAGE_BUTTON_NO3,
		STORAGE_BUTTON_NO4,

		STORAGE_CLOSE_BUTTON
	};

public:
	CStorageChargeCard ();
	virtual	~CStorageChargeCard ();

public:
	void	CreateSubControl ();

private:
	CBasicButton*	CreateFlipButton ( char* szButton, char* szButtonFlip, const UIGUID& cID, const int& nFLIP_TYPE );
	CBasicTextBox*	CreateTextBox ( const CString& strKeyword, const UIGUID& cID );

public:
	void	CheckMouseState ();

	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID cID, DWORD dwMsg );	

private:
	CBasicTextBox*	m_pButtonText2;
	CBasicTextBox*	m_pButtonText3;
	CBasicTextBox*	m_pButtonText4;

	CBasicTextBox*	m_pTextBox2;
	CBasicTextBox*	m_pTextBox3;
	CBasicTextBox*	m_pTextBox4;

	CBasicLineBox*	m_pLineBox2;
	CBasicLineBox*	m_pLineBox3;
	CBasicLineBox*	m_pLineBox4;

	CUIControl*		m_pOVER;

public:
	void InitStorageChargeCard ( const WORD& wPosX, const WORD& wPosY );
	void GetSelectedChargeCard ( WORD& wPosX, WORD& wPosY, WORD& wSTORAGE );

private:
	WORD	m_wItemPosX;
	WORD	m_wItemPosY;
	WORD	m_wSTORAGE;

private:
	BOOL		m_bFirstGap;
	D3DXVECTOR2	m_vGap;

	int			m_PosX;
	int			m_PosY;

	bool		m_bCHECK_MOUSE_STATE;
};