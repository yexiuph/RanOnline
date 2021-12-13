
#include "DxViewPort.h"
#include "DxLightMan.h"
#include "./TextureManager.h"

class DxSkinChar;

enum EMSHADOWDETAIL
{
	SHADOW_2TEX = 4,
	SHADOW_1TEX = 3,
	SHADOW_CHAR = 2,
	SHADOW_NONE = 1,
	SHADOW_DISABLE = 0,

	SHADOW_NSIZE = 5
};

class DxShadowMap
{
protected:
	struct	VERTEX
	{
		D3DXVECTOR3			p;
		D3DXVECTOR2			t;
		static const DWORD	FVF;
	};

protected:	
	static LPDIRECT3DSTATEBLOCK9	m_pSavedStateBlock;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectStateBlock;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedCircleSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectCircleSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedOneShadowSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawOneShadowSB;

protected:
	static		LPDIRECT3DVERTEXBUFFERQ		m_pShadowVB;
	const		static	float				m_fRange;
	const		static	float				EYE_TO_LOOKAT;			// 사각형을 뿌릴때 카메라의 거리 (Point Light)

protected:
	DWORD		m_dwUseSwShader;	// 그래픽 카드가 Shader 를 지원 하는가?
	BOOL		m_bUseMIXED;		// MIXED 인가 ? 아니면 SW 인가.. 확인하라?

	std::string			m_szCircleTex;
	LPDIRECT3DTEXTUREQ	m_pCircleTexture;		// 원 텍스쳐

	EMSHADOWDETAIL		m_emShadowDetail;
	UINT		m_nCurShadow;

protected:
	float				m_fDistance;
	float				m_fDistanceRate;

	float				m_fTexWidth;
	float				m_fTexHeight;

	D3DXVECTOR3			m_vBaseViewMin;
	D3DXVECTOR3			m_vBaseViewMax;

	D3DXVECTOR3			m_vViewMin;
	D3DXVECTOR3			m_vViewMax;

	D3DXMATRIX			m_matView;
	D3DXMATRIX			m_matProj;
	D3DXMATRIX			m_matLastWVP_D;		// 직사광선 카메라의 매트릭스

protected:
	float	m_fPointDis1;
	int		m_nPointNum1;

	float	m_fPointDis2;
	int		m_nPointNum2;

	BOOL	m_bCharUse;
	BOOL	m_bImageBlurUse;

public:
	D3DXVECTOR3		GetViewMax ()			{ return m_vViewMax; }
	D3DXVECTOR3		GetViewMin ()			{ return m_vViewMin; }

public:
	LPDIRECT3DTEXTUREQ	GetCircleTexture ()							{ return m_pCircleTexture; }

	D3DXMATRIX			GetMatrix_D ()								{ return m_matLastWVP_D; }

	EMSHADOWDETAIL		GetShadowDetail ()							{ return m_emShadowDetail; }
	void				SetShadowDetail ( EMSHADOWDETAIL Detail )	{ m_emShadowDetail = Detail; }

public:
	HRESULT ClearShadow ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderShadow ( DxSkinChar *pSkinObj, D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderShadowMob ( DxSkinChar *pSkinObj, D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice );

	HRESULT RenderShadowCharMob ( DxSkinChar *pSkinObj, D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice );
	void	LastImageBlur ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DQ pD3D, LPDIRECT3DDEVICEQ pd3dDevice, D3DSURFACE_DESC &d3dsdBackBuffer );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects();

	HRESULT FrameMove ( float fTime, float fElapsedTime );

protected:
	void	SetPointLight ( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar *pSkinObj, D3DXMATRIX &matRot, 
							D3DXVECTOR3& vCharPos, D3DXVECTOR3& vLightPos, D3DXVECTOR3& vLightDirect );
	void	SetViewProjection ();
	HRESULT	Shadow_Day_Night ( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar *pSkinObj, D3DXMATRIX &matRot );
	HRESULT	Circle_Day_Night ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matRot, DWORD dwTextureFactor );
	void	ReSetClosedPL ( D3DLIGHTQ &sLight, D3DXVECTOR3 vCharPos, int nNum );

protected:
	DxShadowMap();

public:
	virtual ~DxShadowMap();

public:
	static DxShadowMap& GetInstance();
};
