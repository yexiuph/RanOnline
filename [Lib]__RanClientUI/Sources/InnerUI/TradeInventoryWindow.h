//	���� ������
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2003.12.6]
//			@ �ۼ�
//

#pragma	once

#include "UIWindowEx.h"

struct	SITEMCUSTOM;
class	CTradeInventoryPage;
class	CTradeInventoryWindow : public CUIWindowEx
{
protected:
	enum
	{
		TRADEINVENTORY_MONEY_BUTTON = ET_CONTROL_NEXT,
		TRADEINVENTORY_PLUS_MONEY_BUTTON,
		TRADEINVENTORY_MINUS_MONEY_BUTTON,
		TRADEINVENTORY_PAGE,
	};

public:
	CTradeInventoryWindow ();
	virtual	~CTradeInventoryWindow ();

public:
	void	CreateSubControl ();

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void	SetOPENER ( bool bOPENER );

private:
	void	SetMoney ( LONGLONG Money );
	bool	IsTurnITEM ( SITEMCUSTOM& sItemCustom );

public:
	void	SetOneViewSlot ( const int& nONE_VIEW_SLOT );
	const int& GetOneViewSlot () const;

private:
	CTradeInventoryPage*	m_pPage;

private:
	CBasicTextBox*		m_pMoneyTextBox;

	bool	m_bTRADE_MODE;
	bool	m_bOPENER;

	int		m_nONE_VIEW_SLOT;
};