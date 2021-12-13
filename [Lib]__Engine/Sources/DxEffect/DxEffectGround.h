#ifndef __DXEFFECTGROUND_H_
#define __DXEFFECTGROUND_H_

#pragma once

#include "CMemPool.h"
#include "DxVertexFVF.h"
#include "DxEffSingle.h"

#define	USEANI						0x00000001	// 애니메이션
#define	USEROTATE					0x00000004	// 범위 회전 체크
#define	USEHEIGHT					0x00000008	// 높이 조절
#define	USESEQUENCE					0x00010000	// 시퀀스 효과 삽입
#define	USESEQUENCELOOP				0x00020000	// 시퀀스 효과 루프 돌기
#define	USENORMAL					0x00100000	// 노멀을 사용 하려면 이것을
#define	USEPICKING					0x00200000	// 지면에 달라 붙도록 하려면 체크를 한다.

// useful macro to guarantee that values are within a given range
#define Clamp(x, min, max)  ( x = (x<min ? min : x<max ? x : max) )

struct GROUND_PROPERTY : public EFF_PROPERTY
{
	enum { TYPEID = EFFSINGLE_GROUND, };
	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual const char* GetName ()	{ return NAME; }

	struct PROPERTY_100
	{
		BOOL			m_bAni;
		float			m_fAniTime;			// 이녀석의 프레임 돌아가는 속도
		int				m_iCol;
		int				m_iRow;

		BOOL			m_bRotateUse;
		float			m_fRotateAngel;

		BOOL			m_bHeight;
		float			m_fHeightRate1;
		float			m_fHeightRate2;

		float			m_fHeightStart;
		float			m_fHeightMid1;
		float			m_fHeightMid2;
		float			m_fHeightEnd;

		float			m_fSizeRate1;
		float			m_fSizeRate2;

		float			m_fSizeStart;
		float			m_fSizeMid1;
		float			m_fSizeMid2;
		float			m_fSizeEnd;

		float			m_fAlphaRate1;
		float			m_fAlphaRate2;

		float			m_fAlphaStart;
		float			m_fAlphaMid1;
		float			m_fAlphaMid2;
		float			m_fAlphaEnd;

		D3DXCOLOR		m_cColorStart;
		D3DXCOLOR		m_cColorVar;
		D3DXCOLOR		m_cColorEnd;

		int			m_nBlend;
		char		m_szTexture[256];

		PROPERTY_100()
		{
			memset( m_szTexture, 0, sizeof(char)*256 );
		};
	};

	struct PROPERTY			// Note : Ver.101 ~ 102
	{
		DWORD			m_dwFlag;

		float			m_fAniTime;			// 이녀석의 프레임 돌아가는 속도
		int				m_iCol;
		int				m_iRow;

		float			m_fRotateAngel;

		float			m_fHeightRate1;
		float			m_fHeightRate2;

		float			m_fHeightStart;
		float			m_fHeightMid1;
		float			m_fHeightMid2;
		float			m_fHeightEnd;

		float			m_fSizeRate1;
		float			m_fSizeRate2;

		float			m_fSizeStart;
		float			m_fSizeMid1;
		float			m_fSizeMid2;
		float			m_fSizeEnd;

		float			m_fAlphaRate1;
		float			m_fAlphaRate2;

		float			m_fAlphaStart;
		float			m_fAlphaMid1;
		float			m_fAlphaMid2;
		float			m_fAlphaEnd;

		D3DXCOLOR		m_cColorStart;
		D3DXCOLOR		m_cColorVar;
		D3DXCOLOR		m_cColorEnd;

		int			m_nBlend;
		char		m_szTexture[256];

		PROPERTY()
		{
			memset( m_szTexture, 0, sizeof(char)*256 );
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
			DWORD			m_dwFlag;

			float			m_fAniTime;			// 이녀석의 프레임 돌아가는 속도
			int				m_iCol;
			int				m_iRow;

			float			m_fRotateAngel;

			float			m_fHeightRate1;
			float			m_fHeightRate2;

			float			m_fHeightStart;
			float			m_fHeightMid1;
			float			m_fHeightMid2;
			float			m_fHeightEnd;

			float			m_fSizeRate1;
			float			m_fSizeRate2;

			float			m_fSizeStart;
			float			m_fSizeMid1;
			float			m_fSizeMid2;
			float			m_fSizeEnd;

			float			m_fAlphaRate1;
			float			m_fAlphaRate2;

			float			m_fAlphaStart;
			float			m_fAlphaMid1;
			float			m_fAlphaMid2;
			float			m_fAlphaEnd;

			D3DXCOLOR		m_cColorStart;
			D3DXCOLOR		m_cColorVar;
			D3DXCOLOR		m_cColorEnd;

			int			m_nBlend;
			char		m_szTexture[256];
		};
	};

