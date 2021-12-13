#pragma once

#include "./DxEffectFrame.h"

//Note: 저장되어서 모델좌표공간에서 월드좌표공간으로 바뀜
struct WATER2_PROPERTY
{
	int				m_nRow, m_nCol;					//	Note: 가로 세로 행과 열의 수
	float			m_fSize;						//	Note: 버텍스간의 간격			
	int				m_nPlayType;					//	Note: 물의 방향을 위한 힘 변수				
	float			m_fWaveCycle;					//	Note: 사인값에 따른 무게 비율 적용
	float			m_fRefractionRate;				//	Note: 반사 비율 적용 변수 0.0 <= 비율 <= 1.0f 게 설정
	float			m_tu, m_tv;						//	Note: 전체의 텍스쳐를 로테이면 하기 위한 누적 tu, tv값

	float			m_fHeightMax, m_fHeightMin;		//	Note: 물의 최대, 최소 높이
	float			m_fHeightSpeed;					//	Note: 물의 높이 속도
	float			m_fTextureMoveVelocity;			//	Note: 텍스쳐의 이동 속도 	
	D3DXVECTOR3     m_vTextureDirection;
	D3DXCOLOR		m_clrDiffuseMin;				//	Note: 디뷰즈 값 
	D3DXCOLOR		m_clrDiffuseMax;
	D3DFILLMODE		m_d3dFillMode;
	BOOL			m_bReflectionMode;
	float			m_fReflectionAlpha;

	D3DXMATRIX		m_matFrameComb;     
	char			m_szTexture[MAX_PATH];
	
	char			m_szEnvTexture1[MAX_PATH];
	char			m_szEnvTexture2[MAX_PATH];
	char			m_szEnvTexture3[MAX_PATH];
	char			m_szEnvTexture4[MAX_PATH];
	char			m_szEnvTexture5[MAX_PATH];
	char			m_szEnvTexture6[MAX_PATH];

	WATER2_PROPERTY()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
		memset( m_szEnvTexture1, 0, sizeof(char)*MAX_PATH );
		memset( m_szEnvTexture2, 0, sizeof(char)*MAX_PATH );
		memset( m_szEnvTexture3, 0, sizeof(char)*MAX_PATH );
		memset( m_szEnvTexture4, 0, sizeof(char)*MAX_PATH );
		memset( m_szEnvTexture5, 0, sizeof(char)*MAX_PATH );
		memset( m_szEnvTexture6, 0, sizeof(char)*MAX_PATH );
	};
};


//Note: 물 파티클 개별 정보 
struct WATERFACTOR 
{

	D3DXVECTOR3		vVelocity;
	D3DXVECTOR3		vPosition;
	D3DXVECTOR3     vPosition0;     //Note: 초기 위치 
	float tu0, tv0;					//Note: 각 정점의 초기 tu, tv값, 불변 
};

//Note: 물 버텍스 구조 
struct WATER2VERTEX
{
	D3DXVECTOR3		vertex;
	D3DXVECTOR3		normal;	
	D3DCOLOR		diffuse;
	float tu, tv;
};

class DxEffectWater2 : public DxEffectBase
{

	//Note : 이펙트 타임 정의
	//
public:
	const static DWORD TYPEID;
	const static DWORD VERSION;
	const static DWORD FLAG;
	const static char  NAME[];

public: 
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return FLAG; }
	virtual const char* GetName ()	{ return NAME; }

	virtual void GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer );
	virtual void SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer );

	//Note: Water2 버텍스 구조	
	struct D3DWATER2VERTEX
	{
		D3DXVECTOR3		vertex;
		D3DXVECTOR3		normal;	
		D3DCOLOR		diffuse;
		float tu, tv;

		D3DWATER2VERTEX ()  {}

		const static DWORD FVF;
	};

	//Note: 속성
	//
