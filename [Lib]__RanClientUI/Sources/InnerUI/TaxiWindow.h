#pragma	once

#include "UIWindowEx.h"
#include "GLTaxiStation.h"


class	CBasicTextBox;
class	CBasicTextBoxEx;
class	CBasicScrollBarEx;
class	CBasicButton;

class	CTaxiWindow : public CUIWindowEx
{
private:
	enum
	{
		TAXI_MAP_LIST = ET_CONTROL_NEXT,
		TAXI_MAP_SCROLLBAR,
		TAXI_STOP_LIST,
		TAXI_STOP_SCROLLBAR,
		TAXI_MOVEBUTTON,
	};

public:
	CTaxiWindow ();
	virtual ~CTaxiWindow ();

public:
	void	CreateSubControl ();	

private:
	void	LoadMapList ();
	void	LoadStopList ();

public:
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID cID, DWORD dwMsg );
	virtual	void	SetVisibleSingle( BOOL bVisible );


    void InitData();
	void SetInvenPos ( WORD wPosX, WORD wPosY ) { m_wPosX = wPosX; m_wPosY = wPosY; return; }
	void SetCalcMoney();
	void SetMoney( LONGLONG lnMoney );

private:
	int		m_nSelectMap;
	int		m_nSelectStop;

private:
	CBasicTextBox*		m_pMapTitle;
	CBasicTextBox*		m_pStopTitle;

	CBasicTextBoxEx*	m_pMapList;
	CBasicScrollBarEx*	m_pMapListScrollBar;

	CBasicTextBoxEx*	m_pStopList;
	CBasicScrollBarEx*	m_pStopListScrollBar;

	CBasicTextBox*		m_pMoneyTextBox;


    DWORD				m_wPosX;
	DWORD				m_wPosY;
	CBasicButton*		m_pMoveButton;
};

