#pragma once

class CSerialFile;
class DxMaterialHLSL;



// enum for various skinning modes possible
enum METHOD
{
    D3DNONINDEXED,
    D3DINDEXED,
    SOFTWARE,
    D3DINDEXEDVS,
    D3DINDEXEDHLSLVS,
    NONE
};

struct SMeshContainer;
struct DxSkeleton;
struct SVERTEXINFLU;
struct SMATERIAL_PIECE;
struct DXMATERIAL_CHAR_EFF;


//typedef struct _D3DXFRAME {
//    LPSTR Name;
//    D3DXMATRIX TransformationMatrix;
//    LPD3DXMESHCONTAINER pMeshContainer;
//    struct _D3DXFRAME *pFrameSibling;
//    struct _D3DXFRAME *pFrameFirstChild;
//} D3DXFRAME, *LPD3DXFRAME;

//--------------------------------------------------------------------------------------
// Name: struct D3DXFRAME_DERIVED
// Desc: Structure derived from D3DXFRAME so we can add some app-specific
//       info that will be stored with each frame
//--------------------------------------------------------------------------------------
struct SFrame : public D3DXFRAME
{
	D3DXMATRIXA16		CombinedTransformationMatrix;

	SFrame *FindFrame ( char *szFrame )
	{
		SFrame *pframe;
		if ( (Name!=NULL) && (strcmp(Name, szFrame) == 0) )		return this;

		if ( pFrameFirstChild != NULL )
		{
			pframe = ((SFrame*)pFrameFirstChild)->FindFrame ( szFrame );
			if ( pframe != NULL )	return pframe;
        }

		if ( pFrameSibling != NULL )
		{
			pframe = ((SFrame*)pFrameSibling)->FindFrame ( szFrame );
			if ( pframe != NULL )		return pframe;
		}

		return NULL;
	}

	SFrame* FindParentFrame ( SFrame* pFindFrame )
	{
		SFrame *pframeChild = (SFrame*)pFrameFirstChild;
		while ( pframeChild != NULL )
		{
			if ( pframeChild == pFindFrame )	return this;

			SFrame *pframe = pframeChild->FindParentFrame ( pFindFrame );
			if ( pframe != NULL ) return pframe;

			pframeChild = (SFrame*)pframeChild->pFrameSibling;
		}

		return NULL;
	}

	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}
};

enum RENDER_OPT
{
	CTOP_BASE	= 0,
    CTOP_SHADOW = 1,	// Toon - 음영.
    CTOP_EDGE	= 2,	// Toon - Edge
	CTOP_NEWTEX = 3,	// New Textuer
};

struct CHARSETTING
{
	LPD3DXMESHCONTAINER		pMeshContainerBase;
	SMATERIAL_PIECE*		pmtrlPiece;
	DXMATERIAL_CHAR_EFF*	pmtrlSpecular;
	BOOL					bWorldIdentity;
	RENDER_OPT				emRDOP;
};

class DxSkinMesh9
{
public:
	static METHOD	m_SkinningMethod;
	static DWORD	m_dwBehaviorFlags;	// Behavior flags of the 3D device
	
protected:
	char			m_szName[MAX_PATH];				//	형상 xFile 이름.
	char			m_szSkeleton[MAX_PATH];			//	Skeleton xFile 이름.

protected:
	DWORD			m_dwRefCount;
	LPD3DXFRAME		m_pFrameRoot;
	SMeshContainer*	m_pMeshContainerHead;
	DxSkeleton*		m_pSkeleton;

	D3DXVECTOR3		m_vObjectCenter;
	FLOAT			m_fObjectRadius;	// Radius of bounding sphere of object

	ID3DXAnimationController*	m_pAnimController;

public:
	bool			m_bUseSoftwareVP;	// Flag to indicate whether software vp is
										// required due to lack of hardware
protected:
	BOOL		m_bOriginDraw;
	BOOL		m_bAlpha;
	BOOL		m_bShadow;
	BOOL		m_bAlphaTex;
	BOOL		m_bLevel;
	float		m_fHeight;

public:
	D3DXMATRIX	m_matWorld;

public:
	D3DXMATRIXA16	m_matView;			// View matrix
	D3DXMATRIXA16	m_matProj;			// Projection matrix
	D3DXMATRIXA16	m_matProjT;			// Transpose of projection matrix (for asm shader)

public:
	DxSkinMesh9 ();

public:
	DWORD AddRef ()							{ return ++m_dwRefCount; }
	DWORD DisRef ()							{ if (m_dwRefCount>0) --m_dwRefCount; return m_dwRefCount; }

public:
	const D3DXVECTOR3& GetObjectCenter ()	{	return m_vObjectCenter; }
	FLOAT GetObjectRadius ()				{	return m_fObjectRadius; }
	bool IsUseSoftwareVP ()					{	return m_bUseSoftwareVP; }

protected:
	HRESULT SetupNameOnMeshContainer ( LPD3DXFRAME pFrame );

	HRESULT SetupBoneMatrixPointersOnMesh ( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase );
	HRESULT SetupBoneMatrixPointers ( LPD3DXFRAME pFrame );

