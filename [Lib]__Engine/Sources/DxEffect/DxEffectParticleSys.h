#ifndef __CPARTICLESYSTEM_H_
#define __CPARTICLESYSTEM_H_

#pragma once

#include "DxEffSingle.h"
#include "dxsimplemeshman.h"
#include "CMemPool.h"

//#define GRAVITY			D3DXVECTOR3 ( 0.0f, 9.8f, 0.0f)

#define	USERANGE					0x00000001	// 범위 체크
#define	USESPHERE					0x00000002	// 구 체크
#define	USERANGEHIGH				0x00000004	// 범위 회전 체크
#define	USEROTATEL					0x00000008	// 회전 체크
#define	USECOLLISION				0x00000020	// 파티클 충돌 체크
#define	USEATTRACTION				0x00000040	// 자력	
#define	USECENTER					0x00000080	// 모이기
#define	USEDIRECTION				0x00000100	// 퍼지는 방향이 있죠..
#define	USETEXSCALE					0x00000200	// 텍스쳐 늘이기
#define	USETEXROTATE				0x00000400	// 텍스쳐 회전
#define	USEMESH						0x00000800	// 메쉬 사용 체크
#define	USEPARENTROTATEWORLD		0x00001000	// 부모의 회전 매트릭스 적용 함	-_-; 다른 방법 없을까?	월드 에디트  용
#define	USEPARENTMOVE				0x00002000	// 부모의 이동 매트릭스 적용 함	-_-; 다른 방법 없을까?
#define	USEPARENTROTATE				0x00008000	// 부모의 이동 매트릭스 적용 함	-_-; 다른 방법 없을까?
#define	USESEQUENCE					0x00010000	// 시퀀스 효과 삽입
#define	USESEQUENCELOOP				0x00020000	// 시퀀스 효과 루프 돌기
#define	USEBILLBOARDALL				0x00040000	// 모든 파티클에 빌보드 적용 시키기
#define	USEBILLBOARDUP				0x00080000	// 빌보드 했을 때 빠닥 세우기 하자.!!
#define	USETEXTURE					0x00100000	// USEMESH, UESTEXTURE, USEMESH+UESTEXTURE 3가지 경우를 만들 수 있다.
#define	USEBEZIER_P					0x00200000	// 추가 파티클..( 시간이 너무 짧을 때, 이동 할 때 사용한다. )
#define	USEGROUND					0x00400000	// 지면 효과
#define	USEFROMTO					0x00800000	// 어디에서 어디로 목표가 이동한다.
#define	USEGROUNDATTACH				0x01000000	// 지면에 파티클이 달라 붙는다.
#define	USEHEIGHTNO					0x02000000	// 높이값의 변화가 없도록 한다.
#define USENORMAL2					0x04000000	// 노멀
#define USEGROUNDTEX				0x08000000	// 지면 TEX 사용
#define USERANDOMPLAY				0x10000000	// 랜덤한 효과의 생성
#define USERANDOMDIRECT				0x20000000	// 랜덤한 효과의 생성 때의 랜덤 방향
#define USENEWEFF					0x40000000	// 새로운 효과의 생성
#define USENEWEFF_END				0x80000000	// 새로운 효과의 생성.. 부모효과의 끝에 생성시킨다. < 보통은 처음부터 유지 >
#define	USENEWEFF_END_STOP			0x00000010	// 새로운 효과의 생성.. 부모효과의 끝에 생성시킨다. 부모는 움직임을 멈춘다.

// Note : 삭제 대상
#define	USEGOTOCENTER				0x00004000	// 센터 방향으로 이동 시킴

#define STRING_NUM_256		256

 
// useful macro to guarantee that values are within a given range
#define Clamp(x, min, max)  ( x = (x<min ? min : x<max ? x : max) )

struct PARTICLESYS_PROPERTY : public EFF_PROPERTY
{
	enum { TYPEID = EFFSINGLE_PARTICE, };

	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual const char* GetName ()	{ return NAME; }

