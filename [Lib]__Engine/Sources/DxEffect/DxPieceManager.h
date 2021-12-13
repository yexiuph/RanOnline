#pragma once

#include <vector>
#include <map>
#include <string>

struct DxFrame;
class DxFrameMesh;
class DxStaticMesh;

#include "DxPiece.h"

// -----------------------------------------------------------------------------------------------------------------------------------------
//											D	x		P	i	e	c	e		A	A	B	B
// -----------------------------------------------------------------------------------------------------------------------------------------
class DxPieceAABB
{
protected:
	DxPieceNode*		m_pPieceAABBTree;	// AABB Tree
	DxPiece*			m_pPieceHead;		// List
	DxPieceQuickSort	m_sQuickSort;		// Quick Sort

	DWORD			m_dwPieceNUM;		// Piece 갯수..

public:
	void SetPiece( DxPiece* pPiece );
	void MakeTree();

public:
	void FrameMove( const float fTime, const float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, DxPiece* pPiecePick, const CLIPVOLUME &sCV, const float fTime, const float fElapsedTime );
	void RenderAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMesh* pStaticMesh, DxFrameMesh* pFrameMesh );
	void RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMesh* pStaticMesh, DxFrameMesh* pFrameMesh );

	void Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );

	// Edit
	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vCollision );

public:
	void Save( CSerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, const char* pPieceName );

public:
	void CloneData( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceAABB* pSrc );
	void ClearOnlyTree();
	void ClearTree();

public:
	DxPieceAABB();
	~DxPieceAABB();
};

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		P	i	e	c	e		M	a	n	a	g	e	r
// -----------------------------------------------------------------------------------------------------------------------------------------
class DxPieceManager
{
private:
	static const DWORD VERSION_PSF;
	static const DWORD VERSION_WLD;

protected:
	float m_fTime;
	float m_fElapsedTime;

protected:
	struct MATRIXPISNAME
	{
		D3DXMATRIX	m_matWorld;		// 자신의 Matrix
		char*		m_pPisName;
		DxPiece*	m_pPiece;

		void Save( CSerialFile& SFile, const D3DXMATRIX& matLocal, const D3DXMATRIX& matFrame );
		void Load( CSerialFile& SFile );

		void CleanUp();

		MATRIXPISNAME();
		~MATRIXPISNAME();
	};

protected:
	// FrameName, Piece
	typedef std::map<std::string,MATRIXPISNAME*>	MAPCHANGEPIS;
	typedef MAPCHANGEPIS::iterator					MAPCHANGEPIS_ITER;
	MAPCHANGEPIS	m_mapChangePis;

	// Point, Piece
	typedef std::vector<MATRIXPISNAME*>		VECPOINTPIS;
	typedef VECPOINTPIS::iterator			VECPOINTPIS_ITER;
	VECPOINTPIS	m_vecPointPis;

	// PieceName, PieceAABB
	typedef std::map<std::string,DxPieceAABB*>	MAPNAMEPIECE;
	typedef MAPNAMEPIECE::iterator				MAPNAMEPIECE_ITER;
	MAPNAMEPIECE	m_mapNamePiece;

protected:
	DxPiece*			m_pPiecePick;		// Pick Piece
	DxPieceQuickSort	m_sPickSort;		// Pick Quick Sort

public:
	DWORD GetSize()		{ return (DWORD)(m_mapChangePis.size()+m_vecPointPis.size()); }
	BOOL IsGetName( DWORD nNUM, LPCSTR& pPieceName, LPCSTR& pFrameName );
	void SetPiece( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh* pFrameMesh, const char* szFrameName, const char* szPisName );
	void DeletePiece( DxFrameMesh* pFrameMesh, const char* szFrameName );
	void SetPiece( LPDIRECT3DDEVICEQ pd3dDevice, const char* szPisName, const D3DXMATRIX& matWorld );
	void DeletePiece( DWORD dwNumber );
	D3DXMATRIX* MoveCamera( DxFrame* pFrameRoot, const char* szFrameName );
	D3DXMATRIX* MoveCamera( DWORD dwNumber );

	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vCollision );

protected:
	void MakeTree();		// Note : Render를 하기위해 구조 변환 및 Tree 생성

public:
	void FrameMove( const float fTime, const float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void RenderAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMesh* pStaticMesh, DxFrameMesh* pFrameMesh );
	void RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMesh* pStaticMesh, DxFrameMesh* pFrameMesh );

	void Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void SavePSF( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxFrameMesh* pFrameMesh );
	void LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxFrameMesh* pFrameMesh );
	void LoadPSF_100( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxFrameMesh* pFrameMesh );

	void SaveWLD( CSerialFile& SFile );
	void LoadWLD( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile );

	void ClonePSFtoWLD( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceManager* pSrc );	// Game에 맞춘 복제
	void CleanUp();

public:
	DxPieceManager();
	~DxPieceManager();
};