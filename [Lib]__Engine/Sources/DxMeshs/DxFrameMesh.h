// DxFrameMesh.h: interface for the DxFrameMesh class.
//
//
//
//
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXFRAMEMESH_H__ED2134D1_E13E_4E71_8619_C435D1E2403B__INCLUDED_)
#define AFX_DXFRAMEMESH_H__ED2134D1_E13E_4E71_8619_C435D1E2403B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxVertexFVF.h"
#include "./DxObject.h"
#include "./DxCustomTypes.h"
#include "./DxEffectFrame.h"
#include "./DxOctreeMesh.h"
#include "DxAnimationManager.h"

class	CSerialFile;
struct	DxEffFrameBase;
class	DxEffectBase;
struct	DXPIECEOBJ;
struct	DXPIECEOBJNODE;
struct	TEXEFF_PROPERTY;
class	DxTextureEffMan;

//	Note : ���͸��� (Attrib Table) ���� �ٸ� ������ ������ �ϱ� ���ؼ�.
//
enum DXMTRLTYPE
{
	MATERIAL_NORMAL			= 0,
	MATERIAL_ALPHA			= 1,
	MATERIAL_ALPHA_NOZWRI	= 2,
	MATERIAL_ALPHA_NOTEST	= 3,
	MATERIAL_ALPHA_HARD		= 4,
	MATERIAL_ALPHA_SOFT		= 5,

	MATERIAL_NSIZE			= 6
};
struct D3DEXMATERIAL
{

	DXMTRLTYPE	MtrlType;
	BOOL		bSubSetTriRend;

	D3DEXMATERIAL () :
		MtrlType(MATERIAL_NORMAL)
	{
		bSubSetTriRend = FALSE;
	}

	static char szMtrlTypes[MATERIAL_NSIZE][24];
	static DWORD VERSION;
};

enum MESH_METERIAL
{
	EM_MESH_MATERIAL = 0,
	EM_MESH_MATERIAL_RED = 1,
};

struct DxMeshes
{
public:
	static BOOL					g_bAlphaUP;
	static float				g_fAlpha;
	static MESH_METERIAL		g_emMeshMeterial;		// ��Ʈ���� ���� ����.

public:
	static void FrameMove( float fElapsedTime );

public:
	char				*szName;

	LPD3DXMESH			m_pSysMemMesh;
	LPD3DXMESH			m_pLocalMesh;
	DxOctreeMesh		*m_pDxOctreeMesh;

	DWORD					m_dwVert;
	DWORD					m_dwFace;
	DWORD					m_dwAttrib;

	VERTEX*					m_pVertSrc;
	LPDIRECT3DVERTEXBUFFERQ	m_pEffVB;
	LPDIRECT3DINDEXBUFFERQ	m_pEffIB;
	LPD3DXATTRIBUTERANGE	m_pAttrib;

	DWORD				cMaterials;
	D3DMATERIALQ		*rgMaterials;
	D3DEXMATERIAL		*exMaterials;
	TEXEFF_PROPERTY		*pTexEff;
	LPDIRECT3DTEXTUREQ	*pTextures;
	CString				*strTextureFiles;

	DxMeshes			*pMeshNext;

public:
	DxMeshes() :
		szName(NULL),
		m_pSysMemMesh(NULL),
		m_pLocalMesh(NULL),
		m_pDxOctreeMesh(NULL),
		m_dwVert(0L),
		m_dwFace(0L),
		m_dwAttrib(0L),
		m_pVertSrc(NULL),
		m_pEffVB(NULL),
		m_pEffIB(NULL),
		m_pAttrib(NULL),
		cMaterials(0),
		rgMaterials(NULL),
		exMaterials(NULL),
		pTexEff(NULL),
		pTextures(NULL),
		strTextureFiles(NULL),
		pMeshNext(NULL)
	{
	}
	~DxMeshes();

public:
	BOOL MakeEffectMeshs( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF );

	HRESULT MakeLocalMeshs ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF );
	HRESULT SetFVF ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF );

public:
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderAlpha ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bShadow=FALSE );
	
	HRESULT	RenderOctree( LPDIRECT3DDEVICEQ pd3dDevice, const float fAlpha = 1.f );
	HRESULT	RenderOctreeAlphaMap( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bShadow=FALSE, const float fAlpha = 1.f );

	HRESULT RenderTriangle ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	BOOL		SaveFile ( CSerialFile &SFile );
	BOOL		LoadFile ( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void		SaveSet ( CSerialFile &SFile );

	//	Note : �浹 üũ
	//
public:
	void		IsCollision ( const D3DXMATRIX &matCom, const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, D3DXVECTOR3 &vCollision, 
							BOOL &bCollision, char* pName, const BOOL bFrontColl );
	void		GetClosedPointIndex ( const D3DXMATRIX &matCom, const D3DXVECTOR3 &vCollision, DWORD &dwIndex, D3DXVECTOR3 &vVert );
};

#define __DXFRAMEROOT	("__ROOT")

struct OCTREEVERTEX;
struct CLIPVOLUME;
struct DxAnimation;
class  DxOctree;
class DxFrameMesh;

enum FRAMETYPES
{
	FRAME_COMMON,
	FRAME_DOOR
};

#define DXFRM_ALPHAMAP	(0x00000001)
#define DXFRM_NAVIFRAME	(0x00000010)

struct DxFrame
{
	typedef DxFrame* PDXFRAME;

	//	Note : ���� �ɹ�..
	//
	static DWORD	dwFrameTypeNum;		//	������ Ÿ�� ����.
	static char		szFrameType[2][24];	//	������ Ÿ�� ���ڿ�.	- editer ��.


	//	Note : Frame �з�.
	//
	char*			szName;				//	������ �̸�.
	FRAMETYPES		eFrameType;			//	������ Ÿ��.

	//	Note : ������ ����.
	//
	DWORD			bFlag;

	//	Note : �޽� ����.
	//
	DxMeshes		*pmsMeshs;

	union
	{
		struct { D3DXVECTOR3 vTreeMax; };
		struct { float m_fMaxX, m_fMaxY, m_fMaxZ; };
	};
	union
	{
		struct { D3DXVECTOR3 vTreeMin; };
		struct { float m_fMinX, m_fMinY, m_fMinZ; };
	};

	DXPIECEOBJ*		pRePlacePiece;		//	ġȯ ����.
	BOOL			bPieceUSE;			// DxPiece�� ���� ���̴�. DXPIECEOBJ ���� Ʋ�� �༮��.

	//	Note : Ʈ������ ����.
	//
	D3DXMATRIX		matRot;				//	�ش�ð�(���ϰ� ������)�� Ʈ������.
	D3DXMATRIX		matRotOrig;			//	���� �������� �ƴ� ���� Ʈ������.
	D3DXMATRIX		matCombined;		//	�θ� ���� ������ ���� Ʈ������.

	DxAnimation		*pframeFromAnimate;	//	�� �������� �����ϴ� ���ϸ��̼�.
	BOOL			bParentMatrixUSE;	// ����� ���� �ʰ� �θ��� Matrix�� �׳� �ᵵ �Ǵ� �� �ΰ�?

	//	Note : ����Ʈ ó����.
	//
	DxEffectBase	*pEffectPrev;
	DxEffectBase	*pEffect;
	DxEffectBase	*pEffectNext;

	DxEffFrameBase	*pEffFrame;

	DxFrame			*pNextAlphaMapFrm;

	//	Note : �ڽ� ��� �� ���� ��� ����.
	//
	DxFrame			*pframeSibling;		//	���� ����
	DxFrame			*pframeFirstChild;	//	�ڽ� ����

	DxFrame () :
		pmsMeshs(NULL),
		bFlag(NULL),
		eFrameType(FRAME_COMMON),
		vTreeMax(0,0,0),
		vTreeMin(0,0,0),
		pRePlacePiece(NULL),
		pframeFromAnimate(NULL),
		pframeSibling(NULL),
		pframeFirstChild(NULL),
		szName(NULL),
		pEffectPrev(NULL),
		pEffect(NULL),
		pEffectNext(NULL),
		pEffFrame(NULL),
		pNextAlphaMapFrm(NULL),
		bPieceUSE(FALSE)
	{
		D3DXMatrixIdentity(&matRot);
		D3DXMatrixIdentity(&matRotOrig);
		D3DXMatrixIdentity(&matCombined);
	}

	DxFrame::~DxFrame ();

