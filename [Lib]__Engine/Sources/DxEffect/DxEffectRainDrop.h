// DxEffectRain.h: interface for the DxEffectRain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXEFFECTRAINPOINT_H__INCLUDED_)
#define AFX_DXEFFECTRAINPOINT_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
 
#include "CMemPool.h"

#include "./DxEffectFrame.h"
#include "Collision.h"

struct DxFrame;
struct DxMeshes;

#define		PLAY_RAIN		0x001
#define		PLAY_SNOW		0x002
#define		PLAY_LEAVES		0x004

struct RAINPOINT_PROPERTY_100
{
	bool		m_bRain;		// 물방울을 사용 하냐?
	float		m_fRainRate;	// 면적당 나타나는 빗방울 갯수   
	float		m_fHeight;		// 하늘에 올라갈 최고 높이
	float		m_fSpeed;		// 떨어지는 속도
	float		m_fScaleA;		// 빗방울 크기	- 퍼지는 효과 (동글게)
	float		m_fScaleB;		// 빗방울 크기 - 터지는 효과 (파박)
	float		m_fRateUp;		// 하늘에 있을때
	float		m_fRateDown;	// 땅에 있을때
	float		m_fSkyScaleA;	// 하늘에 있을 때의 물체 스케일	- 퍼지는 효과 (동글게)
	float		m_fSkyScaleB;	// 하늘에 있을 때의 물체 스케일 - 터지는 효과 (파박)
	float		m_fSpeedA;		// 터지는 속도 - 퍼지는 효과 (동글게)
	float		m_fSpeedB;		// 터지는 속도 - 터지는 효과 (파박)
	float		m_fAlphaA;		// 빗방울 알파 블렌딩 값 - 퍼지는 효과 (동글게)
	float		m_fAlphaB;		// 빗방울 알파 블렌딩 값 - 터지는 효과 (파박)

	float		m_f11;
	float		m_f12;
	float		m_f13;
	float		m_f14;

	float		m_f21;
	float		m_f22;
	float		m_f23;
	float		m_f24;

	float		m_f31;
	float		m_f32;
	float		m_f33;
	float		m_f34;

	float		m_f41;
	float		m_f42;
	float		m_f43;
	float		m_f44;

	char		m_szTexture[MAX_PATH];
	char		m_szTexture2[MAX_PATH];
	char		m_szTexture3[MAX_PATH];
	char		m_szTexture4[MAX_PATH];

	RAINPOINT_PROPERTY_100()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture2, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture3, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture4, 0, sizeof(char)*MAX_PATH );
	};
};

struct RAINPOINT_PROPERTY_101
{
	float		m_fRainRate;	// 면적당 나타나는 빗방울 갯수   
	float		m_fHeight;		// 하늘에 올라갈 최고 높이
	float		m_fSpeed;		// 떨어지는 속도
	float		m_fScaleA;		// 빗방울 크기	- 퍼지는 효과 (동글게)
	float		m_fScaleB;		// 빗방울 크기 - 터지는 효과 (파박)
	float		m_fRateUp;		// 하늘에 있을때
	float		m_fRateDown;	// 땅에 있을때
	float		m_fSkyScaleA;	// 하늘에 있을 때의 물체 스케일	- 퍼지는 효과 (동글게)
	float		m_fSkyScaleB;	// 하늘에 있을 때의 물체 스케일 - 터지는 효과 (파박)
	float		m_fSpeedA;		// 터지는 속도 - 퍼지는 효과 (동글게)
	float		m_fSpeedB;		// 터지는 속도 - 터지는 효과 (파박)
	float		m_fAlphaA;		// 빗방울 알파 블렌딩 값 - 퍼지는 효과 (동글게)
	float		m_fAlphaB;		// 빗방울 알파 블렌딩 값 - 터지는 효과 (파박)

	D3DXVECTOR3	m_vMax;			// 새롭게 생김 SINGLE 로 변하면서 생김.
	D3DXVECTOR3	m_vMin;

