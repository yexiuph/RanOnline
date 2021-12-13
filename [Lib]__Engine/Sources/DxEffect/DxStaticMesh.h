// DxStaticMeshCreate.h: interface for the CCollision class.
//
//	class DxStaticMesh : FrameMesh 데이타를 넣으면 AABB 트리 적용하고 Octree로 세세하고 나눈다.
//
//	Name : Sung-Hwan Han
//	Begin :2004/10/21
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <set>
#include <map>

#include "SerialFile.h"

#include "DxFrameMesh.h"
#include "NsOCTree.h"
#include "DxStaticMeshThread.h"

class DxTextureEffMan;

class DxStaticMesh
{
protected:
	static const DWORD	VERSION;

protected:
	CSerialFile m_SFile;		// Thread Loading 에 필요함.

public:
	MAPSINGLETEXMESH	m_mapMesh;
	MAPSINGLETEXMESH	m_mapMeshALPHA;
	MAPSINGLETEXMESH	m_mapMeshSOFTALPHA;
	MAPSINGLETEXMESH	m_mapMeshSOFTALPHA01;
	MAPSINGLETEXMESH	m_mapMeshSOFTALPHA02;

protected:
	DxSingleTexMesh*	m_pList;
	DxSingleTexMesh*	m_pAlphaList;
	DxSingleTexMesh*	m_pAlphaSoftList;
	DxSingleTexMesh*	m_pAlphaSoftList01;
	DxSingleTexMesh*	m_pAlphaSoftList02;

	OBJAABBNode*		m_pMeshTree;
	OBJAABBNode*		m_pMeshAlphaTree;
	OBJAABBNode*		m_pMeshSoftAlphaTree;
	OBJAABBNode*		m_pMeshSoftAlphaTree01;
	OBJAABBNode*		m_pMeshSoftAlphaTree02;

protected:
	D3DXVECTOR3		m_vMin;				// 최소값
	D3DXVECTOR3		m_vMax;				// 최대값

private:
	DxStaticMesh_THREAD	m_sThread;			// Loading을 위한 Thread
	LOADINGDATALIST		m_listLoadingData;	// 주소 리스트
	CRITICAL_SECTION	m_CSLockLoading;	// m_listLoadingData의 변화가 중복되지 않도록 하기 위함.

private:
	enum EMRENDERTYPE
	{
		EMRT_OCTREE = 0,
		EMRT_PIECE = 1,
	};

	EMRENDERTYPE	m_emRenderType;

public:
	BOOL IsMesh ()			{ return (BOOL)m_mapMesh.size(); }
	BOOL IsMeshALPHA ()		{ return (BOOL)m_mapMeshALPHA.size(); }
	BOOL IsMeshSOFTALPHA ()	{ return (BOOL)m_mapMeshSOFTALPHA.size(); }


	D3DXVECTOR3 GetAABBMax ()	{ return m_vMax; }
	D3DXVECTOR3 GetAABBMin ()	{ return m_vMin; }

protected:
	void SetAABBMaxMin ();

public:
	void MakeAABBOCTree( const LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh* const pFrameMesh, const BOOL bProgress, BOOL bPiece );

public:
	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, 
						LPCSTR& szName, const BOOL bFrontColl );
	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, const BOOL bFrontColl );

public:
	void FrameMove( const float fElapsedTime );

	// Thread Loading Render
	void Render_THREAD_( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void Render_THREAD_Alpha( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void Render_THREAD_SoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );

	// Dynamic Loading Render	|| General Render <SFile==NULL>
	void Render_DYNAMIC_( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, CSerialFile* const SFile );
	void Render_DYNAMIC_Alpha( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, CSerialFile* const SFile );
	void Render_DYNAMIC_SoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, CSerialFile* const SFile );

	// Piece File Render
	void Render_PIECE_( const LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan::MAPTEXEFF* pmapTexEff );
	void Render_PIECE_Alpha( const LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan::MAPTEXEFF* pmapTexEff );
	void Render_PIECE_SoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan::MAPTEXEFF* pmapTexEff );

	// Piece File Render
	void Render( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV );
	void RenderAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV );
	void RenderSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV );
	void RenderSoftAlpha01( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV );
	void RenderSoftAlpha02( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV );

protected:
	void CleanUp();

public:
	void BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos );
	void BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin );

	// Thread Loading 을 의미함.
	void Save( const char* szName );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName );

	// Dynamic Loading	|| General <SFile==NULL> || Piece File			Loading 을 의미함.
	void Save( CSerialFile& SFile, BOOL bPiece );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const BOOL bDynamicLoad, const BOOL bPiece );

protected:
	void SingleTexMesh_Save( MAPSINGLETEXMESH& mapMesh, CSerialFile& SFile, BOOL bPiece );
	void SingleTexMesh_Load_Insert( LPDIRECT3DDEVICEQ pd3dDevice, MAPSINGLETEXMESH& mapMesh, CSerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece );
	void Load_101( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece );

public:
	void StartThread( LPDIRECT3DDEVICEQ pd3dDevice );
	void EndThread();

public:
	DxStaticMesh();
	~DxStaticMesh();
};

namespace NSSTATICMESH
{
	struct STATICDATA
	{
		D3DXVECTOR3	vMin;
		D3DXVECTOR3	vMax;

		VERTEX*	pVertices;
		DWORD	dwFaceNUM;

		STATICDATA() :
			vMin(FLT_MAX,FLT_MAX,FLT_MAX),
			vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX),
			pVertices(NULL),
			dwFaceNUM(0)
		{
		};
		~STATICDATA()
		{
			SAFE_DELETE_ARRAY ( pVertices );
		};
	};

	typedef std::map<std::string,STATICDATA*>			MAPSTATICDATA;
	typedef std::map<std::string,STATICDATA*>::iterator	MAPSTATICDATA_ITER;

	void ExportMaterialList ( const LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh* const pFrameMesh, DxStaticMesh* pStaticMesh, BOOL bProgress, BOOL bPiece );
	void ConvertMAPtoLIST ( MAPSINGLETEXMESH& mapData, PDXSINGLETEXMESH& pList );
	void ConvertTexEffMesh( const LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEff, MAPSINGLETEXMESH& mapData );
};