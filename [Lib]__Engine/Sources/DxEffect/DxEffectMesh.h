#pragma once
#include "pch.h"

#include "CMemPool.h"
#include "dxeffsingle.h"
#include "dxeffectmesh.h"
#include "dxsimplemeshman.h"


#define	USESCALE			0x00000001	// 스케일
#define	USEROTATE			0x00000004	// 범위 회전 체크
//#define	USEMATRIAL			0x00000008	// 매트리얼 적용 플래그
#define	USEDIRECTION		0x00000100	// 퍼지는 방향이 있죠..
#define	USETEXSCALE			0x00000200	// 텍스쳐 늘이기
#define	USETEXROTATE		0x00000400	// 텍스쳐 회전
#define	USETEXMOVE			0x00000800	// 텍스쳐 회전
#define	USEBLUR				0x00001000	// 잔상 추가
#define	USEBLENDMESH		0x00002000	// 변환 메쉬 적용
#define	USEGOTOCENTER		0x00004000	// 센터 방향으로 이동 시킴
#define	USEOTHERTEX			0x00008000	// 다른 텍스쳐
#define	USESEQUENCE			0x00010000	// 시퀀스 효과 삽입
#define	USESEQUENCELOOP		0x00020000	// 시퀀스 효과 루프 돌기
//#define	USELIGHTING			0x00100000	// 빛을 사용 할 것인가, 말 것인가 ?
#define	USECULLNONE			0x00200000	// Cull Mode를 NONE로 할 것인가, 말 것인가 ?
#define	USESIZEXYZ			0x00400000	// size xyz 3가지 방향으로 움직일 수 있다.
#define	USEHEIGHT_MESH		0x00800000	// 높이 조절이 가능하다.
#define USENORMAL2			0x04000000	// 노멀
#define USEGROUNDTEX		0x08000000	// 지면 TEX 사용

struct MESH_PROPERTY : public EFF_PROPERTY
{
	enum { STRING_NUM = 256, };
	enum { TYPEID = EFFSINGLE_MESH, };
	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID() { return TYPEID; }
	virtual const char *GetName() { return NAME; }

	struct PROPERTY				// Ver. 104 ~ 105
	{
		DWORD		m_dwFlag;

		int			m_nBlurObject;		//  Note : 자신과 같은 잔상 남기기 횟수 Ver.101

		int			m_nCol;			// 행			Ver.102
		int			m_nRow;			// 열			Ver.102
		float		m_fAniTime;		// 설정 시간	Ver.102

		char		m_szMeshFile[STRING_NUM];		//	Note: 메쉬 파일 이름
		char		m_szMeshFile1[STRING_NUM];		//	Note: 메쉬 파일 이름
		char		m_szMeshFile2[STRING_NUM];		//	Note: 메쉬 파일 이름
		char		m_szTexture[STRING_NUM];		//	Note: 텍스쳐 이름		Ver.102
		int			m_nRotationType;		//	Note: 회전 형태 설정 
		int			m_nBlend;				//	Note: 블렌딩 부분
		int			m_nPower;				//	Note: 빛의 세기			Ver.104

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

		PROPERTY()
		{
			memset( m_szMeshFile, 0, sizeof(char)*STRING_NUM );
			memset( m_szMeshFile1, 0, sizeof(char)*STRING_NUM );
			memset( m_szMeshFile2, 0, sizeof(char)*STRING_NUM );
			memset( m_szTexture, 0, sizeof(char)*STRING_NUM );
		};
	};
	
	union
	{
		struct
		{
			PROPERTY	m_Property;
		};

		struct
		{				 
			DWORD		m_dwFlag;

			int			m_nBlurObject;		//  Note : 자신과 같은 잔상 남기기 횟수 Ver.101

			int			m_nCol;			// 행			Ver.102
			int			m_nRow;			// 열			Ver.102
			float		m_fAniTime;		// 설정 시간	Ver.102

