// NsOcTree.h: interface for the CCollision class.
//
//	class DxAABBOctree : 정점 데이터와 면 갯수를 넣으면 Octree 형식의 최적화된 OptimizeMesh 를 만든다.
//
//	Name : Sung-Hwan Han
//	Begin :2004/10/21
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NSOCTREE_H__INCLUDED_)
#define AFX_NSOCTREE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <map>
#include <vector>

#include "DxVertexFVF.h"
#include "Collision.h"
#include "DxTextureEffMan.h"

class DxTexEffBase;
class DxTexEffFlowUV;
class DxTexEffRotate;

enum EM_LAND_TYPE
{
	EMLR_DAYNIGHT_ON = 0,	// 낮밤이 있는 옥트리
	EMLR_DAYNIGHT_OFF = 1,	// 낮밤이 없는 옥트리
	EMLR_PIECE = 2,			// Piece Object
};

// Note : 예전 버젼은 아무 데이타 형이 가능 했지만 현재는 VERTEXCOLORTEX::FVF만 가능하게 되었다.
//		현재 DxEffectTiling 만이 D3DFVF_DIFFUSE를 추가해서 사용한다.
//		20060803 : 데이터 형이 VERTEXCOLORNORTEX로 바뀌었다. 이유는 반사맵을 사용하기 위함임.
struct OBJOCTree
{
	union
	{
		struct { D3DXVECTOR3 vMax; };
		struct { float m_fMaxX, m_fMaxY, m_fMaxZ; };
	};
	union
	{
		struct { D3DXVECTOR3 vMin; };
		struct { float m_fMinX, m_fMinY, m_fMinZ; };
	};

	struct DIRECTPOINTCOLOR
	{
		WORD wAlpha;
		WORD wDayR;
		WORD wDayG;
		WORD wDayB;
		WORD wNightR;
		WORD wNightG;
		WORD wNightB;
		WORD wTemp;		// 사이즈를 맞추기 위함.
	};

	struct NORMALCOLOR
	{
		D3DXVECTOR3 vNor;
		D3DXCOLOR	cColor;

		NORMALCOLOR() :
			vNor(0.f,0.f,0.f),
			cColor(1.f,1.f,1.f,1.f)
		{
		}
	};

	static DWORD	m_dwVERSION;

	OBJOCTree *pLeftChild;
	OBJOCTree *pRightChild;

	BYTE*		pVertex;
	DWORD		dwFace;

	LPD3DXMESH		pOcMesh;			// 최적화 파일을 만들기 전 Mesh이다. Render에는 쓰이지 않는다.
	NORMALCOLOR*	pNormalColor;		// Diffuse 값을 계산하고 난 후에는 사용하지 않는 값이다. Color가 있는것을 대비 한 것이다.
	DxAABBNode*		pCollsionTREE;

	EM_LAND_TYPE			m_emLandType;	// Load 하기 바로 전 혹은 Make할 때 셋팅된다.
	DWORD					m_dwFVF;
	UINT					m_nFVFSize;
	DWORD					m_dwVert;
	DWORD					m_dwFace;
	VERTEXNORCOLORTEX*		m_pVertSrc;		// 저장 시켜 놓을 장소.	Diffuse의 memcpy와 Effect Mesh를 위하여 존재
	DIRECTPOINTCOLOR*		m_pColor;		// Direct와 Point의 컬러	// 음.. 필요 없을경우가 있다. EMLR_DAYNIGHT_OFF,EMLR_PIECE
	LPDIRECT3DVERTEXBUFFERQ	m_pVB;			// 
	LPDIRECT3DINDEXBUFFERQ	m_pIB;
	CRITICAL_SECTION		m_pCSLockVB;	// Thread시 VB의 ... .. . 단일 변화.

	BOOL		m_bCheck;
	BOOL		m_bLoad;
	DWORD		m_dwFileCur;	// bDynamicLoading 을 위해 남겨둔다.

