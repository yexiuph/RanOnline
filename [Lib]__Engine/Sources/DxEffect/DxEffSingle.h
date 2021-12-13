#pragma once

#include <string>
#include <list>

#include "./DxEffectSingleType.h"
#include "./DxEffSinglePropGMan.h"
#include "./Collision.h"
#include "../[Lib]__EngineSound/Sources/DxSound/MovableSound.h"
#include "./GLDefine.h"

class DxEffSingleGroup;

//	Note : "EFF_PROPERTY"와 일대일 대응된다. "EFF_PROPERTY"에서 데이터를 받거나 참조하여 이펙트를 구동시킨다.
//			이를 상속 받아서 실제적인 이펙트 구동 클래스가 제작된다.
//			"class DxEffectPaticleSys : public DxEffSingle"와 같이 상속되어서 사용된다.
//
class DxEffSingle		// : public DXAFFINEMATRIX
{
public:
	DWORD				m_dwRunFlag;
	D3DXMATRIX			m_matLocal;
//	DXAFFINEPARTS		m_AffineParts;
	DxEffSingleGroup*	m_pThisGroup;

public:
	BOOL		m_bMoveObj;		//	
	float		m_fGAge;		//	현제 Eff의 경과시간.
	float		m_fGBeginTime;	//	활성화 시작 시간.
	float		m_fGLifeTime;	//	Eff 생명 기간.

	SMovSound	m_MovSound;	

	DxEffSingle	*m_pChild;		//	자식 Eff
	DxEffSingle	*m_pSibling;	//	형제 Eff

public:
	virtual void ReStartEff ();
	virtual void CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
	{
		if( m_pChild )		m_pChild->CheckAABBBox( vMax, vMin );
		if( m_pSibling )	m_pSibling->CheckAABBBox( vMax, vMin );
	}

protected:
	void PutSleepMemory( PDXEFFSINGLE& pSingle );

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb );

	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup ();

	virtual HRESULT Create ( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		if ( FAILED(OneTimeSceneInit()) )				return E_FAIL;
		if ( FAILED(InitDeviceObjects(pd3dDevice)) )	return E_FAIL;
		if ( FAILED(RestoreDeviceObjects(pd3dDevice)) )	return E_FAIL;

		return S_OK;
	}

	virtual HRESULT CleanUp ()
	{
		InvalidateDeviceObjects();
		DeleteDeviceObjects();
		FinalCleanup();

		return S_OK;
	}

	//	Note : 이펙트 타입 정의.
	//
public:
	virtual DWORD GetTypeID () = 0;
	virtual DWORD GetFlag () = 0;
	virtual const char* GetName () = 0;

public:
	BOOL IsReadyToDie ();

public:
	DxEffSingle(void) :
		m_pThisGroup(NULL),
		m_dwRunFlag(NULL),
		m_fGAge(0.0f),
		m_fGBeginTime(0.0f),
		m_fGLifeTime(0.0f),
		m_pChild(NULL),
		m_pSibling(NULL)
	{
		D3DXMatrixIdentity( &m_matLocal );
	//	UseAffineMatrix ();
	}

	virtual ~DxEffSingle(void)
	{
		CleanUp ();
	}
};
typedef DxEffSingle* PDXEFFSINGLE;

enum EMAFTERTYPE { EFFABODY = 0, EFFASINGLE = 1 };
struct SEFFSINGLE_AFTER
{
	EMAFTERTYPE		m_emType;
	std::string		m_strEffect;

	SEFFSINGLE_AFTER () :
		m_emType(EFFABODY)
	{
	}

	SEFFSINGLE_AFTER ( const SEFFSINGLE_AFTER &value )
	{
		m_emType = value.m_emType;
		m_strEffect = value.m_strEffect;
	}

	SEFFSINGLE_AFTER& operator= ( const SEFFSINGLE_AFTER &value )
	{
		m_emType = value.m_emType;
		m_strEffect = value.m_strEffect;
		return *this;
	}
	
	SEFFSINGLE_AFTER ( const EMAFTERTYPE emType, const std::string strEffect ) :
		m_emType(emType),
		m_strEffect(strEffect)
	{
	}
};

//	Note : "EFF_PROPGROUP"와 일대일 대응된다. "DxEffSing"들을 트리 방식으로 가지고 있으며 이를 관리한다.
//			"OBJAABB"는 이 이펙트 그룹이 게임상에서 실제 구동될때 카메라 프리즘컬링에 사용할 수 있는 환경을 제공하는데에 쓰인다.
//
class DxEffSingleGroup : public OBJAABB
{
public:
	typedef std::list<SEFFSINGLE_AFTER>		AFTERLIST;
	typedef AFTERLIST::iterator				AFTERLIST_ITER;

public:
	std::string			m_strFileName;