	char		m_szRainTex[MAX_PATH];
	char		m_szRainTexZ[MAX_PATH];
	char		m_szSnowTex[MAX_PATH];
	char		m_szLeavesTex[MAX_PATH];

	RAINPOINT_PROPERTY_101()
	{
		memset( m_szRainTex, 0, sizeof(char)*MAX_PATH );
		memset( m_szRainTexZ, 0, sizeof(char)*MAX_PATH );
		memset( m_szSnowTex, 0, sizeof(char)*MAX_PATH );
		memset( m_szLeavesTex, 0, sizeof(char)*MAX_PATH );
	};
};

struct RAINPOINT_PROPERTY_102
{
	float		m_fRainRate;	// 면적당 나타나는 빗방울 갯수   
	float		m_fHeight;		// 하늘에 올라갈 최고 높이
	float		m_fSpeed;		// 떨어지는 속도
	float		m_fScaleA;		// 빗방울 크기	- 퍼지는 효과 (동글게)
	float		m_fScaleB;		// 빗방울 크기 - 터지는 효과 (파박)
	float		m_fRateUp;		// 하늘에 있을때
	float		m_fRateDown;	// 땅에 있을때
	float		m_fSkyScaleA;	// 하늘에 있을 때의 물체 스케일	- 퍼지는 효과 (동글게)
	float		m_fSkyScaleB;	// 하늘에 있을 때의 물체 스케일 - 터지는 효과 (파박)
	float		m_fSpeedA;		// 터지는 속도 - 퍼지는 효과 (동글게)
	float		m_fSpeedB;		// 터지는 속도 - 터지는 효과 (파박)
	float		m_fAlphaA;		// 빗방울 알파 블렌딩 값 - 퍼지는 효과 (동글게)
	float		m_fAlphaB;		// 빗방울 알파 블렌딩 값 - 터지는 효과 (파박)

	float		m_fSnowSize;	// 눈 크기
	float		m_fLeavesSize;  // 낙엽 크기
	float		m_fLeavesSpeed;  // 낙엽 스피드

	D3DXVECTOR3	m_vMax;			// 새롭게 생김 SINGLE 로 변하면서 생김.
	D3DXVECTOR3	m_vMin;

	char		m_szRainTex[MAX_PATH];
	char		m_szRainTexZ[MAX_PATH];
	char		m_szSnowTex[MAX_PATH];
	char		m_szLeavesTex[MAX_PATH];

	RAINPOINT_PROPERTY_102()
	{
		memset( m_szRainTex, 0, sizeof(char)*MAX_PATH );
		memset( m_szRainTexZ, 0, sizeof(char)*MAX_PATH );
		memset( m_szSnowTex, 0, sizeof(char)*MAX_PATH );
		memset( m_szLeavesTex, 0, sizeof(char)*MAX_PATH );
	};
};

struct RAINPOINT_PROPERTY_105		// Ver 103, 104, 105
{
	float		m_fRainRate;	// 면적당 나타나는 빗방울 갯수

	float		m_fSnowSize;	// 눈 크기
	float		m_fLeavesSize;  // 낙엽 크기
	float		m_fLeavesSpeed;  // 낙엽 스피드
	float		m_fGroundSize;	// 지면에 쓰이는 텍스쳐 크기

	D3DXVECTOR3	m_vMax;			// 새롭게 생김 SINGLE 로 변하면서 생김.
	D3DXVECTOR3	m_vMin;

	char		m_szRainTex[MAX_PATH];
	char		m_szCircleTex[MAX_PATH];
	char		m_szRainDropTex[MAX_PATH];
	char		m_szSnowTex[MAX_PATH];
	char		m_szLeavesTex[MAX_PATH];

	char		m_szGroundTex[MAX_PATH];
	char		m_szRain_GTex[MAX_PATH];
	char		m_szSnow_GTex[MAX_PATH];

