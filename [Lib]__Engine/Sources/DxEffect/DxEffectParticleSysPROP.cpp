#include "pch.h"

#include "./TextureManager.h"

#include "./DxEffectMan.h"
#include "./DxEffSingleMan.h"
#include "./SerialFile.h"

#include "./DxEffectParticleSys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//									P	R	O	P	E	R	T	Y		 V	E	R	S	I	O	N
// -----------------------------------------------------------------------------------------------------------------------------------------
struct PROPERTY_100
{
	//	Note : 파티클 시스템.
	//
	D3DXVECTOR3		m_vGVelocity;

	D3DXVECTOR3		m_vGGravityStart;
	D3DXVECTOR3		m_vGGravityEnd;

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

	//	Note : 파티클.
	//
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

	int				m_nBlend;

	PROPERTY_100()
	{
		memset( m_szTexture, 0, sizeof(char)*STRING_NUM_256 );
		memset( m_szMeshFile, 0, sizeof(char)*STRING_NUM_256 );
	};
};

struct PROPERTY_101
{
	//	Note : 파티클 시스템.
	//
	D3DXVECTOR3		m_vGVelocity;

	D3DXVECTOR3		m_vGGravityStart;
	D3DXVECTOR3		m_vGGravityEnd;

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

	int				m_nBlend;

	PROPERTY_101()
	{
		memset( m_szTexture, 0, sizeof(char)*STRING_NUM_256 );
		memset( m_szMeshFile, 0, sizeof(char)*STRING_NUM_256 );
	};
};

struct PROPERTY_102_103
{
	//	Note : 파티클 시스템.
	//
	D3DXVECTOR3		m_vGVelocity;				// 추후 삭제

	D3DXVECTOR3		m_vGGravityStart;			// 추후 삭제
	D3DXVECTOR3		m_vGGravityEnd;				// 추후 삭제

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

	PROPERTY_102_103()
	{
		memset( m_szTexture, 0, sizeof(char)*STRING_NUM_256 );
		memset( m_szMeshFile, 0, sizeof(char)*STRING_NUM_256 );
	};
};

struct PROPERTY_104				// Ver.104
{
	//	Note : 파티클 시스템.
	//
	D3DXVECTOR3		m_vGVelocity;				// 추후 삭제

	D3DXVECTOR3		m_vGGravityStart;			// 추후 삭제
	D3DXVECTOR3		m_vGGravityEnd;				// 추후 삭제

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

	PROPERTY_104()
	{
		memset( m_szTexture, 0, sizeof(char)*STRING_NUM_256 );
		memset( m_szMeshFile, 0, sizeof(char)*STRING_NUM_256 );
	};
};

struct PROPERTY_105				// Ver.105
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

	PROPERTY_105()
	{
		memset( m_szTexture, 0, sizeof(char)*STRING_NUM_256 );
		memset( m_szMeshFile, 0, sizeof(char)*STRING_NUM_256 );
	};
};

// -----------------------------------------------------------------------------------------------------------------------------------------
//									P	R	O	P	E	R	T	Y			N	E	W	O	B	J
// -----------------------------------------------------------------------------------------------------------------------------------------
const DWORD	PARTICLESYS_PROPERTY::VERSION	= 0x0106;
const char	PARTICLESYS_PROPERTY::NAME[]	= "파티클";

