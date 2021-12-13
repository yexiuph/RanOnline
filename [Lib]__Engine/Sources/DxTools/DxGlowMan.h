
class DxGlowMan
{
protected:
	float		m_fMicro_256;
	float		m_fElapsedTime;
	D3DXVECTOR3	m_vDirection;

protected:
	struct	VERTEX
	{
		D3DXVECTOR4			vPos;
		D3DXVECTOR2			vTex;
		static const DWORD	FVF;
	};

	struct	VERTEXCOLOR
	{
		D3DXVECTOR4			vPos;
		D3DCOLOR			cColor;
		D3DXVECTOR2			vTex;
		static const DWORD	FVF;
	};

protected:
	VERTEX			m_sVert_256[4];
	VERTEX			m_sVert_512[4];
	VERTEX			m_sVert_Full[4];
	VERTEXCOLOR		m_sVert_Color_256[4];

protected:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedGlowSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectGlowSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedBurnSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectBurnSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedBlurSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectBlurSB;

protected:
	BOOL			m_bProjectActive;		// 프로젝트에서 동작/미동작
	BOOL			m_bActive;				// 현 프레임에서 동작/미동작
	BOOL			m_bActive_Burn;			// 현 프레임에서 동작/미동작
	BOOL			m_bOptionEnable;		// 옵션에서의 활성화

public:
	void	SetProjectActiveON()			{ m_bProjectActive=TRUE; }
	void	SetGlowON()						{ m_bActive=TRUE; }
	void	SetGlowON_Burn ()				{ m_bActive_Burn=TRUE; }
	void	SetOptionEnable( BOOL bUse )	{ m_bOptionEnable=bUse; }
	BOOL	IsOptionEnable()				{ return m_bOptionEnable; }

	// Glow
public:
	void RenderTex ( const LPDIRECT3DDEVICEQ pd3dDevice );
	void Render ( const LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	BOOL m_bTEX4;

protected:
	void Render2W( const LPDIRECT3DDEVICEQ pd3dDevice );
	void Render2H( const LPDIRECT3DDEVICEQ pd3dDevice );
	void Render4 ( const LPDIRECT3DDEVICEQ pd3dDevice );

	void TexRender2( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2 vOffSet1, const D3DXVECTOR2 vOffSet2, const float fRate );
	void TexRender4();

	// Burn
public:
	void RenderBURN( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	void GlowRender_Burn( LPDIRECT3DDEVICEQ pd3dDevice );
	void TexOneRender( LPDIRECT3DDEVICEQ pd3dDevice, VERTEX* sVertices );
	void TexOneRender( LPDIRECT3DDEVICEQ pd3dDevice, VERTEXCOLOR* sVertices, float fMicro, DWORD cColor, int nPixelW, int nPixelH );

public:
	void FrameMove ( float fTime, float fElapsedTime );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	DxGlowMan();

public:
	virtual ~DxGlowMan();

public:
	static DxGlowMan& GetInstance();
};
