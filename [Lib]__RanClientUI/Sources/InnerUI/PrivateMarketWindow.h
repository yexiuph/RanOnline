#pragma	once

#include "UIWindowEx.h"

class	CItemBankPage;
class	CBasicTextBox;
class	CBasicTextButton;
struct	SINVENITEM;

class	CPrivateMarketPage;
class	CPrivateMarketWindow : public CUIWindowEx
{
protected:
	enum
	{
		PRIVATE_MARKET_PAGE = ET_CONTROL_NEXT,
		PRIVATE_MARKET_START,
		PRIVATE_MARKET_STOP
	};

public:
	CPrivateMarketWindow ();
	virtual	~CPrivateMarketWindow ();

public:
	void	CreateSubControl ();

public:
	virtual	HRESULT	RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID cID, DWORD dwMsg );

public:
	const bool IsOPENER ()									{ return m_bOPENER; }

	void	InitPrivateMarket ( const bool& bOPENER, const DWORD& dwGaeaID );
	DWORD	GetPrivateMarketID ()							{ return m_dwGaeaID; }

	CBasicTextButton*	CreateTextButton ( char* szButton, UIGUID ControlID, char* szText );

	void	SetItemIndex ( int nPosX, int nPosY );
	void	GetItemIndex ( int* pnPosX, int* pnPosY );	

	SINVENITEM&	GetItem ( int nPosX, int nPosY );

private:
	CPrivateMarketPage*	m_pPage;
	CBasicTextButton*	m_pSTART;
	CBasicTextButton*	m_pSTOP;
	CBasicTextBox*		m_pTextBox;

private:
	bool	m_bOPENER;
	DWORD	m_dwGaeaID;

private:
	int		m_nPosX;
	int		m_nPosY;
};