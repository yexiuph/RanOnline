//	UIModal 클래스 - 모달 기반 클래스
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.11.04] - 이 부분, 리팩토링이 끝나면, RanClientLib로 내려간다.
//					 모달은 컨트롤과 메시지가 함께 붙어 있는것이 맞다.
//		[2003.10.31] - 시작
//					 enginelib을 RanClientLib와 분리

#pragma	once

#include "../[Lib]__EngineUI/Sources/UIModal.h"

class	CBasicTextBox;
class	CBasicTextButton;
class	CBasicButton;

class	CUIEditBoxMan;
class	CBasicLineBox;
class	CPrivateMarketSellWindow : public CUIModal
{
private:
static	const char BLANK_SYMBOL;

protected:
	enum
	{
        PRIVATE_MARKET_SELL_WINDOW_MONEY_EDIT = ET_CONTROL_NEXT,
		PRIVATE_MARKET_SELL_WINDOW_ITEMCOUNT_EDIT,
		PRIVATE_MARKET_SELL_WINDOW_OK,
		PRIVATE_MARKET_SELL_WINDOW_CANCEL,
	};

public:
	CPrivateMarketSellWindow ();
	virtual	~CPrivateMarketSellWindow ();

public:
	void	CreateBaseModal ( char* szWindowKeyword );
	void	CreateSubControl ();

public:
	const DWORD	GetItemCount ();
	const DWORD GetItemCost ();

protected:
	CUIControl*		CreateControl ( char* szControl );
	CBasicTextButton*	CreateTextButton ( char* szButton, UIGUID ControlID , char* szText );

public:
//	void	SetCallerID ( UIGUID ControlID );
//	UIGUID	GetCallerID ();

public:
	void	ResetAll ();

public:
	void	SetType ( const bool bCOUNTABLE );

public:
	void	GetItemIndex ( WORD& wPosX, WORD& wPosY );
	void	SetItemIndex ( const WORD wPosX, const WORD wPosY );

public:
	virtual	void	SetVisibleSingle ( BOOL bVisible );

private:
	CBasicTextBox*	CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont );
	CBasicButton*	CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID );

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

protected:
	void	ResizeControl ( char* szWindowKeyword );

private:
	CBasicLineBox*	m_pItemCountLineBox;
	CBasicTextBox*	m_pItemCountStatic;

	CBasicTextBox*	m_pTextBox;
	CUIEditBoxMan*	m_pEditBoxMan;

//	UIGUID	m_CallerID;
	bool	m_bCOUNTABLE;

	WORD		m_wPosX;
	WORD		m_wPosY;
};