	RAINPOINT_PROPERTY_105::RAINPOINT_PROPERTY_105 () :
		m_fRainRate(3.f),
		m_fSnowSize(0.25f),		// 눈 크기
		m_fLeavesSize(0.25f),  // 낙엽 크기
		m_fLeavesSpeed(5.f),
		m_fGroundSize(10.f),
		m_vMax(0.f,0.f,0.f),
		m_vMin(0.f,0.f,0.f)
	{
		StringCchCopy( m_szRainTex,		MAX_PATH, "RAIN.dds" );
		StringCchCopy( m_szCircleTex,	MAX_PATH, "Circle.bmp" );
		StringCchCopy( m_szRainDropTex,	MAX_PATH, "CircleZ.bmp" );
		StringCchCopy( m_szSnowTex,		MAX_PATH, "_Snow_RAIN.BMP" );
		StringCchCopy( m_szLeavesTex,	MAX_PATH, "leaf_02.tga" );

		StringCchCopy( m_szGroundTex,	MAX_PATH, "Pile_Sand_1.tga" );
		StringCchCopy( m_szRain_GTex,	MAX_PATH, "Pile_Rain_1.tga" );
		StringCchCopy( m_szSnow_GTex,	MAX_PATH, "Pile_Snow_1.tga" );
	}
};

struct SParticleData
{
	int			Count;
	float		fMinDisappear;
	float		fMaxDisappear;
	float		fMinFallSpeed;
	float		fMaxFallSpeed;

	SParticleData()
	{
	}

	SParticleData( int count, float fmindisappear, float fmaxdisappear, float fminfallspeed, float fmaxfallspeed )
	{
		Count = count;
		fMinDisappear = fmindisappear;
		fMaxDisappear = fmaxdisappear;
		fMinFallSpeed = fminfallspeed;
		fMaxFallSpeed = fmaxfallspeed;
	}
};

struct RAINPOINT_PROPERTY		// Ver 106
{
	float		m_fRainRate;	// 면적당 나타나는 빗방울 갯수

	float		m_fSnowSize;	// 눈 크기
	float		m_fLeavesSize;  // 낙엽 크기
	float		m_fLeavesSpeed;  // 낙엽 스피드
	float		m_fGroundSize;	// 지면에 쓰이는 텍스쳐 크기

	D3DXVECTOR3	m_vMax;			// 새롭게 생김 SINGLE 로 변하면서 생김.
	D3DXVECTOR3	m_vMin;

	bool		m_bNewRainEffect;

	SParticleData m_SnowParticleData;
	SParticleData m_RainParticleData;
	SParticleData m_LeavesParticleData;

	char		m_szRainTex[MAX_PATH];
	char		m_szCircleTex[MAX_PATH];
	char		m_szRainDropTex[MAX_PATH];
	char		m_szSnowTex[MAX_PATH];
	char		m_szLeavesTex[MAX_PATH];

	char		m_szGroundTex[MAX_PATH];
	char		m_szRain_GTex[MAX_PATH];
	char		m_szSnow_GTex[MAX_PATH];

	RAINPOINT_PROPERTY::RAINPOINT_PROPERTY () :
		m_fRainRate(3.f),
		m_fSnowSize(0.25f),		// 눈 크기
		m_fLeavesSize(0.25f),  // 낙엽 크기
		m_fLeavesSpeed(5.f),
		m_fGroundSize(10.f),
		m_vMax(0.f,0.f,0.f),
		m_vMin(0.f,0.f,0.f),

		m_SnowParticleData( 40, -10.0f, -7.0f, 5.0f, 10.0f ),
		m_RainParticleData( 20, -15.0f, -14.0f, 6.0f, 11.0f ),
		m_LeavesParticleData( 20, -10.0f, -7.0f, 12.0f, 18.0f ),