DxEffSingle* PARTICLESYS_PROPERTY::NEWOBJ ()
{
	//DxEffectParticleSys *pEffect = new DxEffectParticleSys;
	DxEffectParticleSys *pEffect = DxEffectParticleSys::m_pPool->New();


	//	Note : TransPorm Set.		[공통설정]
	pEffect->m_matLocal = m_matLocal;
	
	//	Note : 시간 설정.			[공통설정]
	//
	pEffect->m_fGBeginTime = m_fGBeginTime;
	pEffect->m_fGLifeTime = m_fGLifeTime;

	//	Note : 날아가는 오브젝트	[공통설정]
	//
	pEffect->m_bMoveObj = m_bMoveObj;

	//	Note : 속도 설정.
	pEffect->m_vGVelocity = m_vGVelocity;

	//	Note : 가속도 설정.
	pEffect->m_vGGravity = m_vGGravityStart;
	pEffect->m_vGGravityDelta = ( m_vGGravityEnd - m_vGGravityStart ) / m_fGLifeTime;

	// Note : 랜덤 Play 설정.
	pEffect->m_vPlayTime.x	= m_vPlayTime.x;
	pEffect->m_vPlayTime.y	= m_vPlayTime.y-m_vPlayTime.x;
	pEffect->m_vPlayTime.z	= pEffect->m_vPlayTime.x + pEffect->m_vPlayTime.y*RANDOM_POS;
	pEffect->m_vSleepTime.x	= m_vSleepTime.x;
	pEffect->m_vSleepTime.y	= m_vSleepTime.y-m_vSleepTime.x;
	pEffect->m_vSleepTime.z	= pEffect->m_vSleepTime.x + pEffect->m_vSleepTime.y*RANDOM_POS;

	D3DXVECTOR3		m_vPlayTime;		// 매번의 PlayTime,		시작값, 랜덤범위
	D3DXVECTOR3		m_vSleepTime;		// 매번의 SleepTime,	시작값, 랜덤범위

	//	Flag
	pEffect->m_dwFlag = m_dwFlag;
	pEffect->m_iCenterPoint = m_iCenterPoint;

	pEffect->m_vRange = m_vRange;
	pEffect->m_fRangeRate = m_fRangeRate;
	pEffect->m_fRotateAngel = m_fRotateAngel;

	pEffect->m_uParticlesPerSec = m_uParticlesPerSec;

	//	Note : 회전 반지름 초기화
	//	
	pEffect->m_fRotateLAngel = m_fRotateLAngel;

	pEffect->m_fLength		= m_fLengthStart;

	pEffect->m_fRotateLTime1 = m_fGBeginTime + m_fGLifeTime*m_fRotateLRate1/100.f;
	pEffect->m_fRotateLTime2 = m_fGBeginTime + m_fGLifeTime*m_fRotateLRate2/100.f;

	if ( m_fRotateLRate1 == 0.f )					pEffect->m_fRotateLDelta1	= 0.f;
	else											pEffect->m_fRotateLDelta1 = ( m_fLengthMid1 - m_fLengthStart ) / ( pEffect->m_fRotateLTime1 - m_fGBeginTime );
	if ( m_fRotateLRate2-m_fRotateLRate1 == 0.f )	pEffect->m_fRotateLDelta2	= 0.f;
	else											pEffect->m_fRotateLDelta2 = ( m_fLengthMid2 - m_fLengthMid1 ) / ( pEffect->m_fRotateLTime2 - pEffect->m_fRotateLTime1 );
	if ( m_fRotateLRate2 == 100.f )					pEffect->m_fRotateLDelta3	= 0.f;
	else											pEffect->m_fRotateLDelta3 = ( m_fLengthEnd - m_fLengthMid2 ) / ( m_fGBeginTime + m_fGLifeTime - pEffect->m_fRotateLTime2 );

	pEffect->m_vWindDir = m_vWindDir;

	//	Note : 파티클 속성.
	//		
	pEffect->m_fTexRotateAngel = m_fTexRotateAngel;

	//	Note : 텍스쳐 시작 스케일과 누적 값
	//
	pEffect->m_vTexScale.x = m_vTexScale.x;
	pEffect->m_vTexScale.y = m_vTexScale.y;
	pEffect->m_vTexScaleDelta.x = (m_vTexScale.z-m_vTexScale.x)/m_fLife;
	pEffect->m_vTexScaleDelta.y = (m_vTexScale.w-m_vTexScale.y)/m_fLife;

	pEffect->m_fGravityStart = m_fGravityStart;
	pEffect->m_fGravityVar = m_fGravityVar;
	pEffect->m_fGravityEnd = m_fGravityEnd;

	pEffect->m_fSizeStart = m_fSizeStart;
	pEffect->m_fSizeVar = m_fSizeVar;
	pEffect->m_fSizeEnd = m_fSizeEnd;

	//	Note : 알파..
	//
	pEffect->m_fAlpha = m_fAlphaStart;

	pEffect->m_fAlphaTime1 = m_fLife*m_fAlphaRate1/100.f;
	pEffect->m_fAlphaTime2 = m_fLife*m_fAlphaRate2/100.f;

	if ( m_fAlphaRate1 == 0.f )					pEffect->m_fAlphaDelta1	= 0.f;
	else										pEffect->m_fAlphaDelta1 = ( m_fAlphaMid1 - m_fAlphaStart ) / (pEffect->m_fAlphaTime1);
	if ( m_fAlphaRate2-m_fAlphaRate1 == 0.f )	pEffect->m_fAlphaDelta2	= 0.f;
	else										pEffect->m_fAlphaDelta2 = ( m_fAlphaMid2 - m_fAlphaMid1 ) / (pEffect->m_fAlphaTime2-pEffect->m_fAlphaTime1);
	if ( m_fAlphaRate2 == 100.f || m_fAlphaEnd == m_fAlphaMid2 )	pEffect->m_fAlphaDelta3	= 0.f;
	else															pEffect->m_fAlphaDelta3 = ( m_fAlphaEnd - m_fAlphaMid2 - 0.1f ) / (m_fLife-pEffect->m_fAlphaTime2);

	pEffect->m_cColorStart = m_cColorStart;
	pEffect->m_cColorVar = m_cColorVar;
	pEffect->m_cColorEnd = m_cColorEnd;

	pEffect->m_fSpeed = m_fSpeed;
	pEffect->m_fSpeedVar = m_fSpeedVar;

	pEffect->m_fLife = m_fLife;
	pEffect->m_fLifeVar = m_fLifeVar;

	pEffect->m_fTheta = m_fTheta;

	pEffect->m_nCol = m_nCol;
	pEffect->m_nRow = m_nRow;
	pEffect->m_nAllSprite = m_nCol * m_nRow;
	pEffect->m_fAniTime = m_fAniTime;

	pEffect->m_nPower = m_nPower;
	pEffect->m_nBlend = m_nBlend;

	pEffect->m_fForceVar = 1.f/m_fLife;

	//	Note : 메쉬 설정
	//
	pEffect->m_pMesh = m_pMesh;

	//	Note : 텍스쳐 설정.
	//
	pEffect->m_strTexture = m_szTexture;
	pEffect->m_pTexture = m_pTexture;
	pEffect->m_pPropGroup = m_pPropGroup;

	//	Note : 사운드 설정
	//
	pEffect->m_MovSound = m_MovSound;

	return pEffect;
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//							P	R	O	P	E	R	T	Y			S	A	V	E	&&	L	O	A	D
// -----------------------------------------------------------------------------------------------------------------------------------------
HRESULT PARTICLESYS_PROPERTY::SaveFile ( CSerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	//
	SFile << (DWORD) ( GetSizeBase() + sizeof(m_Property) );

	//	Note : 부모 클레스의 정보.
	SFile.WriteBuffer( m_matLocal, sizeof(D3DXMATRIX) );
	
	SFile << m_bMoveObj;
	SFile << m_fGBeginTime;
	SFile << m_fGLifeTime;

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(PROPERTY) );

	//	Note : 형제, 자식 저장.
	//
	EFF_PROPERTY::SaveFile ( SFile );

	return S_OK;
}

