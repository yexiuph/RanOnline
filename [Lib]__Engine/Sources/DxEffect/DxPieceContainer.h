#pragma once

#include <vector>
#include <map>
#include <string>
#include "StlFunctions.h"

#include "CollisionMap.h"

struct DxFrame;
class DxFrameMesh;
class DxPieceEff;
class DxStaticAniFrame;
class DxPieceTexEff;

//----------------------------------------------------------------------------------------------------------------------
//										D	x		P	i	e	c	e		E	d	i	t
//	Note : 
//----------------------------------------------------------------------------------------------------------------------
class DxPieceEdit
{
protected:
	static const DWORD VERSION;

protected:
	float m_fTime;
	float m_fAniTime;

protected:
	std::string	m_strFileName;
	std::string	m_strFrameName;
	D3DXVECTOR3 m_vMax;			// 전체의 Max	< 물체 밎, 효과의 범위까지 >
	D3DXVECTOR3 m_vMin;			// 전체의 Min	< 물체 밎, 효과의 범위까지 >
	D3DXVECTOR3	m_vStaticMax;	// StaticMesh 만의 한정. < 캐릭터를 가릴경우를 체크하기 위함 >
	D3DXVECTOR3	m_vStaticMin;	// StaticMesh 만의 한정. < 캐릭터를 가릴경우를 체크하기 위함 >
	D3DXVECTOR3 m_vCenter;		// 물체의 중점. 						< 1차 체크를 위함 >		변환된 것
	float		m_fLength;		// 중점에서 원을 그렸을 경우의 반지름.	< 1차 체크를 위함 >
	D3DXMATRIX	m_matLocal;		// 지역적 Local Matrix

protected:
	DxFrameMesh*	m_pFrameMesh;		// FrameMesh EDIT 용.
	DxPieceEff*		m_pPieceEffEDIT;	// 현재 EDIT 하기 위해 만들었다.
	CCollisionMap*	m_pCollisionMap;	// 현재 EDIT 하기 위해 만들었다.

public:
	DxStaticAniFrame*	m_pStaticAniFrame;
	DxPieceEff*			m_pPieceEff;
	DxPieceTexEff*		m_pAniTexEff;

public:
	int		m_nRefCount;

public:
	const char*	GetFrameName()		{ return m_strFrameName.c_str(); }
	D3DXVECTOR3	GetAABBMax()		{ return m_vMax; }
	D3DXVECTOR3	GetAABBMin()		{ return m_vMin; }
	D3DXVECTOR3	GetStaticMax()		{ return m_vStaticMax; }
	D3DXVECTOR3	GetStaticMin()		{ return m_vStaticMin; }
	D3DXVECTOR3	GetCenter()			{ return m_vCenter; }
	float		GetLength()			{ return m_fLength; }
	D3DXMATRIX*	GetMatrixLocal()	{ return &m_matLocal; };

public:
	DxFrameMesh*		GetFrameMesh()		{ return m_pFrameMesh; }
	DxPieceEff*			GetPieceEffEDIT()	{ return m_pPieceEffEDIT; }

public:
	void	NewFrameMesh ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName );
	void	Export( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxFrame*	GetFrameRoot();

public:
	BOOL IsCollision ( const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, BOOL bEditor, const DxFrame* pFrame, char* pName );
	DxFrame* const GetCollisionDetectedFrame ();
	const D3DXVECTOR3 GetCollisionPos ();

	BOOL SetEff( LPDIRECT3DDEVICEQ pd3dDevice, const char* szFrameName, const D3DXVECTOR3& vPos, const char* szEffName, std::string& strEditName );
	void DelEff( const char* szEffName );
	void ModifyEff( LPDIRECT3DDEVICEQ pd3dDevice, const char* szEditName, const char* szEffName );
	LPD3DXMATRIX GetEffMatrix( const char* szEditName );

	void SetCheckCollPoint( const char* szEffName, BOOL bUse );
	BOOL GetCheckCollPoint( const char* szEffName );
	const char* GetEffName( const char* pEditName );

public:
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void OnDestroyDevice();

public:
	void FrameMoveFrame( const float fTime, const float fElapsedTime );
	void RenderFrame( const LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &sCV );

	void FrameMove( const float fTime, const float fElapsedTime );
	void Render( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, const D3DXMATRIX& matWorld );

public:
	void CleanUp();

public:
	void SavePEF( const char* szName );
	HRESULT LoadPEF( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName );
	void LoadFrameEff( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxFrameMesh* pFrameMesh );
	void SavePIE( const char* szName );
	HRESULT LoadPIE( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName );

public:
	DxPieceEdit();
	~DxPieceEdit();
};


//----------------------------------------------------------------------------------------------------------------------
//								D	x		P	i	e	c	e		C	o	n	t	a	i	n	e	r
//	Note : 
//----------------------------------------------------------------------------------------------------------------------
class DxPieceContainer
{
protected:
	static char		m_szPath[MAX_PATH];

public:
	static void		SetPath( char* szPath )	{ StringCchCopy( m_szPath, MAX_PATH, szPath ); }
	static char*	GetPath()				{ return m_szPath; }

public:
	typedef std::map<std::string,DxPieceEdit*>				MAPSTATICPIS;
	typedef std::map<std::string,DxPieceEdit*>::iterator	MAPSTATICPIS_ITER;
	MAPSTATICPIS	m_mapStaticPis;

public:
	DxPieceEdit* LoadPiece ( LPDIRECT3DDEVICEQ pd3dDevice, const char *szFile, const char *szDir=NULL );
	void ReleasePiece ( const char *szFile );

//	DxSingleMesh* GetSingleMesh ( const char *szPis, const char *szMaterial );

protected:
	DxPieceEdit* FindPiece ( const char *szFile );

public:
	void CleanUp();

public:
	DxPieceContainer();
	~DxPieceContainer();

public:
	static DxPieceContainer& GetInstance();
};