		m_bNewRainEffect( FALSE )
	{
		StringCchCopy( m_szRainTex,		MAX_PATH, "RAIN.dds" );
		StringCchCopy( m_szCircleTex,	MAX_PATH, "Circle.bmp" );
		StringCchCopy( m_szRainDropTex,	MAX_PATH, "CircleZ.bmp" );
		StringCchCopy( m_szSnowTex,		MAX_PATH, "_Snow_RAIN.BMP" );
		StringCchCopy( m_szLeavesTex,	MAX_PATH, "leaf_02.tga" );

		StringCchCopy( m_szGroundTex,	MAX_PATH, "Pile_Sand_1.tga" );
		StringCchCopy( m_szRain_GTex,	MAX_PATH, "Pile_Rain_1.tga" );
		StringCchCopy( m_szSnow_GTex,	MAX_PATH, "Pile_Snow_1.tga" );
	}
};

struct SParticle
{
	D3DXVECTOR3 vDir;
	D3DXVECTOR3 vPos;
	float		fDisappearYPos;
	D3DXVECTOR2	vSize;
	float		fAlpha;
	float		fFallSpeed;

	float		fWaveRate;

	bool		bRender;

	D3DXVECTOR3	vFirstCreatePos;

	SParticle() :
		vDir(0.0f,0.0f,0.0f),
		vPos(0.0f,0.0f,0.0f),
		fDisappearYPos(0.0f),
		vSize(1.0f,1.0f),
		fAlpha(1.0f),
		fFallSpeed(0.0f),

		fWaveRate( 0.0f ),

		vFirstCreatePos(0.0f,0.0f,0.0f)
	{
	}

	void CreateParticle( D3DXVECTOR3 vInputDir, D3DXVECTOR3 vInputPos, float fInputDisappearYPos, float fInputFallSpeed,
						 float fInputWaveRate, D3DXVECTOR3 vInputFirstCreatePos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) )
	{
		vDir = vInputDir;
		vPos = vInputPos;
		fWaveRate       = fInputWaveRate;
		fDisappearYPos  = fInputDisappearYPos;
		fFallSpeed      = fInputFallSpeed;
		if( vInputFirstCreatePos.x != 0.0f ||
			vInputFirstCreatePos.y != 0.0f ||
			vInputFirstCreatePos.z != 0.0f )
		vFirstCreatePos = vInputFirstCreatePos;
	}
};

struct SParticleArea
{

    D3DXVECTOR3 vMin;
	D3DXVECTOR3 vMax;
	D3DXVECTOR3 vCenter;

	std::vector<SParticle> vecSnowParticle;
	std::vector<SParticle> vecRainParticle;
	std::vector<SParticle> vecLeavesParticle;

	SParticleArea() :
		vMin(0.0f,0.0f,0.0f),
		vMax(0.0f,0.0f,0.0f),
		vCenter(0.0f,0.0f,0.0f)
	{
		vecSnowParticle.clear();
		vecRainParticle.clear();
		vecLeavesParticle.clear();
	}


	void FirstCreateParticle( const SParticleData snowData, const SParticleData rainData, const SParticleData leavesData );



};

class DxEffectRainPoint : public DxEffectBase
{
	//	Note : 이펙트 타입 정의.
	//
public:
	const static DWORD	TYPEID;
	const static DWORD	VERSION;
	const static DWORD	FLAG;
	const static char	NAME[];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return FLAG; }
	virtual const char* GetName ();

	virtual void GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer );
	virtual void SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer );
	
	//	Note : 쉐이더.
	//