HRESULT PARTICLESYS_PROPERTY::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	if( dwVer == VERSION )
	{
		//	Note : 부모 클레스의 정보.
		SFile.ReadBuffer ( m_matLocal, sizeof(D3DXMATRIX) );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );

		m_pTexture = NULL;
	}
	else if( dwVer == 0x105 )
	{
		//	Note : 부모 클레스의 정보.
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : 이팩트의 Property 를 저장.
		PROPERTY_105	sProp;
		SFile.ReadBuffer( &sProp, sizeof(PROPERTY_105) );

		m_vGVelocity		= sProp.m_vGVelocity;
		m_vGGravityStart	= sProp.m_vGGravityStart;
		m_vGGravityEnd		= sProp.m_vGGravityEnd;
		m_dwFlag			= sProp.m_dwFlag;
		m_iCenterPoint		= sProp.m_iCenterPoint;
		m_vRange			= sProp.m_vRange;
		m_fRangeRate		= sProp.m_fRangeRate;
		m_fRotateAngel		= sProp.m_fRotateAngel;
		m_fRotateLAngel		= sProp.m_fRotateLAngel;
		m_fRotateLRate1		= sProp.m_fRotateLRate1;
		m_fRotateLRate2		= sProp.m_fRotateLRate2;
		m_fLengthStart		= sProp.m_fLengthStart;
		m_fLengthMid1		= sProp.m_fLengthMid1;
		m_fLengthMid2		= sProp.m_fLengthMid2;
		m_fLengthEnd		= sProp.m_fLengthEnd;
		m_uParticlesPerSec	= sProp.m_uParticlesPerSec;
		m_vWindDir			= sProp.m_vWindDir;
		m_fTexRotateAngel	= sProp.m_fTexRotateAngel;
		m_vTexScale			= sProp.m_vTexScale;
		m_fGravityStart		= sProp.m_fGravityStart;
		m_fGravityVar		= sProp.m_fGravityVar;
		m_fGravityEnd		= sProp.m_fGravityEnd;
		m_fSizeStart		= sProp.m_fSizeStart;
		m_fSizeVar			= sProp.m_fSizeVar;
		m_fSizeEnd			= sProp.m_fSizeEnd;
		m_fAlphaRate1		= sProp.m_fAlphaRate1;
		m_fAlphaRate2		= sProp.m_fAlphaRate2;
		m_fAlphaStart		= sProp.m_fAlphaStart;
		m_fAlphaMid1		= sProp.m_fAlphaMid1;
		m_fAlphaMid2		= sProp.m_fAlphaMid2;
		m_fAlphaEnd			= sProp.m_fAlphaEnd;
		m_cColorStart		= sProp.m_cColorStart;
		m_cColorVar			= sProp.m_cColorVar;
		m_cColorEnd			= sProp.m_cColorEnd;
		m_fSpeed			= sProp.m_fSpeed;
		m_fSpeedVar			= sProp.m_fSpeedVar;
		m_fLife				= sProp.m_fLife;
		m_fLifeVar			= sProp.m_fLifeVar;
		m_fTheta			= sProp.m_fTheta;
		m_nCol				= sProp.m_nCol;
		m_nRow				= sProp.m_nRow;
		m_fAniTime			= sProp.m_fAniTime;
		m_nBlend			= sProp.m_nBlend;
		m_nPower			= sProp.m_nPower;
		m_vPlayTime			= sProp.m_vPlayTime;
		m_vSleepTime		= sProp.m_vSleepTime;

		StringCchCopy( m_szTexture,		STRING_NUM_256, sProp.m_szTexture);
		StringCchCopy( m_szMeshFile,	STRING_NUM_256, sProp.m_szMeshFile);
	}
	else if( dwVer == 0x104 )
	{
		//	Note : 부모 클레스의 정보.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : 이팩트의 Property 를 저장.
		PROPERTY_104	sProp;
		SFile.ReadBuffer( &sProp, sizeof(PROPERTY_104) );

		m_vGVelocity		= sProp.m_vGVelocity;
		m_vGGravityStart	= sProp.m_vGGravityStart;
		m_vGGravityEnd		= sProp.m_vGGravityEnd;
		m_dwFlag			= sProp.m_dwFlag;
		m_iCenterPoint		= sProp.m_iCenterPoint;
		m_vRange			= sProp.m_vRange;
		m_fRangeRate		= sProp.m_fRangeRate;
		m_fRotateAngel		= sProp.m_fRotateAngel;
		m_fRotateLAngel		= sProp.m_fRotateLAngel;
		m_fRotateLRate1		= sProp.m_fRotateLRate1;
		m_fRotateLRate2		= sProp.m_fRotateLRate2;
		m_fLengthStart		= sProp.m_fLengthStart;
		m_fLengthMid1		= sProp.m_fLengthMid1;
		m_fLengthMid2		= sProp.m_fLengthMid2;
		m_fLengthEnd		= sProp.m_fLengthEnd;
		m_uParticlesPerSec	= sProp.m_uParticlesPerSec;
		m_vWindDir			= sProp.m_vWindDir;
		m_fTexRotateAngel	= sProp.m_fTexRotateAngel;
		m_vTexScale			= sProp.m_vTexScale;
		m_fGravityStart		= sProp.m_fGravityStart;
		m_fGravityVar		= sProp.m_fGravityVar;
		m_fGravityEnd		= sProp.m_fGravityEnd;
		m_fSizeStart		= sProp.m_fSizeStart;
		m_fSizeVar			= sProp.m_fSizeVar;
		m_fSizeEnd			= sProp.m_fSizeEnd;
		m_fAlphaRate1		= sProp.m_fAlphaRate1;
		m_fAlphaRate2		= sProp.m_fAlphaRate2;
		m_fAlphaStart		= sProp.m_fAlphaStart;
		m_fAlphaMid1		= sProp.m_fAlphaMid1;
		m_fAlphaMid2		= sProp.m_fAlphaMid2;
		m_fAlphaEnd			= sProp.m_fAlphaEnd;
		m_cColorStart		= sProp.m_cColorStart;
		m_cColorVar			= sProp.m_cColorVar;
		m_cColorEnd			= sProp.m_cColorEnd;
		m_fSpeed			= sProp.m_fSpeed;
		m_fSpeedVar			= sProp.m_fSpeedVar;
		m_fLife				= sProp.m_fLife;
		m_fLifeVar			= sProp.m_fLifeVar;
		m_fTheta			= sProp.m_fTheta;
		m_nCol				= sProp.m_nCol;
		m_nRow				= sProp.m_nRow;
		m_fAniTime			= sProp.m_fAniTime;
		m_nBlend			= sProp.m_nBlend;
		m_nPower			= sProp.m_nPower;

		StringCchCopy( m_szTexture,		STRING_NUM_256, sProp.m_szTexture);
		StringCchCopy( m_szMeshFile,	STRING_NUM_256, sProp.m_szMeshFile);

		// Note : Ver.105
		m_vPlayTime = D3DXVECTOR2( 0.5f,5.f );
		m_vSleepTime = D3DXVECTOR2( 0.5f,5.f );
	}
	else if ( dwVer == 0x103 )
	{
		//	Note : 부모 클레스의 정보.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : 이팩트의 Property 를 저장.
		//
		PROPERTY_102_103	sProp;
		SFile.ReadBuffer ( &sProp, sizeof(PROPERTY_102_103) );

		m_Property.m_vGVelocity			= sProp.m_vGVelocity;
		m_Property.m_vGGravityStart		= sProp.m_vGGravityStart;
		m_Property.m_vGGravityEnd		= sProp.m_vGGravityEnd;
		m_Property.m_dwFlag				= sProp.m_dwFlag;
		m_Property.m_iCenterPoint		= sProp.m_iCenterPoint;
		m_Property.m_vRange				= sProp.m_vRange;
		m_Property.m_fRangeRate			= sProp.m_fRangeRate;
		m_Property.m_fRotateAngel		= sProp.m_fRotateAngel;
		m_Property.m_fRotateLAngel		= sProp.m_fRotateLAngel;
		m_Property.m_fRotateLRate1		= sProp.m_fRotateLRate1;
		m_Property.m_fRotateLRate2		= sProp.m_fRotateLRate2;
		m_Property.m_fLengthStart		= sProp.m_fLengthStart;
		m_Property.m_fLengthMid1		= sProp.m_fLengthMid1;
		m_Property.m_fLengthMid2		= sProp.m_fLengthMid2;
		m_Property.m_fLengthEnd			= sProp.m_fLengthEnd;
		m_Property.m_uParticlesPerSec	= sProp.m_uParticlesPerSec;
		m_Property.m_fTexRotateAngel	= sProp.m_fTexRotateAngel;
		m_Property.m_vTexScale			= sProp.m_vTexScale;
		m_Property.m_fGravityStart		= sProp.m_fGravityStart;
		m_Property.m_fGravityVar		= sProp.m_fGravityVar;
		m_Property.m_fGravityEnd		= sProp.m_fGravityEnd;
		m_Property.m_fSizeStart			= sProp.m_fSizeStart;
		m_Property.m_fSizeVar			= sProp.m_fSizeVar;
		m_Property.m_fSizeEnd			= sProp.m_fSizeEnd;
		m_Property.m_fAlphaRate1		= sProp.m_fAlphaRate1;
		m_Property.m_fAlphaRate2		= sProp.m_fAlphaRate2;
		m_Property.m_fAlphaStart		= sProp.m_fAlphaStart;
		m_Property.m_fAlphaMid1			= sProp.m_fAlphaMid1;
		m_Property.m_fAlphaMid2			= sProp.m_fAlphaMid2;
		m_Property.m_fAlphaEnd			= sProp.m_fAlphaEnd;
		m_Property.m_cColorStart		= sProp.m_cColorStart;
		m_Property.m_cColorVar			= sProp.m_cColorVar;
		m_Property.m_cColorEnd			= sProp.m_cColorEnd;
		m_Property.m_fSpeed				= sProp.m_fSpeed;
		m_Property.m_fSpeedVar			= sProp.m_fSpeedVar;
		m_Property.m_fLife				= sProp.m_fLife;
		m_Property.m_fLifeVar			= sProp.m_fLifeVar;
		m_Property.m_fTheta				= sProp.m_fTheta;
		m_Property.m_nBlend				= sProp.m_nBlend;
		m_Property.m_vWindDir			= sProp.m_vWindDir;
		m_Property.m_nCol				= sProp.m_nCol;
		m_Property.m_nRow				= sProp.m_nRow;
		m_Property.m_fAniTime			= sProp.m_fAniTime;

		StringCchCopy( m_Property.m_szTexture,	STRING_NUM_256, sProp.m_szTexture);
		StringCchCopy( m_Property.m_szMeshFile,	STRING_NUM_256, sProp.m_szMeshFile);

		m_Property.m_fTheta *= 2.f;

		if ( sProp.m_nBlend == 1 )		m_nPower = 0;
		else if ( sProp.m_nBlend == 2 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 3 )	m_nPower = 2;
		else if ( sProp.m_nBlend == 4 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 5 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 6 )	m_nPower = 1;

		if ( sProp.m_nBlend == 1 )		m_nBlend = 0;
		else if ( sProp.m_nBlend == 2 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 3 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 4 )	m_nBlend = 5;
		else if ( sProp.m_nBlend == 5 )	m_nBlend = 6;
		else if ( sProp.m_nBlend == 6 )	m_nBlend = 5;

		if ( sProp.m_nBlend == 5 )		m_dwFlag |= USENORMAL2;

		m_pTexture = NULL;

		// Note : Ver.105
		m_vPlayTime = D3DXVECTOR2( 0.5f,5.f );
		m_vSleepTime = D3DXVECTOR2( 0.5f,5.f );
	}
	//	Note : 버전이 일치할 경우. 
	//
	else if ( dwVer == 0x102 )
	{
		//	Note : 부모 클레스의 정보.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : 이팩트의 Property 를 저장.
		//
		PROPERTY_102_103	sProp;
		SFile.ReadBuffer ( &sProp, sizeof(PROPERTY_102_103) );

		m_Property.m_vGVelocity			= sProp.m_vGVelocity;
		m_Property.m_vGGravityStart		= sProp.m_vGGravityStart;
		m_Property.m_vGGravityEnd		= sProp.m_vGGravityEnd;
		m_Property.m_dwFlag				= sProp.m_dwFlag;
		m_Property.m_iCenterPoint		= sProp.m_iCenterPoint;
		m_Property.m_vRange				= sProp.m_vRange;
		m_Property.m_fRangeRate			= sProp.m_fRangeRate;
		m_Property.m_fRotateAngel		= sProp.m_fRotateAngel;
		m_Property.m_fRotateLAngel		= sProp.m_fRotateLAngel;
		m_Property.m_fRotateLRate1		= sProp.m_fRotateLRate1;
		m_Property.m_fRotateLRate2		= sProp.m_fRotateLRate2;
		m_Property.m_fLengthStart		= sProp.m_fLengthStart;
		m_Property.m_fLengthMid1		= sProp.m_fLengthMid1;
		m_Property.m_fLengthMid2		= sProp.m_fLengthMid2;
		m_Property.m_fLengthEnd			= sProp.m_fLengthEnd;
		m_Property.m_uParticlesPerSec	= sProp.m_uParticlesPerSec;
		m_Property.m_fTexRotateAngel	= sProp.m_fTexRotateAngel;
		m_Property.m_vTexScale			= sProp.m_vTexScale;
		m_Property.m_fGravityStart		= sProp.m_fGravityStart;
		m_Property.m_fGravityVar		= sProp.m_fGravityVar;
		m_Property.m_fGravityEnd		= sProp.m_fGravityEnd;
		m_Property.m_fSizeStart			= sProp.m_fSizeStart;
		m_Property.m_fSizeVar			= sProp.m_fSizeVar;
		m_Property.m_fSizeEnd			= sProp.m_fSizeEnd;
		m_Property.m_fAlphaRate1		= sProp.m_fAlphaRate1;
		m_Property.m_fAlphaRate2		= sProp.m_fAlphaRate2;
		m_Property.m_fAlphaStart		= sProp.m_fAlphaStart;
		m_Property.m_fAlphaMid1			= sProp.m_fAlphaMid1;
		m_Property.m_fAlphaMid2			= sProp.m_fAlphaMid2;
		m_Property.m_fAlphaEnd			= sProp.m_fAlphaEnd;
		m_Property.m_cColorStart		= sProp.m_cColorStart;
		m_Property.m_cColorVar			= sProp.m_cColorVar;
		m_Property.m_cColorEnd			= sProp.m_cColorEnd;
		m_Property.m_fSpeed				= sProp.m_fSpeed;
		m_Property.m_fSpeedVar			= sProp.m_fSpeedVar;
		m_Property.m_fLife				= sProp.m_fLife;
		m_Property.m_fLifeVar			= sProp.m_fLifeVar;
		m_Property.m_fTheta				= sProp.m_fTheta;
		m_Property.m_nBlend				= sProp.m_nBlend;
		m_Property.m_vWindDir			= sProp.m_vWindDir;
		m_Property.m_nCol				= sProp.m_nCol;
		m_Property.m_nRow				= sProp.m_nRow;
		m_Property.m_fAniTime			= sProp.m_fAniTime;

		StringCchCopy( m_Property.m_szTexture,	STRING_NUM_256, sProp.m_szTexture );
		StringCchCopy( m_Property.m_szMeshFile,	STRING_NUM_256, sProp.m_szMeshFile );

		m_Property.m_fTheta *= 2.f;

		if ( sProp.m_nBlend == 1 )		m_nPower = 0;
		else if ( sProp.m_nBlend == 2 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 3 )	m_nPower = 2;
		else if ( sProp.m_nBlend == 4 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 5 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 6 )	m_nPower = 1;

		if ( sProp.m_nBlend == 1 )		m_nBlend = 0;
		else if ( sProp.m_nBlend == 2 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 3 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 4 )	m_nBlend = 5;
		else if ( sProp.m_nBlend == 5 )	m_nBlend = 6;
		else if ( sProp.m_nBlend == 6 )	m_nBlend = 5;

		if ( sProp.m_nBlend == 5 )		m_dwFlag |= USENORMAL2;

		m_pTexture = NULL;

		//	Note : 
		//
		if (m_Property.m_dwFlag & USEMESH)	m_Property.m_dwFlag &= ~USETEXTURE;
		else								m_Property.m_dwFlag |= USETEXTURE;

		// Note : Ver.105
		m_vPlayTime = D3DXVECTOR2( 0.5f,5.f );
		m_vSleepTime = D3DXVECTOR2( 0.5f,5.f );
	}
	//	Note : 버전이 일치할 경우. 
	//
	else if ( dwVer == 0x101 )
	{
		//	Note : 부모 클레스의 정보.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : 이팩트의 Property 를 저장.
		//
		PROPERTY_101 sProp;
		SFile.ReadBuffer ( &sProp, sizeof(PROPERTY_101) );
		
		m_Property.m_vGVelocity			= sProp.m_vGVelocity;
		m_Property.m_vGGravityStart		= sProp.m_vGGravityStart;
		m_Property.m_vGGravityEnd		= sProp.m_vGGravityEnd;
		m_Property.m_dwFlag				= sProp.m_dwFlag;
		m_Property.m_iCenterPoint		= sProp.m_iCenterPoint;
		m_Property.m_vRange				= sProp.m_vRange;
		m_Property.m_fRangeRate			= sProp.m_fRangeRate;
		m_Property.m_fRotateAngel		= sProp.m_fRotateAngel;
		m_Property.m_fRotateLAngel		= sProp.m_fRotateLAngel;
		m_Property.m_fRotateLRate1		= sProp.m_fRotateLRate1;
		m_Property.m_fRotateLRate2		= sProp.m_fRotateLRate2;
		m_Property.m_fLengthStart		= sProp.m_fLengthStart;
		m_Property.m_fLengthMid1		= sProp.m_fLengthMid1;
		m_Property.m_fLengthMid2		= sProp.m_fLengthMid2;
		m_Property.m_fLengthEnd			= sProp.m_fLengthEnd;
		m_Property.m_uParticlesPerSec	= sProp.m_uParticlesPerSec;
		m_Property.m_fTexRotateAngel	= sProp.m_fTexRotateAngel;
		m_Property.m_vTexScale			= sProp.m_vTexScale;
		m_Property.m_fGravityStart		= sProp.m_fGravityStart;
		m_Property.m_fGravityVar		= sProp.m_fGravityVar;
		m_Property.m_fGravityEnd		= sProp.m_fGravityEnd;
		m_Property.m_fSizeStart			= sProp.m_fSizeStart;
		m_Property.m_fSizeVar			= sProp.m_fSizeVar;
		m_Property.m_fSizeEnd			= sProp.m_fSizeEnd;
		m_Property.m_fAlphaRate1		= sProp.m_fAlphaRate1;
		m_Property.m_fAlphaRate2		= sProp.m_fAlphaRate2;
		m_Property.m_fAlphaStart		= sProp.m_fAlphaStart;
		m_Property.m_fAlphaMid1			= sProp.m_fAlphaMid1;
		m_Property.m_fAlphaMid2			= sProp.m_fAlphaMid2;
		m_Property.m_fAlphaEnd			= sProp.m_fAlphaEnd;
		m_Property.m_cColorStart		= sProp.m_cColorStart;
		m_Property.m_cColorVar			= sProp.m_cColorVar;
		m_Property.m_cColorEnd			= sProp.m_cColorEnd;
		m_Property.m_fSpeed				= sProp.m_fSpeed;
		m_Property.m_fSpeedVar			= sProp.m_fSpeedVar;
		m_Property.m_fLife				= sProp.m_fLife;
		m_Property.m_fLifeVar			= sProp.m_fLifeVar;
		m_Property.m_fTheta				= sProp.m_fTheta;
		m_Property.m_nBlend				= sProp.m_nBlend;

		StringCchCopy( m_Property.m_szTexture,	STRING_NUM_256, sProp.m_szTexture );
		StringCchCopy( m_Property.m_szMeshFile,	STRING_NUM_256, sProp.m_szMeshFile );

		m_Property.m_fTheta *= 2.f;

		if ( sProp.m_nBlend == 1 )		m_nPower = 0;
		else if ( sProp.m_nBlend == 2 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 3 )	m_nPower = 2;
		else if ( sProp.m_nBlend == 4 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 5 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 6 )	m_nPower = 1;

		if ( sProp.m_nBlend == 1 )		m_nBlend = 0;
		else if ( sProp.m_nBlend == 2 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 3 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 4 )	m_nBlend = 5;
		else if ( sProp.m_nBlend == 5 )	m_nBlend = 6;
		else if ( sProp.m_nBlend == 6 )	m_nBlend = 5;

		if ( sProp.m_nBlend == 5 )		m_dwFlag |= USENORMAL2;

		m_Property.m_vWindDir			= sProp.m_vWindDir;

		m_Property.m_nCol				= 4;
		m_Property.m_nRow				= 4;
		m_Property.m_fAniTime			= 0.03f;

		m_pTexture = NULL;

		//	Note : 
		//
		if (m_Property.m_dwFlag|USEMESH)	m_Property.m_dwFlag &= ~USETEXTURE;
		else								m_Property.m_dwFlag |= USETEXTURE;

		// Note : Ver.105
		m_vPlayTime = D3DXVECTOR2( 0.5f,5.f );
		m_vSleepTime = D3DXVECTOR2( 0.5f,5.f );
	}
	//	Note : 버전이 일치할 경우. 
	//
	else if ( dwVer == 0x100 )
	{
		//	Note : 부모 클레스의 정보.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : 이팩트의 Property 를 저장.
		//
		PROPERTY_100 sProp;
		SFile.ReadBuffer ( &sProp, sizeof(PROPERTY_100) );
		
		m_Property.m_vGVelocity			= sProp.m_vGVelocity;
		m_Property.m_vGGravityStart		= sProp.m_vGGravityStart;
		m_Property.m_vGGravityEnd		= sProp.m_vGGravityEnd;
		m_Property.m_dwFlag				= sProp.m_dwFlag;
		m_Property.m_iCenterPoint		= sProp.m_iCenterPoint;
		m_Property.m_vRange				= sProp.m_vRange;
		m_Property.m_fRangeRate			= sProp.m_fRangeRate;
		m_Property.m_fRotateAngel		= sProp.m_fRotateAngel;
		m_Property.m_fRotateLAngel		= sProp.m_fRotateLAngel;
		m_Property.m_fRotateLRate1		= sProp.m_fRotateLRate1;
		m_Property.m_fRotateLRate2		= sProp.m_fRotateLRate2;
		m_Property.m_fLengthStart		= sProp.m_fLengthStart;
		m_Property.m_fLengthMid1		= sProp.m_fLengthMid1;
		m_Property.m_fLengthMid2		= sProp.m_fLengthMid2;
		m_Property.m_fLengthEnd			= sProp.m_fLengthEnd;
		m_Property.m_uParticlesPerSec	= sProp.m_uParticlesPerSec;
		m_Property.m_fTexRotateAngel	= sProp.m_fTexRotateAngel;
		m_Property.m_vTexScale			= sProp.m_vTexScale;
		m_Property.m_fGravityStart		= sProp.m_fGravityStart;
		m_Property.m_fGravityVar		= sProp.m_fGravityVar;
		m_Property.m_fGravityEnd		= sProp.m_fGravityEnd;
		m_Property.m_fSizeStart			= sProp.m_fSizeStart;
		m_Property.m_fSizeVar			= sProp.m_fSizeVar;
		m_Property.m_fSizeEnd			= sProp.m_fSizeEnd;
		m_Property.m_fAlphaRate1		= sProp.m_fAlphaRate1;
		m_Property.m_fAlphaRate2		= sProp.m_fAlphaRate2;
		m_Property.m_fAlphaStart		= sProp.m_fAlphaStart;
		m_Property.m_fAlphaMid1			= sProp.m_fAlphaMid1;
		m_Property.m_fAlphaMid2			= sProp.m_fAlphaMid2;
		m_Property.m_fAlphaEnd			= sProp.m_fAlphaEnd;
		m_Property.m_cColorStart		= sProp.m_cColorStart;
		m_Property.m_cColorVar			= sProp.m_cColorVar;
		m_Property.m_cColorEnd			= sProp.m_cColorEnd;
		m_Property.m_fSpeed				= sProp.m_fSpeed;
		m_Property.m_fSpeedVar			= sProp.m_fSpeedVar;
		m_Property.m_fLife				= sProp.m_fLife;
		m_Property.m_fLifeVar			= sProp.m_fLifeVar;
		m_Property.m_fTheta				= sProp.m_fTheta;
		m_Property.m_nBlend				= sProp.m_nBlend;

		StringCchCopy( m_Property.m_szTexture,	STRING_NUM_256, sProp.m_szTexture );
		StringCchCopy( m_Property.m_szMeshFile,	STRING_NUM_256, sProp.m_szMeshFile );

		m_Property.m_fTheta *= 2.f;

		if ( sProp.m_nBlend == 1 )		m_nPower = 0;
		else if ( sProp.m_nBlend == 2 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 3 )	m_nPower = 2;
		else if ( sProp.m_nBlend == 4 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 5 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 6 )	m_nPower = 1;

		if ( sProp.m_nBlend == 1 )		m_nBlend = 0;
		else if ( sProp.m_nBlend == 2 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 3 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 4 )	m_nBlend = 5;
		else if ( sProp.m_nBlend == 5 )	m_nBlend = 6;
		else if ( sProp.m_nBlend == 6 )	m_nBlend = 5;

		if ( sProp.m_nBlend == 5 )		m_dwFlag |= USENORMAL2;

		m_Property.m_vWindDir			= D3DXVECTOR3 (0.f,0.f,0.f);

		m_Property.m_nCol				= 4;
		m_Property.m_nRow				= 4;
		m_Property.m_fAniTime			= 0.03f;

		m_pTexture = NULL;

		//	Note : 
		//
		if (m_Property.m_dwFlag|USEMESH)	m_Property.m_dwFlag &= ~USETEXTURE;
		else								m_Property.m_dwFlag |= USETEXTURE;

		// Note : Ver.105
		m_vPlayTime = D3DXVECTOR2( 0.5f,5.f );
		m_vSleepTime = D3DXVECTOR2( 0.5f,5.f );
	}
	else
	{
		//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
		//
		DWORD dwCur = SFile.GetfTell ();
		SFile.SetOffSet ( dwCur+dwSize );
	}

	//	Note : Device 자원을 생성한다.
	//
	hr = Create ( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	//	Note : 형제, 자식 읽기.
	//
	EFF_PROPERTY::LoadFile ( SFile, pd3dDevice );

	return S_OK;
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//							P	R	O	P	E	R	T	Y			I	N	I	T		&&		D	E	L	E	T	E
// -----------------------------------------------------------------------------------------------------------------------------------------
HRESULT PARTICLESYS_PROPERTY::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwFVF = D3DFVF_XYZ|D3DFVF_TEX1;
	if ( m_dwFlag & USENORMAL2 )	dwFVF |= D3DFVF_NORMAL;

	if ( m_dwFlag & USEMESH )
	{
		if ( m_pMesh )
		{
			if ( m_pMesh->GetLocalMesh() )
			{
				DxSimpleMeshMan::GetInstance().Release ( m_szMeshFile, m_pMesh->GetLocalMesh()->GetFVF() );
			}
			m_pMesh = NULL;
		}

		m_pMesh = DxSimpleMeshMan::GetInstance().Load ( m_szMeshFile, pd3dDevice, dwFVF );
		if ( !m_pMesh )
		{
			char szMsg[256];
			StringCchPrintf( szMsg, 256, "메쉬 파일 '%s' 읽기에 실패", m_szMeshFile );
			MessageBox ( NULL, szMsg, "ERROR", MB_OK );

			return E_FAIL;
		}
		//m_pMesh->SetFVF( D3DMESHVERTEX::FVF );	
	}
	if ( m_dwFlag & USETEXTURE ) 
	{
		TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pTexture, 0, 0, TRUE );
	}
	if( m_dwFlag & USENEWEFF ) 
	{
		m_pPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp( m_szEffFile );
	}


	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT PARTICLESYS_PROPERTY::DeleteDeviceObjects ()
{
	if ( m_pMesh )
	{
		if ( m_pMesh->GetLocalMesh() )
		{
			DxSimpleMeshMan::GetInstance().Release ( m_szMeshFile, m_pMesh->GetLocalMesh()->GetFVF() );
		}
		m_pMesh = NULL;
	}

	TextureManager::ReleaseTexture( m_szTexture, m_pTexture );

	DxEffSinglePropGMan::GetInstance().DeleteEffectGProp( m_szEffFile );

	//	Note : 형제, 자식.
	EFF_PROPERTY::DeleteDeviceObjects ();

	return S_OK;
}