#ifndef __DXEFFECTMOVETARGET_H_
#define __DXEFFECTMOVETARGET_H_

#pragma once

#include "CMemPool.h"
#include "DxPhysical.h"
#include "DxEffSingle.h"

											// START ����
#define	USE_S_TARGET			0x00000001	// ��ǥ�� ��ġ ����			- R
#define	USE_S_DIRECT			0x00000002	// �������� ����			- R
#define	USE_S_RANDOM			0x00000004	// ������ ����				- C
#define	USE_S_GUIDED			0x00000008	// ���� ����				- C
#define	USE_S_ELASER			(USE_S_TARGET | USE_S_DIRECT | USE_S_RANDOM | USE_S_GUIDED)
											// END ����
#define	USE_E_CRASH				0x00000010	// �浹						- R
#define	USE_E_TIMEOVER			0x00000020	// TimeOver					- R
#define	USE_E_DONTMOVE			0x00000040	// �̵� �Ұ���				- R
#define	USE_E_ELASER			(USE_E_CRASH | USE_E_TIMEOVER | USE_E_DONTMOVE)

#define	USE_GROUND_CRASH		0x00000100	// �������� �浹			- C

#define	USE_A_EFF				0x00001000	// �߰� Effect ��� ����	- C
#define	USE_A_CRASH				0x00002000	// ó�� �浹��				- R
#define	USE_A_HIGH				0x00004000	// ���� ���� ��				- R
#define	USE_A_CRASHS			0x00008000	// �浹�� ����				- R
#define	USE_A_TIMEOVER			0x00010000	// Time Over				- R
#define	USE_A_DONTMOVE			0x00020000	// �̵� �Ұ���				- R
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

		char			m_szFileName[MAX_PATH];			//	����Ʈ �̱� �̸�
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
			float			m_fRandom_Angel;			// ���� ������ �ƴϴ�.
			float			m_fGuided_Accel;

			char			m_szFileName[MAX_PATH];		//	����Ʈ �̱� �̸�
		};
	};

	//	Note : ����Ʈ ������.
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
	//	Note : ����Ʈ Ÿ�� ����.		--	���� ���� �ڵ�� [����]
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

	char			m_szFileName[MAX_PATH];	//	����Ʈ �̱� �̸�

protected:
	float			m_fElapsedTime;

	BOOL			m_bGetInitPosDir;

	CPhysicalData	m_sData;

	D3DXVECTOR3		m_vInitDir;

	BOOL				m_SingleRender;		// �̱� EFF ���� ?
	D3DXMATRIX			m_matTrans;			//	Ʈ���� ��Ʈ����.
	EFF_PROPGROUP*		m_pPropGroup;		//	�Ӽ���.
	DxEffSingleGroup*	m_pSingleGroup;		//	Ȱ�� ��ü.

public:
	typedef CMemPool<DxEffectMoveTarget>	MOVETARGETPOOL;
    static MOVETARGETPOOL*					m_pPool;
};

#endif //__DXEFFECTMOVETARGET_H_