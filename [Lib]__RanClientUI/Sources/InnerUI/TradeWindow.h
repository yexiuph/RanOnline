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

class	CTradeControlPage;

class	CTradeWindow : public CUIWindowEx
{
private:
	enum
	{
		TAR_TRADE_PAGE = ET_CONTROL_NEXT,
		MY_TRADE_PAGE
	};

public:
	CTradeWindow ();
	virtual	~CTradeWindow ();

public:
	void	CreateSubControl ();

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void	SetTradeInfo ( CString strPlayerName, DWORD dwMasterID );
	DWORD	GetMasterID ();

private:
	CTradeControlPage*	CreateTradeControlPage ( char *szControl, BOOL bMyPage, UIGUID ControlID );

private:
	DWORD	m_dwMasterID;	

private:
	CTradeControlPage*	m_pTarPage;
	CTradeControlPage*	m_pMyPgae;
};