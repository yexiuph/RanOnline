#pragma	once

#include "UIWindowEx.h"
#include "GLBusList.h"

class	CBasicTextBox;
class	CBasicTextBoxEx;
class	CBasicScrollBarEx;
class	CBasicButton;

struct	SNpcTalk;

class	CBusWindow : public CUIWindowEx
{
private:
	enum
	{
		BUS_MAP_LIST = ET_CONTROL_NEXT,
		BUS_MAP_SCROLLBAR,
		BUS_STOP_LIST,
		BUS_STOP_SCROLLBAR,
		BUS_MOVEBUTTON,
	};

public:
	CBusWindow ();
	virtual ~CBusWindow ();

public:
	void	CreateSubControl ();	

	bool	SetBusData ( const DWORD dwGaeaID, SNpcTalk* pNpcTalk );

private:
	void	LoadMapList ();
	void	LoadStopList ();

public:
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID cID, DWORD dwMsg );

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

	CBasicButton*		m_pMoveButton;

private:
	DWORD		m_dwGaeaID;
	SNpcTalk *	m_pNpcTalk;
	GLBusList	m_glBusList;
};