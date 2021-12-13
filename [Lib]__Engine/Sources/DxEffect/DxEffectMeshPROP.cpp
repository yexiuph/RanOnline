#include "pch.h"

#include "./TextureManager.h"

#include "./DxEffectMan.h"
#include "./DxEffSingleMan.h"
#include "./SerialFile.h"

#include "./dxeffectmesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//									P	R	O	P	E	R	T	Y		 V	E	R	S	I	O	N
// -----------------------------------------------------------------------------------------------------------------------------------------
struct PROPERTY_100
{
	DWORD		m_dwFlag;

	char		m_szMeshFile[256];		//	Note: 메쉬 파일 이름
	char		m_szMeshFile1[256];		//	Note: 메쉬 파일 이름
	char		m_szMeshFile2[256];		//	Note: 메쉬 파일 이름
	int			m_nRotationType;		//	Note: 회전 형태 설정 
	int			m_nBlend;				//	Note: 블렌딩 부분

	float		m_fMorphRoopNum;

	float		m_fSizeRate1;
	float		m_fSizeRate2;

	float		m_fSizeStart;
	float		m_fSizeMid1;
	float		m_fSizeMid2;
	float		m_fSizeEnd;	

	float		m_fAlphaRate1;
	float		m_fAlphaRate2;

	float		m_fAlphaStart;
	float		m_fAlphaMid1;
	float		m_fAlphaMid2;
	float		m_fAlphaEnd;
		
	float		m_fRotationAngle;	// Note : 회전 각도 
	float		m_fRotationVelocity;// Note : 회전 속도 

	float		m_fTexRotateAngel;	// Note : 텍스쳐 회전
	int			m_nTexRotateType;	// Note : 텍스쳐 회전 형태
	D3DXVECTOR3	m_vTexCenter;		// Note : 텍스쳐 중심
	
	D3DXVECTOR3	m_vTexVel;			// Note : 텍스쳐 흐르는 방향

	float		m_fTexScaleStart;	// Note : 텍스쳐 스케일 처음값
	float		m_fTexScaleEnd;		// Note : 텍스쳐 스케일 끝값

	float       m_fMaterial[4];		// Note : 머티리얼 색상 정보 4가지
	float       m_fMaterialRatio0;	// Note : 머티리얼 알파 적용 비율 0
	float       m_fMaterialRatio1;	// Note : 머티리얼 알파 적용 비율 1
	
	D3DXCOLOR	m_clrStart;			// Note : 머티리얼 디퓨즈 컬러의 출발색상	
	D3DXCOLOR	m_clrEnd;			// Note : 머티리얼 디퓨즈 컬러의 끝색상

	PROPERTY_100()
	{
		memset( m_szMeshFile, 0, sizeof(char)*256 );
		memset( m_szMeshFile1, 0, sizeof(char)*256 );
		memset( m_szMeshFile2, 0, sizeof(char)*256 );
	};
};

struct PROPERTY_101				// Ver. 101
{
	DWORD		m_dwFlag;

	int			m_nBlurObject;			//  Note : 자신과 같은 잔상 남기기 횟수 Ver.101

	char		m_szMeshFile[256];		//	Note: 메쉬 파일 이름
	char		m_szMeshFile1[256];		//	Note: 메쉬 파일 이름
	char		m_szMeshFile2[256];		//	Note: 메쉬 파일 이름
	int			m_nRotationType;		//	Note: 회전 형태 설정 
	int			m_nBlend;				//	Note: 블렌딩 부분

	float		m_fMorphRoopNum;

	float		m_fSizeRate1;
	float		m_fSizeRate2;

	float		m_fSizeStart;
	float		m_fSizeMid1;
	float		m_fSizeMid2;
	float		m_fSizeEnd;	

	float		m_fAlphaRate1;
	float		m_fAlphaRate2;

	float		m_fAlphaStart;
	float		m_fAlphaMid1;
	float		m_fAlphaMid2;
	float		m_fAlphaEnd;
		
	float		m_fRotationAngle;	// Note : 회전 각도 
	float		m_fRotationVelocity;// Note : 회전 속도 

	float		m_fTexRotateAngel;	// Note : 텍스쳐 회전
	int			m_nTexRotateType;	// Note : 텍스쳐 회전 형태
	D3DXVECTOR3	m_vTexCenter;		// Note : 텍스쳐 중심
	
	D3DXVECTOR3	m_vTexVel;			// Note : 텍스쳐 흐르는 방향

	float		m_fTexScaleStart;	// Note : 텍스쳐 스케일 처음값
	float		m_fTexScaleEnd;		// Note : 텍스쳐 스케일 끝값