	void DrawEff( const LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL	IsLoad()
	{
		if( !m_bLoad )			return FALSE;
		if( !m_pVB )			return FALSE;
		//if ( !pCollsionTREE )	return FALSE;	// Prev Version - No Data

		return TRUE; 
	}

	void CleanUp();

	void CreateBASE( const LPDIRECT3DDEVICEQ pd3dDevice, const DWORD dwVert, const DWORD dwFaces );
	void CreateVertSrc( BYTE* pByte, const DWORD dwFVF, const DWORD dwVert );
	void CreateNormalColor( BYTE* pByte, const DWORD dwFVF, const DWORD dwVert );
	void ComputeNormalColor( const DWORD dwVert );
	void ComputeNormalColorDAYNIGHT_OFF( const DWORD dwVert );
	void CopyVertSrcToVB( BYTE* pByte, VERTEXNORCOLORTEX* pVertSrc, LPDIRECT3DVERTEXBUFFERQ	pVB, const DWORD dwVert );
	void CopyMeshToIB( WORD* pSrc, LPDIRECT3DINDEXBUFFERQ pIB, const DWORD dwFaces );
	void CovertPieceSetting( LPDIRECT3DDEVICEQ pd3dDevice );

	void InsertColorList( LOADINGDATALIST &listColorData );
	void FrameMoveCOLOR();
	void ComputeCOLOR( const DWORD& dwStart, const DWORD& dwEnd );

	void Save( CSerialFile& SFile, BOOL bPiece );
	void Load( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const BOOL bDynamicLoad, BOOL bPiece );
	void ThreadLoad( LOADINGDATALIST &listLoadingData, CRITICAL_SECTION &CSLockLoading );
	BOOL DynamicLoad( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const BOOL bThread );
	BOOL OctreeLoad( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const BOOL bThread );
	BOOL OctreeLoad_100( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const BOOL bThread );
	BOOL OctreeLoadOLD( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DWORD dwFVF, const BOOL bThread );
	void Load_VER100( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile );

	OBJOCTree () :
		vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX),
		vMin(FLT_MAX,FLT_MAX,FLT_MAX),
		pLeftChild(NULL),
		pRightChild(NULL),
		pVertex(NULL),
		dwFace(0),
		pOcMesh(NULL),
		pNormalColor(NULL),
		pCollsionTREE(NULL),

		m_emLandType(EMLR_DAYNIGHT_ON),
		m_dwFVF(VERTEXNORCOLORTEX::FVF),
		m_nFVFSize(0),
		m_dwVert(0L),
		m_dwFace(0L),
		m_pVertSrc(NULL),
		m_pColor(NULL),
		m_pVB(NULL),
		m_pIB(NULL),

		m_bCheck(FALSE),
		m_bLoad(FALSE),
		m_dwFileCur(0)
	{
		InitializeCriticalSection(&m_pCSLockVB);
	};

	~OBJOCTree ()
	{
		CleanUp();

		DeleteCriticalSection(&m_pCSLockVB);

		SAFE_DELETE(pLeftChild);
		SAFE_DELETE(pRightChild);
	};
};
typedef OBJOCTree* POBJOCTREE;


class DxAABBOctree
{
protected:
	static const DWORD	VERSION;

protected:
	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;
	OBJOCTree*			m_pOcTree;

public:
	void GetAABBSize ( D3DXVECTOR3 &_vMax, D3DXVECTOR3 &_vMin )
	{
		_vMax = m_vMax; _vMin = m_vMin;
	}
	void FirstLoad( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, CSerialFile* const SFile );
	void RenderTHREAD( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, LOADINGDATALIST &listLoadingData, CRITICAL_SECTION &CSLockLoading );
	void RenderTHREAD( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void RenderTHREAD_FlowUV( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxTexEffFlowUV* pEff );
	void RenderTHREAD_Rotate( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxTexEffRotate* pEff );
	void RenderDYNAMIC ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, CSerialFile* const SFile );
	void Render ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void Render ( const LPDIRECT3DDEVICEQ pd3dDevice );									// 조각파일
	void Render_FlowUV( const LPDIRECT3DDEVICEQ pd3dDevice, DxTexEffFlowUV* pEff );		// 조각파일
	void Render_Rotate( const LPDIRECT3DDEVICEQ pd3dDevice, DxTexEffRotate* pEff );		// 조각파일

	// Note : Color Thread 를 위한 작업
public:
	void InsertColorList( LOADINGDATALIST &listColorData );

public:
	void Create( const LPDIRECT3DDEVICEQ pd3dDevice, const DWORD dwFaceNUM, BYTE* pVertices, const DWORD dwFVF, EM_LAND_TYPE emLandType );

public:
	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, const BOOL bFrontColl );

public:
	void Save ( CSerialFile& SFile, BOOL bPiece );
	void Load ( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const BOOL bDynamicLoad, BOOL bPiece );
	void Load_VER100( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, std::string& szName );

protected:
	void CleanUp();

public:
	DxAABBOctree() :
		m_vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX),
		m_vMin(FLT_MAX,FLT_MAX,FLT_MAX),
		m_pOcTree(NULL)
	{
	};
	~DxAABBOctree();
};
typedef DxAABBOctree* PDXAABBOCTREE;

