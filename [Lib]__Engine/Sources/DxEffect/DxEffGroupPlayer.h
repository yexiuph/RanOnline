#pragma once

struct EFF_PROPERTY;
struct EFF_PROPGROUP;
class DxEffSingleGroup;

#include <list>
#include <map>
#include "GLDefine.h"

//	Note : "class DxEffSingleGroup"인스턴스들을 (즉, 지금 동작하고 있는 "이펙트 그룹"개체들을 같이 모아서 관리하고
//			이를 동작시키며 렌더링 한다. ( Update & Render ) 또한 이들이 소멸시점인지를 체크한후 소멸되게 한다.
//
class DxEffGroupPlayer
{
protected:
	typedef std::multimap<STARGETID,DxEffSingleGroup*>		EFFSGMAP;
	typedef EFFSGMAP::iterator								EFFSGMAP_ITER;
	typedef std::pair<EFFSGMAP_ITER,EFFSGMAP_ITER>			EFFSGMAP_PAIR;

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	float				m_fTimer;
	EFFSGMAP			m_mapESG;

protected:
	void	NewAfterEff ( DxEffSingleGroup* pEffSG );

public:
	DWORD	GetAmount ()				{ return DWORD(m_mapESG.size()); }
	
	void	SetTime ( float fTimer )	{ m_fTimer = fTimer; }
	float	GetTime ()					{ return m_fTimer; }

public:
	D3DXVECTOR3 GetTargetIDPos ( STARGETID *pTargetID );

public:
	HRESULT PassiveEffect ( const char* szFileName, const D3DXMATRIX &matTrans, const STARGETID &sTargetID );
	HRESULT DeletePassiveEffect ( const char* szFileName, const STARGETID &sTargetID );
	HRESULT DeletePassiveEffect ( const STARGETID &sTargetID );
	BOOL FindPassiveEffect ( const char* szFileName, const STARGETID &sTargetID );

public:
	DxEffSingleGroup* NewEffGroup ( const char* szFileName, const D3DXMATRIX &matTrans, const STARGETID* pTargetID=NULL );
	DxEffSingleGroup* NewEffGroup ( EFF_PROPGROUP* pPropGroup, const D3DXMATRIX &matTrans, const STARGETID* pTargetID=NULL );
	HRESULT NewEffBody ( const char* szFileName, const STARGETID* pTargetID, D3DXVECTOR3 *pDir );

	//	Note : 단일 이팩트만 화면에 나오게 하는 메소드. 모든 이펙트 생성후 REND 안함.
	HRESULT NewEffGroup ( EFF_PROPGROUP* pPropGroup, EFF_PROPERTY* pTargetProp, const D3DXMATRIX &matTrans );

	////	Note : 단일 이팩트만 화면에 나오게 하는 메소드.
	//HRESULT NewEffSingle ( EFF_PROPERTY* pTargetProp, const D3DXMATRIX &matTrans );

	HRESULT RemoveAllEff ();
	HRESULT DeleteEff ( const char* szFile );
	HRESULT DeleteAllEff ( const char* szFile );

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ();

	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();

protected:
	DxEffGroupPlayer (void);

public:
	~DxEffGroupPlayer (void);

public:
	static DxEffGroupPlayer& GetInstance();
};