	void AddFrame ( DxFrame *pframe )
	{
		if ( pframeFirstChild == NULL )
		{
			pframeFirstChild = pframe;
		}
		else
		{
			pframe->pframeSibling = pframeFirstChild->pframeSibling;
			pframeFirstChild->pframeSibling = pframe;
		}
	}

	void AddMesh ( DxMeshes *pms )
	{
		pms->pMeshNext = pmsMeshs;
		pmsMeshs = pms;
	}

	DxFrame* FindFrame ( const char *szFrame )
	{
		DxFrame *pframe;
		if ( ( szName != NULL ) && ( strcmp ( szName, szFrame ) == 0 ) )
			return this;

		if ( pframeFirstChild != NULL )
		{
			pframe = pframeFirstChild->FindFrame ( szFrame );
			if ( pframe != NULL )	return pframe;
		}
		
		if ( pframeSibling != NULL )
		{
			pframe = pframeSibling->FindFrame ( szFrame );
			if ( pframe != NULL )	return pframe;
		}

		return NULL;
	}

	DxFrame* FindParentFrame ( DxFrame* pDxFindFrame )
	{
		DxFrame *pframeChild = pframeFirstChild;
		while ( pframeChild != NULL )
		{
			if ( pframeChild == pDxFindFrame )	return this;

			DxFrame *pframe = pframeChild->FindParentFrame ( pDxFindFrame );
			if ( pframe != NULL ) return pframe;

			pframeChild = pframeChild->pframeSibling;
		}

		return NULL;
	}

public:
	BOOL	IsAlphaMapMesh ();
	BOOL	IsReplace ();	// ȿ����ü�� ġȯ ���� ���� ��ü���� ���� ��� ��� �ؾ���.

public:
	D3DXVECTOR3		GetTreeCenter() { return (vTreeMax+vTreeMin)*(0.5f); };
	float			GetTreeXWidth() { return vTreeMax.x-vTreeMin.x; };
	float			GetTreeYWidth() { return vTreeMax.y-vTreeMin.y; };
	float			GetTreeZWidth() { return vTreeMax.z-vTreeMin.z; };

public:
	HRESULT CalculateBoundingBox ( D3DXMATRIX *pmatCur, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, float fUnitTime, BOOL bAnimate=FALSE );
	HRESULT CalculateBoundingBoxKey ( D3DXMATRIX *pmatCur, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, float dwTime );
	HRESULT CalculateBoundingSphere ( D3DXMATRIX *pmatCur, float fUnitTime );

public:
	void		GetAllNodeData ( DWORD* pIndices, DWORD* pPrevIndexPos,
					OCTREEVERTEX* pVertices, DWORD* pPrevVertexPos,
					DxFrame** ppDxFrames, DWORD* pPrevDxFramePos,
					DxFrame* pDxFrameThis, bool breplace_in, const BOOL bEdit );
	HRESULT		FrameMeshToOctree	( DxOctree *pDxOctree, D3DXVECTOR3 *pvMin, D3DXVECTOR3 *pvMax, LPDIRECT3DDEVICEQ pd3dDevice, int debug, int debug2 );

public:
	BOOL		SaveFile ( CSerialFile &SFile );
	BOOL		LoadFile ( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice, PDXEFFECTBASE &pEffGlobalHead, PDXFRAME &pAlphaMapHead );
	DxEffectBase* LoadEffect ( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void		SaveSet ( CSerialFile &SFile );

protected:	
	//	Note : Octree�� �ޱ� ���� DxFrame�� �Ʒ��� �޼ҵ带 �̿��ؼ�
	//		   ��ȯ�ؼ� ����(��带 ����)�ϴ� �Լ�
	HRESULT		DxFrameNodeCopy ( DxFrame *pDst, DxFrame *pSrc, D3DXVECTOR3 *pvMin, D3DXVECTOR3 *pvMax,
		LPDIRECT3DDEVICEQ pd3dDevice, char *pParentName );

	//	Note : DxMeshes�� ID3DXMESH�� DxOctreeMesh�� �ٲ��ִ� �ֿ� �޽��	
	HRESULT		ConvertMesh		( DxMeshes **ppDstMesh, DxMeshes *pSrcMesh, D3DXVECTOR3 *pvMin, D3DXVECTOR3 *pvMax, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT		GetMeshRawData	( BYTE	**ppIndices, BOOL b32BitMesh, OCTREEVERTEX	**ppVertices, LPD3DXMESH pMeshData, int NumOfIndices, int NumOfVertices );
	HRESULT		CheckInTheBox	( int *pNumOfInBoxFaces, BOOL **ppbInBoxFaces,
										int *pNumOfInBoxVertices, BOOL **ppbInBoxVertices,
										BYTE *pIndices, BOOL b32BitMesh, OCTREEVERTEX *pVertices,
										int NumOfIndices, int NumOfVertices, D3DXVECTOR3 *pvMin, D3DXVECTOR3 *pvMax );

	HRESULT		MakeOctreeMeshData ( DxMeshes **ppDstMesh, DxMeshes *pSrcMesh,
										 int NumOfInBoxFaces, BOOL *pbInBoxFaces,
										 int NumOfInBoxVertices, BOOL *pbInBoxVertices,
										 BYTE *pIndices, BOOL b32BitMesh, OCTREEVERTEX	*pVertices,
										 LPDIRECT3DDEVICEQ pd3dDevice );

	void		DelMeshRawData ( BYTE *pIndices, BOOL b32BitMesh, OCTREEVERTEX *pVertices, BOOL *pbInBoxFaces, BOOL *pbInBoxVertices );	

public:
	//	Note : ���ϸ��̼� Frame ����.
	//
	HRESULT		AniFrameTreeCopy ( DxFrame *pDst, DxFrame *pSrc, DxAnimationMan *pDxAnimationMan,
								LPDIRECT3DDEVICEQ pd3dDevice, char *pParentName );
	HRESULT		GetAniFrameMesh ( LPDXANIMATIONMAN &pDxAnimationMan,
								float fUNITTIME, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : �浹 üũ
	//
public:
	BOOL		IsCollision ( const D3DXVECTOR3 &vPoint1, const D3DXVECTOR3 &vPoint2, D3DXMATRIX* pMat=NULL );
	BOOL		BoxIsInTheFrustum ( D3DXMATRIX* pMat, CLIPVOLUME &cv, LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL		CollisionDetect ( const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, D3DXVECTOR3 &vCollision, char* pName, const BOOL bFrontColl );

protected:
	void		CollisionDetect ( DxFrame *pframeCur, const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2,
								BOOL &bCollision, D3DXVECTOR3 &vCollision, char* pName, const BOOL bFrontColl );

public:	
	void		DxFrameNodeCount ( DWORD &nNodeCount );
};

typedef DxFrame* LPDXFRAME;

class DxFrameMesh : public DxObject
{
public:
	static BOOL	m_bNaviDraw;
	static DWORD VERSION;

	//	Note : ���� ����.
protected:
	DWORD		m_dwFVF;
	DxFrame*	m_pDxFrame;
	char*		m_pszFileName;

	union
	{
		struct { D3DXVECTOR3	vTreeMax; };
		struct { float m_fMaxX, m_fMaxY, m_fMaxZ; };
	};
	union
	{
		struct { D3DXVECTOR3	vTreeMin; };
		struct { float m_fMinX, m_fMinY, m_fMinZ; };
	};

	DWORD		m_dwVertices;	//	�� ���ؽ���.
	DWORD		m_dwTriangles;	//	�� ���̽���.

	//	Note : Alpha Map ������ ���.
	DxFrame			*m_pDxAlphaMapFrameHead;

	//	Note : ġȯ ���� ����.
	DXPIECEOBJ		*m_pPieceObjHead;	//	ġȯ ���� ����Ʈ.
	DXPIECEOBJNODE	*m_pPieceObjTree;	//	ġȯ ���� Ʈ��.


	//	Note : ���ϸ���Ʈ
protected:
	float				fCurTime;		//	���ϸ���Ʈ �ð�.
	float				m_UNITTIME;		//	���ϸ��̼� ���� �ð�.
	DxAnimation*		pAnimHead;		//	���ϸ��̼� ������ ���.
	DxAnimationMan*		pAnimManHead;	//	���ϸ��̼� ��� ������ ����.

	//	Note : ����Ʈ ó����.
	DxEffectBase*		m_pEffectHead;

	//	Note : �浹 üũ
protected:
	D3DXVECTOR3			m_vCollionPos;
	LPDXFRAME			m_pCollDetectFrame;	//	�浹 üũ�� ����� FRAME.

	// Note : TextureEff
protected:
	DxTextureEffMan*	m_pTextureEff;

protected:
	void CollisionDetect ( DxFrame *pframeCur, const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, BOOL &bCollision, char* pName, const BOOL bFrontColl );

public:
	BOOL IsCollision ( const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, char* pName, const BOOL bFrontColl );
	LPDXFRAME const		GetCollisionDetectedFrame ()	{ return m_pCollDetectFrame; }
	void				SetCollisionDetectedFrame ( LPDXFRAME pDxFrame )	{ m_pCollDetectFrame = pDxFrame; }
	const D3DXVECTOR3	GetCollisionPos ()				{ return m_vCollionPos; }

public:
	DxEffectBase*		GetEffectList () { return m_pEffectHead; }

	void AddEffect ( DxEffectBase *pEffectAdd );
	BOOL DelEffect ( DxFrame *pDxFrame, DxEffectBase *pEffectDel );

public:
	void AddAlphaMapFrameAll ( DxFrame *pDxFrame );
	void DelAlphaMapFrameAll ( DxFrame *pDxFrame );

	void AddAlphaMapFrame ( DxFrame *pDxFrame );
	void DelAlphaMapFrame ( DxFrame *pDxFrame );

public:
	BOOL AddPieceObj ( char *szFrameName, const char *szPieceName );
	void DelPieceObj ( char *szFrameName );

	DXPIECEOBJ* GetPieceList () { return m_pPieceObjHead; }

	void MakePieceAABBTree ();
	void CleanPieceAABBTree ();

public:
	void ModiflyAlphaMapFrame ( DxFrame *pDxFrame )
	{
		if ( pDxFrame->IsAlphaMapMesh() )
		{
			pDxFrame->bFlag |= DXFRM_ALPHAMAP;
			AddAlphaMapFrame ( pDxFrame );
		}
		else
		{
			pDxFrame->bFlag &= ~DXFRM_ALPHAMAP;
			DelAlphaMapFrame ( pDxFrame );
		}
	}

public:
	void			SetFVF ( DWORD dwFVF, BOOL bNow=FALSE, LPDIRECT3DDEVICEQ pd3dDevice=NULL );
	HRESULT			SetFVF ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, DWORD *pdwFVF = NULL );

public:
	void			SetCurTime ( float fCur ) { fCurTime = fCur; }

public:
	char*			GetFileName () { return m_pszFileName; }
	DxFrame*		GetFrameRoot () { return m_pDxFrame; }
	DWORD			GetVertices()	{ return m_dwVertices; }
	DWORD			GetTriangles()	{ return m_dwTriangles;}

	float			GetUnitTime ()	{ return m_UNITTIME; }

public:
	const D3DXVECTOR3&	GetTreeMax()	{ return vTreeMax; }
	const D3DXVECTOR3&	GetTreeMin()	{ return vTreeMin; }
	const D3DXVECTOR3	GetTreeCenter() { return (vTreeMax+vTreeMin)*(0.5f); };
	float				GetTreeXWidth() { return vTreeMax.x-vTreeMin.x; };
	float				GetTreeYWidth() { return vTreeMax.y-vTreeMin.y; };
	float				GetTreeZWidth() { return vTreeMax.z-vTreeMin.z; };

protected:
	void AddAnimationFrame ( DxAnimation *pAnim )
	{
		pAnim->pAnimNext = pAnimHead;
		pAnimHead = pAnim;
	}

	void MakeAnimationList ();
	void MakeAnimationList ( DxFrame * pDxFrame );

public:
	DxAnimation* GetAnimationList ()	{ return pAnimHead; }
	DxAnimation* FindAnimation ( char *szName )
	{
		DxAnimation* pCur = pAnimHead;
		while ( pCur )
		{
			if ( !strcmp(pCur->szName,szName) )
			{
				return pCur;
			}

			pCur = pCur->pAnimNext;
		}

		return NULL;
	}

	DWORD GetAniManCount ()
	{
		DWORD	nCount = 0;
		DxAnimationMan*		pDxAniMan = pAnimManHead;
		while ( pDxAniMan )
		{
			nCount++;
			pDxAniMan = pDxAniMan->m_pNext;
		}
		return nCount;
	}
	DxAnimationMan* FindAniRootFrame ( DxFrame *pDxFrame ); // ������ ��ɿ��� �ʿ�.

	DxFrame* GetAniFrame( const char*szFrameName );

	DxTextureEffMan* GetTextureEffMan()		{ return m_pTextureEff; }

public:
	DxFrame* FindFrame ( const char *szName )
	{
		if ( m_pDxFrame == NULL )	return NULL;
		else						return m_pDxFrame->FindFrame ( szName );
	}

	DxFrame* FindParentFrame ( DxFrame* pDxFrame )
	{
		if ( m_pDxFrame == NULL || pDxFrame == NULL )	return NULL;
		else	return m_pDxFrame->FindParentFrame ( pDxFrame );
	}

	void CalculateBoundingSphere ()
	{
		D3DXMATRIX mCur;
		D3DXMatrixIdentity ( &mCur );
		m_pDxFrame->CalculateBoundingSphere ( &mCur, m_UNITTIME );

		vTreeMax = m_pDxFrame->vTreeMax;
		vTreeMin = m_pDxFrame->vTreeMin;
	}

protected:
	HRESULT LoadMeshHierarchy ( LPDIRECT3DDEVICEQ pd3dDevice );
	
	HRESULT LoadMesh ( LPD3DXFILEDATA pxofobjCur, DWORD fvf, LPDIRECT3DDEVICEQ pD3DDevice,
					DxFrame *pframeParent);
	HRESULT LoadFrames ( LPD3DXFILEDATA pxofobjCur, DWORD fvf, LPDIRECT3DDEVICEQ pD3DDevice,
					DxFrame *pframeParent);
	HRESULT LoadAnimation ( LPD3DXFILEDATA pxofobjCur, DWORD fvf, LPDIRECT3DDEVICEQ pD3DDevice );
	HRESULT LoadAnimationSet ( LPD3DXFILEDATA pxofobjCur, DWORD fvf, LPDIRECT3DDEVICEQ pD3DDevice );

	HRESULT MakeLocalMeshs ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur );
	HRESULT DeleteLocalMeshs ( DxFrame *pframeCur );

protected:
	float m_fTime, m_fElapsedTime;

protected:
	HRESULT UpdateFrames ( DxFrame *pframeCur, D3DXMATRIX &matCur );
	HRESULT DrawFrames ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, CLIPVOLUME *pCV );
	HRESULT DrawFrames_NOEFF ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, CLIPVOLUME *pCV );
	HRESULT DrawFrames_Reflect ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, CLIPVOLUME *pCV );

	HRESULT DrawAlphaMapFrames ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV, BOOL bRefelct=FALSE );