	void ReleaseAttributeTable( LPD3DXFRAME pFrameBase );

public:
	void DrawFrame ( IDirect3DDevice9 *pd3dDevice, LPD3DXFRAME pFrame );
	virtual void DrawMeshContainer( IDirect3DDevice9 *pd3dDevice, const CHARSETTING& sCharSetting ) = 0;
	virtual void DrawMeshSpecular( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPieceNULL, 
							DXMATERIAL_CHAR_EFF* pmtrlSpecular=NULL, BOOL bWorldIdentity=FALSE ) = 0;
	virtual void DrawMeshCartoon( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPiece, RENDER_OPT emCTOP ) = 0;
	virtual void DrawMeshNORMALMAP( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPieceNULL, 
							DxMaterialHLSL* pMaterialHLSL, BOOL bWorldIdentity=FALSE ) = 0;

	// Note	: 일반메쉬를 렌더링 할 경우 쓰이는 것.
	// Parameters : pMatrix는 NULL 이 오면 절대 안된다.  
	//				에러를 유발한다.
	void DrawFrameMesh( IDirect3DDevice9 *pd3dDevice, SMeshContainer* pMeshContainer, SMATERIAL_PIECE* pmtrlPiece, LPD3DXMATRIX pMatrix );

	void UpdateSkinningMethod ( LPD3DXFRAME pFrameBase );
	void UpdateFrameMatrices ( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix );

protected:
	HRESULT RenderDefault ( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr, DWORD dwThisAttribId, 
							SMATERIAL_PIECE *pmtrlPiece, DXMATERIAL_CHAR_EFF* pmtrlSpecular );

protected:
	BOOL CheckLoadTexture( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pMeshContainer, DWORD dwThisAttribId );

public:
	HRESULT OnNewSkinnedMethod ( METHOD NewSkinningMethod );

public:
	void Init_Detail_Texture ();
	void SetDrawState ( BOOL bOriginDraw=TRUE, BOOL bAlpha=FALSE, BOOL bShadow=FALSE, BOOL bAlphaTex=TRUE, BOOL bLevel=FALSE, float fHeight=0.f );

public:
	SMeshContainer* FindMeshContainer ( LPD3DXFRAME pFrame, char *szName );

	SFrame* GetFrameRoot ()					{ return (SFrame*)m_pFrameRoot; }
	const char* GetSkinFileName ()			{ return m_szName; }
	D3DXVECTOR3 GetCenter ()				{ return m_vObjectCenter; }
	float GetRadius ()						{ return m_fObjectRadius; }

	SMeshContainer* GetMeshClobalList ()	{ return (SMeshContainer*)m_pMeshContainerHead; }

public:
	void AddMeshGlobal ( SMeshContainer *pmc );

public:
	SFrame* FindFrame ( char *szName );
	SFrame* FindParentFrame ( SFrame* pFrame );
	SMeshContainer* FindMeshContainer ( char *szName )		{ return FindMeshContainer(m_pFrameRoot,szName); }
	HRESULT CalculateBoundingBox ( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer* pmcMesh, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

public:
	HRESULT OnCreateSkin ( IDirect3DDevice9* pd3dDevice, const char* szSkinFile, const char *szSkeletonFile, BOOL bThread );


public:
	void CreateCartoonMesh( LPDIRECT3DDEVICEQ pd3dDevice, float fThickness );

public:
	static HRESULT StaticCreate (IDirect3DDevice9* pd3dDevice );
	static HRESULT StaticResetDevice ( IDirect3DDevice9* pd3dDevice );
	static void StaticLost ();
	static void StaticDestroy ();

public:
	void OnSkinDestroyDevice ();
	void OnSkinCleanUp ();

public:
	HRESULT OnSkinFrameMove ( double fTime, float fElapsedTime, D3DXMATRIXA16 &matWorld );
	void OnSkinFrameRender ( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );

public:
	void SaveNORMAL( TCHAR* pName );
	void SaveHLSL( TCHAR* pName );
	void SaveNormalMesh( TCHAR* pName );
	void SaveLOD_SW( TCHAR* pName );
	virtual void Load( TCHAR* pName, IDirect3DDevice9 *pd3dDevice ) = 0;

private:
	void SaveNORMAL( CSerialFile& SFile );
	void SaveHLSL( CSerialFile& SFile );
	void SaveNormalMesh( CSerialFile& SFile );
	void SaveLOD_SW( CSerialFile& SFile );
	virtual void Load( CSerialFile& SFile, IDirect3DDevice9 *pd3dDevice ) = 0;
};



//--------------------------------------------------------------------------------------
// Name: class CAllocateHierarchy
// Desc: Custom version of ID3DXAllocateHierarchy with custom methods to create
//       frames and meshcontainers.
//--------------------------------------------------------------------------------------
class CAllocateHierarchy: public ID3DXAllocateHierarchy
{
public:
	BOOL			m_bThread;
	DxSkinMesh9*	m_pSkinMesh;

public:
    STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
    STDMETHOD(CreateMeshContainer)(THIS_ 
        LPCSTR Name, 
        CONST D3DXMESHDATA *pMeshData,
        CONST D3DXMATERIAL *pMaterials, 
        CONST D3DXEFFECTINSTANCE *pEffectInstances, 
        DWORD NumMaterials, 
        CONST DWORD *pAdjacency, 
        LPD3DXSKININFO pSkinInfo, 
        LPD3DXMESHCONTAINER *ppNewMeshContainer);
    STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
    STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);

	CAllocateHierarchy () :
		m_pSkinMesh(NULL)
	{}
};