	struct PROPERTY				// Ver.106
	{
		//	Note : 파티클 시스템.
		//
		D3DXVECTOR3		m_vGVelocity;
		D3DXVECTOR3		m_vGGravityStart;
		D3DXVECTOR3		m_vGGravityEnd;

		D3DXVECTOR2		m_vPlayTime;
		D3DXVECTOR2		m_vSleepTime;

		DWORD			m_dwFlag;
		int				m_iCenterPoint;

		D3DXVECTOR3		m_vRange;		// 범위	회전
		float			m_fRangeRate;	// 속 안에 파티클이 생성 안됨.
		float			m_fRotateAngel;

		float			m_fRotateLAngel;

		float			m_fRotateLRate1;
		float			m_fRotateLRate2;

		float			m_fLengthStart;
		float			m_fLengthMid1;
		float			m_fLengthMid2;
		float			m_fLengthEnd;

		DWORD			m_uParticlesPerSec;

		char			m_szTexture[STRING_NUM_256];
		char			m_szMeshFile[STRING_NUM_256];
		char			m_szEffFile[STRING_NUM_256];

		//	Note : 파티클.
		//
		D3DXVECTOR3		m_vWindDir;		//  바람 디폴트 추가

		float			m_fTexRotateAngel;

		D3DXVECTOR4		m_vTexScale;

		float			m_fGravityStart;
		float			m_fGravityVar;
		float			m_fGravityEnd;

		float			m_fSizeStart;
		float			m_fSizeVar;
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

		float			m_fSpeed;
		float			m_fSpeedVar;

		float			m_fLife;
		float			m_fLifeVar;

		float			m_fTheta;

		int				m_nCol;			// 행
		int				m_nRow;			// 열
		float			m_fAniTime;		// 설정 시간

		int				m_nBlend;
		int				m_nPower;

		PROPERTY()
		{
			memset( m_szTexture, 0, sizeof(char)*STRING_NUM_256 );
			memset( m_szMeshFile, 0, sizeof(char)*STRING_NUM_256 );
			memset( m_szEffFile, 0, sizeof(char)*STRING_NUM_256 );
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
			D3DXVECTOR3		m_vGVelocity;
			D3DXVECTOR3		m_vGGravityStart;
			D3DXVECTOR3		m_vGGravityEnd;

			D3DXVECTOR2		m_vPlayTime;
			D3DXVECTOR2		m_vSleepTime;

			DWORD			m_dwFlag;
			int				m_iCenterPoint;

			D3DXVECTOR3		m_vRange;		// 범위	회전
			float			m_fRangeRate;	// 속 안에 파티클이 생성 안됨.
			float			m_fRotateAngel;

			float			m_fRotateLAngel;

			float			m_fRotateLRate1;
			float			m_fRotateLRate2;

			float			m_fLengthStart;
			float			m_fLengthMid1;
			float			m_fLengthMid2;
			float			m_fLengthEnd;

			DWORD			m_uParticlesPerSec;

			char			m_szTexture[STRING_NUM_256];
			char			m_szMeshFile[STRING_NUM_256];
			char			m_szEffFile[STRING_NUM_256];

			//	Note : 파티클.
			//
			D3DXVECTOR3		m_vWindDir;		//  바람 디폴트 추가

			float			m_fTexRotateAngel;

			D3DXVECTOR4		m_vTexScale;

			float			m_fGravityStart;
			float			m_fGravityVar;
			float			m_fGravityEnd;

			float			m_fSizeStart;
			float			m_fSizeVar;
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

			float			m_fSpeed;
			float			m_fSpeedVar;

			float			m_fLife;
			float			m_fLifeVar;

			float			m_fTheta;

			int				m_nCol;			// 행
			int				m_nRow;			// 열
			float			m_fAniTime;		// 설정 시간

			int				m_nBlend;
			int				m_nPower;
		};
	};

	struct D3DMESHVERTEX
	{
		D3DXVECTOR3		vPos;
		D3DXVECTOR3		vNormal;
		float			tu, tv;		
		