protected:
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Effect;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Snow;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Leaves;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Ground;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Effect_SAVE;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Snow_SAVE;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Leaves_SAVE;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Ground_SAVE;

	static LPDIRECT3DSTATEBLOCK9	m_pSB_FireFly;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_FireFly_SAVE;

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			RAINPOINT_PROPERTY	m_Property;
		};

		struct
		{
			float		m_fRainRate;	// 면적당 나타나는 빗방울 갯수

			float		m_fSnowSize;	// 눈 크기
			float		m_fLeavesSize;  // 낙엽 크기
			float		m_fLeavesSpeed;  // 낙엽 스피드
			float		m_fGroundSize;	// 지면에 쓰이는 텍스쳐 크기

			D3DXVECTOR3	m_vMax;			// 새롭게 생김 SINGLE 로 변하면서 생김.
			D3DXVECTOR3	m_vMin;

			bool		m_bNewRainEffect;

			SParticleData m_SnowParticleData;
			SParticleData m_RainParticleData;
			SParticleData m_LeavesParticleData;

			char		m_szRainTex[MAX_PATH];
			char		m_szCircleTex[MAX_PATH];
			char		m_szRainDropTex[MAX_PATH];
			char		m_szSnowTex[MAX_PATH];
			char		m_szLeavesTex[MAX_PATH];

			char		m_szGroundTex[MAX_PATH];
			char		m_szRain_GTex[MAX_PATH];
			char		m_szSnow_GTex[MAX_PATH];
		};
	};

public:
	void SetProperty ( RAINPOINT_PROPERTY& Property )
	{
		m_Property = Property;
	}
	RAINPOINT_PROPERTY& GetProperty () { return m_Property; }

protected:
	float		m_fRainSize;			// 비 사이즈......... 추후 버전업 시 올리자.

	LPDIRECT3DTEXTUREQ	m_pRainTex;
	LPDIRECT3DTEXTUREQ	m_pCircleTex;
	LPDIRECT3DTEXTUREQ	m_pRainDropTex;
	LPDIRECT3DTEXTUREQ	m_pddsSnowTex;
	LPDIRECT3DTEXTUREQ	m_pddsLeavesTex;

	LPDIRECT3DTEXTUREQ	m_pGroundTex;
	LPDIRECT3DTEXTUREQ	m_pRain_GTex;
	LPDIRECT3DTEXTUREQ	m_pSnow_GTex;
	LPDIRECT3DTEXTUREQ	m_pFireFlyTex;

	struct VERTEX
	{
		D3DXVECTOR3 p;       // vertex position
		D3DXVECTOR3 n;       // vertex normal
		D3DXVECTOR2 t;
		static const DWORD FVF;
	};

	struct D3DVERTEX
	{
		D3DXVECTOR3		vPos;
		D3DCOLOR		Diffuse;
		D3DXVECTOR2		vTex;
		
		static const DWORD FVF;
	};

	struct RAINVERTEX
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vDirect;
		float		fHeight;
		float		fSize;
		float		fAlpha;
		float		fRandom;	// Loop point
		float		fLoopSum;	// Loop point
		BOOL		bRainDrop;
	};

	struct SNOWVERTEX
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3	vDirection;
		float		fSize;
		float		fRandom; // Loop point
		float		fLoopSum; // Loop point
	};

	struct LEAVESVERTEX
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3	vDirection;
		D3DXVECTOR2	vWindPos;			// 바람에 의한 낙엽 위치 누적값
		float		fSize;
		float		fRandom; // Loop point
		float		fLoopSum; // Loop point
		float		fRotate;
	};

	struct FIREFLY
	{
		D3DXVECTOR3 vPos;			// 현재 위치
		D3DXVECTOR3 vDirect;		// 가고자 하는 방향		- 변해야 한다.
		float		fSpeed;			// 이 녀석의 스피드
		float		fLife;			// 이 녀석의 현재 생명
		float		fLifeEnd;		// 이 녀석의 전체 생명
		BOOL		bChangeUSE;		// 방향변경
		float		fSize;			// 처음 사이즈
		D3DXVECTOR4 vColor;			// 처음 색
		FIREFLY*	pNext;

		FIREFLY();
		~FIREFLY();
	};

	struct FIREFLYORIGINAL
	{
		float	fCurTime;	// 현재 시간.
		float	fEndTime;	// 루프 시간.
	};

	struct RAINRENDER
	{
		D3DXVECTOR3 vPos;
		D3DCOLOR	cColor;
		D3DXVECTOR2	vTex;
		const static DWORD FVF;
	};

	struct SNOWRENDER
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR2	vTex;
		const static DWORD FVF;
	};

	struct LEAVESRENDER
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vNor;
		D3DXVECTOR2	vTex;
		const static DWORD FVF;
	};

	struct POSITIONBOOL
	{
		D3DXVECTOR3 vPos;
		BOOL		bUse;
	};

	POSITIONBOOL*		m_pVertexArray;		// 옥트리 일때는 값이 필요가 없다.
	DWORD				m_dwFaces;

	DxFrame				*m_pframeCur;