//protected:
public:
	union
	{
		struct 
		{
			WATER2_PROPERTY m_Property;
		};

		struct 
		{
			int				m_nRow, m_nCol;				//	Note: 가로 세로 행과 열의 수
			float			m_fSize;					//	Note: 버텍스간의 간격			
			int				m_nPlayType;				//	Note: 물의 방향을 위한 힘 변수				
			float			m_fWaveCycle;				//	Note: 사인값에 따른 무게 비율 적용
			float			m_fRefractionRate;			//	Note: 반사 비율 적용 변수 0.0 <= 비율 <= 1.0f 게 설정
			float			m_tu, m_tv;					//	Nite: 전체의 텍스쳐를 로테이면 하기 위한 누적 tu, tv값
		
			float			m_fHeightMax, m_fHeightMin;	//	Note: 물의 최대, 최소 높이 
			float			m_fHeightSpeed;				//	Note: 물의 높이 속도
			float			m_fTextureMoveVelocity;		//	Note: 텍스쳐의 이동 속도 	
			D3DXVECTOR3     m_vTextureDirection;
			D3DXCOLOR		m_clrDiffuseMin;			//	Note: 디뷰즈 값 
			D3DXCOLOR		m_clrDiffuseMax;
			D3DFILLMODE		m_d3dFillMode;
			BOOL			m_bReflectionMode;		
			float			m_fReflectionAlpha;

			D3DXMATRIX		m_matFrameComb;   
			char			m_szTexture[MAX_PATH];		
			char			m_szEnvTexture1[MAX_PATH];
			char			m_szEnvTexture2[MAX_PATH];
			char			m_szEnvTexture3[MAX_PATH];
			char			m_szEnvTexture4[MAX_PATH];
			char			m_szEnvTexture5[MAX_PATH];
			char			m_szEnvTexture6[MAX_PATH];
		};
	};

protected:
	static DWORD	m_dwEffReflect;
	static char		m_strEffReflect[];

	static LPDIRECT3DSTATEBLOCK9	m_pSavedStateBlock;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawStateBlock;

	static LPDIRECT3DSTATEBLOCK9	m_pReflectSavedSB;
	static LPDIRECT3DSTATEBLOCK9	m_pReflectSB;


protected:
	LPDIRECT3DDEVICEQ		m_pd3dDevice;
	D3DMATERIALQ			m_Material;
	LPDIRECT3DTEXTUREQ		m_pddsTexture;    //Note: 텍스쳐 

	DWORD					m_dwNumIndices;
	DWORD					m_dwNumVertices;	
	LPDIRECT3DVERTEXBUFFERQ m_pVB;
	LPDIRECT3DINDEXBUFFERQ  m_pIB;   

	WATERFACTOR				*m_pWaterFactor;      //Note: 물 정점 정보 (파티클) 
	BOOL					m_bCreate;                   //Note: 초기화 부율 변수 
	float					m_fElapsedTimed;
	float					m_fDepth;

	//------------------ 환경 매핑 멤버 요소 ---------------//

	D3DXMATRIX m_matProject;
	D3DXMATRIX m_matView;
	D3DXMATRIX m_matWorld;

	LPDIRECT3DCUBETEXTUREQ	m_pCubeTexture;		// 반사			// 큐브 텍스쳐

public:
	void SetProperty( WATER2_PROPERTY& Property ) { 	m_Property = Property;	}
	WATER2_PROPERTY& GetProperty() { return m_Property ; }

	void SetWireMode( BOOL bModeFlag ) { bModeFlag? m_d3dFillMode = D3DFILL_WIREFRAME: m_d3dFillMode= D3DFILL_SOLID; }
	void SetColRow( int nCol, int nRow ) {	m_nCol = nCol;	m_nRow = nRow; }
	void SetPlayType( int nPlayType ) { m_nPlayType = nPlayType; } 
	void SetTextureDirection( D3DXVECTOR3 vDirection ) { m_vTextureDirection = vDirection ; }
	void SetTexture( const char *filename );
	void SetSize(float size) ;
			
protected: 
	float CalculateDisplacement( float fHeight );	//Note:  굴절 계산 함수 
	void SetNormal( WATER2VERTEX *pVertices ) ;      //Note: 고라운드 쉐이딩 셋함수 	
	void ApplyWaterRenderOption(void);

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

public:
	DxEffectWater2(void);
	~DxEffectWater2(void);

	//Note: OBJAABB 부부분 
public:
	virtual void GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual BOOL IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );
	
	//물속성 셋하는 부분 
public:

	void SetFadeColor( D3DCOLOR Source, D3DCOLOR Dest );
	void DxEffectWater2::SetReFlectionMode( BOOL bFlag ) {	m_bReflectionMode = bFlag;}

	HRESULT Init( LPDIRECT3DDEVICEQ pd3dDevice, int nRow, int nCol, int nPlayType, D3DXVECTOR3 vDirection );		
};