		static const DWORD FVF;
	};

	//	Note : 타입 마다 존재하는 리소스.
	DxSimMesh*				m_pMesh;
	LPDIRECT3DTEXTUREQ		m_pTexture;
	EFF_PROPGROUP*			m_pPropGroup;

	//	Note : 이팩트 생성자.
	//
	virtual DxEffSingle* NEWOBJ ();
	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( CSerialFile &SFile );

	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	virtual HRESULT DeleteDeviceObjects();

	PARTICLESYS_PROPERTY () : EFF_PROPERTY(),
		m_vGVelocity(0,0,0),
		m_vGGravityStart(0.f,0.f,0.f),
		m_vGGravityEnd(0.f,0.f,0.f),

		m_vPlayTime(0.5f,5.f),
		m_vSleepTime(0.5f,5.f),

		m_vRange(5.f,0.f,5.f),
		m_fRangeRate(0.0f),
		m_fRotateAngel(200.f),

		m_fRotateLAngel(2.f),

		m_fRotateLRate1(25.f),
		m_fRotateLRate2(75.f),

		m_fLengthStart(0.f),
		m_fLengthMid1(2.f),
		m_fLengthMid2(1.f),
		m_fLengthEnd(0.f),
		
		m_uParticlesPerSec(10),

		m_vWindDir(0.f,0.f,0.f),

		m_fTexRotateAngel(10.f),

		m_vTexScale(1.f,1.f,1.f,30.f),	// 

		m_fGravityStart(0.0f),
		m_fGravityVar(0.0f),
		m_fGravityEnd(0.0f),

		m_fSizeStart(15.0f),
		m_fSizeVar(0.0f),
		m_fSizeEnd(15.0f),

		m_fAlphaRate1(25.0f),
		m_fAlphaRate2(75.0f),

		m_fAlphaStart(0.0f),
		m_fAlphaMid1(1.0f),
		m_fAlphaMid2(1.0f),
		m_fAlphaEnd(0.0f),

		m_cColorStart(1.0f,1.0f,1.0f,1.0f),
		m_cColorVar(0.0f,0.0f,0.0f,0.0f),
		m_cColorEnd(1.0f,1.0f,1.0f,1.0f),

		m_fSpeed(5.0f),
		m_fSpeedVar(0.0f),

		m_fLife(2.0f),
		m_fLifeVar(0.5f),

		m_fTheta(360.f),

		m_nCol(4),			// 행
		m_nRow(4),		// 열
		m_fAniTime(0.03f),		// 설정 시간

		m_nPower(1),
		m_nBlend(0),
		
		m_pMesh(NULL),
		m_pTexture(NULL),
		m_pPropGroup(NULL)
	{
		m_dwFlag = 0;
		m_dwFlag |= USETEXTURE;
		m_iCenterPoint = 1;		// 위, 중심, 아래

		StringCchCopy( m_szTexture,		STRING_NUM_256, "_Eff_flare.tga" );
		StringCchCopy( m_szMeshFile,	STRING_NUM_256, "sd_stone.X" );
		StringCchCopy( m_szEffFile,		STRING_NUM_256, "" );
	}
};