public:
	HRESULT DrawSelectedFrame ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV );	//	editer ��.
	HRESULT DrawSelectedPoint ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 vVert );	//	editer ��.

public:
	void SetFile ( const char *szFileName );

	virtual HRESULT OneTimeSceneInit();
	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	virtual HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	virtual HRESULT InvalidateDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects();
	virtual HRESULT FinalCleanup();

	void	SetPieceUseFALSE();
	void	SetPieceUseChild( DxFrame* pFrame, const BOOL bUse );
	void	AddPiece2( const char* pName );
	void	DelPiece2( const char* pName );
	void	SetAnimationTimeZero();
	HRESULT FrameMovePiece ( float fTime, float fElapsedTime );
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX *pWorld, CLIPVOLUME *pCV );
	HRESULT Render_EFF ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX *pWorld, CLIPVOLUME *pCV );
	HRESULT Render_NOEFF ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV );
	HRESULT Render_Reflect ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV );

	HRESULT UpdateFrames ()
	{
		D3DXMATRIXA16 mCur;
		D3DXMatrixIdentity ( &mCur );
		return UpdateFrames ( m_pDxFrame, mCur );		
	}
	
public:
	BOOL LoadSet( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL LoadSet_VER100( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	void SaveSet( CSerialFile &SFile );

	void SaveFrameEff( CSerialFile &SFile );

public:
	DxFrameMesh();
	virtual ~DxFrameMesh();
};

#endif // !defined(AFX_DXFRAMEMESH_H__ED2134D1_E13E_4E71_8619_C435D1E2403B__INCLUDED_)