			char		m_szMeshFile[STRING_NUM];		//	Note: 메쉬 파일 이름
			char		m_szMeshFile1[STRING_NUM];		//	Note: 메쉬 파일 이름
			char		m_szMeshFile2[STRING_NUM];		//	Note: 메쉬 파일 이름
			char		m_szTexture[STRING_NUM];		//	Note: 텍스쳐 이름		Ver.102
			int			m_nRotationType;		//	Note: 회전 형태 설정 
			int			m_nBlend;				//	Note: 블렌딩 부분
			int			m_nPower;				//	Note: 빛의 세기			Ver.104

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
		};
	};

public:
	DxSimMesh*			m_pMesh;
	LPDIRECT3DTEXTUREQ	m_pTexture;
		
	virtual DxEffSingle* NEWOBJ ();
	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( CSerialFile &SFile );

	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects();

	MESH_PROPERTY () : EFF_PROPERTY (),	
		m_nBlurObject(0),

		m_nCol(4),			// 행
		m_nRow(4),			// 열
		m_fAniTime(0.03f),		// 설정 시간

		m_nRotationType(0),
		m_fRotationAngle(3.0f),

		m_fMorphRoopNum(4.f),

		m_fSizeRate1(25.f),
		m_fSizeRate2(75.f),

		m_fSizeStart(0.f),
		m_fSizeMid1(3.f),
		m_fSizeMid2(2.f),
		m_fSizeEnd(5.f),

		m_vSizeXYZ(2.f,1.f,2.f),

		m_fAlphaRate1(25.0f),
		m_fAlphaRate2(75.0f),

		m_fAlphaStart(0.0f),
		m_fAlphaMid1(1.0f),
		m_fAlphaMid2(1.0f),
		m_fAlphaEnd(0.0f),

		m_vHeight(0.f, 5.f),

		m_fTexRotateAngel(2.f),
		m_nTexRotateType(0),
		m_vTexCenter(D3DXVECTOR3(0.f,0.f,0.f)),

		m_vTexVel(D3DXVECTOR3(0.f,0.f,0.f)),

		m_fTexScaleStart(1.f),
		m_fTexScaleEnd(0.f),

		m_pTexture( NULL ),
		m_pMesh( NULL )
	{	
		m_dwFlag = 0;

		//m_dwFlag |= USELIGHTING;

		m_nPower	= 0;
		m_nBlend	= 0;
		StringCchCopy( m_szMeshFile,	STRING_NUM, "sd_stone.X" );
		StringCchCopy( m_szMeshFile1,	STRING_NUM, "Dolphin2.x" );
		StringCchCopy( m_szMeshFile2,	STRING_NUM, "Dolphin3.x" );
		StringCchCopy( m_szTexture,		STRING_NUM, "1d_Lighting.bmp" );
		
		m_clrStart = D3DXCOLOR( 1.0f,1.0f,1.0f,1.0f);
		m_clrEnd   = D3DXCOLOR( 1.0f,1.0f,1.0f,1.0f);
	};

};

//----------------------------------- 구동 메쉬 시작 ---------------------------------------//
class DxEffectMesh : public DxEffSingle
{
private:
	float					m_fScale0;		//	Note: 두개의 스케일 정보 저장
	float					m_fScale1;		//  Note: 상동 
	float					m_fTime0;		//	Note: 스케일에 따른 시간 비율 
	float					m_fTime1;		//	Note: 스케일에 따른 시간 비율
	float					m_fAngle;		//	Note: 각도 
	D3DXVECTOR3				m_vDistance;	//	Note: 거리

public:
	const static DWORD TYPEID;
	const static DWORD FLAG;
	const static char  NAME[];


	float			m_fElapsedTime;			//ㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋ
	D3DXMATRIX		m_matWorld;

public:	
	
