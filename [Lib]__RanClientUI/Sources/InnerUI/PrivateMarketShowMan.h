#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "GLCrowRenList.h"
#include "CMemPool.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSEIN_MARKET_ADVERTISE_LBDNUP = UIMSG_USER5;
////////////////////////////////////////////////////////////////////

class	CPrivateMarketShow;
class	CPrivateMarketShowMan : public CUIGroup
{
private:
	enum
	{
		BASE_CONTROLID = NO_ID + 1,
		BASE_CONTROLID_END = BASE_CONTROLID + 5000,
	};

public:	
	typedef std::map<DWORD,CPrivateMarketShow*>	PRIVATE_MARKET_SHOW_MAP;
	typedef PRIVATE_MARKET_SHOW_MAP::iterator	PRIVATE_MARKET_SHOW_MAP_ITER;

	typedef CMemPool<CPrivateMarketShow>		PRIVATE_MARKET_POOL;

public:
	CPrivateMarketShowMan ();
	virtual	~CPrivateMarketShowMan ();

public:
	virtual	HRESULT	InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual void RemoveAll();

public:
	void	ADD_MARKET ( const CROWREN &sDISP );
	void	DEL_MARKET ( DWORD dwGaeaID );
	void	UPDATE_MARKET ( const CROWREN &sDISP );

	void	UPDATE_MARKET_POS ( CPrivateMarketShow* pVarTextBox, const CROWREN& sDISP );
	bool	IS_VISIBLE_NAME ( const CROWREN& sDISP, D3DXVECTOR3 & vScreen );

	CPrivateMarketShow*	MAKE_PRIVATEMARKET();

	// MEMO : 재정의
	bool DeleteControl ( UIGUID ControlID, int refactoring ); // 컨트롤을 삭제한다.

private:
	UIGUID	m_CONTROL_NEWID;
	bool	m_bFIRST_TOUCH;

	PRIVATE_MARKET_SHOW_MAP	m_mapPrivateMarketShow;
	PRIVATE_MARKET_POOL		m_PrivateMarketPool;
	LPDIRECT3DDEVICEQ		m_pd3dDevice;
};