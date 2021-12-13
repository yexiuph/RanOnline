//	마켓 윈도우
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.12.6]
//			@ 작성
//

#pragma	once

#include "UIWindowEx.h"
#include "GLDefine.h"
#include "GLCrowData.h"

class	CMarketPage;
class	CBasicTextButton;
struct	SINVENITEM;

class	CMarketWindow : public CUIWindowEx
{
private:
static	const	int		nOUTOFRANGE;
protected:
	enum
	{
		nMAXPAGE = SCROWACTION::SALENUM
	};

	enum
	{
		MARKET_PAGE0 = ET_CONTROL_NEXT,
		MARKET_PAGE1,
		MARKET_PAGE2,
		MARKET_PAGE_END = MARKET_PAGE0 + nMAXPAGE,
		MARKET_WINDOW_PAGE_BUTTON0,
		MARKET_WINDOW_PAGE_BUTTON1,
		MARKET_WINDOW_PAGE_BUTTON2,
		MARKET_WINDOW_PAGE_BUTTON_END = MARKET_WINDOW_PAGE_BUTTON0 + nMAXPAGE,
	};

public:
	CMarketWindow ();
	virtual	~CMarketWindow ();

public:
	void	CreateSubControl ();
	void	SetVisiblePage ( int nPage );	

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void		InitMarket ( SNATIVEID sNativeID );

public:
	SNATIVEID	GetMarketID ();

	void	SetItemIndex ( int nPosX, int nPosY );
	void	GetItemIndex ( int* pnPosX, int* pnPosY );	

	int		GetPageIndex ();

	SINVENITEM&	GetItem ( int nPosX, int nPosY );

	BOOL	IsUsingMaketPage( int nPage );

private:
	SNATIVEID	m_sMarketID;

private:
	int		m_nPosX;
	int		m_nPosY;

	int		m_nPageIndex;

protected:
	CMarketPage*	CreateMarketPage ( UIGUID ControlID );
	CBasicTextButton*	CreateTextButton ( const char* szButton, UIGUID ControlID, const char* szText );

private:
	CMarketPage*		m_pPage[nMAXPAGE];
	CBasicTextButton*	m_pPageButton[nMAXPAGE];
};