	DWORD				m_dwFlag;
	STARGETID			m_TargetID;
	D3DXMATRIX			m_matWorld;

	float				m_fElapsedTime;
	float				m_fGAge;
	float				m_fGBeginTime;	// 효과 이동하는 시간
	float				m_fGLifeTime;	// 효과 이동 시간
	D3DXVECTOR3			m_vGDelta;		// 시간당 이동하는 거리
	D3DXVECTOR3			m_vGNowPos;		// 현재 물체 위치

	D3DXVECTOR3			m_vStartPos;		// 처음 시작했던 위치.
	D3DXVECTOR3			m_vLocal;			// 만들때의 Y 축 높이를 고려해서 뿌려준다.
	D3DXVECTOR3			m_vDirection;		// 생성되는 파티클들의 처음 이동 방향 * 힘 을 나타냄

	BOOL				m_bOut_Particle;	// 새로운 파티클을 생성한다.
	float				m_fOut_Scale;		// 외부에서 스케일을 줌
	BOOL				m_bOut_Direct;		// 외부에서 방향을 줌
	BOOL				m_bAutoMove;		// 외부에서 방향을 줌
	D3DXVECTOR3			m_vTargetPos;		// 목표 위치.		외부에서 값을 주는 것임
	D3DXVECTOR3			m_vDirect;			// 방향 방향...		외부에서 값을 주는 것임
	D3DXMATRIX			m_matOutRotate;		// 외부의 회전 매트릭스

	DxEffSingle*		m_pEffSingleRoot;
	DWORD				m_dwAliveCount;

	AFTERLIST			m_listEffSAfter;

public:
	DxEffSingleGroup	*m_pNext;

public:
	D3DXVECTOR3* m_pMax;			//	AABB에 사용.
	D3DXVECTOR3* m_pMin;			//	AABB에 사용.

public:
	float GetAppElapsedTime ()		{ return m_fElapsedTime; }
	virtual void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );
	virtual OBJAABB* GetNext ()	{ return m_pNext; }

	BOOL IsBeginAfterEffect ()
	{
		if ( m_listEffSAfter.empty() )		return FALSE;
		return ( m_fGAge >= (m_fGBeginTime+m_fGLifeTime) );
	}

public:
	void SetTargetID ( const STARGETID* pTargetID );
	void AddEffAfter ( const EMAFTERTYPE emType, const std::string strEffSingle )
	{
		m_listEffSAfter.push_back ( SEFFSINGLE_AFTER(emType,strEffSingle) );
	}

public:
	void	ReStartEff ();

protected:
	void PutSleepMemory( PDXEFFSINGLE& pSingle );

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT OneTimeSceneInit ();
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();
	HRESULT FinalCleanup ();

public:
	HRESULT Create(LPDIRECT3DDEVICEQ pd3dDevice)
	{
		if ( FAILED(OneTimeSceneInit()) )				return E_FAIL;
		if ( FAILED(InitDeviceObjects(pd3dDevice)) )	return E_FAIL;
		if ( FAILED(RestoreDeviceObjects(pd3dDevice)) )	return E_FAIL;

		return S_OK;
	}

	HRESULT CleanUp ()
	{
		InvalidateDeviceObjects();
		DeleteDeviceObjects();
		FinalCleanup();

		return S_OK;
	}

public:
	DxEffSingleGroup () :
		m_dwFlag(NULL),
		m_dwAliveCount(0xFFFFFFFF),
		m_pEffSingleRoot(NULL),
		m_fElapsedTime(0.0f),
		m_fGAge(0.0f),
		m_fGBeginTime(0.f),
		m_fGLifeTime(0.f),
		m_vGDelta(D3DXVECTOR3(0.f,0.f,0.f)),
		m_vGNowPos(D3DXVECTOR3(0.f,0.f,0.f)),
		m_pMax(NULL),
		m_pMin(NULL),
		m_vLocal(0.f,0.f,0.f),
		m_vDirection(0.f,0.f,0.f),

		m_bOut_Particle(TRUE),
		m_fOut_Scale(1.f),
		m_bOut_Direct(FALSE),
		m_bAutoMove(FALSE),

		m_vTargetPos(0.f,0.f,0.f),
		m_vDirect(0.f,0.f,0.f),
		m_pNext(NULL)
	{
		D3DXMatrixIdentity ( &m_matOutRotate );
		D3DXMatrixIdentity ( &m_matWorld );
	}

	~DxEffSingleGroup ()
	{
		CleanUp ();
	}
};

typedef DxEffSingleGroup* PDXEFFSINGLEGROUP;

