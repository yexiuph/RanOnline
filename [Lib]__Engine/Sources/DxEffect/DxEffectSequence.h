#ifndef __CSEQUENCE_H_
#define __CSEQUENCE_H_

#pragma once

#include "CMemPool.h"
#include "DxVertexFVF.h"
#include "DxEffSingle.h"

#define	USEANI			0x00000001	// 애니메이션
#define	USEROTATE		0x00000004	// 범위 회전 체크
#define	USECOLLISION	0x00000020	// 파티클 충돌 체크
#define	USEDIRECTION	0x00000100	// 퍼지는 방향이 있죠..
#define	USETEXROTATE	0x00000400	// 텍스쳐 회전
#define	USEGOTOCENTER	0x00004000	// 센터 방향으로 이동 시킴
#define	USESEQUENCELOOP	0x00020000	// 시퀀스 효과 루프 돌기
#define	USEBILLBOARD	0x00040000	// 빌보드 적용 시키기
#define	USEBILLBOARDUP	0x00080000	// 빌보드 했을 때 빠닥 세우기 하자.!!
#define	USELIGHTING		0x00100000	// 조명에 영향을 받을 경우
#define	USEDYNAMICSCALE	0x00200000	// 거리에 따라 스케일이 조정 된다.
#define	USEDEFAULTPOS	0x00400000	// 위치의 초기화
#define	NOT_WORLD_RS	0x00800000	// 월드 회전,스케일에 영향을 받지 않는다.
#define	USERANDOMLIFE	0x01000000	// 생명의 변화
//#define	USEBILLDIRECT	0x00400000	// 방향에 따른 빌보드


// useful macro to guarantee that values are within a given range
#define Clamp(x, min, max)  ( x = (x<min ? min : x<max ? x : max) )

struct SEQUENCE_PROPERTY : public EFF_PROPERTY
{
	enum { TYPEID = EFFSINGLE_SEQUENCE, };
	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual const char* GetName ()	{ return NAME; }

	struct PROPERTY_100
	{
		//	Note : 파티클
		//
		DWORD			m_dwFlag;

		D3DXVECTOR3		m_vGVelocity;

		D3DXVECTOR3		m_vGGravityStart;
		D3DXVECTOR3		m_vGGravityEnd;

		float			m_fFlarePos;

		float			m_fRotateAngel;

		float			m_fRotateRate1;
		float			m_fRotateRate2;

		float			m_fLengthStart;
		float			m_fLengthMid1;
		float			m_fLengthMid2;
		float			m_fLengthEnd;

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

		float			m_fAniTime;			// 이녀석의 프레임 돌아가는 속도
		int				m_iCol;
		int				m_iRow;

		BOOL			m_bTexRotateUse;		// 텍스쳐 회전
		float			m_fTexRotateAngel;

		BOOL			m_bGIsColliding;

		int				m_nBlend;

		char			m_szTexture[256];

		PROPERTY_100()
		{
			memset( m_szTexture, 0, sizeof(char)*256 );
		};
	};

	struct PROPERTY			// Ver. 101 ~ 102
	{
		//	Note : 파티클
		//
		DWORD			m_dwFlag;

		D3DXVECTOR3		m_vGVelocity;

		D3DXVECTOR3		m_vGGravityStart;
		D3DXVECTOR3		m_vGGravityEnd;

		float			m_fFlarePos;

		float			m_fRotateAngel;

		float			m_fRotateRate1;
		float			m_fRotateRate2;

		float			m_fLengthStart;
		float			m_fLengthMid1;
		float			m_fLengthMid2;
		float			m_fLengthEnd;

		float			m_fSizeRate1;
		float			m_fSizeRate2;

		float			m_fSizeStart;
		float			m_fSizeMid1;
		float			m_fSizeMid2;
		float			m_fSizeEnd;

		float			m_fWidthRate;
		float			m_fHeightRate;

		float			m_fAlphaRate1;
		float			m_fAlphaRate2;

		float			m_fAlphaStart;
		float			m_fAlphaMid1;
		float			m_fAlphaMid2;
		float			m_fAlphaEnd;

		D3DXCOLOR		m_cColorStart;
		D3DXCOLOR		m_cColorVar;
		D3DXCOLOR		m_cColorEnd;

		float			m_fAniTime;			// 이녀석의 프레임 돌아가는 속도
		int				m_iCol;
		int				m_iRow;

		BOOL			m_bTexRotateUse;		// 텍스쳐 회전
		float			m_fTexRotateAngel;

		BOOL			m_bGIsColliding;

		int				m_nBlend;

		char			m_szTexture[256];

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
			//	Note : 파티클
			//
			DWORD			m_dwFlag;

			D3DXVECTOR3		m_vGVelocity;

			D3DXVECTOR3		m_vGGravityStart;
			D3DXVECTOR3		m_vGGravityEnd;

			float			m_fFlarePos;

			float			m_fRotateAngel;

			float			m_fRotateRate1;
			float			m_fRotateRate2;

			float			m_fLengthStart;
			float			m_fLengthMid1;
			float			m_fLengthMid2;
			float			m_fLengthEnd;

			float			m_fSizeRate1;
			float			m_fSizeRate2;

			float			m_fSizeStart;
			float			m_fSizeMid1;
			float			m_fSizeMid2;
			float			m_fSizeEnd;

			float			m_fWidthRate;
			float			m_fHeightRate;

			float			m_fAlphaRate1;
			float			m_fAlphaRate2;