	float       m_fMaterial[4];		// Note : 머티리얼 색상 정보 4가지
	float       m_fMaterialRatio0;	// Note : 머티리얼 알파 적용 비율 0
	float       m_fMaterialRatio1;	// Note : 머티리얼 알파 적용 비율 1
	
	D3DXCOLOR	m_clrStart;			// Note : 머티리얼 디퓨즈 컬러의 출발색상	
	D3DXCOLOR	m_clrEnd;			// Note : 머티리얼 디퓨즈 컬러의 끝색상

	PROPERTY_101()
	{
		memset( m_szMeshFile, 0, sizeof(char)*256 );
		memset( m_szMeshFile1, 0, sizeof(char)*256 );
		memset( m_szMeshFile2, 0, sizeof(char)*256 );
	};
};

struct PROPERTY_102				// Ver. 102
{
	DWORD		m_dwFlag;

	int			m_nBlurObject;		//  Note : 자신과 같은 잔상 남기기 횟수 Ver.101

	int			m_nCol;			// 행			Ver.102
	int			m_nRow;			// 열			Ver.102
	float		m_fAniTime;		// 설정 시간	Ver.102

	char		m_szMeshFile[256];		//	Note: 메쉬 파일 이름
	char		m_szMeshFile1[256];		//	Note: 메쉬 파일 이름
	char		m_szMeshFile2[256];		//	Note: 메쉬 파일 이름
	char		m_szTexture[256];		//	Note: 텍스쳐 이름		Ver.102
	int			m_nRotationType;		//	Note: 회전 형태 설정 
	int			m_nBlend;				//	Note: 블렌딩 부분

	float		m_fMorphRoopNum;

	float		m_fSizeRate1;
	float		m_fSizeRate2;

	float		m_fSizeStart;
	float		m_fSizeMid1;
	float		m_fSizeMid2;
	float		m_fSizeEnd;	

	float		m_fAlphaRate1;
	float		m_fAlphaRate2;

	float		m_fAlphaStart;
	float		m_fAlphaMid1;
	float		m_fAlphaMid2;
	float		m_fAlphaEnd;
		
	float		m_fRotationAngle;	// Note : 회전 각도 
	float		m_fRotationVelocity;// Note : 회전 속도 

	float		m_fTexRotateAngel;	// Note : 텍스쳐 회전
	int			m_nTexRotateType;	// Note : 텍스쳐 회전 형태
	D3DXVECTOR3	m_vTexCenter;		// Note : 텍스쳐 중심
	
	D3DXVECTOR3	m_vTexVel;			// Note : 텍스쳐 흐르는 방향

	float		m_fTexScaleStart;	// Note : 텍스쳐 스케일 처음값
	float		m_fTexScaleEnd;		// Note : 텍스쳐 스케일 끝값

	float       m_fMaterial[4];		// Note : 머티리얼 색상 정보 4가지
	float       m_fMaterialRatio0;	// Note : 머티리얼 알파 적용 비율 0
	float       m_fMaterialRatio1;	// Note : 머티리얼 알파 적용 비율 1
	
	D3DXCOLOR	m_clrStart;			// Note : 머티리얼 디퓨즈 컬러의 출발색상	
	D3DXCOLOR	m_clrEnd;			// Note : 머티리얼 디퓨즈 컬러의 끝색상

	PROPERTY_102()
	{
		memset( m_szMeshFile, 0, sizeof(char)*256 );
		memset( m_szMeshFile1, 0, sizeof(char)*256 );
		memset( m_szMeshFile2, 0, sizeof(char)*256 );
		memset( m_szTexture, 0, sizeof(char)*256 );
	};
};

struct PROPERTY_103				// Ver. 103
{
	DWORD		m_dwFlag;

	int			m_nBlurObject;		//  Note : 자신과 같은 잔상 남기기 횟수 Ver.101

	int			m_nCol;			// 행			Ver.102
	int			m_nRow;			// 열			Ver.102
	float		m_fAniTime;		// 설정 시간	Ver.102

	char		m_szMeshFile[256];		//	Note: 메쉬 파일 이름
	char		m_szMeshFile1[256];		//	Note: 메쉬 파일 이름
	char		m_szMeshFile2[256];		//	Note: 메쉬 파일 이름
	char		m_szTexture[256];		//	Note: 텍스쳐 이름		Ver.102
	int			m_nRotationType;		//	Note: 회전 형태 설정 
	int			m_nBlend;				//	Note: 블렌딩 부분

	float		m_fMorphRoopNum;

	float		m_fSizeRate1;
	float		m_fSizeRate2;

