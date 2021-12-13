#ifndef __CLIGHTING_H_
#define __CLIGHTING_H_

#pragma once

#include "CMemPool.h"
#include "DxEffSingle.h"

#define	USEIN			0x00000001	// 안 사용
#define	USEOUT			0x00000002	// 밖 사용
#define	USERANROTATE	0x00000004	// 랜덤하게 휘도록 만든다.
#define	USESTART		0x00000010	// 시작 모임
#define	USEEND			0x00000020	// 끝 모임

// useful macro to guarantee that values are within a given range
#define Clamp(x, min, max)  ( x = (x<min ? min : x<max ? x : max) )

struct LIGHTNING_PROPERTY : public EFF_PROPERTY
{
	enum { TYPEID = EFFSINGLE_LIGHTING, };
	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual const char* GetName ()	{ return NAME; }

	struct PROPERTY_100
	{
		//	Note : 파티클
		//
		DWORD			m_dwFlag;

		DWORD			m_dwLightning;	// 전기 갯수

		DWORD			m_dwDivision;	// 분할 갯수.

		float			m_fWidth_In;
		float			m_fWidth_Out;

		DWORD			m_dwMaxLenth;	// Out 휘도록
		DWORD			m_dwVelocity;	// In 휘도록

		float			m_fAlphaRate1;
		float			m_fAlphaRate2;

		float			m_fAlphaStart;
		float			m_fAlphaMid1;
		float			m_fAlphaMid2;
		float			m_fAlphaEnd;

		D3DXCOLOR		m_cColorStart;
		D3DXCOLOR		m_cColorEnd;

		int				m_nBlend;

		char			m_szTexture_In[256];
		char			m_szTexture_Out[256];

		PROPERTY_100()
		{
			memset( m_szTexture_In, 0, sizeof(char)*256 );
			memset( m_szTexture_Out, 0, sizeof(char)*256 );
		};
	};

	struct PROPERTY			// Note : Ver.101 ~ 102
	{
		//	Note : 파티클
		//
		DWORD			m_dwFlag;

		DWORD			m_dwLightning;	// 전기 갯수

		DWORD			m_dwDivision;	// 분할 갯수.

		float			m_fWidth_In;
		float			m_fWidth_Out;

		float			m_fMaxLenth;	// Out 휘도록
		float			m_fVelocity;	// In 휘도록

		float			m_fAlphaRate1;
		float			m_fAlphaRate2;

		float			m_fAlphaStart;
		float			m_fAlphaMid1;
		float			m_fAlphaMid2;
		float			m_fAlphaEnd;

		D3DXCOLOR		m_cColorStart;
		D3DXCOLOR		m_cColorEnd;

		int				m_nBlend;

		char			m_szTexture_In[256];
		char			m_szTexture_Out[256];

		PROPERTY()
		{
			memset( m_szTexture_In, 0, sizeof(char)*256 );
			memset( m_szTexture_Out, 0, sizeof(char)*256 );
		};
	};

	union
	{
		struct
		{
			PROPERTY		m_Property;
		};

		struct
		{
			//	Note : 파티클
			//
			DWORD			m_dwFlag;

			DWORD			m_dwLightning;	// 전기 갯수

			DWORD			m_dwDivision;	// 분할 갯수.	

			float			m_fWidth_In;
			float			m_fWidth_Out;

			float			m_fMaxLenth;	// Out 휘도록
			float			m_fVelocity;	// In 휘도록

			float			m_fAlphaRate1;
			float			m_fAlphaRate2;

			float			m_fAlphaStart;
			float			m_fAlphaMid1;
			float			m_fAlphaMid2;
			float			m_fAlphaEnd;

			D3DXCOLOR		m_cColorStart;
			D3DXCOLOR		m_cColorEnd;

			int				m_nBlend;

			char			m_szTexture_In[256];
			char			m_szTexture_Out[256];
		};
	};

	//	Note : 타입 마다 존재하는 리소스.
	//
	LPDIRECT3DTEXTUREQ		m_pTexture_In;
	LPDIRECT3DTEXTUREQ		m_pTexture_Out;