protected:
	static const	float			RAINTIME;
	static const	float			RAINDROP_S;
	static const	float			SNOWTIME;
	static const	float			SNOWTIMEUP;
	static const	float			SNOWTIMEDOWN;
	static const	float			SNOWTIMEALPHA;
	static const	float			LEAVESTIME;
	static const	float			LEAVESTIMEUP;
	static const	float			LEAVESTIMEDOWN;
	static const	float			LEAVESTIMEALPHA;

	static const	float			SEPARATELENGTH;
	static const	DWORD			MAX_OBJECT;
	static const	DWORD			MAX_OBJECT_LEAVES;

	static LPDIRECT3DVERTEXBUFFERQ		m_pVB;			// 사각형의 VB
	static LPDIRECT3DVERTEXBUFFERQ		m_pColorVB_1;	// 디퓨즈가 들어간 VB
	static LPDIRECT3DVERTEXBUFFERQ		m_pColorVB_2;	// 디퓨즈가 들어간 VB
	static LPDIRECT3DVERTEXBUFFERQ		m_pLeavesVB;	// Leaves VB
	static LPDIRECT3DINDEXBUFFERQ		m_pIB;			// 사각형의 IB
	static LPDIRECT3DVERTEXBUFFERQ		m_pNewSnowVB;	// New Snow VB
	static LPDIRECT3DVERTEXBUFFERQ		m_pNewLeavesVB;	// New Leave VB

	typedef CMemPool<FIREFLY>		FIREFLYPOOL;
	static FIREFLYPOOL*				m_pFireFlyPool;
	static FIREFLY*					m_pFireFlyHead;
	static D3DVERTEX*				m_pFireFlyArray;
	static DWORD					m_dwFireFlyCount;
	static const DWORD				MAX_FIREFLY;

