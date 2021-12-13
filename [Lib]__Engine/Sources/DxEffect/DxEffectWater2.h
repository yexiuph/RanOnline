#pragma once

#include "./DxEffectFrame.h"

//Note: ����Ǿ ����ǥ�������� ������ǥ�������� �ٲ�
struct WATER2_PROPERTY
{
	int				m_nRow, m_nCol;					//	Note: ���� ���� ��� ���� ��
	float			m_fSize;						//	Note: ���ؽ����� ����			
	int				m_nPlayType;					//	Note: ���� ������ ���� �� ����				
	float			m_fWaveCycle;					//	Note: ���ΰ��� ���� ���� ���� ����
	float			m_fRefractionRate;				//	Note: �ݻ� ���� ���� ���� 0.0 <= ���� <= 1.0f �� ����
	float			m_tu, m_tv;						//	Note: ��ü�� �ؽ��ĸ� �����̸� �ϱ� ���� ���� tu, tv��

	float			m_fHeightMax, m_fHeightMin;		//	Note: ���� �ִ�, �ּ� ����
	float			m_fHeightSpeed;					//	Note: ���� ���� �ӵ�
	float			m_fTextureMoveVelocity;			//	Note: �ؽ����� �̵� �ӵ� 	
	D3DXVECTOR3     m_vTextureDirection;
	D3DXCOLOR		m_clrDiffuseMin;				//	Note: ����� �� 
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


//Note: �� ��ƼŬ ���� ���� 
struct WATERFACTOR 
{

	D3DXVECTOR3		vVelocity;
	D3DXVECTOR3		vPosition;
	D3DXVECTOR3     vPosition0;     //Note: �ʱ� ��ġ 
	float tu0, tv0;					//Note: �� ������ �ʱ� tu, tv��, �Һ� 
};

//Note: �� ���ؽ� ���� 
struct WATER2VERTEX
{
	D3DXVECTOR3		vertex;
	D3DXVECTOR3		normal;	
	D3DCOLOR		diffuse;
	float tu, tv;
};

class DxEffectWater2 : public DxEffectBase
{

	//Note : ����Ʈ Ÿ�� ����
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

	//Note: Water2 ���ؽ� ����	
	struct D3DWATER2VERTEX
	{
		D3DXVECTOR3		vertex;
		D3DXVECTOR3		normal;	
		D3DCOLOR		diffuse;
		float tu, tv;

		D3DWATER2VERTEX ()  {}

		const static DWORD FVF;
	};

	//Note: �Ӽ�
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
			int				m_nRow, m_nCol;				//	Note: ���� ���� ��� ���� ��
			float			m_fSize;					//	Note: ���ؽ����� ����			
			int				m_nPlayType;				//	Note: ���� ������ ���� �� ����				
			float			m_fWaveCycle;				//	Note: ���ΰ��� ���� ���� ���� ����
			float			m_fRefractionRate;			//	Note: �ݻ� ���� ���� ���� 0.0 <= ���� <= 1.0f �� ����
			float			m_tu, m_tv;					//	Nite: ��ü�� �ؽ��ĸ� �����̸� �ϱ� ���� ���� tu, tv��
		
			float			m_fHeightMax, m_fHeightMin;	//	Note: ���� �ִ�, �ּ� ���� 
			float			m_fHeightSpeed;				//	Note: ���� ���� �ӵ�
			float			m_fTextureMoveVelocity;		//	Note: �ؽ����� �̵� �ӵ� 	
			D3DXVECTOR3     m_vTextureDirection;
			D3DXCOLOR		m_clrDiffuseMin;			//	Note: ����� �� 
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
	LPDIRECT3DTEXTUREQ		m_pddsTexture;    //Note: �ؽ��� 

	DWORD					m_dwNumIndices;
	DWORD					m_dwNumVertices;	
	LPDIRECT3DVERTEXBUFFERQ m_pVB;
	LPDIRECT3DINDEXBUFFERQ  m_pIB;   

	WATERFACTOR				*m_pWaterFactor;      //Note: �� ���� ���� (��ƼŬ) 
	BOOL					m_bCreate;                   //Note: �ʱ�ȭ ���� ���� 
	float					m_fElapsedTimed;
	float					m_fDepth;

	//------------------ ȯ�� ���� ��� ��� ---------------//

	D3DXMATRIX m_matProject;
	D3DXMATRIX m_matView;
	D3DXMATRIX m_matWorld;

	LPDIRECT3DCUBETEXTUREQ	m_pCubeTexture;		// �ݻ�			// ť�� �ؽ���

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
	float CalculateDisplacement( float fHeight );	//Note:  ���� ��� �Լ� 
	void SetNormal( WATER2VERTEX *pVertices ) ;      //Note: ����� ���̵� ���Լ� 	
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

	//Note: OBJAABB �κκ� 
public:
	virtual void GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual BOOL IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );
	
	//���Ӽ� ���ϴ� �κ� 
public:

	void SetFadeColor( D3DCOLOR Source, D3DCOLOR Dest );
	void DxEffectWater2::SetReFlectionMode( BOOL bFlag ) {	m_bReflectionMode = bFlag;}

	HRESULT Init( LPDIRECT3DDEVICEQ pd3dDevice, int nRow, int nCol, int nPlayType, D3DXVECTOR3 vDirection );		
};