	//	Note : 이팩트 생성자.
	//
	virtual DxEffSingle* NEWOBJ ();
	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( CSerialFile &SFile );

	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	virtual HRESULT DeleteDeviceObjects();

	LIGHTNING_PROPERTY () : EFF_PROPERTY(),
		m_dwDivision(7),		// 분할.!

		m_dwLightning(2),

		m_fWidth_In(1.f),
		m_fWidth_Out(0.2f),

		m_fMaxLenth(3.f),		// 각각.. 휘도록
		m_fVelocity(2.f),		// 전체.. 휘도록

		m_fAlphaRate1(25.0f),
		m_fAlphaRate2(75.0f),

		m_fAlphaStart(0.0f),
		m_fAlphaMid1(1.0f),
		m_fAlphaMid2(1.0f),
		m_fAlphaEnd(0.0f),

		m_cColorStart(1.0f,0.8f,0.5f,0.5f),
		m_cColorEnd(0.4f,0.5f,1.0f,0.0f),

		m_nBlend(1)
	{
		m_dwFlag = USEIN | USEOUT;
		StringCchCopy( m_szTexture_In,	256, "_Eff_flare03.tga" );
		StringCchCopy( m_szTexture_Out, 256, "_Eff_flare03.tga" );
	}
};

class DxEffectLightning : public DxEffSingle
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

	//	Note : 파티클 시스템 내부용 구조체.
	//
protected:
	struct D3DVERTEX
	{
		D3DXVECTOR3		vPos;
		D3DCOLOR		Diffuse;
		D3DXVECTOR2		vTex;
		
		static const DWORD FVF;
	};

	D3DVERTEX*				m_pVertex;
	D3DVERTEX*				m_pVertex_Out;

	struct D3DROOT
	{
		D3DXVECTOR3		vPos;
		D3DXVECTOR3		vLookatCross;
		D3DXVECTOR3		vAccumulation;
	};
	D3DROOT*			m_pRootVertex;

	//	Note : 파티클 공통 리소스 자원들 관리.
	//
protected:
	static BOOL						VAILEDDEVICE;

protected:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedRenderStats;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawRenderStats;

protected:
	D3DXVECTOR3		DirectSum ( D3DROOT* pPos, int i, int End );
	void			UpdatePos ( D3DXVECTOR3 vPos );
	void			UpdatePos_Out ();

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnInitDevice_STATIC();
	static void OnDeleteDevice_STATIC();

public:
	virtual void ReStartEff ();
	virtual void CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );

public:
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb );

	virtual HRESULT FinalCleanup();

public:
	DxEffectLightning();
	virtual ~DxEffectLightning();

public:
	LPDIRECT3DDEVICEQ		m_pd3dDevice;

public:
	std::string				m_strTexture_IN;
	std::string				m_strTexture_OUT;
	LPDIRECT3DTEXTUREQ		m_pTexture_In;
	LPDIRECT3DTEXTUREQ		m_pTexture_Out;

public:
	DWORD				m_dwFlag;

	DWORD				m_dwLightning;	// 전기 갯수

	int					m_nBlend;

	int					m_nVertexNum;
	int					m_dwOriginNum;

	float				m_fWidth_In;
	float				m_fWidth_Out;

	float				m_fMaxLenth;
	float				m_fVelocity;

	float				m_fAlpha;

	float				m_fAlphaDelta1;
	float				m_fAlphaDelta2;
	float				m_fAlphaDelta3;

	float				m_fAlphaTime1;
	float				m_fAlphaTime2;

	D3DXCOLOR			m_cColor;
	D3DXCOLOR			m_cColorDelta;

	float				m_fAlphaStart;
	D3DXCOLOR			m_cColorStart;

	float				m_fElapsedTime;

	BOOL				m_bFirstPlay;

public:
	typedef CMemPool<DxEffectLightning>	LIGHTINGPOOL;
    static LIGHTINGPOOL*				m_pPool;
};

#endif //__CLIGHTING_H_