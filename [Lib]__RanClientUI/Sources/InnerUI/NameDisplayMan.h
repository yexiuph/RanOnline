#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "GLCrowRenList.h"
#include "CMemPool.h"

class	CNameDisplay;
struct	GLPARTY_CLIENT;

enum
{
	NAME_DISPLAY_DEFAULT = 1,
	NAME_DISPLAY_ALL,
	NAME_DISPLAY_ITEMMONEY,
	NAME_DISPLAY_MOBCHAR,
	NAME_DISPLAY_PARTY,			//	파티 이름 표시
	NAME_DISPLAY_PARTY_CONFT,	//	파티 대련 중일 경우
	NAME_DISPLAY_CLUB_CONFT,	//	CLUB CONFT
};

class CNameDisplayMan : public CUIGroup
{
	enum
	{
		BASE_CONTROLID = NO_ID + 1,
		BASE_CONTROLID_END = BASE_CONTROLID + 10000,
	};

	typedef std::pair<EMCROW,DWORD>						CROWRENCOPY_PAIR;
	typedef std::map<CROWRENCOPY_PAIR,CNameDisplay*>	CROWRENCOPY_LIST;
	typedef CROWRENCOPY_LIST::iterator					CROWRENCOPY_LIST_ITER;

	typedef CMemPool<CNameDisplay>						NAMEDISPLAY_POOL;

private:
	UIGUID				m_CONTROL_NEWID;

	int					m_nNAMETYPE;
	CROWRENCOPY_LIST	m_listDISP_NAME;
	NAMEDISPLAY_POOL	m_NameDisplayPool;
	DWORD				m_dwCOUNT;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;

public:
	CNameDisplayMan ();
	virtual	~CNameDisplayMan ();

public:
	void SetNameType ( int nType )		{ m_nNAMETYPE = nType; }
	int GetNameType()					{ return m_nNAMETYPE; }

	// MEMO : 재정의
	bool DeleteControl ( UIGUID ControlID, int refactoring ); // 컨트롤을 삭제한다.

private:
	virtual	HRESULT	InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CNameDisplay*	MAKE_NAMEDISPLAY ();
	void			UpdateHold ( DWORD _dwCOUNT );

private:
	void	UPDATE_NAME_POS ( CNameDisplay* pNameDisplay, const CROWREN& sCrowRen );
	bool	IS_VISIBLE_NAME ( const CROWREN& sCrowRen, D3DXVECTOR3 & vScreen );

public:
	void	SET_COUNT ( DWORD _dwCOUNT )	{ m_dwCOUNT = _dwCOUNT; }
	void	ADD_DISP_NAME ( CROWREN &sDISP_NAME, const TCHAR * szOwnerName = NULL );
	void	VisibleTargetInfoCtrl ( EMCROW emCROW, DWORD dwCtrID, BOOL bVISIBLE );

public:
	const CROWREN* GET_DISP_INFO ( EMCROW emCROW, DWORD dwCtrID );

public:
	void	ClearList ();
};