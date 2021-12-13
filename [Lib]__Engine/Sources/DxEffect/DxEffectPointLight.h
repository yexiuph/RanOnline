#ifndef __DxEffectPointLight_H_
#define __DxEffectPointLight_H_

#pragma once

#include "CMemPool.h"
#include "DxEffSingle.h"

#define	USERANDROTATE					0x00000001	// 처음의 랜덤 회전

// useful macro to guarantee that values are within a given range
#define Clamp(x, min, max)  ( x = (x<min ? min : x<max ? x : max) )

struct POINTLIGHT_PROPERTY : public EFF_PROPERTY
{
	enum { TYPEID = EFFSINGLE_POINTLIGHT, };
	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual const char* GetName ()	{ return NAME; }

	struct PROPERTY				// Ver. 100 ~ 101
	{
		DWORD			m_dwFlag;

		float			m_fPowerRate1;
		float			m_fPowerRate2;

		float			m_fPowerStart;
		float			m_fPowerMid1;
		float			m_fPowerMid2;
		float			m_fPowerEnd;

		float			m_fScaleRate1;
		float			m_fScaleRate2;

		float			m_fScaleStart;
		float			m_fScaleMid1;
		float			m_fScaleMid2;
		float			m_fScaleEnd;

		D3DXCOLOR		m_cColor;
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

			float			m_fPowerRate1;
			float			m_fPowerRate2;

			float			m_fPowerStart;
			float			m_fPowerMid1;
			float			m_fPowerMid2;
			float			m_fPowerEnd;

			float			m_fScaleRate1;
			float			m_fScaleRate2;

			float			m_fScaleStart;
			float			m_fScaleMid1;
			float			m_fScaleMid2;
			float			m_fScaleEnd;

			D3DXCOLOR		m_cColor;
		};
	};

	//	Note : 이팩트 생성자.
	//
	virtual DxEffSingle* NEWOBJ ();
	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( CSerialFile &SFile );

	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	virtual HRESULT DeleteDeviceObjects();

	POINTLIGHT_PROPERTY () : EFF_PROPERTY(),
		m_fPowerRate1(20.0f),
		m_fPowerRate2(80.0f),

		m_fPowerStart(1.0f),
		m_fPowerMid1(1.0f),
		m_fPowerMid2(1.0f),
		m_fPowerEnd(0.0f),

		m_fScaleRate1(20.0f),
		m_fScaleRate2(80.0f),

		m_fScaleStart(20.0f),
		m_fScaleMid1(20.0f),
		m_fScaleMid2(20.0f),
		m_fScaleEnd(20.0f),

		m_cColor(0.5f,0.5f,0.5f,1.0f)
	{
		m_dwFlag = 0L;
	}
};

class DxEffectPointLight : public DxEffSingle
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

public:
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb );

	virtual HRESULT FinalCleanup();

public:
	DxEffectPointLight(void);
	~DxEffectPointLight(void);

public:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	float				m_fPowerTime1;
	float				m_fPowerTime2;

	float				m_fPowerDelta1;
	float				m_fPowerDelta2;
	float				m_fPowerDelta3;

	float				m_fScaleTime1;
	float				m_fScaleTime2;

	float				m_fScaleDelta1;
	float				m_fScaleDelta2;
	float				m_fScaleDelta3;

public:
	D3DXVECTOR3		m_vGLocation;

	float			m_fTime;

	DWORD			m_dwFlag;

	float			m_fPower;
	float			m_fPowerStart;

	float			m_fScale;
	float			m_fScaleStart;

	D3DXCOLOR		m_cColor;

public:
	typedef CMemPool<DxEffectPointLight>	POINTLIGHTPOOL;
    static POINTLIGHTPOOL*					m_pPool;
};

#endif //__DxEffectPointLight_H_