class DxEffectParticleSys : public DxEffSingle
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

	struct D3DPARTICLEVERTEX
	{
		D3DXVECTOR3		vPos;
		float			fSize;
		D3DXVECTOR4		Diffuse;
		
		static const DWORD FVF;
	};

	struct PARTICLE
	{
	public:
		D3DXVECTOR3		m_vPrevLocation;
		D3DXVECTOR3		m_vLocation;
		D3DXVECTOR3		m_vVelocity;

		float			m_fAlphaTime1;
		float			m_fAlphaTime2;

		float			m_fAlpha;
		float			m_fAlphaDelta1;
		float			m_fAlphaDelta2;
		float			m_fAlphaDelta3;

		D3DXVECTOR4		m_cColor;
		D3DXVECTOR3		m_cColorDelta;

		float			m_fAge;
		float			m_fLifetime;
		
		float			m_fSize;
		float			m_fSizeDelta;

		float			m_fGravity;
		float			m_fGravityDelta;

		D3DXVECTOR2		m_vTexScale;
		D3DXVECTOR2		m_vTexScaleDelta;

		float			m_fTexRotateAngel;
		float			m_fTexRotateAngelSum;

		int				m_nNowSprite;		// 지금 녀석의 스프라이트 번호
		BOOL			m_bSequenceRoop;

		D3DXMATRIX		m_matRotate;

		bool			m_bCollision;

		D3DXVECTOR3		m_vForceFull;		// 이동하고 있는 방향으로 영향을 받는다.
		float			m_fForceVar;

		BOOL				m_bPlay;
		D3DXMATRIX			m_matWorld;
		DxEffSingleGroup*	m_pSingleGroup;	//	활성 개체.	음. 난감. 새로 만들어야 하네. ㅜㅜ
		
	public:
		PARTICLE();
		~PARTICLE();

	public:
		BOOL FrameMove( float fTimeDelta, D3DXVECTOR3 *pAttractLoc, D3DXVECTOR3 *pGravity, BOOL bFromTo, DWORD dwFlag );
		void CreateSingleGroup( LPDIRECT3DDEVICEQ pd3dDevice, EFF_PROPGROUP* pPropGroup );

	public:
		PARTICLE		*m_pNext;
	};
	typedef PARTICLE*	LPPARTICLE;

	typedef CMemPool<DxEffectParticleSys::PARTICLE> PARTICLEPOOL;

	//	Note : 정적 수치 참조 값.
	//
protected:
	static const float	MIN_SPEED;		// in world units / sec
	static const float	MIN_LIFETIME;	// in seconds
	static const float	MIN_SIZE;		// in world units
	static const float	MIN_GRAVITY;	// as a multiple of normal gravity 
	static const float	MIN_ALPHA;		// as a ratio 

	static const float	MAX_SPEED;		// in world units / sec
	static const float	MAX_LIFETIME;	// in seconds
	static const float	MAX_SIZE;		// in world units
	static const float	MAX_GRAVITY;	// as a multiple of normal gravity 
	static const float	MAX_ALPHA;		// as a ratio 

	static const float	_SCALEFACT_PAR;

	//	Note : 파티클 공통 리소스 자원들 관리.
	//
protected:
	static BOOL						VAILEDDEVICE;
	static const DWORD				MAX_PARTICLES;
	static LPDIRECT3DINDEXBUFFERQ	m_pIBuffer;

	static D3DVERTEX*				m_pVertices;

	static const char				m_strParticle[];
	static const DWORD				m_dwParticleVertexShader[];
	static DWORD					m_dwPointSpriteSh;

	static PARTICLEPOOL*			m_pParticlePool;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedRenderStats;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawRenderStats;

	static D3DXVECTOR3				START_POS;
	static D3DXVECTOR3				TARGET_POS;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnInitDevice_STATIC();
	static void OnDeleteDevice_STATIC();

	static void PutSleepParticle ( PARTICLE* pParticle );
	static void PutSleepParticles ( PARTICLE* pParticles );

	static PARTICLEPOOL* GetParticlePool ()		{ return m_pParticlePool; }

	//-------------------------------------------------------------------------------------------
public:
	DxEffectParticleSys ();
	virtual ~DxEffectParticleSys ();

protected:
	void	NewParticleCreate( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, D3DXVECTOR3 vPos_NowPrev );	// 새로운 파티클을 계속 생산해 나간다.
	void	SetMatrix( D3DXMATRIX &matWorld, D3DXMATRIX &matComb );												// 매트릭스의 셋팅

public:
	virtual void CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );

public:
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb );

	virtual HRESULT FinalCleanup();

