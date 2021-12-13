//	마켓 윈도우
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.12.6]
//			@ 작성
//

#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicButton;
class	CBasicTextButton;
class	CBasicTextBox;
class	CTradePage;
struct	SINVENITEM;

class	CTradeControlPage : public CUIGroup
{
private:
static	const	int	nOUTOFRANGE;

private:
	enum
	{
		TRADE_MONEY_BUTTON = NO_ID + 1,
		TRADE_OK_BUTTON,
		TRADE_CANCEL_BUTTON,
		TRADE_SLOT_PAGE
	};

public:
	CTradeControlPage ();
	virtual	~CTradeControlPage ();

public:
	void	CreateSubControl ( BOOL bMyPage );

private:
	void	ApplyButtonUpdate ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
    void	SetPlayerName ( CString strName );
	void	SetMoney ( LONGLONG lnMoney );

public:
	void	SetItemIndex ( int nPosX, int nPosY );
	void	GetItemIndex ( int* pnPosX, int* pnPosY );

public:
	SINVENITEM&	GetItem ( int nPosX, int nPosY );

private:
	CUIControl*	CreateControl ( char* szControl );
	CBasicButton*		CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID, WORD wFlipType );
	CBasicTextButton*	CreateTextButton ( char* szButton, UIGUID ControlID, char* szText );
	CBasicTextBox*		CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );

private:
	CBasicTextButton*	m_pOK;
	CBasicTextButton*	m_pCANCEL;

private:
	CBasicTextBox*	m_pPlayerName;
	CBasicTextBox*	m_pMoney;

private:
	CTradePage*		m_pPage;

private:
	int		m_nPosX;
	int		m_nPosY;

private:
	BOOL	m_bMyPage;
};