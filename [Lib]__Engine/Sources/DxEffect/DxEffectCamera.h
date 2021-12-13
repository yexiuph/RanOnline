#ifndef __DXEFFECTCAMERA_H_
#define __DXEFFECTCAMERA_H_

#pragma once

#include "CMemPool.h"
#include "DxEffSingle.h"

// useful macro to guarantee that values are within a given range
#define Clamp(x, min, max)  ( x = (x<min ? min : x<max ? x : max) )

struct CAMERA_PROPERTY : public EFF_PROPERTY
{
	enum { TYPEID = EFFSINGLE_CAMERA, };
	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual const char* GetName ()	{ return NAME; }

	struct PROPERTY		// Note : Ver.100 ~ 101
	{
		int				m_iCameraSet;
		float			m_fScale;
		float			m_fPlayNum;
	};

	union
	{
		struct
		{
			PROPERTY		m_Property;
		};

		struct
		{
			int				m_iCameraSet;
			float			m_fScale;
			float			m_fPlayNum;
		};
	};

	//	Note : 이팩트 생성자.
	//
	virtual DxEffSingle* NEWOBJ ();
	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( CSerialFile &SFile );

	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	virtual HRESULT DeleteDeviceObjects();

	CAMERA_PROPERTY () : EFF_PROPERTY(),
		m_iCameraSet(1),
		m_fScale(0.5f),
		m_fPlayNum(20.f)
	{
	}
};

class DxEffectCamera : public DxEffSingle
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

	//	Note : 파티클 공통 리소스 자원들 관리.
	//
protected:
	static BOOL						VAILEDDEVICE;

protected:
	DWORD					m_dwSavedRenderStats;
	DWORD					m_dwDrawRenderStats;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnInitDevice_STATIC();
	static void OnDeleteDevice_STATIC();

public:
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb );

	virtual HRESULT FinalCleanup();

public:
	DxEffectCamera(void);
	~DxEffectCamera(void);

public:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

public:
	int				m_iCameraSet;
	float			m_fScale;
	float			m_fPlayTime;

public:
	D3DXVECTOR3		m_vFromPtSum;
	D3DXVECTOR3		m_vLookatPtSum;
	float			m_fTimeSum;

public:
	typedef CMemPool<DxEffectCamera>	CAMERAPOOL;
    static CAMERAPOOL*					m_pPool;
};

#endif //__DXEFFECTCAMERA_H_