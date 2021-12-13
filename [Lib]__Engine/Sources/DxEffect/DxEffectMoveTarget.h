#ifndef __DXEFFECTMOVETARGET_H_
#define __DXEFFECTMOVETARGET_H_

#pragma once

#include "CMemPool.h"
#include "DxPhysical.h"
#include "DxEffSingle.h"

											// START 조건
#define	USE_S_TARGET			0x00000001	// 목표한 위치 있음			- R
#define	USE_S_DIRECT			0x00000002	// 방향으로 설정			- R
#define	USE_S_RANDOM			0x00000004	// 랜덤한 각도				- C
#define	USE_S_GUIDED			0x00000008	// 유도 형식				- C
#define	USE_S_ELASER			(USE_S_TARGET | USE_S_DIRECT | USE_S_RANDOM | USE_S_GUIDED)
											// END 조건
#define	USE_E_CRASH				0x00000010	// 충돌						- R
#define	USE_E_TIMEOVER			0x00000020	// TimeOver					- R
#define	USE_E_DONTMOVE			0x00000040	// 이동 불가능				- R
#define	USE_E_ELASER			(USE_E_CRASH | USE_E_TIMEOVER | USE_E_DONTMOVE)

#define	USE_GROUND_CRASH		0x00000100	// 지형과의 충돌			- C

#define	USE_A_EFF				0x00001000	// 추가 Effect 사용 가능	- C
#define	USE_A_CRASH				0x00002000	// 처음 충돌시				- R
#define	USE_A_HIGH				0x00004000	// 제일 높을 때				- R
#define	USE_A_CRASHS			0x00008000	// 충돌때 마다				- R
#define	USE_A_TIMEOVER			0x00010000	// Time Over				- R
#define	USE_A_DONTMOVE			0x00020000	// 이동 불가능				- R
#define	USE_A_ELASER			(USE_A_CRASH | USE_A_HIGH | USE_A_CRASHS | USE_A_TIMEOVER | USE_A_DONTMOVE)

// useful macro to guarantee that values are within a given range
#define Clamp(x, min, max)  ( x = (x<min ? min : x<max ? x : max) )

struct MOVETARGET_PROPERTY : public EFF_PROPERTY
{
	enum { TYPEID = EFFSINGLE_MOVETARGET, };
	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual const char* GetName ()	{ return NAME; }

	struct PROPERTY		// Ver. 100 ~ 101
	{
		DWORD			m_dwFlag;

		float			m_fObjectRadius;

		D3DXVECTOR3		m_vCustom_Velocity;
		float			m_fRandom_Angel;
		float			m_fGuided_Accel;

		char			m_szFileName[MAX_PATH];			//	이펙트 싱글 이름
	};

	union
	{
		struct
		{
			PROPERTY		m_Property;
		};

		struct
		{
			DWORD			m_dwFlag;

			float			m_fObjectRadius;

			D3DXVECTOR3		m_vCustom_Velocity;
			float			m_fRandom_Angel;			// 이젠 각도가 아니다.
			float			m_fGuided_Accel;

			char			m_szFileName[MAX_PATH];		//	이펙트 싱글 이름
		};
	};

	//	Note : 이팩트 생성자.
	//
	virtual DxEffSingle* NEWOBJ ();
	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( CSerialFile &SFile );

	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	virtual HRESULT DeleteDeviceObjects();

	MOVETARGET_PROPERTY () : 
		EFF_PROPERTY(),
		m_fObjectRadius(0.5f),
		m_vCustom_Velocity(100.f, 50.f, 0.f),
		m_fRandom_Angel(10.f),
		m_fGuided_Accel(10.f)
	{
		m_dwFlag = 0L;
		m_dwFlag |= USE_S_TARGET;
		m_dwFlag |= USE_E_CRASH;
		m_dwFlag |= USE_A_CRASH;

		StringCchCopy( m_szFileName, MAX_PATH, "" );
	}
};

class DxEffectMoveTarget : public DxEffSingle
{
	//	Note : 이펙트 타입 정의.		--	정적 정의 코드부 [시작]
	//
public:
	const static DWORD	TYPEID;
	const static DWORD	FLAG;
	const static char	NAME[];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return FLAG; }
	virtual const char* GetName ()	{ return NAME; }

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnInitDevice_STATIC();
	static void OnDeleteDevice_STATIC();

public:
	virtual void ReStartEff ();
	virtual void CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT FinalCleanup ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb );

public:
	DxEffectMoveTarget(void);
	~DxEffectMoveTarget(void);

protected:
	BOOL	Render_Target ( D3DXMATRIX &matWorld );
	void	Render_Direct ( D3DXMATRIX &matWorld );

	void	Crash ( int nColl );

public:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

public:
	D3DXVECTOR3		m_vGLocation;

	DWORD			m_dwFlag;

	float			m_fObjectRadius;

	D3DXVECTOR3		m_vCustom_Velocity;
	float			m_fRandom_Angel;
	float			m_fGuided_Accel;

	char			m_szFileName[MAX_PATH];	//	이펙트 싱글 이름

protected:
	float			m_fElapsedTime;

	BOOL			m_bGetInitPosDir;

	CPhysicalData	m_sData;

	D3DXVECTOR3		m_vInitDir;

	BOOL				m_SingleRender;		// 싱글 EFF 실행 ?
	D3DXMATRIX			m_matTrans;			//	트랜스 매트릭스.
	EFF_PROPGROUP*		m_pPropGroup;		//	속성값.
	DxEffSingleGroup*	m_pSingleGroup;		//	활성 개체.

public:
	typedef CMemPool<DxEffectMoveTarget>	MOVETARGETPOOL;
    static MOVETARGETPOOL*					m_pPool;
};

#endif //__DXEFFECTMOVETARGET_H_