	//	Note : 타입 마다 존재하는 리소스.
	//
	LPDIRECT3DTEXTUREQ		m_pTexture;

	//	Note : 이팩트 생성자.
	//
	virtual DxEffSingle* NEWOBJ ();
	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( CSerialFile &SFile );

	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	virtual HRESULT DeleteDeviceObjects();

	GROUND_PROPERTY () : EFF_PROPERTY(),
		m_fAniTime(0.03f),
		m_iCol(4),
		m_iRow(4),

		m_fRotateAngel(10.f),

		m_fHeightRate1(25.f),
		m_fHeightRate2(75.f),

		m_fHeightStart(0.f),
		m_fHeightMid1(2.f),
		m_fHeightMid2(2.f),
		m_fHeightEnd(0.f),

		m_fSizeRate1(25.f),
		m_fSizeRate2(75.f),

		m_fSizeStart(50.0f),
		m_fSizeMid1(100.0f),
		m_fSizeMid2(100.0f),
		m_fSizeEnd(50.0f),

		m_fAlphaRate1(25.f),
		m_fAlphaRate2(75.f),

		m_fAlphaStart(0.f),
		m_fAlphaMid1(1.f),
		m_fAlphaMid2(1.f),
		m_fAlphaEnd(0.f),

		m_cColorStart(1.0f,1.0f,1.0f,1.0f),
		m_cColorVar(0.0f,0.0f,0.0f,0.0f),
		m_cColorEnd(1.0f,1.0f,1.0f,1.0f),

		m_nBlend(1),

		m_pTexture(NULL)
	{
		m_dwFlag = 0;

		StringCchCopy( m_szTexture, 256, "_Eff_flare.tga" );
	}
};

class DxEffectGround : public DxEffSingle
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

protected:
	DWORD			m_dwColor;
	VERTEXCOLORTEX2	m_pGroundVertex[4];
	VERTEX			m_pGroundVertNor[4];

	//	Note : 파티클 공통 리소스 자원들 관리.
protected:
	static BOOL						VAILEDDEVICE;

protected:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedRenderStats;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawRenderStats;

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
	DxEffectGround(void);
	~DxEffectGround(void);

public:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	D3DXMATRIX			m_matWorld;

public:
	D3DXVECTOR3		m_vGLocation;

	DWORD			m_dwFlag;

	int				m_iAllSprite;		// 전체 스프라이트 갯수
	int				m_iNowSprite;		// 지금 녀석의 스프라이트 번호
	float			m_fTimeSum;

	float			m_fRotateAngelSum;

	float			m_fHeightTime1;
	float			m_fHeightTime2;

	float			m_fHeightDelta1;
	float			m_fHeightDelta2;
	float			m_fHeightDelta3;

	float			m_fSizeTime1;
	float			m_fSizeTime2;

	float			m_fSizeDelta1;
	float			m_fSizeDelta2;
	float			m_fSizeDelta3;

	float			m_fAlphaTime1;
	float			m_fAlphaTime2;

	float			m_fAlphaDelta1;
	float			m_fAlphaDelta2;
	float			m_fAlphaDelta3;

	D3DXCOLOR		m_cColorDelta;

	int				m_nBlend;

public:
	float			m_fHeightStart;
	float			m_fSizeStart;
	float			m_fAlphaStart;

	BOOL			m_bAni;
	float			m_fAniTime;			// 이녀석의 프레임 돌아가는 속도
	int				m_iCol;
	int				m_iRow;

	BOOL			m_bRotateUse;
	float			m_fRotateAngel;

	BOOL			m_bHeight;
	float			m_fHeight;

	float			m_fSize;

	float			m_fAlpha;

	D3DXCOLOR		m_cColorStart;
	D3DXCOLOR		m_cColorVar;
	D3DXCOLOR		m_cColorEnd;

	BOOL			m_bSequenceRoop;	// TRUE 앞으로, FALSE 뒤로     ( 이동한다. 그림을 뿌린다. - -; )
	D3DXVECTOR3		m_vPrevPicking;

	D3DMATERIAL9	m_sMaterial;

public:
	std::string				m_strTexture;
	LPDIRECT3DTEXTUREQ		m_pTexture;

public:
	typedef CMemPool<DxEffectGround>	GROUNDPOOL;
    static GROUNDPOOL*					m_pPool;
};

#endif //__DXEFFECTGROUND_H_