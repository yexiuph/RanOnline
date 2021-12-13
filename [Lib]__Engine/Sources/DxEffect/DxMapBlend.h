#pragma once

#include <algorithm>
#include <map>
#include <list>

#define	USE_LENGTH		0x0001

enum	DEFAULT_MAP_SETTING
{	
	EM_TEXSIZE		= 128,							// 텍스쳐 사이즈
	EM_TEXSIZE_2	= EM_TEXSIZE-2,					// 텍스쳐 사이즈
	EM_TEXSIZE_4	= EM_TEXSIZE-4,					// 텍스쳐 사이즈
	EM_TEXSIZE_4_A	= EM_TEXSIZE_4+EM_TEXSIZE_4,	// 텍스쳐 사이즈
	EM_TEXSIZE_EDIT	= EM_TEXSIZE+EM_TEXSIZE-2,		// Edit에서 사용
	EM_GRID			= 31,							// 격자 수
	EM_QURD			= EM_GRID*EM_GRID,				// 격자 수
	EM_FACE			= EM_QURD*2,					// 격자 수
	EM_VERTEX_PART	= 32,								// 버텍스 수
	EM_VERTEX_ALL	= EM_VERTEX_PART*EM_VERTEX_PART,	// 버텍스 수 가로세로 전체
	EM_VERTEX_EDIT	= EM_VERTEX_PART+EM_VERTEX_PART-1,	// Edit 할때 쓰이는 버텍스 수
	EM_QURD_VERTEX	= EM_QURD*4,							// 사각형으로 따졌을 때의 정점 갯수
};

struct TILE_TEX
{
	std::string	szTexture;
};

struct VERTEX_XT2
{
	D3DXVECTOR3			vPos;
	D3DXVECTOR2			vTex1;
	D3DXVECTOR2			vTex2;
	const static DWORD	FVF;
};

struct VERTEX_XNT2
{
	D3DXVECTOR3			vPos;
	D3DXVECTOR3			vNor;
	D3DXVECTOR2			vTex1;
	D3DXVECTOR2			vTex2;
	const static DWORD	FVF;
};

struct TILE
{
	VERTEX_XNT2	sVertex[4];	// 정점 정보
};

class DxBlendData
{
protected:
	BOOL		m_bEnable;

protected:
	DWORD					m_dwGrid;		// 격자 수.						< Edit, Game >
	DWORD					m_dwFaces;		// 면 수.						< Edit, Game >
	DWORD					m_dwVertices;	// 점 수.						< Edit, Game >
	LPDIRECT3DVERTEXBUFFERQ	m_pVB;			// VB.							< Edit, Game >
	LPDIRECT3DINDEXBUFFERQ	m_pIB;			// IB.							< Edit, Game >
	LPDIRECT3DTEXTUREQ		m_pTex;			// 사용자가 만든 타일 맵.		< Edit, Game >
	LPDIRECT3DTEXTUREQ		m_pAlphaTex;	// 프로그램이 만든 알파 맵.		< Edit, Game >
	BOOL*					m_pTileUSE;		// IB 생성을 위해서 필요.		< Edit>			[EM_GRID][EM_GRID]
	BOOL*					m_pAlphaUSE;	// AphaTex 생성을 위해서 필요.	< Edit>			[EM_TEXSIZE][EM_TEXSIZE]

	std::string				m_szBaseTex;

public:
	BOOL	IsEnable()	{ return m_bEnable; }

public:
	void	InitData ( LPDIRECT3DDEVICEQ pd3dDevice, TILE* pTile );			// 기본 초기화
	void	InitData ( LPDIRECT3DDEVICEQ pd3dDevice, const char* cName );	// New 초기화

	void	ModifyAlphaTex ( int _X, int _Y, BOOL bUSE );					// 알파 텍스쳐 수정용

	BOOL	CheckTileUSE ();												// VB, IB 수정용
	void	ModifyVB ( LPDIRECT3DDEVICEQ pd3dDevice,TILE* pTile );			// VB, IB 수정용
	void	ModifyIB ( LPDIRECT3DDEVICEQ pd3dDevice );						// VB, IB 수정용

	void	Modify_Height_PART ( TILE* pTile );								// 높이 데이터 수정용

	void	Render ( LPDIRECT3DDEVICEQ pd3dDevice );						// 렌더

protected:
	void	InitAlphaTex ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwColor );	// 알파 초기화
	BOOL	IsTileGrid ( int initX, int initZ );							// CheckTileUSE() 에서 타일이 쓰이는지 알아내기 위한 함수

public:
	void	SaveFile_Edit ( CSerialFile &SFile );
	void	LoadFile_Edit ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile, const char* pName );
	void	SaveFile_Game ( CSerialFile &SFile );
	void	LoadFile_Game ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile, const char* pName );

public:
	DxBlendData ();
	~DxBlendData ();
};

typedef std::map<std::string,DxBlendData*>	BLENDMAP;
typedef BLENDMAP::iterator					BLENDMAP_ITER;

class DxBlend
{
protected:
	BLENDMAP		m_mapBlend;		// BLENDING 하기위한 정보들.	< Edit, Game >
	TILE_TEX*		m_pTileTex;		// 각 지점의 타일이름			< Edit >		[EM_TEXSIZE][EM_TEXSIZE]

public:
	BOOL	IsEnable();
	void	Create ( LPDIRECT3DDEVICEQ pd3dDevice, TILE* pTile );
	void	Modify ( LPDIRECT3DDEVICEQ pd3dDevice, TILE* pTile );
	void	ModifyTex ( LPDIRECT3DDEVICEQ pd3dDevice, const char* strName, int nTempX, int nTempY );
	void	Modify_Height ( TILE* pTile );
	void	Render ( LPDIRECT3DDEVICEQ pd3dDevice );

	void	SaveFile_Edit ( CSerialFile &SFile );
	void	LoadFile_Edit ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile );
	void	SaveFile_Game ( CSerialFile &SFile );
	void	LoadFile_Game ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile );

public:
	DxBlend ();
	~DxBlend ();
};

class DxColor
{
protected:
	BOOL		m_bEnable;

protected:
	LPDIRECT3DTEXTUREQ		m_pTex;				// 최종 결과 본	< Edit, Game >
	D3DXCOLOR*				m_pColor;			// 원본 컬러	< Edit >		[EM_TEXSIZE][EM_TEXSIZE]

public:
	BOOL	IsEnable()		{ return	m_bEnable; }
	void	Create ( LPDIRECT3DDEVICEQ pd3dDevice );
	void	ColorChange ( D3DXCOLOR _cColor, int _X, int _Y );

	LPDIRECT3DTEXTUREQ GetTexture()		{ return m_pTex; }

	void	SaveFile_Edit ( CSerialFile &SFile );
	void	LoadFile_Edit ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile );
	void	SaveFile_Game ( CSerialFile &SFile );
	void	LoadFile_Game ( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile &SFile );

public:
	DxColor ();
	~DxColor ();
};