	int			m_nBlend;					//	Note: 블렌딩 부분
	int			m_nPower;					//	Note: 빛의 세기
    DWORD		m_dwFlag;

	int			m_nBlurObject;

	int			m_iAllSprite;				// 전체 스프라이트 갯수
	int			m_nCol;						// 행	Note : Ver.1.02
	int			m_nRow;						// 열
	float		m_fAniTime;					// 설정 시간

	int			m_nRotationType;			//	Note: 회전 형태 결정
	int			m_nDelta;					//	Note: 스케일과 시간 이동 인덱스 
	int			m_nMaterialDelta;			//	Note: 머티리얼 변화 변위

	float		m_fRotationAngle;			//	Note: 회전 각도 

	float		m_fMorphRoopNum;

	float		m_fSizeStart;
	float		m_fAlphaStart;

	float		m_fSize;

	float		m_fSizeTime1;
	float		m_fSizeTime2;

	float		m_fSizeDelta1;
	float		m_fSizeDelta2;
	float		m_fSizeDelta3;

	D3DXVECTOR3	m_vSizeXYZ;
	D3DXVECTOR3	m_vSizeXYZEnd;

	float		m_fAlpha;

	float		m_fAlphaTime1;
	float		m_fAlphaTime2;

	float		m_fAlphaDelta1;
	float		m_fAlphaDelta2;
	float		m_fAlphaDelta3;

	float		m_fHeight;			// Note : 높이에 관한. ^^;
	float		m_fHeightStart;
	float		m_fHeightDelta;

	float		m_fTexRotateAngel;	// Note: 텍스쳐 회전
	int			m_nTexRotateType;	// Note: 텍스쳐 회전 형태
	D3DXVECTOR3	m_vTexCenter;		// Note: 텍스쳐 중심

	D3DXVECTOR3	m_vTexVel;			// Note: 텍스쳐 흐르는 방향
	D3DXVECTOR3	m_vTexVelSum;		// Note: 텍스쳐 흐르는 속도 최종 합

	float		m_fTexScale;		// Note: 
	float		m_fTexScaleDelta;	// Note: 

	float       m_fMaterial[4];				//	Note: 머티리얼 시간 퍼센트 비율 
	float		m_fMaterialTime[4];			//	Note: 머티리얼 시간 정보 

	D3DXCOLOR	m_clrStart;					//	Note: 머티리얼 디퓨즈 컬러의 출발색상	
	D3DXCOLOR	m_clrEnd;					//	Note: 머티리얼 디퓨즈 컬러의 끝색상
	D3DXCOLOR	m_cColorVar;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedRenderStats;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawRenderStats;	

	std::string			m_strTexture;
	LPDIRECT3DTEXTUREQ	m_pTexture;					//	Note : Ver.1.02
	DxSimMesh*			m_pMesh;					//	Note: 렌더링 메쉬 저장 부분 

	std::string			m_szCollTex;
	LPDIRECT3DTEXTUREQ	m_pCollTEX;		// 충돌된 텍스쳐.

public:
	virtual DWORD GetTypeID() { return TYPEID; }
	virtual DWORD GetFlag()	  { return FLAG; }
	virtual const char* GetName() { return NAME; }

protected:
	D3DXVECTOR2			m_pSequenceUV[4];
	DWORD				m_dwVertices;

	int			m_iNowSprite;				// 지금 녀석의 스프라이트 번호
	float		m_fTimeSum;
	BOOL		m_bSequenceRoop;			// 시퀀스 루프

public:
	static HRESULT CreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnInitDevice_STATIC();
	static void OnDeleteDevice_STATIC();

public:
	DxEffectMesh(void);
	~DxEffectMesh(void);

public:
	virtual void ReStartEff ();
	virtual void CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );

public:
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb );
	virtual HRESULT FinalCleanup ();

public:
	typedef CMemPool<DxEffectMesh>	MESHPOOL;
    static MESHPOOL*				m_pPool;
};
