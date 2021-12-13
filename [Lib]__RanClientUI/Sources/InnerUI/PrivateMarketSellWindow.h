//	UIModal Ŭ���� - ��� ��� Ŭ����
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2003.11.04] - �� �κ�, �����丵�� ������, RanClientLib�� ��������.
//					 ����� ��Ʈ�Ѱ� �޽����� �Բ� �پ� �ִ°��� �´�.
//		[2003.10.31] - ����
//					 enginelib�� RanClientLib�� �и�

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