	float		m_fSizeStart;
	float		m_fSizeMid1;
	float		m_fSizeMid2;
	float		m_fSizeEnd;	

	D3DXVECTOR3	m_vSizeXYZ;			// Note : 사이즈를 XYZ 방향으로 ~!		Ver.103

	float		m_fAlphaRate1;
	float		m_fAlphaRate2;

	float		m_fAlphaStart;
	float		m_fAlphaMid1;
	float		m_fAlphaMid2;
	float		m_fAlphaEnd;

	D3DXVECTOR2	m_vHeight;			// x 처음 y 나중
		
	float		m_fRotationAngle;	// Note : 회전 각도 

	float		m_fTexRotateAngel;	// Note : 텍스쳐 회전
	int			m_nTexRotateType;	// Note : 텍스쳐 회전 형태
	D3DXVECTOR3	m_vTexCenter;		// Note : 텍스쳐 중심
	
	D3DXVECTOR3	m_vTexVel;			// Note : 텍스쳐 흐르는 방향

	float		m_fTexScaleStart;	// Note : 텍스쳐 스케일 처음값
	float		m_fTexScaleEnd;		// Note : 텍스쳐 스케일 끝값

	float       m_fMaterial[4];		// Note : 머티리얼 색상 정보 4가지
	float       m_fMaterialRatio0;	// Note : 머티리얼 알파 적용 비율 0
	float       m_fMaterialRatio1;	// Note : 머티리얼 알파 적용 비율 1
	
	D3DXCOLOR	m_clrStart;			// Note : 머티리얼 디퓨즈 컬러의 출발색상	
	D3DXCOLOR	m_clrEnd;			// Note : 머티리얼 디퓨즈 컬러의 끝색상

	PROPERTY_103()
	{
		memset( m_szMeshFile, 0, sizeof(char)*256 );
		memset( m_szMeshFile1, 0, sizeof(char)*256 );
		memset( m_szMeshFile2, 0, sizeof(char)*256 );
		memset( m_szTexture, 0, sizeof(char)*256 );
	};
};

// -----------------------------------------------------------------------------------------------------------------------------------------
//									P	R	O	P	E	R	T	Y			N	E	W	O	B	J
// -----------------------------------------------------------------------------------------------------------------------------------------
const DWORD MESH_PROPERTY::VERSION = 0x0105;
const char	MESH_PROPERTY::NAME[]   = "메쉬효과";