protected:
	class CNEWRAINDATA
	{
	public:										// 공통적인 것
		static float				fElapsedTime;
		static DWORD				dwFlag;

	protected:									// PLAY, 일시적인 것
		LPDIRECT3DTEXTUREQ			pRainTex;
		LPDIRECT3DTEXTUREQ			pCircleTex;
		LPDIRECT3DTEXTUREQ			pRainDropTex;
		LPDIRECT3DTEXTUREQ			pSnowTex;
		LPDIRECT3DTEXTUREQ			pLeavesTex;

		LPDIRECT3DTEXTUREQ			pGroundTex;
		LPDIRECT3DTEXTUREQ			pRain_GTex;
		LPDIRECT3DTEXTUREQ			pSnow_GTex;

	public:
		std::vector<SParticleArea>		vecParticleArea;
		
		CNEWRAINDATA()
		{
			vecParticleArea.clear();
		}

		void CreateParticleArea( D3DXVECTOR3 vMin, D3DXVECTOR3 vMax, D3DXVECTOR3 vCenter,
							     SParticleData SnowParticleData, SParticleData RainParticleData, SParticleData LeavesParticleData );

		void SetTexture ( LPDIRECT3DTEXTUREQ _pRainTex, LPDIRECT3DTEXTUREQ _pCircleTex, LPDIRECT3DTEXTUREQ _pRainDropTex,
			LPDIRECT3DTEXTUREQ _pSnowTex, LPDIRECT3DTEXTUREQ _pLeavesTex,
			LPDIRECT3DTEXTUREQ _pGroundTex, LPDIRECT3DTEXTUREQ _pRain_GTex, LPDIRECT3DTEXTUREQ _pSnow_GTex );

		void NewRender( LPDIRECT3DDEVICEQ pd3dDevice );

		bool	UpdateNewSnow( SParticleArea &partArea );
		bool	UpdateNewRain( SParticleArea &partArea );
		bool	UpdateNewLeaves( SParticleArea &partArea );
		void	RenderNewSnow( SParticleArea &partArea, LPDIRECT3DDEVICEQ pd3dDevice );
		void	RenderNewRain( SParticleArea &partArea, LPDIRECT3DDEVICEQ pd3dDevice );
		void	RenderNewLeaves( SParticleArea &partArea, LPDIRECT3DDEVICEQ pd3dDevice );


	};
	class SEPERATEOBJ : public OBJAABB
	{
	public:										// 공통적인 것
		float						fTime;
		static	float				fElapsedTime03;
		static	float				m_fElapsedTime;
		static	DWORD				dwFlag;
		static	float				fRainSize;
		static	float				fSnowSize;
		static	float				fLeavesSize;
		static	float				fLeavesSpeed;

	protected:									// PLAY, 일시적인 것
		LPDIRECT3DTEXTUREQ			pRainTex;
		LPDIRECT3DTEXTUREQ			pCircleTex;
		LPDIRECT3DTEXTUREQ			pRainDropTex;
		LPDIRECT3DTEXTUREQ			pSnowTex;
		LPDIRECT3DTEXTUREQ			pLeavesTex;

		LPDIRECT3DTEXTUREQ			pGroundTex;
		LPDIRECT3DTEXTUREQ			pRain_GTex;
		LPDIRECT3DTEXTUREQ			pSnow_GTex;

	public:										// 저장하는것
		DWORD				dwFaces;
		DWORD				dwGroundFaces;
		D3DXVECTOR3			vMax, vMin, vCenter;
		D3DXVECTOR3*		pVertexArray;
		RAINVERTEX*			pRainArray;
		SNOWVERTEX*			pSnowArray;
		LEAVESVERTEX*		pLeavesArray;
		FIREFLYORIGINAL*	pFireFlyArray;



		SEPERATEOBJ*		pNext;




		SEPERATEOBJ () :
			fTime(0.f),
			dwFaces(0),
			dwGroundFaces(0),
			vMax(0,0,0),
			vMin(0,0,0),
			pVertexArray(NULL),
			pRainArray(NULL),
			pSnowArray(NULL),
			pLeavesArray(NULL),
			pFireFlyArray(NULL),
			pRainTex(NULL),
			pCircleTex(NULL),
			pRainDropTex(NULL),
			pSnowTex(NULL),
			pLeavesTex(NULL),
			pGroundTex(NULL),
			pRain_GTex(NULL),
			pSnow_GTex(NULL),
			pNext(0)
		{
		}

		~SEPERATEOBJ ()
		{
			SAFE_DELETE_ARRAY ( pVertexArray );
			SAFE_DELETE_ARRAY ( pRainArray );
			SAFE_DELETE_ARRAY ( pSnowArray );
			SAFE_DELETE_ARRAY ( pLeavesArray );
			SAFE_DELETE_ARRAY ( pFireFlyArray );
			SAFE_DELETE(pNext);
		}

		HRESULT	CreateSeperateVB ( LPDIRECT3DDEVICEQ pd3dDevice );

		void	SetTexture ( LPDIRECT3DTEXTUREQ _pRainTex, LPDIRECT3DTEXTUREQ _pCircleTex, LPDIRECT3DTEXTUREQ _pRainDropTex,
									LPDIRECT3DTEXTUREQ _pSnowTex, LPDIRECT3DTEXTUREQ _pLeavesTex,
									LPDIRECT3DTEXTUREQ _pGroundTex, LPDIRECT3DTEXTUREQ _pRain_GTex, LPDIRECT3DTEXTUREQ _pSnow_GTex );

		void	UpdateRain ( float fElapsedTime, float fAngle, DWORD dwRenderFaces );
		void	UpdateParticle ( float fElapsedTime, DWORD dwRenderFaces );
		void	UpdateLeaves ( float fElapsedTime, DWORD dwRenderFaces );



		void CreateFireFly( const D3DXVECTOR3& vSrcPos );

		float	GetLengthRate ( D3DXVECTOR3 &vLookat );

		virtual void GetAABBSize ( D3DXVECTOR3 &_vMax, D3DXVECTOR3 &_vMin )
		{
			_vMax = vMax; _vMin = vMin;
		}

		virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
		{
			return COLLISION::IsWithInPoint ( vDivMax, vDivMin, (vMax+vMin)/2 );
		}

		virtual OBJAABB* GetNext () { return pNext; }

		virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV )
		{
			fTime += m_fElapsedTime;

			RenderGround ( pd3dDevice, pCV );

			{
				if ( dwFlag & PLAY_LEAVES )		RenderLeaves ( pd3dDevice, pCV );

				if( dwFlag & (PLAY_RAIN | PLAY_SNOW) )
				{
					if ( dwFlag & PLAY_RAIN )		RenderRain ( pd3dDevice, pCV );
					if ( dwFlag & PLAY_SNOW )		RenderSnow ( pd3dDevice, pCV );
				}
				else
				{
					RenderFireFly( pd3dDevice, pCV );
				}
			
			}
			return S_OK;
		}

	public:
		void	Save( CSerialFile &SFile );
		void	Load( CSerialFile &SFile, DWORD version );

	protected:
		HRESULT RenderRain ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV );
		HRESULT RenderSnow ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV );
		HRESULT RenderLeaves ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV );
		void RenderFireFly( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV );

		HRESULT RenderGround ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV );
	};



	D3DXMATRIX			m_matWorld;
	D3DXMATRIX			m_matFrameComb;
	SEPERATEOBJ*		m_pSeperateList;
	OBJAABBNode*		m_pSeperateTree;
	CNEWRAINDATA 		m_NewRainData;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

	static void CreateDevice_FireFly( LPDIRECT3DDEVICEQ pd3dDevice );
	static void ReleaseDevice_FireFly( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	HRESULT AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT AdaptToDxFrame ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );

	HRESULT AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : 
	//
