#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "CMemPool.h"

class CItemShopIcon;

class CItemShopIconMan : public CUIGroup
{
	enum
	{
		BASE_CONTROLID = NO_ID + 1,
		BASE_CONTROLID_END = BASE_CONTROLID + 1000,
	};

	typedef std::map<DWORD,CItemShopIcon*>				SHOPICON_MAP;
	typedef SHOPICON_MAP::iterator						SHOPICON_MAP_ITER;

	typedef CMemPool<CItemShopIcon>						SHOPICON_POOL;

private:

	UIGUID			m_CONTROL_NEWID;
	SHOPICON_MAP	m_mapIcon;
	SHOPICON_POOL	m_IconPool;	
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	
public:
	CItemShopIconMan ();
	virtual	~CItemShopIconMan ();

public:

	// MEMO : 재정의
	bool DeleteControl ( UIGUID ControlID, int refactoring ); // 컨트롤을 삭제한다.

private:
	virtual	HRESULT	InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CItemShopIcon*	MAKE_SHOPICON ();

private:
	void	UPDATE_POS ( CItemShopIcon* pShopIcon, D3DXVECTOR3 vPos );
	bool	IS_VISIBLE_NAME ( D3DXVECTOR3 vPos, D3DXVECTOR3 & vScreen );

public:
	void	ADD_SHOP_ICON ( DWORD dwGaeaID );
	void	DEL_SHOP_ICON ( DWORD dwGaeaID );	
	void	SetRender( DWORD dwGaeaID );
	
public:
	void	ClearList ();
};