public:
//	BOOL			IsColliding ()							{ return m_bIsColliding; }
	BOOL			IsAttractive ()							{ return (m_dwFlag & USEATTRACTION); }

	void			GetLocation ( D3DVECTOR& vLocation )	{ vLocation = m_vGLocation; }
	DWORD			NumParticles ()							{ return m_uParticlesAlive; }

public:
	DWORD			m_uParticlesAlive;

	DWORD			m_uParticlesCreated;

	float			m_fElapsedTime;

public:
	float			m_fEmissionResidue;
	LPPARTICLE		m_pParticleHead;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	D3DXMATRIX			m_matWorld;

	float				m_fRotateAngelSum;
	float				m_fRotateLAngelSum;
	float				m_fElapsedTimeSum;

	float				m_fAlphaDelta1;
	float				m_fAlphaDelta2;
	float				m_fAlphaDelta3;

	float				m_fAlphaTime1;
	float				m_fAlphaTime2;

	float				m_fRotateLDelta1;
	float				m_fRotateLDelta2;
	float				m_fRotateLDelta3;

	float				m_fRotateLTime1;
	float				m_fRotateLTime2;

	D3DXVECTOR2			m_vTexScale;
	D3DXVECTOR2			m_vTexScaleDelta;

	

	//	Note : 파티클 시스템 덩어리 제어.
	//
public:
	D3DXVECTOR3		m_vGLocation;
	D3DXVECTOR3		m_vGPrevLocation;

	D3DXVECTOR3		m_vGVelocity;

	D3DXVECTOR3		m_vGGravity;
	D3DXVECTOR3		m_vGGravityDelta;

	DWORD			m_dwFlag;
	int				m_iCenterPoint;

	D3DXVECTOR3		m_vRange;		// 범위	회전
	float			m_fRangeRate;	// 속 안에 파티클이 생성 안됨.
	float			m_fRotateAngel;

	float			m_fRotateLAngel;

	float			m_fLength;

	int				m_nPower;
	int				m_nBlend;

	BOOL			m_bRandomPlay;
	float			m_fRandomPlayTime;
	D3DXVECTOR3		m_vPlayTime;		// 시작값, 랜덤범위, 매번의 PlayTime
	D3DXVECTOR3		m_vSleepTime;		// 시작값, 랜덤범위, 매번의 SleepTime
	int				m_nCountR;
	D3DXMATRIX		m_matRandomR;

public:
	DxSimMesh*				m_pMesh;
	EFF_PROPGROUP*			m_pPropGroup;

	std::string				m_strTexture;
	LPDIRECT3DTEXTUREQ		m_pTexture;
	std::string				m_szCollTex;
	LPDIRECT3DTEXTUREQ		m_pCollTEX;		// 충돌된 텍스쳐.

	//	Note : 파티클 각각 설정.
	//
public:
	DWORD			m_uParticlesPerSec;

	D3DXVECTOR3		m_vWindDir;		//  바람 디폴트 추가

	float			m_fTexRotateAngel;

	float			m_fGravityStart;
	float			m_fGravityVar;
	float			m_fGravityEnd;

	float			m_fSizeStart;
	float			m_fSizeVar;
	float			m_fSizeEnd;

	float			m_fAlpha;

	D3DXCOLOR		m_cColorStart;
	D3DXCOLOR		m_cColorVar;
	D3DXCOLOR		m_cColorEnd;

	float			m_fSpeed;
	float			m_fSpeedVar;

	float			m_fLife;
	float			m_fLifeVar;

	D3DXMATRIX		m_matRotate;

	float			m_fTheta;

	int				m_nCol;			// 행
	int				m_nRow;			// 열
	float			m_fAniTime;		// 설정 시간

	int				m_nAllSprite;		// 전체 스프라이트 갯수
	float			m_fTimeSum;

	float			m_fForceVar;
	
protected:
	D3DXMATRIX		matPrevComb;

public:
	typedef CMemPool<DxEffectParticleSys>	PARTICLESYSPOOL;
    static PARTICLESYSPOOL*					m_pPool;
};

#endif //__CPARTICLESYSTEM_H_