//	Note: 기본적인 초기화를 한 데이타를 실제 구동 클래스로 넘겨준다.
//		  최종 셋팅된 모든 데이타를 넘기는 부분이다.
//        이 부분이 호출되기 전까지는 구동클래스가 생성되지 않는다.
DxEffSingle* MESH_PROPERTY::NEWOBJ ()
{
	//DxEffectMesh *pEffect = new DxEffectMesh;
	DxEffectMesh *pEffect = DxEffectMesh::m_pPool->New();

	//	Note : TransPorm Set.		[공통설정]
	pEffect->m_matLocal = m_matLocal;
	
	//	Note : 시간 설정.			[공통설정]
	pEffect->m_fGBeginTime		= m_fGBeginTime;
	pEffect->m_fGLifeTime		= m_fGLifeTime;		

	//	Note : 날아가는 오브젝트	[공통설정]
	pEffect->m_bMoveObj = m_bMoveObj;
	
	// Note: 초기 셋팅 부분 
	pEffect->m_dwFlag = m_dwFlag;

	pEffect->m_nBlurObject = m_nBlurObject;

	pEffect->m_iAllSprite = m_nCol * m_nRow;
	pEffect->m_nCol = m_nCol;
	pEffect->m_nRow = m_nRow;
	pEffect->m_fAniTime = m_fAniTime;

	pEffect->m_nBlend     = m_nBlend;
	pEffect->m_nPower     = m_nPower;

	pEffect->m_fMorphRoopNum	= m_fMorphRoopNum;

	pEffect->m_fSize = 1.f;

	//	Note: 스케일 초기 셋팅 부분
	if( pEffect->m_dwFlag & USESCALE )
	{	
		//	Note : 사이즈 초기화
		//
		pEffect->m_fSize	 = m_fSizeStart*0.2f;
		pEffect->m_fSizeStart = m_fSizeStart*0.2f;

		pEffect->m_fSizeTime1 = m_fGBeginTime + m_fGLifeTime*m_fSizeRate1/100.f;
		pEffect->m_fSizeTime2 = m_fGBeginTime + m_fGLifeTime*m_fSizeRate2/100.f;

		if ( m_fSizeRate1 == 0.f )				pEffect->m_fSizeDelta1	= 0.f;
		else									pEffect->m_fSizeDelta1 = ( m_fSizeMid1 - m_fSizeStart ) / (pEffect->m_fSizeTime1-m_fGBeginTime);
		if ( m_fSizeRate2-m_fSizeRate1 == 0.f )	pEffect->m_fSizeDelta2	= 0.f;
		else									pEffect->m_fSizeDelta2 = ( m_fSizeMid2 - m_fSizeMid1 ) / (pEffect->m_fSizeTime2-pEffect->m_fSizeTime1);
		if ( m_fSizeRate2 == 100.f )			pEffect->m_fSizeDelta3	= 0.f;
		else									pEffect->m_fSizeDelta3 = ( m_fSizeEnd - m_fSizeMid2 ) / (m_fGBeginTime+m_fGLifeTime-pEffect->m_fSizeTime2);
	}

	//	Note : 사이즈 조정 XYZ 각각 제어 가능
	if( pEffect->m_dwFlag & USESIZEXYZ )
	{
		pEffect->m_vSizeXYZ.x	= m_vSizeXYZ.x-1.f;
		pEffect->m_vSizeXYZ.y	= m_vSizeXYZ.y-1.f;
		pEffect->m_vSizeXYZ.z	= m_vSizeXYZ.z-1.f;
	}

	//	Note: 회전 초기 셋팅 타입 부분 
	if( pEffect->m_dwFlag & USEROTATE )
	{		
		pEffect->m_nRotationType     = m_nRotationType;
		pEffect->m_fRotationAngle     = m_fRotationAngle;
	}

	//	Note: 머티리얼 초기 셋팅 부분 
	//if( pEffect->m_dwFlag & USEMATRIAL )
	//{	
		pEffect->m_clrStart = m_clrStart;
		pEffect->m_clrEnd   = m_clrEnd;
		
		pEffect->m_fAlpha = m_fAlphaStart;
		pEffect->m_fAlphaStart = m_fAlphaStart;

		pEffect->m_fAlphaTime1 = m_fGBeginTime + m_fGLifeTime*m_fAlphaRate1/100.f;
		pEffect->m_fAlphaTime2 = m_fGBeginTime + m_fGLifeTime*m_fAlphaRate2/100.f;

		if ( m_fAlphaRate1 == 0.f )					pEffect->m_fAlphaDelta1	= 0.f;
		else										pEffect->m_fAlphaDelta1 = ( m_fAlphaMid1 - m_fAlphaStart ) / (pEffect->m_fAlphaTime1-m_fGBeginTime);
		if ( m_fAlphaRate2-m_fAlphaRate1 == 0.f )	pEffect->m_fAlphaDelta2	= 0.f;
		else										pEffect->m_fAlphaDelta2 = ( m_fAlphaMid2 - m_fAlphaMid1 ) / (pEffect->m_fAlphaTime2-pEffect->m_fAlphaTime1);
		if ( m_fAlphaRate2 == 100.f || m_fAlphaEnd == m_fAlphaMid2 )	pEffect->m_fAlphaDelta3	= 0.f;
		else															pEffect->m_fAlphaDelta3 = ( m_fAlphaEnd - m_fAlphaMid2 - 0.1f ) / (m_fGBeginTime+m_fGLifeTime-pEffect->m_fAlphaTime2);
	//}

	if ( pEffect->m_dwFlag & USEHEIGHT_MESH )
	{
		pEffect->m_fHeight		= m_vHeight.x;
		pEffect->m_fHeightStart	= m_vHeight.x;
		pEffect->m_fHeightDelta	= (m_vHeight.y-m_vHeight.x)/m_fGLifeTime;
	}

	if ( pEffect->m_dwFlag & USETEXROTATE )
	{
		pEffect->m_fTexRotateAngel = m_fTexRotateAngel;	
		pEffect->m_nTexRotateType = m_nTexRotateType;	
		pEffect->m_vTexCenter = m_vTexCenter;
	}

	if ( pEffect->m_dwFlag & USETEXMOVE )
	{
		pEffect->m_vTexVel = m_vTexVel;
	}

	if ( pEffect->m_dwFlag & USETEXSCALE )
	{
		pEffect->m_fTexScale		= m_fTexScaleStart;		 
		pEffect->m_fTexScaleDelta	= (m_fTexScaleEnd-m_fTexScaleStart)/m_fGLifeTime;	
	}

	pEffect->m_strTexture = m_szTexture;
	pEffect->m_pTexture = m_pTexture;
	pEffect->m_pMesh = m_pMesh;
	
	//	Note : 사운드 설정
	//
	pEffect->m_MovSound = m_MovSound;

	return pEffect;
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//							P	R	O	P	E	R	T	Y			S	A	V	E	&&	L	O	A	D
// -----------------------------------------------------------------------------------------------------------------------------------------
HRESULT MESH_PROPERTY::SaveFile ( CSerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
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

HRESULT MESH_PROPERTY::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
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
	}
	else if( dwVer == 0x0104 )
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
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else if( dwVer == 0x0103 )
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
		PROPERTY_103 sProp;
		SFile.ReadBuffer ( &sProp, sizeof(PROPERTY_103) );

		m_dwFlag			= sProp.m_dwFlag;
		m_nRotationType		= sProp.m_nRotationType;		//	Note: 회전 형태 설정 
		m_fMorphRoopNum		= sProp.m_fMorphRoopNum;
		m_fSizeRate1		= sProp.m_fSizeRate1;
		m_fSizeRate2		= sProp.m_fSizeRate2;
		m_fSizeStart		= sProp.m_fSizeStart;
		m_fSizeMid1			= sProp.m_fSizeMid1;
		m_fSizeMid2			= sProp.m_fSizeMid2;
		m_fSizeEnd			= sProp.m_fSizeEnd;	
		m_fAlphaRate1		= sProp.m_fAlphaRate1;
		m_fAlphaRate2		= sProp.m_fAlphaRate2;
		m_fAlphaStart		= sProp.m_fAlphaStart;
		m_fAlphaMid1		= sProp.m_fAlphaMid1;
		m_fAlphaMid2		= sProp.m_fAlphaMid2;
		m_fAlphaEnd			= sProp.m_fAlphaEnd;
		m_fRotationAngle	= sProp.m_fRotationAngle;	// Note : 회전 각도 
		m_fTexRotateAngel	= sProp.m_fTexRotateAngel;	// Note : 텍스쳐 회전
		m_nTexRotateType	= sProp.m_nTexRotateType;	// Note : 텍스쳐 회전 형태
		m_vTexCenter		= sProp.m_vTexCenter;		// Note : 텍스쳐 중심
		m_vTexVel			= sProp.m_vTexVel;			// Note : 텍스쳐 흐르는 방향
		m_fTexScaleStart	= sProp.m_fTexScaleStart;	// Note : 텍스쳐 스케일 처음값
		m_fTexScaleEnd		= sProp.m_fTexScaleEnd;		// Note : 텍스쳐 스케일 끝값
		m_fMaterial[0]      = sProp.m_fMaterial[0];		// Note : 머티리얼 색상 정보 4가지
		m_fMaterial[1]		= sProp.m_fMaterial[1];
		m_fMaterial[2]      = sProp.m_fMaterial[2];
		m_fMaterial[3]      = sProp.m_fMaterial[3];
		m_fMaterialRatio0   = sProp.m_fMaterialRatio0;	// Note : 머티리얼 알파 적용 비율 0
		m_fMaterialRatio1   = sProp.m_fMaterialRatio1;	// Note : 머티리얼 알파 적용 비율 1
		m_clrStart			= sProp.m_clrStart;			// Note : 머티리얼 디퓨즈 컬러의 출발색상	
		m_clrEnd			= sProp.m_clrEnd;
		m_nBlurObject		= sProp.m_nBlurObject;

		m_vSizeXYZ			= sProp.m_vSizeXYZ;
		m_vHeight			= sProp.m_vHeight;

		StringCchCopy( m_Property.m_szMeshFile,	STRING_NUM, sProp.m_szMeshFile);
		StringCchCopy( m_Property.m_szMeshFile1,STRING_NUM, sProp.m_szMeshFile1);
		StringCchCopy( m_Property.m_szMeshFile2,STRING_NUM, sProp.m_szMeshFile2);
		StringCchCopy( m_Property.m_szTexture,	STRING_NUM, sProp.m_szTexture);

		if ( sProp.m_nBlend == 1 )		m_nPower = 0;
		else if ( sProp.m_nBlend == 2 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 3 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 4 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 5 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 6 )	m_nPower = 0;

		if ( sProp.m_nBlend == 1 )		m_nBlend = 0;
		else if ( sProp.m_nBlend == 2 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 3 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 4 )	m_nBlend = 5;
		else if ( sProp.m_nBlend == 5 )	m_nBlend = 6;
		else if ( sProp.m_nBlend == 6 )	m_nBlend = 5;

		if ( sProp.m_nBlend == 5 )		m_dwFlag |= USENORMAL2;
	}
	else if( dwVer == 0x0102 )
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
		PROPERTY_102 sProp;
		SFile.ReadBuffer ( &sProp, sizeof(PROPERTY_102) );

		m_dwFlag			= sProp.m_dwFlag;
		m_nRotationType		= sProp.m_nRotationType;		//	Note: 회전 형태 설정 
		m_nBlend			= sProp.m_nBlend;				//	Note: 블렌딩 부분
		m_fMorphRoopNum		= sProp.m_fMorphRoopNum;
		m_fSizeRate1		= sProp.m_fSizeRate1;
		m_fSizeRate2		= sProp.m_fSizeRate2;
		m_fSizeStart		= sProp.m_fSizeStart;
		m_fSizeMid1			= sProp.m_fSizeMid1;
		m_fSizeMid2			= sProp.m_fSizeMid2;
		m_fSizeEnd			= sProp.m_fSizeEnd;	
		m_vSizeXYZ			= D3DXVECTOR3 ( 2.f, 1.f, 2.f );
		m_fAlphaRate1		= sProp.m_fAlphaRate1;
		m_fAlphaRate2		= sProp.m_fAlphaRate2;
		m_fAlphaStart		= sProp.m_fAlphaStart;
		m_fAlphaMid1		= sProp.m_fAlphaMid1;
		m_fAlphaMid2		= sProp.m_fAlphaMid2;
		m_fAlphaEnd			= sProp.m_fAlphaEnd;
		m_fRotationAngle	= sProp.m_fRotationAngle;	// Note : 회전 각도 
		m_fTexRotateAngel	= sProp.m_fTexRotateAngel;	// Note : 텍스쳐 회전
		m_nTexRotateType	= sProp.m_nTexRotateType;	// Note : 텍스쳐 회전 형태
		m_vTexCenter		= sProp.m_vTexCenter;		// Note : 텍스쳐 중심
		m_vTexVel			= sProp.m_vTexVel;			// Note : 텍스쳐 흐르는 방향
		m_fTexScaleStart	= sProp.m_fTexScaleStart;	// Note : 텍스쳐 스케일 처음값
		m_fTexScaleEnd		= sProp.m_fTexScaleEnd;		// Note : 텍스쳐 스케일 끝값
		m_fMaterial[0]      = sProp.m_fMaterial[0];		// Note : 머티리얼 색상 정보 4가지
		m_fMaterial[1]		= sProp.m_fMaterial[1];
		m_fMaterial[2]      = sProp.m_fMaterial[2];
		m_fMaterial[3]      = sProp.m_fMaterial[3];
		m_fMaterialRatio0   = sProp.m_fMaterialRatio0;	// Note : 머티리얼 알파 적용 비율 0
		m_fMaterialRatio1   = sProp.m_fMaterialRatio1;	// Note : 머티리얼 알파 적용 비율 1
		m_clrStart			= sProp.m_clrStart;			// Note : 머티리얼 디퓨즈 컬러의 출발색상	
		m_clrEnd			= sProp.m_clrEnd;

		StringCchCopy( m_Property.m_szMeshFile,	STRING_NUM, sProp.m_szMeshFile);
		StringCchCopy( m_Property.m_szMeshFile1,STRING_NUM, sProp.m_szMeshFile1);
		StringCchCopy( m_Property.m_szMeshFile2,STRING_NUM, sProp.m_szMeshFile2);

		m_nBlurObject		= 0;

		//	Note : 추가
		//
		//m_dwFlag			|= USELIGHTING;
		StringCchCopy( m_Property.m_szTexture, STRING_NUM, "1d_Lighting.bmp" );

		if ( sProp.m_nBlend == 1 )		m_nPower = 0;
		else if ( sProp.m_nBlend == 2 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 3 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 4 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 5 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 6 )	m_nPower = 0;

		if ( sProp.m_nBlend == 1 )		m_nBlend = 0;
		else if ( sProp.m_nBlend == 2 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 3 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 4 )	m_nBlend = 5;
		else if ( sProp.m_nBlend == 5 )	m_nBlend = 6;
		else if ( sProp.m_nBlend == 6 )	m_nBlend = 5;

		if ( sProp.m_nBlend == 5 )		m_dwFlag |= USENORMAL2;
	}
	else if( dwVer == 0x0101 )
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

		m_dwFlag			= sProp.m_dwFlag;
		m_nRotationType		= sProp.m_nRotationType;		//	Note: 회전 형태 설정 
		m_nBlend			= sProp.m_nBlend;				//	Note: 블렌딩 부분
		m_fMorphRoopNum		= sProp.m_fMorphRoopNum;
		m_fSizeRate1		= sProp.m_fSizeRate1;
		m_fSizeRate2		= sProp.m_fSizeRate2;
		m_fSizeStart		= sProp.m_fSizeStart;
		m_fSizeMid1			= sProp.m_fSizeMid1;
		m_fSizeMid2			= sProp.m_fSizeMid2;
		m_fSizeEnd			= sProp.m_fSizeEnd;	
		m_vSizeXYZ			= D3DXVECTOR3 ( 2.f, 1.f, 2.f );
		m_fAlphaRate1		= sProp.m_fAlphaRate1;
		m_fAlphaRate2		= sProp.m_fAlphaRate2;
		m_fAlphaStart		= sProp.m_fAlphaStart;
		m_fAlphaMid1		= sProp.m_fAlphaMid1;
		m_fAlphaMid2		= sProp.m_fAlphaMid2;
		m_fAlphaEnd			= sProp.m_fAlphaEnd;
		m_fRotationAngle	= sProp.m_fRotationAngle;	// Note : 회전 각도 
		m_fTexRotateAngel	= sProp.m_fTexRotateAngel;	// Note : 텍스쳐 회전
		m_nTexRotateType	= sProp.m_nTexRotateType;	// Note : 텍스쳐 회전 형태
		m_vTexCenter		= sProp.m_vTexCenter;		// Note : 텍스쳐 중심
		m_vTexVel			= sProp.m_vTexVel;			// Note : 텍스쳐 흐르는 방향
		m_fTexScaleStart	= sProp.m_fTexScaleStart;	// Note : 텍스쳐 스케일 처음값
		m_fTexScaleEnd		= sProp.m_fTexScaleEnd;		// Note : 텍스쳐 스케일 끝값
		m_fMaterial[0]      = sProp.m_fMaterial[0];		// Note : 머티리얼 색상 정보 4가지
		m_fMaterial[1]		= sProp.m_fMaterial[1];
		m_fMaterial[2]      = sProp.m_fMaterial[2];
		m_fMaterial[3]      = sProp.m_fMaterial[3];
		m_fMaterialRatio0   = sProp.m_fMaterialRatio0;	// Note : 머티리얼 알파 적용 비율 0
		m_fMaterialRatio1   = sProp.m_fMaterialRatio1;	// Note : 머티리얼 알파 적용 비율 1
		m_clrStart			= sProp.m_clrStart;			// Note : 머티리얼 디퓨즈 컬러의 출발색상	
		m_clrEnd			= sProp.m_clrEnd;

		StringCchCopy( m_Property.m_szMeshFile,	STRING_NUM, sProp.m_szMeshFile);
		StringCchCopy( m_Property.m_szMeshFile1,STRING_NUM, sProp.m_szMeshFile1);
		StringCchCopy( m_Property.m_szMeshFile2,STRING_NUM, sProp.m_szMeshFile2);

		m_nBlurObject		= 0;

		//	Note : 추가
		//
		//m_dwFlag			|= USELIGHTING;
		StringCchCopy( m_Property.m_szTexture, STRING_NUM, "1d_Lighting.bmp" );

		if ( sProp.m_nBlend == 1 )		m_nPower = 0;
		else if ( sProp.m_nBlend == 2 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 3 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 4 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 5 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 6 )	m_nPower = 0;

		if ( sProp.m_nBlend == 1 )		m_nBlend = 0;
		else if ( sProp.m_nBlend == 2 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 3 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 4 )	m_nBlend = 5;
		else if ( sProp.m_nBlend == 5 )	m_nBlend = 6;
		else if ( sProp.m_nBlend == 6 )	m_nBlend = 5;

		if ( sProp.m_nBlend == 5 )		m_dwFlag |= USENORMAL2;
	}
	else if( dwVer == 0x0100 )
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

		m_dwFlag			= sProp.m_dwFlag;
		m_nRotationType		= sProp.m_nRotationType;		//	Note: 회전 형태 설정 
		m_nBlend			= sProp.m_nBlend;				//	Note: 블렌딩 부분
		m_fMorphRoopNum		= sProp.m_fMorphRoopNum;
		m_fSizeRate1		= sProp.m_fSizeRate1;
		m_fSizeRate2		= sProp.m_fSizeRate2;
		m_fSizeStart		= sProp.m_fSizeStart;
		m_fSizeMid1			= sProp.m_fSizeMid1;
		m_fSizeMid2			= sProp.m_fSizeMid2;
		m_fSizeEnd			= sProp.m_fSizeEnd;	
		m_vSizeXYZ			= D3DXVECTOR3 ( 2.f, 1.f, 2.f );
		m_fAlphaRate1		= sProp.m_fAlphaRate1;
		m_fAlphaRate2		= sProp.m_fAlphaRate2;
		m_fAlphaStart		= sProp.m_fAlphaStart;
		m_fAlphaMid1		= sProp.m_fAlphaMid1;
		m_fAlphaMid2		= sProp.m_fAlphaMid2;
		m_fAlphaEnd			= sProp.m_fAlphaEnd;
		m_fRotationAngle	= sProp.m_fRotationAngle;	// Note : 회전 각도 
		m_fTexRotateAngel	= sProp.m_fTexRotateAngel;	// Note : 텍스쳐 회전
		m_nTexRotateType	= sProp.m_nTexRotateType;	// Note : 텍스쳐 회전 형태
		m_vTexCenter		= sProp.m_vTexCenter;		// Note : 텍스쳐 중심
		m_vTexVel			= sProp.m_vTexVel;			// Note : 텍스쳐 흐르는 방향
		m_fTexScaleStart	= sProp.m_fTexScaleStart;	// Note : 텍스쳐 스케일 처음값
		m_fTexScaleEnd		= sProp.m_fTexScaleEnd;		// Note : 텍스쳐 스케일 끝값
		m_fMaterial[0]      = sProp.m_fMaterial[0];		// Note : 머티리얼 색상 정보 4가지
		m_fMaterial[1]		= sProp.m_fMaterial[1];
		m_fMaterial[2]      = sProp.m_fMaterial[2];
		m_fMaterial[3]      = sProp.m_fMaterial[3];
		m_fMaterialRatio0   = sProp.m_fMaterialRatio0;	// Note : 머티리얼 알파 적용 비율 0
		m_fMaterialRatio1   = sProp.m_fMaterialRatio1;	// Note : 머티리얼 알파 적용 비율 1
		m_clrStart			= sProp.m_clrStart;			// Note : 머티리얼 디퓨즈 컬러의 출발색상	
		m_clrEnd			= sProp.m_clrEnd;

		StringCchCopy( m_Property.m_szMeshFile,	STRING_NUM, sProp.m_szMeshFile );
		StringCchCopy( m_Property.m_szMeshFile1,STRING_NUM, sProp.m_szMeshFile1 );
		StringCchCopy( m_Property.m_szMeshFile2,STRING_NUM, sProp.m_szMeshFile2 );

		m_nBlurObject		= 0;

		//	Note : 추가
		//
		//m_dwFlag			|= USELIGHTING;
		StringCchCopy( m_Property.m_szTexture, STRING_NUM, "1d_Lighting.bmp" );

		if ( sProp.m_nBlend == 1 )		m_nPower = 0;
		else if ( sProp.m_nBlend == 2 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 3 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 4 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 5 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 6 )	m_nPower = 0;

		if ( sProp.m_nBlend == 1 )		m_nBlend = 0;
		else if ( sProp.m_nBlend == 2 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 3 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 4 )	m_nBlend = 5;
		else if ( sProp.m_nBlend == 5 )	m_nBlend = 6;
		else if ( sProp.m_nBlend == 6 )	m_nBlend = 5;

		if ( sProp.m_nBlend == 5 )		m_dwFlag |= USENORMAL2;
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
HRESULT MESH_PROPERTY::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( m_dwFlag & USEOTHERTEX )
	{
		TextureManager::LoadTexture( m_szTexture, pd3dDevice, m_pTexture, 0, 0, TRUE );
	}

	DWORD dwFVF = D3DFVF_XYZ|D3DFVF_TEX1;
	if ( m_dwFlag & USENORMAL2 )	dwFVF |= D3DFVF_NORMAL;

	if ( m_dwFlag & USEBLENDMESH )
	{
		TCHAR* szTex[3] =
		{
			m_szMeshFile, m_szMeshFile1, m_szMeshFile2
		};
		m_pMesh = DxSimpleMeshMan::GetInstance().BlendLoad ( szTex, pd3dDevice, dwFVF );	
	}
	else
		m_pMesh = DxSimpleMeshMan::GetInstance().Load ( m_szMeshFile, pd3dDevice, dwFVF );


	if ( !m_pMesh )		return S_OK;

	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice );	

	return S_OK;
}


HRESULT MESH_PROPERTY::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szTexture, m_pTexture );

	if ( m_pMesh )
	{
		if ( m_pMesh->GetLocalMesh() )
		{
			if ( m_dwFlag & USEBLENDMESH )
				DxSimpleMeshMan::GetInstance().Release( m_szMeshFile, m_pMesh->GetLocalMesh()->GetFVF(), TRUE );
			else
				DxSimpleMeshMan::GetInstance().Release( m_szMeshFile, m_pMesh->GetLocalMesh()->GetFVF(), FALSE );
		}
	}
	m_pMesh = NULL;
	
	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::DeleteDeviceObjects ();	

	return S_OK;
}