			float			m_fAlphaStart;
			float			m_fAlphaMid1;
			float			m_fAlphaMid2;
			float			m_fAlphaEnd;

			D3DXCOLOR		m_cColorStart;
			D3DXCOLOR		m_cColorVar;
			D3DXCOLOR		m_cColorEnd;

			float			m_fAniTime;			// 이녀석의 프레임 돌아가는 속도
			int				m_iCol;
			int				m_iRow;

			BOOL			m_bTexRotateUse;		// 텍스쳐 회전
			float			m_fTexRotateAngel;

			BOOL			m_bGIsColliding;

			int				m_nBlend;

			char			m_szTexture[256];
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

	SEQUENCE_PROPERTY () : EFF_PROPERTY(),
		//	Note : 파티클
		//
		m_vGVelocity(0,0,0),
		m_vGGravityStart(0,0,0),
		m_vGGravityEnd(0,0,0),

		m_fFlarePos(0.f),

		m_fRotateAngel(2.f),

		m_fRotateRate1(25.f),
		m_fRotateRate2(75.f),

		m_fLengthStart(0.f),
		m_fLengthMid1(2.f),
		m_fLengthMid2(1.f),
		m_fLengthEnd(0.f),

		m_fSizeRate1(25.f),
		m_fSizeRate2(75.f),

		m_fSizeStart(50.f),
		m_fSizeMid1(100.f),
		m_fSizeMid2(100.f),
		m_fSizeEnd(50.f),

		m_fWidthRate(1.f),
		m_fHeightRate(1.f),

		m_fAlphaRate1(20.0f),
		m_fAlphaRate2(80.0f),

		m_fAlphaStart(0.0f),
		m_fAlphaMid1(1.0f),
		m_fAlphaMid2(1.0f),
		m_fAlphaEnd(0.0f),

		m_cColorStart(1.0f,1.0f,1.0f,1.0f),
		m_cColorVar(0.0f,0.0f,0.0f,0.0f),
		m_cColorEnd(1.0f,1.0f,1.0f,1.0f),

		m_fAniTime(0.03f),	
		m_iCol(4),
		m_iRow(4),

		m_fTexRotateAngel(10.f),

		m_nBlend(1),
		
		m_pTexture(NULL)
	{
		m_dwFlag = 0;
		m_dwFlag |= USEBILLBOARD;
		StringCchCopy( m_szTexture, 256, "_Eff_flare.tga" );
	}
};

class DxEffectSequence : public DxEffSingle
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
	VERTEXCOLORTEX2	m_sSequenceVB[4];

public:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedRenderStats;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawRenderStats;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnInitDevice_STATIC();
	static void OnDeleteDevice_STATIC();

	//-------------------------------------------------------------------------------------------
public:
	DxEffectSequence ();
	virtual ~DxEffectSequence ();

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
	float			m_fElapsedTime;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	D3DXMATRIX			m_matWorld;

	DWORD				m_dwFlag;

	int					m_iAllSprite;		// 전체 스프라이트 갯수
	int					m_iNowSprite;		// 지금 녀석의 스프라이트 번호
	float				m_fTimeSum;

	D3DXVECTOR3			m_vGLocation;
	D3DXVECTOR3			m_vGPrevLocation;

	D3DXVECTOR3			m_vGVelocity;
	D3DXVECTOR3			m_vGVelocityStart;

	D3DXVECTOR3			m_vGGravity;
	D3DXVECTOR3			m_vGGravityStart;
	D3DXVECTOR3			m_vGGravityDelta;

	float				m_fSizeTime1;
	float				m_fSizeTime2;

	float				m_fSizeDelta1;
	float				m_fSizeDelta2;
	float				m_fSizeDelta3;

	float				m_fWidthRate;
	float				m_fHeightRate;

	float				m_fAlphaTime1;
	float				m_fAlphaTime2;

	float				m_fAlphaDelta1;
	float				m_fAlphaDelta2;
	float				m_fAlphaDelta3;

	float				m_fRotateTime1;
	float				m_fRotateTime2;

	float				m_fRotateDelta1;
	float				m_fRotateDelta2;
	float				m_fRotateDelta3;

	float				m_fRotateAngelSum;
	float				m_fTexRotateAngelSum;

	D3DXCOLOR			m_cColorDelta;

	int					m_nBlend;

	D3DXVECTOR3			m_vPlayPos;

public:
	//std::string			m_strTexture;
	char				m_szTexture[256];
	LPDIRECT3DTEXTUREQ	m_pTexture;

public:
	//	Note : 파티클
	//
	float			m_fSizeStart;
	float			m_fAlphaStart;
	float			m_fLengthStart;

	float			m_fSize;

	float			m_fAlpha;

	float			m_fRotateAngel;
	float			m_fLength;

	D3DXCOLOR		m_cColorStart;
	D3DXCOLOR		m_cColorVar;
	D3DXCOLOR		m_cColorEnd;

	float			m_fAniTime;			// 이녀석의 프레임 돌아가는 속도
	int				m_iCol;
	int				m_iRow;

	float			m_fFlarePos;

	BOOL			m_bSequenceRoop;	// TRUE 앞으로, FALSE 뒤로     ( 이동한다. 그림을 뿌린다. - -; )

	float			m_fTexRotateAngel;

public:
	typedef CMemPool<DxEffectSequence>	SEQUENCEPOOL;
    static SEQUENCEPOOL*				m_pPool;
};

#endif //__CSEQUENCE_H_