public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );	
	HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );


	

	void PutSleepFireFly( FIREFLY* pFireFlys );
	void PutSleepFireFlys( FIREFLY* pFireFlys );
	void FrameMoveFireFly( float fElapsedTime );
	void RenderFireFly( LPDIRECT3DDEVICEQ pd3dDevice );

private:
	HRESULT MakeEffRainMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs );
	float	CalcSurfaceArea(VERTEX* pFV[3]);

protected:
	void	SeparateVB ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 vMax, D3DXVECTOR3 vMin, float fDistance );
	void	FindPoint ( SEPERATEOBJ* pCur, D3DXVECTOR3 vMax, D3DXVECTOR3 vMin );
	void	SetSeperateTexture ();

	HRESULT SetBoundBox ( LPD3DXMESH pMesh, D3DXMATRIX *matWorld );

public:
	DxEffectRainPoint();
	virtual ~DxEffectRainPoint();

//Note: OBJAABB 부부분 
public:
	virtual void GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual BOOL IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );

protected:
	virtual void SaveBuffer ( CSerialFile &SFile );
	virtual void LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : DxFrameMesh 의 효과를 DxLandMan 에 삽입 할 때 대량의 버퍼 데이타를
	//		복제하기 위해서 사용한다.
public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice );
};

#endif // !defined(AFX_DXEFFECTRAINPOINT_H__INCLUDED_)