class DxSingleTexMesh : public OBJAABB
{
protected:
	static const DWORD	VERSION;

protected:
	DWORD				m_dwFlag;
	DxAABBOctree*		m_pAABBOctree;
	std::string			m_szTexName;
	LPDIRECT3DTEXTUREQ	m_pTexture;
	BOOL				m_bCheckTex;	// Texture 로딩은 1번만 시도하도록 한다.

protected:
	DxTexEffBase*		m_pTexEffList;	// Texture Effect List.

public:
	DxSingleTexMesh*	m_pNext;		// Tree 만들기 위한 임시 포인터

public:
	virtual void GetAABBSize ( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
	{ 
		if( m_pAABBOctree )
		{
			m_pAABBOctree->GetAABBSize( vMax, vMin );
		}
		else
		{
			vMax = D3DXVECTOR3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
			vMin = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );
		}
	}
public:
	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
	{
		D3DXVECTOR3 vMax, vMin;
		GetAABBSize( vMax, vMin );
		return COLLISION::IsWithInPoint ( vDivMax, vDivMin, (vMax+vMin)/2 );
	}
	virtual OBJAABB* GetNext () { return m_pNext; }
	virtual HRESULT RenderTHREAD ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, LOADINGDATALIST &listLoadingData, CRITICAL_SECTION &CSLockLoading );
	virtual HRESULT RenderDYNAMIC ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, CSerialFile* const SFile );
	virtual HRESULT FirstLoad( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, CSerialFile* const SFile );
	virtual void	FrameMove( const float fElpasedTime );
	HRESULT Render ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME& sCV );
	void	Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan::MAPTEXEFF* pmapTexEff );

	// Note : Color Thread 를 위한 작업
public:
	virtual void InsertColorList( LOADINGDATALIST &listColorData );
	
public:
	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, 
						LPCSTR& szName, const BOOL bFrontColl );
	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, const BOOL bFrontColl );
	void Create( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const DWORD dwFaceNUM, BYTE* pVertices, const DWORD dwFVF, BOOL bPiece );
	void ConvertTexEffMesh( const LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEff );

public:
	void Save ( CSerialFile& SFile, BOOL bPiece );
	void Load ( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const BOOL bDynamicLoad, BOOL bPiece );
	void Load_VER100 ( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile );

protected:
	void CleanUp();

public:
	DxSingleTexMesh();
	~DxSingleTexMesh();
};
typedef DxSingleTexMesh* PDXSINGLETEXMESH;
typedef std::map<std::string,DxSingleTexMesh*>				MAPSINGLETEXMESH;
typedef std::map<std::string,DxSingleTexMesh*>::iterator	MAPSINGLETEXMESH_ITER;

namespace NSOCTREE
{
	void OneTimeSceneInit();
	void CleanUp();

	void EnableDynamicLoad();
	BOOL IsDynamicLoad();

	extern float g_fCOLL_LENGTH;
	void MakeOctree( POBJOCTREE& pTree, BYTE* pVertex, DWORD dwFace, float fDisMAX, DWORD dwFaceMAX, const UINT nSIZE );	// Tree 를 만든다.
	void MakeOcMesh( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree, const DWORD dwFVF );								// Tree 안에 있는 Mesh 데이터를 생성한다.
	void MakeOptimizeMesh( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree );											// Tree 안에 있는 Mesh 데이터를 최적화 한다.
	void MakeCollisionData( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree );											// Tree 안에 Collsion 데이터를 만든다.
	void MakeLoadCheckOK( OBJOCTree* pTree );

	void MakeVBIBAndDelMesh( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree );	// Tree 안에 Mesh를 삭제하고 VB, IB, Normal을 빼낸다.
	void DeleteNormalColor( OBJOCTree* pTree );											// Normal을 삭제
	void CovertPieceSetting( LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree );			// Mesh를 Piece에 맞게 셋팅해준다.

	void InsertColorList( OBJOCTree* pTree, LOADINGDATALIST &listColorData );

	void RenderOctree( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJOCTree* pTree );
	void RenderOctree_FlowUV( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJOCTree* pTree, DxTexEffFlowUV* pEff );
	void RenderOctree_Rotate( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJOCTree* pTree, DxTexEffRotate* pEff );
	void RenderList( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree );
	void RenderList_FlowUV( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree, DxTexEffFlowUV* pEff );
	void RenderList_Rotate( const LPDIRECT3DDEVICEQ pd3dDevice, OBJOCTree* pTree, DxTexEffRotate* pEff );
	void AddNormalMesh( OBJOCTree* pTree );
	void CollisionLine( OBJOCTree* pTree, const D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, 
						BOOL& bColl, const BOOL bFrontColl );

	void SaveOctree ( CSerialFile& SFile, OBJOCTree* pTree, BOOL bPiece );
	void LoadOctree ( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, POBJOCTREE& pTree, const BOOL bDynamicLoad, BOOL bPiece );

	void FirstLoad( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, OBJOCTree* pTree, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin );
	void ThreadLoad( OBJOCTree* pTree, const CLIPVOLUME &sCV, LOADINGDATALIST &listLoadingData, CRITICAL_SECTION &CSLockLoading );
	void DynamicLoad( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, OBJOCTree* pTree, const CLIPVOLUME &sCV );
	void LoadOctreeVER_100( const LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, POBJOCTREE& pTree );
};

#endif // !defined(AFX_NSOCTREE_H__INCLUDED_)
